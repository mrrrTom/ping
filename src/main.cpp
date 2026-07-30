#include "i_network.hpp"
#include "ui.hpp"
#include "node.hpp"
#include "command_type.hpp"
#include <iostream>
#include <string>
#include "command.hpp"

using namespace mtj_ping;
using namespace std;
namespace {
	void ping(command& cmd, ui ui, i_network nw) {
		node nd = nw.ping(cmd.arg, ui.out_);
		if (nd.get_status() == node_status::active) {
			time_t time_stamp = nd.get_last_changed();
			ui.out_<< ">ping " << cmd.arg << " succeded "
				<< ctime(&time_stamp) << endl;
		}
	}
}

int main (int argc, char *argv[]) {
	ui ui{cout};
	i_network nw;
	vector<node> nds;
	while(true) {
		command cmd = ui.get_next();
		switch(cmd.type) {
			case command_type::quit:
				break;
			case command_type::ping: {
				ping(cmd, ui, nw);
				break;
			}
			case command_type::trace:
				nds = nw.trace(cmd.arg, ui.out_);
				break;
			default:
				ui.out_ << "smth went wrong" << endl;
				return 1;
				break;
		}
	}

	return 0;
}
