# TCP-UDP-Proxy
A proxy server program written in C++ that can forward TCP or UDP packets to and from some pre-configured address. It's a transparent proxy in that remote clients only have to change their destination IP/PORT into those of the proxy, and the proxy will automatically forword packets according to pre-configured rules.

A typical usage scenario is to bypass a company firewall that blacks packets from some limited internal IP/Subnets.

It can also be used for simulating public network environment in an intranet by configuring bandwidth limit, network delay, or packet drop rate(only for UDP).
