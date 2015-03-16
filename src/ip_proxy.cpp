#include "ip_proxy.h"
#include "type.h"
#include "args.h"

using namespace std;

namespace proxy_ns
{
	IPProxies ip_proxies;

	IPProxies::IPProxies()
	{
		bandwidth[0] = 0;
		bandwidth[1] = 0;
	}

	void IPProxies::start(int send_bandwith_, int recv_bandwidth_)
	{
		bandwidth[0] = send_bandwith_;
		bandwidth[1] = recv_bandwidth_;
		if (bandwidth[0] > 0 || bandwidth[1] > 0)
		{
			grp.create_thread(boost::bind(&IPProxies::routineThreadHandler, this));
		}
	}

	void IPProxies::stop()
	{
		grp.join_all();
	}

	IPProxy *IPProxies::getIPProxy(boost::asio::ip::address ip_addr)
	{
		if (bandwidth[0] == 0 && bandwidth[1] == 0)
		{
			return NULL;
		}
		boost::mutex::scoped_lock lock(mtx);
		IPProxy& proxy = addrs[ip_addr];
		++proxy.ref_cnt;

		return &proxy;
	}

	void IPProxies::routineThreadHandler()
	{
		global::msec_t now = global::currenttime::getms();
		global::msec_t last_flush_tm = now;
		int interval = 0;
		vector<boost::asio::ip::address> delete_proies;

		while (args.running_flag)
		{
			global::sleepms(10);
			now = global::currenttime::getms();
			interval = (int)(now - last_flush_tm);
			if (interval <= 0)
			{
				continue;
			}
			last_flush_tm = now;

			int bytes[2];
			for (int i = 0; i < 2; ++i)
			{
				bytes[i] = interval * bandwidth[i] / 1000;
			}

			delete_proies.clear();
			boost::mutex::scoped_lock lock(mtx);
			for (map<boost::asio::ip::address, IPProxy>::iterator it = addrs.begin(); it != addrs.end(); ++it)
			{
				IPProxy& proxy = it->second;
				boost::mutex::scoped_lock lock2(proxy.mtx);
				if (proxy.ref_cnt <= 0)
				{
					delete_proies.push_back(it->first);
					continue;
				}
				for (int i = 0; i < 2; ++i)
				{
					if (proxy.avail_bytes[i] < 0)
					{
						proxy.avail_bytes[i] += bytes[i];
					}
				}
			}
			for (vector<boost::asio::ip::address>::iterator it = delete_proies.begin(); it != delete_proies.end(); ++it)
			{
				addrs.erase(*it);
			}

		}
	}

}
