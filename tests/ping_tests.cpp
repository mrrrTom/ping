#define BOOST_TEST_MODULE my_tests
#include <boost/test/unit_test.hpp>
#include "../src/i_network.hpp"
using namespace mtj_ping;

BOOST_AUTO_TEST_CASE(ping_localhost) {
    	i_network in{};
	node nd = in.ping("127.0.0.1", cout);
	node_status got_status = nd.get_status();
	bool succeeded = got_status == node_status::active;
	BOOST_TEST(succeeded);
}
