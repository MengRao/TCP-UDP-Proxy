#pragma once

#include "xassert.h"
#include "xttime.h"
#include <boost/thread.hpp>

namespace global
{

	class xrecursive_mutex
	{
	public:

		enum {NOCARE_PRIMARY_LEVEL = -1};

		xrecursive_mutex(int pri_level_ = NOCARE_PRIMARY_LEVEL)
			: recursion_count(0)
			, pri_level(pri_level_)
		{
			XASSERT(pri_level >= NOCARE_PRIMARY_LEVEL);
		}

		~xrecursive_mutex()
		{
		}		

		//	lock of RAII style
		class scoped_lock
		{
		public:

			scoped_lock(xrecursive_mutex& mtx_)
				: mtx(mtx_)
			{
				mtx.lock();
			}

			~scoped_lock()
			{
				mtx.unlock();
			}

		private:
			
			xrecursive_mutex& mtx;
		};

		friend class scoped_lock;
		friend class condition_variable_recursive_mutex;
		friend class boost::condition_variable_any;
#ifdef WIN32
		friend class boost::detail::basic_condition_variable;	
#else
		friend class boost::thread_cv_detail::lock_on_exit<xrecursive_mutex>;
#endif

	private:

		int recursion_count;
		int pri_level;
		boost::recursive_mutex mutex;

		//	serious warning: you must only call this func in the thread already hold permission of this mutex!
		int get_recursion_count()
		{			
			return recursion_count;
		}

		void lock();

		//	this is a very critical warning : 
		//	you must only call this func in the thread already hold permission of this mutex!
		void unlock();

	};

	//	notice: if use condition_variable_recursive_mutex on xrecursive_mutex which (pri_level != -1),
	//	then this xrecursive_mutex must have biggest pri_level among all xrecursive_mutexs hold by current thread!
	class condition_variable_recursive_mutex
	{
	public:

		condition_variable_recursive_mutex()
		{
		}
		
		~condition_variable_recursive_mutex()
		{
		}

		void wait(xrecursive_mutex& mtx);

		//false if the call is returning because the time period specified by duration has elapsed, true otherwise. 
		bool timed_wait(xrecursive_mutex& mtx, int duration);

		void notify_one();

		void notify_all();		

	private:

		boost::condition_variable_any cond;

		void check(xrecursive_mutex& mtx);

	};

	

}

