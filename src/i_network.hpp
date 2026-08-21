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
#include "netdb.h"
#include <sys/stat.h>
#include <filesystem>
#include <fstream>

namespace mtj_ping {
	class i_network {
		private:
			const string _net_devices_dir = "/sys/class/net/";
			const string _state_file_name = "operstate";
		public:
			node ping(string address, ostream& out);
			vector<node> trace(string address, ostream& out);
			i_network();
	};
}
#endif // !I_NETWORK_MTJ_PING_HPP
