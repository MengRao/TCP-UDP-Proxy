#include "ServerArgsBase.h"
#include "xpath.h"
#include "xdaemon.h"
#include <fstream>
#include <iostream>
#include "watch.h"
#include "xmisc.h"
#include <stdexcept>

using namespace std;

namespace global
{

	ServerInfo& ServerInfo::instance()
	{
		static ServerInfo info;
		return info;
	}

	void ServerInfo::init(const std::string& software_name, const std::string& module_name, const std::string& version_str)
	{
		this->software_name = software_name;
		this->module_name = module_name;
		this->version_str = version_str;
	}


	ServerArgsBase::ServerArgsBase()
	{
		this->help_flag = false;
		this->version_flag = false;

#ifdef WIN32
		this->work_dir = std::string("D:") + PATH_SEPARATOR + std::string("env") + PATH_SEPARATOR + ServerInfo::instance().software_name;
#else
		this->work_dir = PATH_SEPARATOR + std::string("usr") + PATH_SEPARATOR + std::string("local") + PATH_SEPARATOR + ServerInfo::instance().software_name;
#endif

		this->sbin_dir = this->work_dir + PATH_SEPARATOR + std::string("sbin");
		this->bin_dir = this->work_dir + PATH_SEPARATOR + std::string("bin");
		this->etc_dir = this->work_dir + PATH_SEPARATOR + std::string("etc");
		this->var_dir = this->work_dir + PATH_SEPARATOR + std::string("var");

		this->log_dir = this->var_dir + PATH_SEPARATOR + std::string("log");
		this->run_dir = this->var_dir + PATH_SEPARATOR + std::string("run");
		this->spool_dir = this->var_dir + PATH_SEPARATOR + std::string("spool");
		this->tmp_dir = this->var_dir + PATH_SEPARATOR + std::string("tmp");
		this->data_dir = this->var_dir + PATH_SEPARATOR + std::string("data");
		this->conf_dir = this->var_dir + PATH_SEPARATOR + std::string("conf");

		this->config_file = this->etc_dir + PATH_SEPARATOR + ServerInfo::instance().module_name + std::string(".conf");
		this->pid_file = this->run_dir + PATH_SEPARATOR + ServerInfo::instance().module_name + std::string(".pid");
		this->log_file = this->log_dir + PATH_SEPARATOR + ServerInfo::instance().module_name + std::string(".log");
		this->log_level = 6;

		this->daemon_flag = false;
		this->guardian_flag = false;
		this->running_flag = true;
	}

	void ServerArgsBase::init(int argc, char* argv[])
	{
		ServerArgsBase def;
		using namespace boost::program_options;

		boost::program_options::options_description commandDesc("Command Options");
		commandDesc.add_options()
			("help", "help message")
			("version", "version message")
			;
		boost::program_options::options_description generalDesc("Basic Server Options");
		generalDesc.add_options()
			("work-dir", value<std::string>( &this->work_dir )->default_value( def.work_dir ), "")

			("sbin-dir", value<std::string>( &this->sbin_dir )->default_value( def.sbin_dir ), "")
			("bin-dir", value<std::string>( &this->bin_dir )->default_value( def.bin_dir ), "")
			("etc-dir", value<std::string>( &this->etc_dir )->default_value( def.etc_dir ), "")
			("var-dir", value<std::string>( &this->var_dir )->default_value( def.var_dir ), "")

			("log-dir", value<std::string>( &this->log_dir )->default_value( def.log_dir ), "")
			("run-dir", value<std::string>( &this->run_dir )->default_value( def.run_dir ), "")
			("spool-dir", value<std::string>( &this->spool_dir )->default_value( def.spool_dir ), "")
			("tmp-dir", value<std::string>( &this->tmp_dir )->default_value( def.tmp_dir ), "")
			("data-dir", value<std::string>( &this->data_dir )->default_value( def.data_dir ), "")
			("conf-dir", value<std::string>( &this->conf_dir )->default_value( def.conf_dir ), "")

			("config-file", value<std::string>( &this->config_file )->default_value( def.config_file ), "")
			("pid-file", value<std::string>( &this->pid_file )->default_value( def.pid_file ), "")
			("log-file", value<std::string>( &this->log_file )->default_value( def.log_file ), "")

			("log-level", value<uint>( &this->log_level )->default_value( def.log_level ), "")

			("daemon", value<bool>( &this->daemon_flag )->default_value( def.daemon_flag ), "start as daemon")
			("guardian", value<bool>( &this->guardian_flag )->default_value( def.guardian_flag ), "launch guardian")
			;
		commandDesc.add(generalDesc);
		
		boost::program_options::options_description extraDesc("Extra Options");
		extraDesc.add(getExtraDesc());
		if (extraDesc.options().size() > 0)
		{
			commandDesc.add(extraDesc);			
		}

		variables_map vm;
		try
		{
			global::watch::reset(log_file, log_level);
			store(parse_command_line(argc, argv, commandDesc), vm);
			notify(vm);
			if (vm.count("help") > 0)
			{
				std::cout << ServerInfo::instance().version_str << std::endl;
				std::cout << commandDesc << std::endl;
				exit(0);
			}
			if (vm.count("version") > 0)
			{
				std::cout << ServerInfo::instance().version_str << std::endl;
				exit(0);
			}
			std::ifstream ifs(this->config_file.c_str());
			if (ifs)
			{
				parsed_options parsed = parse_config_file(ifs, commandDesc);
				store(parsed, vm);
				notify(vm);
			}
			global::watch::reset(log_file, log_level);
			checkAfterInit();

			ofstream fout(this->pid_file.c_str());
			if (fout)
			{
				fout << getpid() << endl;
			}
			if (this->daemon_flag && !global::daemonize())
			{
				throw std::runtime_error("global::daemonize() failed");
			}

			if (this->guardian_flag && !global::guardian())
			{
				throw std::runtime_error("global::guardian() failed");				
			}
		}
		catch(std::exception& e)
		{
			LErrStr(boost::format("start %s fail, an error occured when parsing arguments: %s") % ServerInfo::instance().module_name % e.what());
			exit(1);
		}
		catch(...)
		{
			LErrStr(boost::format("start %s fail, an error occured when parsing arguments") % ServerInfo::instance().module_name);
			exit(1);
		}
	}

	boost::program_options::options_description ServerArgsBase::getExtraDesc()
	{
		return boost::program_options::options_description();
	}
	
	void ServerArgsBase::checkAfterInit()
	{
	}

}
