#include "i_network.hpp"
using namespace  std;
using namespace mtj_ping;
//     0                   1                   2                   3
//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |     Type      |     Code      |          Checksum             |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |                             unused                            |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//   |      Internet Header + 64 bits of Original Data Datagram      |
//   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
namespace {
	constexpr int icmp_echo_request_code = 0;
	constexpr int ipv4_max_header_size = 60;
	constexpr int icmp_max_header_size = 8;
	constexpr int icmp_ip_data_copy_size = 8;
	constexpr int icmpv4_max_packet_size = (ipv4_max_header_size
			+ icmp_max_header_size
			+ ipv4_max_header_size
			+ icmp_ip_data_copy_size); // https://www.rfc-editor.org/info/rfc792/
	constexpr int socket_timeout_sec = 1;

	// The checksum is the 16-bit ones's complement of the one's
      	// complement sum of the ICMP message starting with the ICMP Type.
      	// For computing the checksum , the checksum field should be zero.
      	// This checksum may be replaced in the future. (c)
	// https://www.rfc-editor.org/info/rfc792/
	uint16_t checksum16(const void* data, size_t len) {
    		const uint16_t* buf = static_cast<const uint16_t*>(data);
    		uint32_t sum = 0;

    		while (len > 1) {
       			sum += *buf++;
        		len -= 2;
    		}

    		if (len == 1) {	sum += *(const uint8_t*)buf; }
    		// fold carries
    		while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    		return static_cast<uint16_t>(~sum);
	}

	int acquire_raw_socket_connection(in_addr ip_addr, ostream& out) {
		int sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (sock_fd == -1) {
			out << "> socket creating went wrong:" << endl;
			out << "> " << strerror(errno) << endl;
			return -1;
		}

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_addr = ip_addr;
		int connect_result = connect(sock_fd, (sockaddr*)&hint,
				sizeof(hint));
		if (connect_result == -1) {
			out << "> connection went wrong" << endl;
			return -1;
		}

		// Define the timeout value
    		struct timeval timeout;
    		timeout.tv_sec = socket_timeout_sec;  // Timeout in seconds
    		timeout.tv_usec = 0; // Timeout in microseconds

    		// Set the timeout for receiving data
    		setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
		return sock_fd;
	}

	bool try_resolve_ip(const string input_address, ostream& out, in_addr& result_ip) {
		addrinfo* addr_info;

		//posix
		int error = getaddrinfo(input_address.c_str(), nullptr, nullptr, &addr_info);
		if (error != 0) {
			out << "> name resolution went wrong:" << endl;
			out << "> " << gai_strerror(error) << endl;
			out << "> " << strerror(errno) << endl;
			return false;
		}
		// https://beej.us/guide/bgnet/html/#structs
		//To deal with struct sockaddr, programmers created a parallel structure:
		//struct sockaddr_in (“in” for “Internet”) to be used with IPv4.

		//And this is the important bit: a pointer to a struct sockaddr_in can be
		//cast to a pointer to a struct sockaddr and vice-versa. So even though
		//connect() wants a struct sockaddr*, you can still use a struct sockaddr_in
		//and cast it at the last minute!

		sockaddr_in* ipv4 = (sockaddr_in*)(addr_info -> ai_addr);
		const char* resolved_ip = inet_ntoa(ipv4 -> sin_addr);
		out << "> resolved address: " << resolved_ip << endl;
		result_ip = ipv4 -> sin_addr;
		return true;
	}

	icmphdr create_request_icmp_header() {
		icmphdr header{};
		header.type = ICMP_ECHO; //8 it is
		header.code = icmp_echo_request_code; // icmp protocol: echo request = 8 + 0 : https://en.wikipedia.org/wiki/Internet_Control_Message_Protocol
		header.un.echo.id = static_cast<uint16_t>(getpid() & 0xFFFF); // for sender process identification
		headhttps://www.rfc-editor.org/info/rfc792/er.un.echo.sequence = 1;
    		header.checksum = 0;
    		header.checksum = checksum16(&header, sizeof(header)); // do i realy need this?
		return header;
	}

	node resolve_node_from_icmp_echo_answer(int response_size, string address, uint8_t* buf, ostream& out) {
		if (response_size == -1) {
			out << "> bad answer" << endl;
			out << "> " << strerror(errno) << endl;
			return node{address, node_status::failed, time(nullptr)};
		}

		icmphdr* response_header = reinterpret_cast<icmphdr*>(buf
						+ ipv4_max_header_size);
		if (response_header -> type == 0 &&
		    response_header -> code == 0) {
			return node(address, node_status::active, time(nullptr));
		}

		return node{address, node_status::failed, time(nullptr)};
	}
}

namespace mtj_ping {
	i_network::i_network() {
		// ToDo check all net devices - is smth up
		const char* dir = _net_devices_dir.c_str();
		struct stat sb;
		if (stat(dir, &sb) != 0) {
			cout << "> no information about net devices" << endl;
			return;
		}

		vector<string> devices;
		for (auto entry : filesystem::directory_iterator(dir)) {
			devices.push_back(entry.path());
		}

		for (string device_dir : devices) {
			string state_file_path = device_dir + "/" + _state_file_name;
			ifstream fs(state_file_path);
			if (!fs.is_open()) {
				cout << "> could not get state of " << device_dir << endl;
				continue;
			}

			string state;
			getline(fs, state);
			cout << "> device " << device_dir << " state: " << state << endl;
			fs.close();
		}
	}

	node i_network::ping(string address, ostream& out) {
		in_addr ip_addr;
		bool ip_resolved = try_resolve_ip(address, out, ip_addr);
		if (!ip_resolved) {
			out << "> could not resolve the ip address" << endl;
			return node{address, node_status::unknown, time(nullptr)};
		}

		int sock_fd = acquire_raw_socket_connection(ip_addr, out);
		if (sock_fd == -1) {
			out << "> could not acquire socket" << endl;
			return node{address, node_status::unknown, time(nullptr)};
		}

		icmphdr icmp_request_header = create_request_icmp_header();
		int s_result = send(sock_fd, &icmp_request_header, sizeof(icmp_request_header), 0);

		uint8_t response_packet[icmpv4_max_packet_size] {};
		int response_size = recv(sock_fd, &response_packet, sizeof(response_packet), 0);
		close(sock_fd);
		node result = resolve_node_from_icmp_echo_answer(response_size, address, response_packet, out);
		return result;
	}

	vector<node> i_network::trace(string address, ostream& out) {
	}
}
