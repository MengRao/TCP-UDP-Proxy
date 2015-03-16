#include "tcp_proxy.h"
#include "args.h"
#include <iostream>

using namespace std;


namespace proxy_ns
{

	TcpConnection::TcpConnection(TcpProxy& proxy_, TcpSocket socket)
		: close_flag(false)
		, proxy(proxy_)
		, active_time(proxy.getNow())
		, full_open(false)
		, client_ip(ip_proxies.getIPProxy(socket->remote_endpoint().address()))
	{
		socket_pair[0] = socket;
		for (int pair_id = 0; pair_id < 2; ++pair_id)
		{
			available_bytes_pair[pair_id] = 0;
			writing_flag_pair[pair_id] = false;
			reading_flag_pair[pair_id] = true;
		}
	}

	TcpConnection::~TcpConnection()
	{
		if (client_ip)
		{
			boost::mutex::scoped_lock lock(client_ip->mtx);
			--client_ip->ref_cnt;
		}
	}

	bool TcpConnection::start()
	{
		socket_pair[1].reset(new boost::asio::ip::tcp::socket(proxy.getIoService()));
		socket_pair[1]->async_connect(proxy.getPassiveDst(), boost::bind(&TcpConnection::handleConnect, this, _1));
		return true;
	}

	void TcpConnection::stop()
	{
		close_flag = true;
		boost::system::error_code ec;
		for (int pair_id = 0; pair_id < 2; ++pair_id)
		{
			socket_pair[pair_id]->close(ec);
		}
	}

	bool TcpConnection::flushMsgs(int flush_bytes, global::sec_t expire_time)
	{
		if (close_flag)
		{
			return false;
		}
		if (active_time < expire_time)
		{
			stop();
			return false;
		}
		if (!full_open && msg_queue_pair[0].empty() && msg_queue_pair[1].empty())
		{
			return true;
		}
		

		for (int pair_id = 0; pair_id < 2; ++pair_id)
		{
			if (proxy.getBandWidth() > 0 && available_bytes_pair[pair_id] < 0)
			{
				available_bytes_pair[pair_id] += flush_bytes;
			}

			if (!writing_flag_pair[pair_id])
			{
				handleWrite(pair_id, boost::system::error_code());
			}
		}
		return true;
	}

	void TcpConnection::handleWrite(int pair_id, const boost::system::error_code& ec)
	{
		if (ec)
		{
			if (ec != boost::asio::error::operation_aborted)
			{
				cout << "TcpConnection::handleWrite err: " << ec << " " << ec.message() << " " << socket_pair[pair_id]->remote_endpoint().address() << endl;
				stop();
			}
			return;
		}

		if (writing_flag_pair[pair_id])
		{
			XASSERT(!msg_queue_pair[pair_id].empty());
			msg_queue_pair[pair_id].pop();
			if (!full_open && msg_queue_pair[0].empty() && msg_queue_pair[1].empty())
			{
				stop();
				return;
			}
			if (reading_flag_pair[1-pair_id] == false)
			{
				reading_flag_pair[1-pair_id] = true;
				asyncRead(1-pair_id);
			}
		}
		if (!msg_queue_pair[pair_id].empty() &&
			available_bytes_pair[pair_id] >= 0 &&
			msg_queue_pair[pair_id].front().first <= proxy.getNow())
		{
			global::Buffer& buf = msg_queue_pair[pair_id].front().second;
			if (ip_proxies.bandwidth[1-pair_id] > 0)
			{
				boost::mutex::scoped_lock lock(client_ip->mtx);
				if (client_ip->avail_bytes[1-pair_id] >= 0)
				{
					client_ip->avail_bytes[1-pair_id] -= buf.get_size() + TCP_PACKET_EXTRA_OVERHEAD;
				}
				else
				{
					writing_flag_pair[pair_id] = false;
					return;
				}
			}
			boost::asio::async_write(*socket_pair[pair_id], 
				boost::asio::buffer(buf.get(), buf.get_size()),
				boost::bind(&TcpConnection::handleWrite, this, pair_id, _1));
			if (proxy.getBandWidth() > 0)
			{
				available_bytes_pair[pair_id] -= buf.get_size() + TCP_PACKET_EXTRA_OVERHEAD;
			}
			writing_flag_pair[pair_id] = true;
		}
		else
		{
			writing_flag_pair[pair_id] = false;
		}

	}

	void TcpConnection::handleConnect(const boost::system::error_code& ec)
	{
		if (ec)
		{
			if (ec != boost::asio::error::operation_aborted)
			{
				stop();
			}
			return;
		}
		active_time = proxy.getNow();
		full_open = true;
		asyncRead(0);
		asyncRead(1);
	}

	void TcpConnection::asyncRead(int pair_id)
	{
		recv_buf_pair[pair_id].reset(new BYTE[TCP_BUFFER_SIZE]);
		socket_pair[pair_id]->async_read_some(boost::asio::buffer(recv_buf_pair[pair_id].get(), TCP_BUFFER_SIZE), 
												boost::bind(&TcpConnection::handleRead, this, pair_id, _1, _2));
	}

	void TcpConnection::handleRead(int pair_id, const boost::system::error_code& ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			if (ec != boost::asio::error::operation_aborted)
			{
				if (!msg_queue_pair[1-pair_id].empty())
					full_open = false;
				else
					stop();
			}
			return;
		}
		if (full_open)
		{
			active_time = proxy.getNow();
		}

		msg_queue_pair[1-pair_id].push(Msg(active_time+proxy.getDelayMs(), global::Buffer(recv_buf_pair[pair_id], bytes_transferred)));
		if (msg_queue_pair[1-pair_id].size() > MAX_TCP_MSG_QUEUE_SIZE)
		{
			reading_flag_pair[pair_id] = false;
			return;
		}

		asyncRead(pair_id);
	}


	///////////////////////////////////////////////////////////////////////////////////////

	TcpProxy::TcpProxy(boost::asio::io_service& io_service, TcpProxyRule proxy_rule_)
		: acceptor(io_service)
		, accept_socket()
		, proxy_rule(proxy_rule_)
		, close_flag(false)
		, routine_timer(io_service)
		, now(global::currenttime::getms())
		, last_flush_tm(now)
	{

	}

	TcpProxy::~TcpProxy()
	{

	}

	bool TcpProxy::start()
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), proxy_rule.src_port);
		boost::system::error_code ec;

		acceptor.open(endpoint.protocol(), ec);
		if (ec)
		{
			return false;
		}
		acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
		if (ec)
		{
			return false;
		}
		acceptor.bind(endpoint, ec);	
		if (ec)
		{
			return false;
		}
		acceptor.listen(boost::asio::socket_base::max_connections, ec);
		if (ec)
		{
			return false;
		}
		startAccept();
		asyncWait();

		return true;
	}

	void TcpProxy::stop()
	{
		close_flag = true;
	}


	void TcpProxy::startAccept()
	{
		accept_socket.reset(new boost::asio::ip::tcp::socket(getIoService()));
		acceptor.async_accept(*accept_socket, boost::bind(&TcpProxy::handleAccept, this, boost::asio::placeholders::error));

	}

	void TcpProxy::handleAccept(const boost::system::error_code& error)
	{
		if (error)
		{
			if (error != boost::asio::error::operation_aborted)
			{
				LErrStr(boost::format("tcp socket handleAccept err: %s") % error.message());
				boost::system::error_code ec;
				acceptor.close(ec);
				if (!start())
				{
					stop();
				}
			}
			return;
		}

		TcpConn new_conn(new TcpConnection(*this, accept_socket));
		if (new_conn->start())
		{
			tcp_connections.push_back(new_conn);
		}
		startAccept();
	}

	void TcpProxy::asyncWait()
	{
		boost::system::error_code ec;
		routine_timer.expires_from_now(boost::posix_time::milliseconds(10), ec);
		if (ec)
		{
			close_flag = true;
			close();
			return;
		}
		routine_timer.async_wait(boost::bind(&TcpProxy::handleWait, this, _1));
	}

	void TcpProxy::handleWait(const boost::system::error_code& error)
	{
		if (error)
		{
			return;
		}
		if (close_flag)
		{
			close();
			return;
		}

		asyncWait();
		now = global::currenttime::getms();
		if (now <= last_flush_tm)
		{
			return;
		}
		int flush_bytes = (int)((now - last_flush_tm) * proxy_rule.max_bandwidth / 1000);
		last_flush_tm = now;

		global::msec_t expire_time = now - args.tcp_timeout;
		for (TcpConnections::iterator it = tcp_connections.begin(); it != tcp_connections.end();)
		{
			if ((*it)->flushMsgs(flush_bytes, expire_time))
			{
				++it;
			}
			else
			{
				it = tcp_connections.erase(it);
			}
		}

	}

	void TcpProxy::close()
	{
		XASSERT(close_flag);
		boost::system::error_code ec;
		acceptor.close(ec);
		routine_timer.cancel(ec);
		for (TcpConnections::iterator it = tcp_connections.begin(); it != tcp_connections.end(); ++it)
		{
			(*it)->stop();
		}

	}

	void TcpProxy::getInfo(std::ostream &os)
	{
		os << "local port: " << proxy_rule.src_port << "\r\n";
		os << "dest endpoint: " << proxy_rule.dst_endpoint << "\r\n";
		os << "max bandwidth: " << proxy_rule.max_bandwidth << "\r\n";
		os << "delay ms: " << proxy_rule.delay_ms << "\r\n";
		os << "conn cnt: " << tcp_connections.size() << "\r\n";
	}



}
