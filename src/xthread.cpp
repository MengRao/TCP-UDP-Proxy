#include "xthread.h"
#include "watch.h"
#include <boost/thread/tss.hpp>

namespace global
{
	using namespace std;

	class quick_int_stack
	{
	public:

		quick_int_stack()
			: top_pos(-1)
		{
		}

		bool empty()
		{
			return (top_pos == -1);
		}

		int top()
		{
			return elements[top_pos];
		}

		void push(int i)
		{
			elements[++ top_pos] = i;			
		}

		void pop()
		{
			-- top_pos;
		}

	private:

		enum {MAX_STACK_ELEMENT = 1024};
		int elements[MAX_STACK_ELEMENT];
		int top_pos;
	};

	boost::thread_specific_ptr<global::quick_int_stack> locks_hold_by_current_thread;

	void xrecursive_mutex::lock()
	{
		mutex.lock();
		++ recursion_count;
		if (pri_level != NOCARE_PRIMARY_LEVEL)
		{
			if (recursion_count == 1)
			{
				quick_int_stack* pri_levels = locks_hold_by_current_thread.get();
				if (pri_levels == NULL)
				{
					pri_levels = new quick_int_stack();
					locks_hold_by_current_thread.reset(pri_levels);				
				}				
				if (!pri_levels->empty() && pri_levels->top() >= pri_level)
				{	//	wrong order, lock failed
					-- recursion_count;
					mutex.unlock();
					XASSERT(false);
				}
				pri_levels->push(pri_level);
			}
		}		
	}

	void xrecursive_mutex::unlock()
	{		
		quick_int_stack* pri_levels;
		if (pri_level != NOCARE_PRIMARY_LEVEL && recursion_count == 1 &&
			((pri_levels = locks_hold_by_current_thread.get()) != NULL))
		{
			if (!pri_levels->empty()
				&& (pri_levels->top() == pri_level))
			{				
				pri_levels->pop();
			}	
			else
			{
				stacktrace::call_stack st;
				LErr(st.to_string());
			}
		}
		-- recursion_count;
		mutex.unlock();			
	}

	void condition_variable_recursive_mutex::check(xrecursive_mutex& mtx)
	{
		if (mtx.pri_level != xrecursive_mutex::NOCARE_PRIMARY_LEVEL)
		{
			quick_int_stack* pri_levels = locks_hold_by_current_thread.get();
			XASSERT((pri_levels != NULL) && (!pri_levels->empty()) &&
				(pri_levels->top() == mtx.pri_level));
		}
	}

	void condition_variable_recursive_mutex::wait(xrecursive_mutex& mtx)
	{
		check(mtx);
		int extra_lock = mtx.get_recursion_count() - 1;
		for (int i = 0; i < extra_lock; i++)
		{
			mtx.unlock();
		}
		XASSERT(mtx.get_recursion_count() == 1);		
		cond.wait(mtx);		
		for (int i = 0; i < extra_lock; i++)
		{
			mtx.lock();
		}
	}

	//false if the call is returning because the time period specified by duration has elapsed, true otherwise. 
	bool condition_variable_recursive_mutex::timed_wait(xrecursive_mutex& mtx, int duration)
	{
		check(mtx);
		int extra_lock = mtx.get_recursion_count() - 1;
		bool ret = false;
		for (int i = 0; i < extra_lock; i++)
		{
			mtx.unlock();
		}
		XASSERT(mtx.get_recursion_count() == 1);
		ret = cond.timed_wait(mtx, boost::posix_time::millisec(duration));
		for (int i = 0; i < extra_lock; i++)
		{
			mtx.lock();
		}
		return ret;
	}

	void condition_variable_recursive_mutex::notify_one()
	{
		cond.notify_one();
	}

	void condition_variable_recursive_mutex::notify_all()
	{
		cond.notify_all();
	}


}

