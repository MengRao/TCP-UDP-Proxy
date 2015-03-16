#include "watch.h"
#include "trackerr.h"
#include "xfstream.h"

#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <iterator>
#include <map>

namespace global
{	

	namespace watch
	{		

		static std::map<std::string, loglevel_t> mapLogLevel = boost::assign::map_list_of
			(std::string("EMG"), LOG_EMERG)
			(std::string("ALT"), LOG_ALERT)
			(std::string("CRT"), LOG_CRIT)
			(std::string("ERR"), LOG_ERR)
			(std::string("WRN"), LOG_WARNING)
			(std::string("NTC"), LOG_NOTICE)
			(std::string("INF"), LOG_INFO)
			(std::string("DBG"), LOG_DEBUG);

		static const_char_string LOGLEVEL_NAME[9] =
		{
			"EMG",		//	0: /* system is unusable */
			"ALT",		//	1: /* action must be taken immediately */
			"CRT",		//	2: /* critical conditions */
			"ERR",		//	3: /* error conditions */
			"WRN",		//	4: /* warning conditions */
			"NTC",		//	5: /* normal but significant condition */
			"INF",		//	6: /* informational */
			"DBG",		//	7: /* debug-level messages */
			"BADVALUE",	//	¥ÌŒÛ÷µ
		};

		std::string logpath;
		loglevel_t maxloglevel = LOG_WARNING;
		static bool loglocation = false;
		static boost::recursive_mutex mtx;

		using namespace std;

		void fwriteln(std::string const& strPath_, std::string const& strMessage_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);

			ofstream fout(strPath_.c_str(), std::ios_base::out | std::ios_base::app );
			if (fout)
				fout << strMessage_ << std::endl;
			else
				std::cerr << strMessage_ << std::endl;
		}

		std::string dict_loglevelname(loglevel_t theLogLevel_)
		{
			if (0 <= theLogLevel_ && theLogLevel_ <= 7)
			{
				return LOGLEVEL_NAME[int(theLogLevel_)];
			}
			else
			{
				return LOGLEVEL_NAME[8];
			}
		}

		loglevel_t dict_loglevel(std::string const& theName_)
		{
			std::map<std::string, loglevel_t>::iterator it = mapLogLevel.find(theName_);

			if (it != mapLogLevel.end())
			{
				return it->second;
			}
			else
			{
				return LOG_BADVALUE;
			}
		}

		void reset(std::string const& logpath_, loglevel_t maxloglevel_, bool loglocation_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			std::string tmp = logpath_;
			boost::replace_all(tmp, "\\", "/");
			boost::filesystem::path path = boost::filesystem::system_complete(boost::filesystem::path(tmp));
#ifdef WIN32
			std::locale::global(std::locale(""));
			std::locale::global(std::locale("C", LC_NUMERIC));
#endif
			logpath = path.string();
			maxloglevel = maxloglevel_;
			loglocation = loglocation_;
		}

		void reset_logpath(std::string const& logpath_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			logpath = logpath_;
		}

		void reset_maxloglevel(loglevel_t maxloglevel_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			maxloglevel = maxloglevel_;
		}

		void reset_loglocation(bool loglocation_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			loglocation = loglocation_;
		}

		std::string get_logpath()
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			return logpath;
		}

		loglevel_t get_maxloglevel()
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			return maxloglevel;
		}

		bool get_loglocation()
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			return loglocation;
		}

		void dolog_(std::string const& loglevel_, DEBUG_INFORMATION_ARGS_FUNCDEFINE, std::string const& message_)
		{
			boost::recursive_mutex::scoped_lock lock(mtx);
			loglevel_t lv = dict_loglevel(loglevel_);
			if (lv != LOG_BADVALUE)
			{
				dolog_(lv, DEBUG_INFORMATION_ARGS, message_);
			}
			else
			{
				dolog_(LOG_DEBUG, DEBUG_INFORMATION_ARGS, message_);
			}
		}

		void dolog_(loglevel_t loglevel_, DEBUG_INFORMATION_ARGS_FUNCDEFINE, std::string const& message_)
		{			
			if (loglevel_ <= maxloglevel)
			{
				boost::recursive_mutex::scoped_lock lock(mtx);

				std::string msg;

				msg += dict_loglevelname(loglevel_) + " ";

				msg += currenttime::getLocalTimeString(currenttime::TIMESTYLE_FORMAL) + " ";

				msg += message_ + " ";

				if (loglocation)
					msg += boost::str(boost::format("%s@[%s:%u]") % SRC_PRETTY_FUNCTION_ % SRC_FILE_ % SRC_LINE_);

				fwriteln(logpath.c_str(), msg);

			}
		}

	}

}

