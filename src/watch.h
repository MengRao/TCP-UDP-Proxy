#pragma once

#include "type.h"

#include <string>

#include <boost/format.hpp>

#include "xttime.h"

#ifndef WIN32
#	include <sys/syslog.h>
#endif

typedef int loglevel_t;
loglevel_t const LOG_BADVALUE = -1;
#ifdef WIN32
	loglevel_t const LOG_EMERG = 0;		/* system is unusable */
	loglevel_t const LOG_ALERT = 1;		/* action must be taken immediately */
	loglevel_t const LOG_CRIT = 2;		/* critical conditions */
	loglevel_t const LOG_ERR = 3;			/* error conditions */
	loglevel_t const LOG_WARNING = 4;		/* warning conditions */
	loglevel_t const LOG_NOTICE = 5;		/* normal but significant condition */
	loglevel_t const LOG_INFO = 6;		/* informational */
	loglevel_t const LOG_DEBUG = 7;		/* debug-level messages */
#endif

struct LEMG {enum{LEVEL=LOG_EMERG};};	//	0: /* system is unusable */
struct LALT {enum{LEVEL=LOG_ALERT};};	//	1: /* action must be taken immediately */
struct LCRT {enum{LEVEL=LOG_CRIT};};	//	2: /* critical conditions */
struct LERR {enum{LEVEL=LOG_ERR};};		//	3: /* error conditions */
struct LWRN {enum{LEVEL=LOG_WARNING};};	//	4: /* warning conditions */
struct LNTC {enum{LEVEL=LOG_NOTICE};};	//	5: /* normal but significant condition */
struct LINF {enum{LEVEL=LOG_INFO};};	//	6: /* informational */
struct LDBG {enum{LEVEL=LOG_DEBUG};};	//	7: /* debug-level messages */

#define LWatchRaw(loglevel_, DEBUG_INFORMATION_ARGS_USE, logmessage_) global::watch::dolog_(loglevel_, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LEmergRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_EMERG, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LAlertRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_ALERT, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LCritRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_CRIT, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LErrRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_ERR, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LWarningRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)	LWatchRaw(LOG_WARNING, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LNoticeRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_NOTICE, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LInfoRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_INFO, DEBUG_INFORMATION_ARGS_USE, logmessage_)
#define LDebugRaw(DEBUG_INFORMATION_ARGS_USE, logmessage_)		LWatchRaw(LOG_DEBUG, DEBUG_INFORMATION_ARGS_USE, logmessage_)

#define LWatch(loglevel_, logmessage_) LWatchRaw(loglevel_, DEBUG_INFORMATION_ARGS, logmessage_)
#define LEmerg(logmessage_)		if (global::watch::maxloglevel >= LOG_EMERG) LWatch(LOG_EMERG, logmessage_)
#define LAlert(logmessage_)		if (global::watch::maxloglevel >= LOG_ALERT) LWatch(LOG_ALERT, logmessage_)
#define LCrit(logmessage_)		if (global::watch::maxloglevel >= LOG_CRIT) LWatch(LOG_CRIT, logmessage_)
#define LErr(logmessage_)		if (global::watch::maxloglevel >= LOG_ERR) LWatch(LOG_ERR, logmessage_)
#define LWarning(logmessage_)	if (global::watch::maxloglevel >= LOG_WARNING) LWatch(LOG_WARNING, logmessage_)
#define LWarningCMD(cmd_)		LWarning(#cmd_);cmd_;LWarning(#cmd_)
#define LNotice(logmessage_)	if (global::watch::maxloglevel >= LOG_NOTICE) LWatch(LOG_NOTICE, logmessage_)
#define LNoticeCMD(cmd_)		LNotice(#cmd_);cmd_;LNotice(#cmd_)
#define LInfo(logmessage_)		if (global::watch::maxloglevel >= LOG_INFO) LWatch(LOG_INFO, logmessage_)
#define LInfoCMD(cmd_)		LInfo(#cmd_);cmd_;LInfo(#cmd_)
#define LDebug(logmessage_)		if (global::watch::maxloglevel >= LOG_DEBUG) LWatch(LOG_DEBUG, logmessage_)
#define LDebugCMD(cmd_) LDebug(#cmd_);cmd_;LDebug(#cmd_)

#define LWatch0(loglevel_)	LWatch(loglevel_, "")
#define LEmerg0				LEmerg("")
#define LAlert0				LAlert("")
#define LCrit0				LCrit("")
#define LErr0				LErr("")
#define LWarning0			LWarning("")
#define LNotice0			LNotice("")
#define LInfo0				LInfo("")
#define LDebug0				LDebug("")

#define LWatchStr(loglevel_, logmessage_) LWatch(loglevel_, boost::str(logmessage_))
#define LEmergStr(logmessage_)		if (global::watch::maxloglevel >= LOG_EMERG) LWatch(LOG_EMERG, boost::str(logmessage_))
#define LAlertStr(logmessage_)		if (global::watch::maxloglevel >= LOG_ALERT) LWatch(LOG_ALERT, boost::str(logmessage_))
#define LCritStr(logmessage_)		if (global::watch::maxloglevel >= LOG_CRIT) LWatch(LOG_CRIT, boost::str(logmessage_))
#define LErrStr(logmessage_)		if (global::watch::maxloglevel >= LOG_ERR) LWatch(LOG_ERR, boost::str(logmessage_))
#define LWarningStr(logmessage_)	if (global::watch::maxloglevel >= LOG_WARNING) LWatch(LOG_WARNING, boost::str(logmessage_))
#define LNoticeStr(logmessage_)		if (global::watch::maxloglevel >= LOG_NOTICE) LWatch(LOG_NOTICE, boost::str(logmessage_))
#define LInfoStr(logmessage_)		if (global::watch::maxloglevel >= LOG_INFO) LWatch(LOG_INFO, boost::str(logmessage_))
#define LDebugStr(logmessage_)		if (global::watch::maxloglevel >= LOG_DEBUG) LWatch(LOG_DEBUG, boost::str(logmessage_))

#define LErrEx() catch(int ret){\
	LErrStr(boost::format("catch int exception : %d") % ret);}\
	catch(std::exception& e){\
	LErrStr(boost::format("catch std::exception : %s") % e.what());}\
	catch(...){LErr("catch an unknown exception");}
#define LWarningEx() catch(int ret){\
	LWarningStr(boost::format("catch int exception : %d") % ret);}\
	catch(std::exception& e){\
	LWarningStr(boost::format("catch std::exception : %s") % e.what());}\
	catch(...){LWarning("catch an unknown exception");}
#define LNoticeEx() catch(int ret){\
	LNoticeStr(boost::format("catch int exception : %d") % ret);}\
	catch(std::exception& e){\
	LNoticeStr(boost::format("catch std::exception : %s") % e.what());}\
	catch(...){LNotice("catch an unknown exception");}
#define LInfoEx() catch(int ret){\
	LInfoStr(boost::format("catch int exception : %d") % ret);}\
	catch(std::exception& e){\
	LInfoStr(boost::format("catch std::exception : %s") % e.what());}\
	catch(...){LInfo("catch an unknown exception");}
#define LDebugEx() catch(int ret){\
	LDebugStr(boost::format("catch int exception : %d") % ret);}\
	catch(std::exception& e){\
	LDebugStr(boost::format("catch std::exception : %s") % e.what());}\
	catch(...){LDebug("catch an unknown exception");}


namespace global
{

	namespace watch
	{
		extern std::string dict_loglevelname(loglevel_t loglevel_);
		extern loglevel_t dict_loglevel(std::string const& logname_);

		extern void reset(std::string const& logpath_, loglevel_t maxloglevel_ = LOG_WARNING, bool loglocation_ = true);
		extern void reset_logpath(std::string const& logpath_);
		extern void reset_maxloglevel(loglevel_t loglevel_);
		extern void reset_loglocation(bool loglocation_);

		extern std::string get_logpath();
		extern loglevel_t get_maxloglevel();
		extern bool get_loglocation();		
		extern loglevel_t maxloglevel;
		extern std::string logpath;
				
		extern void dolog_(std::string const& loglevel_, DEBUG_INFORMATION_ARGS_FUNCDEFINE, std::string const& message_);
		extern void dolog_(loglevel_t loglevel_, DEBUG_INFORMATION_ARGS_FUNCDEFINE, std::string const& message_);		

	}

	struct LBWatch_wrap_
	{
		LBWatch_wrap_(loglevel_t theLogLevel_, DEBUG_INFORMATION_ARGS_FUNCDEFINE, std::string const& strEnter_, std::string const& strExit_)
			: theLogLevel(theLogLevel_)
			, ts_start(currenttime::getms())
			, strExit(strExit_)
			, DEBUG_INFORMATION_ARGS_INITIAL
		{
			LWatchRaw(theLogLevel, DEBUG_INFORMATION_ARGS_USE, strEnter_);
		}

		~LBWatch_wrap_()
		{
			LWatchRaw(theLogLevel, DEBUG_INFORMATION_ARGS_USE, strExit + boost::str(boost::format("(%llums)") % (currenttime::getms() - ts_start)));
		}

		loglevel_t theLogLevel;
		msec_t const ts_start;
		std::string const strExit;
		DEBUG_INFORMATION_ARGS_DECLARE;
	};

#define LBWatch_(loglevel_, DEBUG_INFORMATION_ARGS_FUNCUSE, strEnter_, strExit_) global::LBWatch_wrap_ theLBWatch_wrap_##SRC_LINE_(loglevel_, DEBUG_INFORMATION_ARGS_FUNCUSE, strEnter_, strExit_);

#define LBWatch(loglevel_, strEnter_, strExit_)	LBWatch_(loglevel_, DEBUG_INFORMATION_ARGS, strEnter_, strExit_)
#define LBEmerg(strEnter_, strExit_)	LBWatch(LOG_EMERG, strEnter_, strExit_)
#define LBAlert(strEnter_, strExit_)	LBWatch(LOG_ALERT, strEnter_, strExit_)
#define LBCrit(strEnter_, strExit_)		LBWatch(LOG_CRIT, strEnter_, strExit_)
#define LBErr(strEnter_, strExit_)		LBWatch(LOG_ERR, strEnter_, strExit_)
#define LBWarning(strEnter_, strExit_)	LBWatch(LOG_WARNING, strEnter_, strExit_)
#define LBInfo(strEnter_, strExit_)		LBWatch(LOG_INFO, strEnter_, strExit_)
#define LBDebug(strEnter_, strExit_)	LBWatch(LOG_DEBUG, strEnter_, strExit_)


#define LBWatch_FunctionInfo(loglevel_)	LBWatch(loglevel_, boost::str(boost::format("%s enter") % __PRETTY_FUNCTION__), boost::str(boost::format("%s leave") % __PRETTY_FUNCTION__))
#define LBEmerg_FunctionInfo()		LBWatch_FunctionInfo(LOG_EMERG)
#define LBAlert_FunctionInfo()		LBWatch_FunctionInfo(LOG_ALERT)
#define LBCrit_FunctionInfo()		LBWatch_FunctionInfo(LOG_CRIT)
#define LBErr_FunctionInfo()		LBWatch_FunctionInfo(LOG_ERR)
#define LBWarning_FunctionInfo()	LBWatch_FunctionInfo(LOG_WARNING)
#define LBInfo_FunctionInfo()		LBWatch_FunctionInfo(LOG_INFO)
#define LBDebug_FunctionInfo()		LBWatch_FunctionInfo(LOG_DEBUG)

}

