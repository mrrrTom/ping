#ifndef I_NETWORK_MTJ_PING_HPP
#define I_NETWORK_MTJ_PING_HPP
#include <string>
#include <iostream>
#include <vector>
#include "node.hpp"
namespace mtj_ping {
	class i_network {
		public:
			node ping(string address, ostream& out);
			vector<node> trace(string address, ostream& out);
	};
}
#endif // !I_NETWORK_MTJ_PING_HPP
