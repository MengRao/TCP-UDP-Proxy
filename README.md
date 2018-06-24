# TCP-UDP-Proxy
A proxy server program written in C++ that can forward TCP or UDP packets to and from some pre-configured address. It's a transparent proxy in that remote clients only have to change their destination IP/PORT into those of the proxy, and the proxy will automatically forword packets according to pre-configured rules.

A typical usage scenario is to bypass a company firewall that blocks packets from some limited internal IP/Subnets.

It can also be used for simulating public network environment in an intranet by configuring bandwidth limit, network delay, or packet drop rate(only for UDP).

[Install]
Boost Library is required. Please look into src/Makefile to find out how Boost libs are linked, and you can change the path according to your environment.

On Linux:
$cd src
$make

On Windows:
Becasue the project is cross-platform, it can be compiled as a Visual Studio C++ project. What you have to do is to create a project and import all files under src dir, also remember to link those Boost libs listed in Makefile.

[Configuration and Usage]
Refer to the comments in proxy.conf to see how to configure the forwarding rules.
A typical proxy.conf looks like:

tcp 3443 180.166.47.38 3443 
tcp 1234 180.166.47.38 1234 100000
udp 6500 10.35.60.1 6500 100000 100 1000

The 1st line shows the proxy will open local tcp port 3443 to receive new connections which are to be forwarded directly to  and from remote address 180.166.47.38/3443.

The 2nd line does similar functions, the difference is it also imposes a bandwidth limit of 100K/S in both directions.

The 3rd line is a rule to forward UDP packets, from local port 6500 to remote address 10.35.60.1/6500, and also in reverse direction. Additionally, it indicates a bandwidth limit of 100K/S, a delay of 100ms, and a packet drop rate of 10%, in both directions.


