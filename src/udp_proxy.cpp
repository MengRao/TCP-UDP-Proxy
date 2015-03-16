#include "udp_proxy.h"
#include "xrandom.h"
#include "args.h"

#include <list>

using namespace global;
using namespace std;

namespace proxy_ns
{
	UdpEndpointProxy::UdpEndpointProxy(boost::asio::io_service& io_service, UdpProxyRule proxy_rule_)
		: proxy_rule(proxy_rule_)
		, proxy_socket(io_service)
		, recving_msg()
		, server_flag(proxy_rule.src_port != 0)
		, active_tm(0)
		, recv_avail_bytes(0)
		, send_avail_bytes(0)
		, recving_flag(true)
		, client_ip(server_flag ? NULL : ip_proxies.getIPProxy(proxy_rule.dst_endpoint.address()))
	{
		init();
	}

	void UdpEndpointProxy::init()
	{
		boost::system::error_code ec;
		boost::asio::ip::udp::endpoint endpoint(proxy_rule.dst_endpoint.protocol(), proxy_rule.src_port);
		proxy_socket.open(proxy_rule.dst_endpoint.protocol(), ec);
		if (ec)
		{
			LErrStr(boost::format("udp socket open err: %s") % ec.message());
			return;
		}
		proxy_socket.set_option(boost::asio::ip::udp::socket::reuse_address(true) ,ec);
		if (ec)
		{
			LErrStr(boost::format("udp socket set_option err: %s") % ec.message());
			return;
		}

		proxy_socket.bind(endpoint, ec);
		if (ec)
		{
			LErrStr(boost::format("udp socket bind err: %s") % ec.message());
			return;
		}
		asyncRecv();
		active_tm = global::currenttime::getms();
	}

	UdpEndpointProxy::~UdpEndpointProxy()
	{
		if (client_ip)
		{
			boost::mutex::scoped_lock lock(client_ip->mtx);
			--client_ip->ref_cnt;
		}
	}

	void UdpEndpointProxy::stop()
	{
		// mustn't lock
		proxy_socket.close();
		active_tm = 0;
	}

	void UdpEndpointProxy::asyncRecv()
	{
		proxy_socket.async_receive_from(boost::asio::buffer(recving_msg.msg.get(), UDP_BUFFER_SIZE), recving_msg.src, boost::bind(&UdpEndpointProxy::handleRecv, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}

	void UdpEndpointProxy::handleRecv(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error)
		{
			if (error == boost::asio::error::connection_refused ||
				error == boost::asio::error::connection_reset)
			{
				asyncRecv();
			}
			else if (error != boost::asio::error::operation_aborted)
			{
				boost::mutex::scoped_lock lock(mtx);
				LErrStr(boost::format("udp socket handleRecv err: %s") % error.message());
				stop();
				if (server_flag)
				{
					init();
				}
			}
			return;
		}

		boost::mutex::scoped_lock lock(mtx);

		active_tm = global::currenttime::getms();

		if (proxy_rule.drop_prob > 0 && global::random::int32_range(1, 10000) <= proxy_rule.drop_prob)
		{
			asyncRecv();
			return;
		}

		recving_msg.time = active_tm + proxy_rule.delay_ms;
		recving_msg.msg.resize(bytes_transferred);

		recv_queue.push(recving_msg);
		recving_msg = UdpMsg();

		if (recv_queue.size() > MAX_UDP_MSG_QUEUE_SIZE)
		{
			recving_flag = false;
			return;
		}

		asyncRecv();
	}

	////////////////////////////////////////////////////////////

	UdpProxyServer::UdpProxyServer()
	{

	}

	void UdpProxyServer::addServerProxy(boost::asio::io_service& io_service, UdpProxyRule proxy_rule)
	{
		endpoints[proxy_rule.dst_endpoint].reset(new UdpEndpointProxy(io_service, proxy_rule));
	}

	void UdpProxyServer::getInfo(ostream &os)
	{
		os << "udp endpoint cnt: " << endpoints.size() << "\r\n";
	}

	void UdpProxyServer::start()
	{
		grp.create_thread(boost::bind(&UdpProxyServer::routineThreadHandler, this));
	}

	void UdpProxyServer::stop()
	{
		grp.join_all();
	}

	void UdpProxyServer::routineThreadHandler()
	{
		global::msec_t now = global::currenttime::getms();
		global::msec_t last_flush_tm = now;
		global::msec_t interval = 0;
		global::msec_t active_timeout_tm = now - args.udp_timeout * 1000;
		while (args.running_flag)
		{
			global::sleepms(10);
			now = global::currenttime::getms();
			interval = now - last_flush_tm;
			if (interval <= 0)
			{
				continue;
			}
			last_flush_tm = now;
			active_timeout_tm = now - args.udp_timeout;

			Endpoints new_epts;

			for (Endpoints::iterator it = endpoints.begin(); it != endpoints.end();)
			{
				UdpEPTProxy ept = it->second;
				boost::mutex::scoped_lock lock(ept->mtx);
				if (ept->active_tm == 0 || (ept->active_tm < active_timeout_tm && !ept->server_flag))
				{
					endpoints.erase(it++);
					continue;
				}


				if (!ept->server_flag && ept->proxy_rule.max_bandwidth > 0)
				{
					int bytes = (int)(interval * ept->proxy_rule.max_bandwidth / 1000);
					if (ept->recv_avail_bytes < 0)
					{
						ept->recv_avail_bytes += bytes;
					}
					if (ept->send_avail_bytes < 0)
					{
						ept->send_avail_bytes += bytes;
					}
				}

				while (!ept->recv_queue.empty() &&
					   ept->recv_avail_bytes >= 0)
				{
					UdpMsg& msg = ept->recv_queue.front();
					XASSERT(msg.src != it->first);
					if (msg.time > now)
					{
						break;
					}
					if (!ept->server_flag && ip_proxies.bandwidth[1] > 0)
					{
						IPProxy& ip_proxy = *ept->client_ip;
						boost::mutex::scoped_lock lock(ip_proxy.mtx);
						if (ip_proxy.avail_bytes[1] >= 0)
						{
							ip_proxy.avail_bytes[1] -= msg.msg.get_size() + UDP_PACKET_EXTRA_OVERHEAD;
						}
						else
						{
							break;
						}
					}
					Endpoints::iterator it2 = endpoints.find(msg.src);
					UdpEPTProxy sender;
					if (it2 == endpoints.end())
					{
						UdpProxyRule proxy_rule = ept->proxy_rule;
						proxy_rule.dst_endpoint = msg.src;
						proxy_rule.src_port = 0;
						sender.reset(new UdpEndpointProxy(ept->proxy_socket.get_io_service(), proxy_rule));
						if (sender->active_tm != 0)
						{
							new_epts[msg.src] = sender;
						}
					}
					else
					{
						sender = it2->second;
						boost::mutex::scoped_lock lock(sender->mtx);
						if (sender->active_tm != 0)
						{
							sender->active_tm = now;
						}
					}
					if (sender->send_queue.size() < MAX_UDP_MSG_QUEUE_SIZE)
					{
						msg.src = it->first;
						sender->send_queue.push(msg);
					}
					if (!ept->server_flag && ept->proxy_rule.max_bandwidth > 0)
					{
						ept->recv_avail_bytes -= msg.msg.get_size() + UDP_PACKET_EXTRA_OVERHEAD;
					}
					ept->recv_queue.pop();
					if (ept->recving_flag == false)
					{
						ept->recving_flag = true;
						ept->asyncRecv();
					}
				}

				++it;

			}

			endpoints.insert(new_epts.begin(), new_epts.end());

			boost::system::error_code ec;
			for (Endpoints::iterator it = endpoints.begin(); it != endpoints.end(); ++it)
			{
				UdpEPTProxy& ept = it->second;
				boost::mutex::scoped_lock lock(ept->mtx);
				while (!ept->send_queue.empty() &&
					   ept->send_avail_bytes >= 0)
				{
					UdpMsg& msg = ept->send_queue.front();
					if (!ept->server_flag && ip_proxies.bandwidth[0] > 0)
					{
						IPProxy& ip_proxy = *ept->client_ip;
						boost::mutex::scoped_lock lock(ip_proxy.mtx);
						if (ip_proxy.avail_bytes[0] >= 0)
						{
							ip_proxy.avail_bytes[0] -= msg.msg.get_size() + UDP_PACKET_EXTRA_OVERHEAD;
						}
						else
						{
							break;
						}
					}
					ept->proxy_socket.send_to(
						boost::asio::buffer(msg.msg.get(), msg.msg.get_size())
						, msg.src
						, 0
						, ec);
					if (ec)
					{
						LErrStr(boost::format("udp socket send_to err: %s") % ec.message());
						ept->stop();
						if (ept->server_flag)
						{
							ept->init();
						}
						break;
					}
					if (!ept->server_flag && ept->proxy_rule.max_bandwidth > 0)
					{
						ept->send_avail_bytes -= msg.msg.get_size() + UDP_PACKET_EXTRA_OVERHEAD;
					}
					ept->send_queue.pop();
				}
			}

		}
		//server stop
		for (Endpoints::iterator it = endpoints.begin(); it != endpoints.end(); ++it)
		{
			boost::mutex::scoped_lock lock(it->second->mtx);
			it->second->stop();
		}
	}

}
