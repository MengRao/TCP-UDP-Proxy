#pragma once

#include "type.h"
#include "xthread.h"
#include "xassert.h"
#include "watch.h"

#ifndef WIN32
#	include <unistd.h>
#	include <signal.h>
#	include <sys/param.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/wait.h>
#	include <fcntl.h>
#	include <pthread.h>
#	include <sys/mman.h>
#endif

#include <iostream>

namespace global
{
	extern xrecursive_mutex exit_mtx;
	extern condition_variable_recursive_mutex exit_condition;
	extern bool* global_running_flag;

	//	返回 true 说明 guardian 已经启动，false 说明 guardian 启动失败，guardian 自己不返回
	inline bool guardian(int child_cnt = 1)	
	{
#ifdef WIN32
		return true;
#else
		XASSERT(child_cnt > 0);
		int now_child_cnt = 0;
		int times_left = 10;
		global_running_flag = (bool*)mmap(NULL, sizeof(bool), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
		if (global_running_flag == NULL)
		{
			return false;
		}
		*global_running_flag = true;
		while (*global_running_flag)
		{
			if (now_child_cnt < child_cnt)
			{
				switch (fork()) 
				{
				case -1:
					return false;
				case 0:	
					//	child process				
					return true;
				default:
					//	guardian process
					++ now_child_cnt;								
				}
			}
			else
			{				
				int status;
				if (wait(&status) == -1)
				{
					return false;
				}
				else
				{
					if (--times_left == 0)
					{
						return false;
					}
					-- now_child_cnt;
				}
			}
		}
		if (*global_running_flag == false)
		{
			exit(0);
		}

		return true;
#endif
	}

#ifndef WIN32
	void terminate_handler(int signum);
#endif

	inline bool daemonize()
	try
#ifdef WIN32
	{
		return true;
	}
#else
	{
		//屏蔽一些有关控制终端操作的信号
		signal(SIGTTOU, SIG_IGN);
		signal(SIGTTIN, SIG_IGN);
		signal(SIGTSTP, SIG_IGN);
		signal(SIGHUP , SIG_IGN);
		signal(SIGPIPE, SIG_IGN);
		signal(SIGTERM, &terminate_handler);

		//派生一个新的进程
		int pid = fork();
		if(pid != 0)		//结束父进程
		{
			exit(0);
		}
		else if(pid < 0)	//派生子进程失败
		{
			//记录日志：
			//ERROR:fork a child process failed!
			exit(1);
		}

		//这个被派生的进程成为会话组长和进程组长
		if(setsid() < 0)
		{
			exit(1);
		}

		pid = fork();
		if(pid != 0)		//结束该进程
		{
			exit(0);
		}
		else if(pid < 0)	//派生子进程失败
		{
			//记录日志：
			//ERROR:fork a child process failed!
			exit(1);
		}

		for (int fd = 0, fd_num = 3; fd < fd_num; ++ fd)	//关闭打开的文件描述符
		{
			close(fd);
		}

		open("/dev/null", O_RDWR);
		open("/dev/null", O_RDWR);
		open("/dev/null", O_RDWR);

		//重设文件创建掩模
		//	umask(0);
		return true;
	}
#endif
	catch (...)
	{
		LWarning("fail to daemonize");
		return false;
	}


	void terminate();

}
