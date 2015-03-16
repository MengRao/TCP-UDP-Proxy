#pragma once
#include "common.h"
#include "buffer.h"
#include "ip_proxy.h"
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <map>
#include <queue>

namespace proxy_ns
{
	struct UdpMsg
	{
		global::Buffer msg;
		global::msec_t time;
		boost::asio::ip::udp::endpoint src;

		UdpMsg()
			: msg(UDP_BUFFER_SIZE)
			, time(0)
			, src()
		{

		}
	};

	class UdpProxyServer;

	class UdpEndpointProxy
	{
	public:
		UdpEndpointProxy(boost::asio::io_service& io_service, UdpProxyRule proxy_rule_);

		~UdpEndpointProxy();

	private:
		friend class UdpProxyServer;

		void init();

		void asyncRecv();

		void handleRecv(const boost::system::error_code& error, std::size_t bytes_transferred);

		void stop();

	private:
		UdpProxyRule proxy_rule;
		boost::asio::ip::udp::socket proxy_socket;
		UdpMsg recving_msg;
		bool server_flag; // 服务器端代理，不超时，不限速
		global::msec_t active_tm;
		std::queue<UdpMsg> recv_queue;
		std::queue<UdpMsg> send_queue;
		int recv_avail_bytes;
		int send_avail_bytes;
		bool recving_flag;
		IPProxy *client_ip;
		boost::mutex mtx;
	};

	typedef boost::shared_ptr<UdpEndpointProxy> UdpEPTProxy;

	class UdpProxyServer
	{
	public:
		UdpProxyServer();

		void start();

		void stop();

		void addServerProxy(boost::asio::io_service& io_service, UdpProxyRule proxy_rule);

		void getInfo(std::ostream &os);

	private:

		void routineThreadHandler();

	private:

		typedef std::map<boost::asio::ip::udp::endpoint, UdpEPTProxy> Endpoints;
		Endpoints endpoints;

		boost::thread_group grp;
	};
}
