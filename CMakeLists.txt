cmake_minimum_required(VERSION 4.0.0)
project(ping
	VERSION 1.0
	DESCRIPTION "Classic ping app and smth more"
	LANGUAGES CXX)
add_executable(ping
	src/command.hpp
	src/command_type.hpp
	src/ui.hpp
	src/tui.cpp
	src/node.hpp
	src/i_network.hpp
	src/i_network.cpp
	src/main.cpp)
target_compile_features(ping PRIVATE cxx_std_20)
target_compile_options(ping PRIVATE -Wall -Wextra -Wpedantic)
