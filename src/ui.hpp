#ifndef TUI_MTJ_PING_HPP
#define TUI_MTJ_PING_HPP
#include <iostream>
#include "command.hpp"

using namespace std;

namespace mtj_ping {
	class ui {
	private:
		// some streams and ui staff
	public:
		ostream& out_;
		command get_next(void);
		ui() = delete;
		ui(ostream& out) : out_{out} {};
	};
}
#endif
