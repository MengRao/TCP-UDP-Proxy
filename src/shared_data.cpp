#include "shared_data.h"
#include "args.h"
#include <string>
#include <boost/algorithm/string.hpp>

using namespace std;

namespace proxy_ns
{

	SharedData sdata;

	SharedData::SharedData()
		: network_io_service_pool(NULL)
	{

	}

	void SharedData::init()
	{
		network_io_service_pool = new global::io_service_pool(args.network_thread_num);
		string conf_file = args.proxy_config_file;
		ifstream fin(conf_file.c_str());
		if (!fin.is_open())
		{
			LWarningStr(boost::format("open proxy config file : %s fail!") % conf_file);
			return;
		}

		int line_num = 0;
		string line;
		istringstream is;
		boost::system::error_code ec;
		string proto;
		uint16 src_port;
		string dst_ip;
		boost::asio::ip::address ip_addr;
		uint16 dst_port;
		int ip_send_bandwith = 0;
		int ip_recv_bandwidth = 0;
		while (!fin.eof())
		{
			++line_num;
			getline(fin, line);
			boost::trim(line);
			if (line.empty() || line[0] == '#')
			{
				continue;
			}
			is.str(line);
			is.clear();

			if (!(is >> proto))
			{
				LErrStr(boost::format("proxy config file format error in line %d: %s") % line_num % line);
				continue;
			}
			if (proto == "ip")
			{
				if (!(is >> ip_send_bandwith) ||
					!(is >> ip_recv_bandwidth))
				{
					LErrStr(boost::format("proxy config file format error in line %d: %s") % line_num % line);
					continue;
				}
			}
			else if (proto == "tcp")
			{
				if (!(is >> src_port) ||
					!(is >> dst_ip) ||
					(ip_addr = boost::asio::ip::address::from_string(dst_ip, ec), ec) ||
					!(is >> dst_port))
				{
					LErrStr(boost::format("proxy config file format error in line %d: %s") % line_num % line);
					continue;
				}
				TcpProxyRule proxy_rule(src_port, boost::asio::ip::tcp::endpoint(ip_addr, dst_port));
				int extra_param;
				if (is >> extra_param)
				{
					proxy_rule.max_bandwidth = extra_param;
				}
				if (is >> extra_param)
				{
					proxy_rule.delay_ms = extra_param;
				}
				boost::shared_ptr<TcpProxy> proxy(new TcpProxy(network_io_service_pool->get_io_service(), proxy_rule));

				if (!proxy->start())
				{
					LErrStr(boost::format("tcp proxy start error in line %d: %s") % line_num % line);
					continue;
				}
				proxies.push_back(proxy);
			}
			else if (proto == "udp")
			{
				if (!(is >> src_port) ||
					!(is >> dst_ip) ||
					(ip_addr = boost::asio::ip::address::from_string(dst_ip, ec), ec) ||
					!(is >> dst_port))
				{
					LErrStr(boost::format("proxy config file format error in line %d: %s") % line_num % line);
					continue;
				}
				UdpProxyRule proxy_rule(src_port, boost::asio::ip::udp::endpoint(ip_addr, dst_port));

				int extra_param;
				if (is >> extra_param)
				{
					proxy_rule.max_bandwidth = extra_param;
				}
				if (is >> extra_param)
				{
					proxy_rule.delay_ms = extra_param;
				}
				if (is >> extra_param)
				{
					proxy_rule.drop_prob = extra_param;
				}
				udp_proxy_server.addServerProxy(network_io_service_pool->get_io_service(), proxy_rule);
			}
			else
			{
				LErrStr(boost::format("proxy config file format error in line %d: %s") % line_num % line);
				continue;
			}
		}
		fin.close();
		ip_proxies.start(ip_send_bandwith, ip_recv_bandwidth);
	}

	void SharedData::start()
	{
		udp_proxy_server.start();
		network_io_service_pool->start();
	}

	void SharedData::stop()
	{
		for (list<boost::shared_ptr<TcpProxy> >::iterator it = proxies.begin(); it != proxies.end(); ++it)
		{
			(*it)->stop();
		}
		udp_proxy_server.stop();
		ip_proxies.stop();
		if (network_io_service_pool)
		{
			global::sleepms(100);
			network_io_service_pool->stop();
			network_io_service_pool->join();
			delete network_io_service_pool;
			network_io_service_pool = NULL;
		}
	}

}
