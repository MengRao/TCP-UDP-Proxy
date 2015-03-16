#pragma once

#include "type.h"
#include "xassert.h"

#include <ctime>
#include <map>
#include <sys/timeb.h>
#include <boost/format.hpp>
#include <boost/thread.hpp>

#ifdef WIN32
#	include <Windows.h>
#else
#	include <unistd.h>
#endif

namespace global
{
	typedef int64 usec_t;
	typedef int64 msec_t;
	typedef int64 sec_t;

	typedef sec_t timestamp_t;

	struct currenttime
	{
		enum TIMESTYLE
		{
			TIMESTYLE_FORMAL,
			TIMESTYLE_FILENAME,
			TIMESTYLE_DATE,
		};

		static usec_t getus()
		{
			return global::currenttime::getms() * 1000;
		}

		static msec_t getms()
		{
			timeb tb;
			ftime(&tb);
			return msec_t(tb.time) * 1000 + msec_t(tb.millitm);
		}

		static sec_t getsec()
		{
			return std::time(0);
		}

		static std::string getCurrentTimeStr(const char* format)//	效率不是很高
		{
			tm t;
			if (!getLocalTime(&t))
			{
				return 0;
			}
			char tmp[64];
			strftime(tmp, sizeof(tmp), format, &t);
			return std::string(tmp);
		}

		static int getLocalTimeFieldValue(const char* format)//	效率不是很高
		{
			tm t;
			if (!getLocalTime(&t))
			{
				return 0;
			}
			char tmp[64];
			strftime(tmp, sizeof(tmp), format, &t);
			return atoi(tmp); 
		}

		static int getDayOfMonth()
		{
			return getLocalTimeFieldValue("%d"); 
		}

		static int getDayOfYear()
		{
			return getLocalTimeFieldValue("%j"); 
		}

		static bool getLocalTime(tm* t)
		{
			time_t now = time(NULL);
#ifdef WIN32
			return localtime_s(t, &now) == 0;
#else
			return localtime_r(&now, t) != NULL;
#endif
		}

		static bool getSpecifyTime(time_t timesec, tm * t)
		{

#ifdef WIN32
			return localtime_s(t, &timesec) == 0;
#else
			return localtime_r(&timesec, t) != NULL;
#endif
		}

		static bool getUTCTime(tm* t)
		{
			timeb tb;
			ftime(&tb);
#ifdef WIN32
			return gmtime_s(t, &tb.time) == 0;
#else
			return gmtime_r(&tb.time, t) != NULL;
#endif
		}

		static std::string getTimeStringBySpecifyTime(int64 timesec, const char* format)
		{
			tm t;
			if (!getSpecifyTime((time_t)timesec, &t))
			{
				return std::string();
			}
			char tmp[64];
			strftime(tmp, sizeof(tmp), format, &t);
			return std::string(tmp);					
		}

		static std::string getTimeStringBySpecifyTime(int64 timesec, TIMESTYLE TimeStyle_=TIMESTYLE_FORMAL)
		{
			static const_char_string TimeStyle_String[] = 
			{
				"%04d-%02d-%02d %02d:%02d:%02d",
				"%04d.%02d.%02d.%02d.%02d.%02d",
				"%04d-%02d-%02d",
			};

			tm t;
			if (!getSpecifyTime((time_t)timesec, &t))
			{
				return std::string();
			}
			else
			{
				if (TimeStyle_ <= TIMESTYLE_FILENAME)
				{
					return boost::str(boost::format(TimeStyle_String[TimeStyle_]) % (t.tm_year + 1900) % (t.tm_mon + 1) % t.tm_mday % t.tm_hour % t.tm_min % t.tm_sec);
				}
				else
				{
					return boost::str(boost::format(TimeStyle_String[TimeStyle_]) % (t.tm_year + 1900) % (t.tm_mon + 1) % t.tm_mday);
				}
			}
		}

		static std::string getLocalTimeString(TIMESTYLE TimeStyle_)
		{
			static const_char_string TimeStyle_String[] = 
			{
				"%04d-%02d-%02d %02d:%02d:%02d",
				"%04d.%02d.%02d.%02d.%02d.%02d",
				"%04d-%02d-%02d",
			};
			tm t;
			if (!getLocalTime(&t))
			{
				//cout << boost::posix_time::second_clock::local_time();
				return std::string();
			}
			else
			{
				if (TimeStyle_ <= TIMESTYLE_FILENAME)
				{
					return boost::str(boost::format(TimeStyle_String[TimeStyle_]) % (t.tm_year + 1900) % (t.tm_mon + 1) % t.tm_mday % t.tm_hour % t.tm_min % t.tm_sec);
				}
				else
				{
					return boost::str(boost::format(TimeStyle_String[TimeStyle_]) % (t.tm_year + 1900) % (t.tm_mon + 1) % t.tm_mday);
				}
			}
		}
	};

	inline bool is_launchtime(msec_t& TS_CHECKOUT, msec_t const interval, msec_t const TSNOW)
	{
		msec_t const TS_LAST(TS_CHECKOUT - interval);
		msec_t const TS_NEXT(TS_CHECKOUT + interval);

		if (TSNOW > TS_NEXT)
		{
			TS_CHECKOUT = TSNOW + interval;
			return true;
		}
		else if (TSNOW > TS_CHECKOUT)
		{
			TS_CHECKOUT = TS_NEXT;
			return true;
		}
		else if (TSNOW > TS_LAST)
		{
			return false;
		}
		else
		{
			TS_CHECKOUT = TSNOW + interval;
			return true;
		}

	}

	inline void sleepms(msec_t ms)
	{
#ifdef WIN32
		::Sleep( uint32(ms) );
#else
		::usleep(ms * 1000);
#endif
	}

	inline void sleepus(usec_t us)
	{
#ifdef WIN32
		::Sleep( uint32((us + 500) / 1000) );
#else
		::usleep(us);
#endif
	}
	
	usec_t const usec_err = ~0;
	msec_t const msec_err = ~0;
	sec_t const sec_err = ~0;
	timestamp_t const timestamp_err = ~0;

	class interval_checker
	{
	public:
		interval_checker()
			: interval(0)
			, last_time_ms(0)
			
		{
		}

		interval_checker(global::msec_t interval_, bool available_at_once_falg)
			: interval(interval_)
			, last_time_ms(0)
			
		{
			if (!available_at_once_falg)
			{
				last_time_ms = currenttime::getms();
			}
		}

		bool check_and_update(global::msec_t check_time)
		{
			if (check_time - last_time_ms >= interval)
			{
				last_time_ms = check_time;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool check_and_update_now()
		{
			return check_and_update(currenttime::getms());
		}

	private:
		global::msec_t interval;
		global::msec_t last_time_ms;

	};

	template <class key_t>
	class interval_manager
	{
	public:

		interval_manager()
		{
		}

		void add_checker(key_t key, global::msec_t interval, bool available_at_once_falg = false)
		{
			boost::mutex::scoped_lock lock(mtx);
			if (interval_checkers.find(key) == interval_checkers.end())
			{
				interval_checkers[key] = interval_checker(interval, available_at_once_falg);
			}
		}

		bool check_and_update(key_t key, global::msec_t check_time)
		{
			boost::mutex::scoped_lock lock(mtx);
			XASSERT(interval_checkers.find(key) != interval_checkers.end());
			return interval_checkers[key].check_and_update(check_time);		
		}

		bool check_and_update_now(key_t key)
		{
			boost::mutex::scoped_lock lock(mtx);
			XASSERT(interval_checkers.find(key) != interval_checkers.end());
			return interval_checkers[key].check_and_update_now();	
		}

		void remove_checker(key_t key)
		{
			boost::mutex::scoped_lock lock(mtx);
			interval_checkers.remove(key);
		}		

	private:

		std::map<key_t, interval_checker> interval_checkers;
		boost::mutex mtx;
	};

}
