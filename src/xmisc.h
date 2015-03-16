#pragma once

#include "type.h"

#ifdef WIN32
#	include <process.h>
#	include <sys/stat.h>
#	include <boost/asio.hpp>
#else
#	include <iostream>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/ioctl.h>
#	include <sys/socket.h>
#	include <net/if.h>
#	include <netinet/in.h>
#	include <net/if_arp.h>
#	include <arpa/inet.h>

#	define MAXINTERFACES   16

#endif

#include "xttime.h"
#include <stdio.h>

#include <string>
#include <cstdlib>

namespace global
{
#ifdef WIN32
	typedef int pid_t;
#endif

	inline pid_t getpid()
	{
#ifdef WIN32
		return ::_getpid();
#else
		return ::getpid();
#endif
	}

	inline int64 get_thread_id()
	{
#ifdef WIN32
		return (int64)GetCurrentThreadId();
#else
		return (int64)pthread_self();
#endif
	}

#ifdef WIN32
	typedef struct ::_stat file_stat;
#else
	typedef struct ::stat file_stat;
#endif

	inline bool get_file_stat(std::string const& path, file_stat& buf)
	{
		int ret;
#ifdef WIN32
		ret = ::_stat(path.c_str(), &buf);
#else
		ret = ::stat(path.c_str(), &buf);
#endif
		return ret == 0;
	}

	inline sec_t get_file_modify_time(std::string const& path)
	{
		file_stat buf;
		bool ret = get_file_stat(path, buf);
		if (ret)
			return buf.st_mtime;
		else
			return 0;
	}

	inline int64 get_file_size(std::string const& path)
	{
		file_stat buf;
		bool ret = get_file_stat(path, buf);
		if (ret)
			return buf.st_size;
		else
			return 0;
	}

	inline sec_t get_file_access_time(std::string const& path)
	{
		file_stat buf;
		bool ret = get_file_stat(path, buf);
		if (ret)
			return buf.st_atime;
		else
			return 0;
	}

	inline sec_t get_file_state_change_time(std::string const& path)
	{
		file_stat buf;
		bool ret = get_file_stat(path, buf);
		if (ret)
			return buf.st_ctime;
		else
			return 0;
	}

	inline bool remove_file(std::string const& filename)
	{
		int ret = ::remove(filename.c_str());
		if (ret == 0)
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	extern bool _global_is_little_endian_flag;
	inline bool is_little_endian()
	{
		return _global_is_little_endian_flag;
	}
	
	inline void get_local_ip_strs(OUT std::vector<std::string>& ips)
	{
#ifdef WIN32
		boost::asio::io_service io_service;
		boost::asio::ip::tcp::resolver resolver(io_service);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
		while(iterator != boost::asio::ip::tcp::resolver::iterator())
		{
			boost::asio::ip::tcp::endpoint endpoint = *iterator;
			ips.push_back(endpoint.address().to_string());
			++iterator;	
		}
#else
		int fd, intrface;
		struct ifreq buf[MAXINTERFACES];
		struct ifconf ifc;

		if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
		{
			return;
		}

		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl (fd, SIOCGIFCONF, (char *) &ifc))
		{
			intrface = ifc.ifc_len / sizeof (struct ifreq);
			while (intrface-- > 0)
			{
				/*Get IP of the net card */
				if (!(ioctl (fd, SIOCGIFADDR, (char *) &buf[intrface])))
				{
					std::string strIp = inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr);
					if (strIp != "127.0.0.1")
					{
						ips.push_back(strIp);
					}
				}

			}
		}
		close (fd);
#endif
	}



}
