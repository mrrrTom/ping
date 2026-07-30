#include "ui.hpp"
#include "command_type.hpp"
using namespace mtj_ping;
namespace {
	command_type convert(string src) {
		if(src == "ping") return command_type::ping;
		if(src == "trace") return command_type::trace;
		throw runtime_error("Bad command type convertion" + src);
	}
}
namespace mtj_ping {
	command ui::get_next(void) {
		ostream& out = this -> out_;
		out << "what to do (ping <address> or trace <address>)?" << endl;
		string c_type;
		cin >> c_type;
		string address;
		cin >> address;
		command_type ct = convert(c_type);
		command cmd{ct, address};
		return cmd;
	}
}
