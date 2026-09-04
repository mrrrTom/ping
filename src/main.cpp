#include "i_network.hpp"
#include "ui.hpp"
#include "node.hpp"
#include "command_type.hpp"
#include <iostream>
#include <string>
#include "command.hpp"
#include <sstream>
using namespace mtj_ping;
using namespace std;
namespace {
	void ping(command& cmd, ui ui, i_network nw) {
		node nd = nw.ping(cmd.arg, ui.out_);
		if (nd.get_status() == node_status::active) {
			time_t time_stamp = nd.get_last_changed();
			ui.out_<< "> ping " << cmd.arg << " succeded "
				<< ctime(&time_stamp) << endl;
		}
		else if (nd.get_status() == node_status::failed) {
			time_t time_stamp = nd.get_last_changed();
			ui.out_ << "> pinging " << cmd.arg << " failed "
				<< ctime(&time_stamp) << endl;
		}
		else if (nd.get_status() == node_status::unknown) {
			ui.out_ << "> could not send a ping msg" << endl;
		}
	}

	void trace(command&cmd, ui ui, i_network nw) {
		string debug_info;
		stringstream ss{debug_info};
		vector<node> response = nw.trace(cmd.arg, ss);
		if (response.size() == 0)
			ui.out_ << "> trace failed, no node found" << endl;
		for (int i = 0; i < response.size(); ++i) {
			node nd = response[i];
			time_t time_stamp = nd.get_last_changed();
			if (nd.get_status() == node_status::active) {
				ui.out_ << "> responded " << nd.get_addr() << " at "
				<< ctime(&time_stamp) << endl;
			}
			else {
				ui.out_ << "> ************** " << nd.get_addr() << " at "
					<< ctime(&time_stamp) << endl;
			}
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
				return 0;
			case command_type::ping: {
				ping(cmd, ui, nw);
				break;
			}
			case command_type::trace:
				trace(cmd, ui, nw);
				break;
			default:
				ui.out_
				<< "> resolving command went wrong" << endl;
				return 1;
				break;
		}
	}

	return 0;
}
