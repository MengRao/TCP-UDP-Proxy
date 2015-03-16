#pragma once
#include "type.h"
#include "io_service_pool.h"
#include "common.h"
#include "udp_proxy.h"
#include "tcp_proxy.h"
#include <list>

namespace proxy_ns
{
	class SharedData
	{
	public:
		SharedData();

		void init();

		void start();

		void stop();

		std::list<boost::shared_ptr<TcpProxy> > proxies;

		UdpProxyServer udp_proxy_server;


		global::io_service_pool *network_io_service_pool;
	};

	extern SharedData sdata;
}
