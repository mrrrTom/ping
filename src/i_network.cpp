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

	int acquire_raw_socket_connection(string ip_addr, ostream& out) {
		int sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if (sock_fd == -1) {
			out << "socket creating went wrong" << endl;
			return -1;
		}

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		inet_pton(AF_INET, ip_addr.c_str(), &hint.sin_addr);
		int connect_result = connect(sock_fd, (sockaddr*)&hint, sizeof(hint));
		if (connect_result == -1) {
			out << "connection went wrong" << endl;
			return -1;
		}

		return sock_fd;
	}

	string resolve_ip(string address, ostream& out) {
		// todo
		return address;
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

	node resolve_node_from_icmp_echo_answer(string address, uint8_t* buf, ostream& out) {
		icmphdr* response_header = reinterpret_cast<icmphdr*>(buf
						+ ipv4_max_header_size);
		if (response_header -> type == 0 &&
		    response_header -> code == 0) {
			return node(address, node_status::active, time(nullptr));
		}

		return node(address, node_status::failed, time(nullptr));
	}
}

namespace mtj_ping {
	node i_network::ping(string address, ostream& out) {
		string ip_addr = resolve_ip(address, out);
		int sock_fd = acquire_raw_socket_connection(ip_addr, out);
		icmphdr icmp_request_header = create_request_icmp_header();
		int s_result = send(sock_fd, &icmp_request_header, sizeof(icmp_request_header), 0);

		uint8_t response_packet[icmpv4_max_packet_size] {};
		int response_size = recv(sock_fd, &response_packet, sizeof(response_packet), 0);
		node result = resolve_node_from_icmp_echo_answer(address, response_packet, out);
		return result;
	}

	vector<node> i_network::trace(string address, ostream& out) {
	}
}
