#include "i_network.hpp"
#include "ui.hpp"
#include "node.hpp"
#include "command_type.hpp"
#include <iostream>
#include <string>
#include "command.hpp"

using namespace mtj_ping;
using namespace std;


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
				node nd = nw.ping(cmd.arg, ui.out_);
				if (nd.get_status() == node_status::active) {
					cout << cmd.arg << "success " << nd.get_last_changed() << endl;
				}

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
