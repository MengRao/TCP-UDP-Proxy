#include "args.h"
#include "xassert.h"

namespace proxy_ns
{
	
	using namespace std;
	
	ServerArgs args;
	
	ServerArgs::ServerArgs()
		: global::ServerArgsBase()
	{
		#ifdef DEFAULT_CONFIG_FILE
		config_file = MACRO_STRING(DEFAULT_CONFIG_FILE);
		#endif
		network_thread_num = 4;	//number of threads that serve tcp and udp
		tcp_timeout = 600000;	//tcp socket connect or read timeout(ms)
		udp_timeout = 600000;	//udp read timeout(ms)
		proxy_config_file = "proxy.conf";	//proxy config file name in dir: var/conf/
		admin_port = 6600;	//admin tcp port
		admin_passwd = "123456";	//admin passwd
		
	}

	void ServerArgs::checkAfterInit()
	{
		
		XASSERT( network_thread_num != 0);
	}

	boost::program_options::options_description ServerArgs::getExtraDesc()
	{
		ServerArgs def;
		
		using namespace boost::program_options;
		options_description desc;
		desc.add_options()
		("network_thread_num", value<uint32>( &network_thread_num )->default_value( def.network_thread_num), "number of threads that serve tcp and udp")
		("tcp_timeout", value<int>( &tcp_timeout )->default_value( def.tcp_timeout), "tcp socket connect or read timeout(ms)")
		("udp_timeout", value<int>( &udp_timeout )->default_value( def.udp_timeout), "udp read timeout(ms)")
		("proxy_config_file", value<std::string>( &proxy_config_file )->default_value( def.proxy_config_file), "proxy config file name")
		("admin_port", value<uint16>( &admin_port )->default_value( def.admin_port), "admin tcp port")
		("admin_passwd", value<std::string>( &admin_passwd )->default_value( def.admin_passwd), "admin passwd")
		
		;
		return desc;
	}
}

