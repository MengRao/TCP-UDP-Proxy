#include "xdaemon.h"

namespace global
{
	xrecursive_mutex exit_mtx;
	condition_variable_recursive_mutex exit_condition;
	bool* global_running_flag = NULL;

	void terminate_handler(int signum)
	{
		terminate();
	}

	void terminate()
	{
		if (global_running_flag != NULL)
		{
			*global_running_flag = false;
		}
		exit_condition.notify_all();
	}


}
