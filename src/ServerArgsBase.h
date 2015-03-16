#pragma once

#include "type.h"
#include <string>
#include <boost/program_options.hpp>

namespace global
{
	class ServerInfo
	{
	public:	

		std::string software_name;
		std::string module_name;
		std::string version_str;
		
		static ServerInfo& instance();		
		void init(const std::string& software_name, const std::string& module_name, const std::string& version_str);

	private:

		ServerInfo(){}
	};

	class ServerArgsBase
	{
	public:

		bool help_flag;
		bool version_flag;

		std::string work_dir;

		std::string sbin_dir;
		std::string bin_dir;
		std::string etc_dir;
		std::string var_dir;

		std::string log_dir;		
		std::string run_dir;
		std::string spool_dir;
		std::string tmp_dir;
		std::string data_dir;
		std::string conf_dir;

		std::string config_file;
		std::string pid_file;
		std::string log_file;
		uint log_level;

		bool daemon_flag;
		bool guardian_flag;
		bool running_flag;

		ServerArgsBase();
		virtual ~ServerArgsBase(){}
		void init(int argc, char* argv[]);

	protected:
		virtual boost::program_options::options_description getExtraDesc();
		virtual void checkAfterInit();		
	}; 

}

