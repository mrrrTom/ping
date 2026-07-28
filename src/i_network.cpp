#include "i_network.hpp"
using namespace  std;
namespace {
	// copy paste from google
	// is it faire checksum?
	// is it evere checked?
	// and if it is - where?
	static uint16_t checksum16(const void* data, size_t len) {
    		const uint16_t* buf = static_cast<const uint16_t*>(data);
    		uint32_t sum = 0;

    		while (len > 1) {
       			sum += *buf++;
        		len -= 2;
    		}

    		if (len == 1) {
        		sum += *(const uint8_t*)buf;
    		}

    		// fold carries
    		while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    		return static_cast<uint16_t>(~sum);
	}
}

namespace mtj_ping {
	// ToDo: slice to function
	// use out
	node i_network::ping(string address, ostream& out) {
		int sock_fd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);
		if(sock_fd == -1) {
			cout << "socket creating went wrong" << endl;
			//return 1;
		}

		//int packet_size = recvfrom(sock, buffer, 65536, MSG_PEEK, nullptr, nullptr);
		// gef installed - try ping, look through the packet.

		//sendto(int fd, const void *buf, size_t n, int flags, const struct sockaddr *addr, socklen_t addr_len)
		string ip_address = "";
		sockaddr_in hint;
		hint.sin_family = AF_INET;
		inet_pton(AF_INET, ip_address.c_str(), &hint.sin_addr);
		int connect_result = connect(sock_fd, (sockaddr*)&hint, sizeof(hint));
		if (connect_result == -1) {
			cout << "connection went wrong" << endl;
			//return 1;
		}

		icmphdr icmp{};
		icmp.type = ICMP_ECHO;
		icmp.code = 0;
		icmp.un.echo.id = static_cast<uint16_t>(getpid() & 0xFFFF); // for sender process identification
		icmp.un.echo.sequence = 1;

		string msg = "Hello world!";
		const char* payload = msg.c_str(); // u can send 0 bytes here
		uint8_t packet [sizeof(icmphdr) + sizeof(payload)];
		memcpy(packet, &icmp, sizeof(icmp));
		memcpy(packet + sizeof(icmp), payload, msg.length() + 1);

		// can calculate checksum for icmp.checksum

		auto* icmp_in_packet = reinterpret_cast<icmphdr*>(packet);
    		icmp_in_packet->checksum = 0;
    		icmp_in_packet->checksum = checksum16(packet, sizeof(packet));

		int s_result = send(sock_fd, packet, sizeof(packet) /* this is how much i recieved */, 0);


		int packet_size = recv(sock_fd, packet, sizeof(packet), 0);

		cout << packet_size << endl;
		char* cur_symb = reinterpret_cast<char*>(packet);
		for (int i = 0; i < sizeof(packet); ++i) {
			cout << *cur_symb << endl;
			cur_symb++;
		}

		cout << s_result;
	}

	vector<node> i_network::trace(string address, ostream& out) {
	}
}
