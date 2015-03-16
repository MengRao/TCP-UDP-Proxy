#pragma once
#include "type.h"
#include "xttime.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

namespace proxy_ns
{

enum{UDP_BUFFER_SIZE = 1500, TCP_BUFFER_SIZE = 4096};
enum{MAX_UDP_MSG_QUEUE_SIZE = 50, MAX_TCP_MSG_QUEUE_SIZE = 1};
enum {UDP_PACKET_EXTRA_OVERHEAD = 42};	//	8 + 20 + 14
enum {TCP_PACKET_EXTRA_OVERHEAD = 54};	//	20 + 20 + 14

typedef boost::shared_ptr<boost::asio::ip::udp::socket> UdpSocket;
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> TcpSocket;

struct TcpProxyRule
{
	uint16 src_port;
	boost::asio::ip::tcp::endpoint dst_endpoint;
	int max_bandwidth; // Bps (1Bps = 8bps)
	int delay_ms;

	TcpProxyRule(uint16 src_port_, const boost::asio::ip::tcp::endpoint& dst_enpoint_)
		: src_port(src_port_)
		, dst_endpoint(dst_enpoint_)
		, max_bandwidth(0)
		, delay_ms(0)
	{

	}
};

struct UdpProxyRule
{
	uint16 src_port;
	boost::asio::ip::udp::endpoint dst_endpoint;
	int max_bandwidth; // Bps (1Bps = 8bps)
	int delay_ms;
	int drop_prob;		// 0(= 0% drop) - 10000 (= 100% drop) , 100 = 1% drop

	UdpProxyRule(uint16 src_port_, const boost::asio::ip::udp::endpoint& dst_endpoint_)
		: src_port(src_port_)
		, dst_endpoint(dst_endpoint_)
		, max_bandwidth(0)
		, delay_ms(0)
		, drop_prob(0)
	{
	}

};



}


