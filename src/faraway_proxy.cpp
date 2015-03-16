#include "common.h"
#include "shared_data.h"
#include "io_service_pool.h"
#include "xdaemon.h"
#include "args.h"

#include <boost/algorithm/string.hpp>
#include <list>

using namespace std;

namespace proxy_ns
{

class ProxyServer
{
public:
	ProxyServer()
	{
		sdata.init();
	}

	~ProxyServer()
	{
		LDebug("");
		args.running_flag = false;
		sdata.stop();
	}

	void run()
	{
		args.running_flag = true;
		sdata.start();

		{
			global::xrecursive_mutex::scoped_lock lock(global::exit_mtx);
			global::exit_condition.wait(global::exit_mtx);
		}
		args.running_flag = false;

	}

private:

};

}

int main(int ac, char* av[])
try
{
	using namespace proxy_ns;
	global::ServerInfo::instance().init("proxy", "faraway_proxy", "faraway_proxy 1.0.000");
	args.init(ac, av);	
	
	LNotice("faraway_proxy launched");
	ProxyServer server;
	server.run();
	LNotice("faraway_proxy stoped");
}
catch(global::trackerr& e)
{
	LErrStr(boost::format("faraway_proxy fail, an error occured: %s") % e.what());
	return 1;
}
catch(std::exception& e)
{
	LErrStr(boost::format("faraway_proxy fail, an error occured: %s") % e.what());
	return 1;
}
catch(...)
{
	LErr("faraway_proxy fail, an unknown error occured");
	return 1;
}

