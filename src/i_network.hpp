#ifndef I_NETWORK_MTJ_PING_HPP
#define I_NETWORK_MTJ_PING_HPP
#include <iostream>
#include <string>
#include "sys/socket.h"
#include "netinet/in.h"
#include <arpa/inet.h>
#include "netinet/ip_icmp.h"
#include "netinet/ip.h"
#include <unistd.h> // getpid
#include <cstring>
#include "node.hpp"
#include <vector>

namespace mtj_ping {
	class i_network {
		public:
			node ping(string address, ostream& out);
			vector<node> trace(string address, ostream& out);
	};
}
#endif // !I_NETWORK_MTJ_PING_HPP
