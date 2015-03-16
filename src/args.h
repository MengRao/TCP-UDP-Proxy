#pragma once

#include "type.h"
#include "ServerArgsBase.h"
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <boost/asio.hpp>
#include "xpath.h"
#include "trackerr.h"
#include "watch.h"

namespace proxy_ns
{
	
	class ServerArgs
		: public global::ServerArgsBase
	{
	public:

		// number of threads that serve tcp and udp
		uint32 network_thread_num;
		
		// tcp socket connect or read timeout(ms)
		int tcp_timeout;
		
		// udp read timeout(ms)
		int udp_timeout;
		
		// proxy config file name in dir: var/conf/
		std::string proxy_config_file;
		
		// admin tcp port
		uint16 admin_port;
		
		// admin passwd
		std::string admin_passwd;
		
		ServerArgs();
		
	protected:

		virtual boost::program_options::options_description getExtraDesc();
		
		virtual void checkAfterInit();
		
	};

	extern ServerArgs args;
	
}



