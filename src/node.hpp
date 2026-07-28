#ifndef NODE_MTJ_PING_HPP
#define NODE_MTJ_PING_HPP
#include <string>
#include <ctime>
#include "node_status.hpp"

using namespace std;
namespace mtj_ping {
	struct node {
	private:
		time_t t_stamp;
		string address;
		node_status status;
	public:
		time_t get_last_changed(void) const { return t_stamp; }
		string get_addr(void) const { return address; }
		node_status get_status(void) const { return status; }
		void set_status(node_status status_, time_t ts_) {
			status = status_;
			t_stamp = ts_;
		}

		node() = delete;
		node(string addr_, node_status status_, time_t ts_) :
		t_stamp {ts_}, address {addr_}, status {status_}  {}
	};
}
#endif // !NODE_MTJ_PING_HPP
