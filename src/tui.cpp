#include "ui.hpp"
#include "command_type.hpp"
using namespace mtj_ping;
namespace {
	command_type convert(string src) {
		if(src == "ping") return command_type::ping;
		if(src == "trace") return command_type::trace;
		if(src == "quit") return command_type::quit;
		throw runtime_error("Bad command type convertion" + src);
	}
}
namespace mtj_ping {
	command ui::get_next(void) {
		ostream& out = this -> out_;
		out << "what to do (ping <address> or trace <address>)?" << endl;
		string c_type;
		cin >> c_type;
		command_type ct = convert(c_type);
		string address;
		if (ct != command_type::quit) cin >> address;
		command cmd{ct, address};
		return cmd;
	}
}
