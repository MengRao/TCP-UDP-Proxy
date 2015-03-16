#pragma once
#include "common.h"
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <map>

namespace proxy_ns
{
	struct IPProxy
	{
		int avail_bytes[2]; // 0: send_avail_bytes 1:recv_avail_bytes
		int ref_cnt;
		boost::mutex mtx;

		IPProxy()
			: ref_cnt(0)
		{
			avail_bytes[0] = 0;
			avail_bytes[1] = 0;
		}

		IPProxy(const IPProxy& rhs)
			: ref_cnt(rhs.ref_cnt)
		{
			avail_bytes[0] = rhs.avail_bytes[0];
			avail_bytes[1] = rhs.avail_bytes[1];
		}
	};

	typedef boost::shared_ptr<IPProxy> IPProxyPtr;

	class IPProxies
	{
	public:
		IPProxies();

		void start(int send_bandwith_, int recv_bandwidth_);

		void stop();

		IPProxy *getIPProxy(boost::asio::ip::address ip_addr);

	private:

		void routineThreadHandler();

	private:

		std::map<boost::asio::ip::address, IPProxy> addrs;

		boost::thread_group grp;

		boost::mutex mtx;

	public:

		int bandwidth[2]; // 0: send_bandwidth   1:recv_bandwidth
	};

	extern IPProxies ip_proxies;
}
