#ifndef COMMAND_MTJ_PING_HPP
#define COMMAND_MTJ_PING_HPP
#include "command_type.hpp"
#include <string>
using namespace std;

namespace mtj_ping {
	struct command {
	public:
		command_type type;
		string arg;
		command() = delete;
		command(command_type type_, string arg_) : type{type_}, arg{arg_} {};
	};
}

#endif
