#include <vector>
#include <iostream>
#include <thread>
#include "doctest.h"
#include "tux/request_response.hpp"
#include "tux/cstring.hpp"
#include "tux/xml.hpp"
#include "tux/init_request.hpp"
#include "tux/context.hpp"
#include "tux/pub_sub.hpp"
#include "tux/unsolicited_notification.hpp"


using namespace std;
using namespace tux;

TEST_SUITE("service");

namespace
{


vector<string> invocation_names;

void process_event(char* data, long len, long flags)
{
    invocation_names.push_back(data);
}

int wait_and_check()
{
    this_thread::sleep_for(chrono::milliseconds(5));
    return check_unsolicited();
}

}

TEST_CASE("service forward")
{
    cstring request("hello world");
    cstring reply = call("FORWARDING_SVC", request.move_buffer());
    CHECK(reply == "hello world->FORWARDING->FORWARDED");

}

TEST_CASE("service advertise / unadvertise")
{

      cstring service_name = call("REVEAL_SECRET");
      CHECK(service_name == "dECODER_rING");
      cstring secret_message = call("dECODER_rING");
      CHECK(secret_message == "drink more ovaltine");
      
      call("HIDE_SECRET");
      CHECK_THROWS(call("dECODER_rING"));

}

#if TUXEDO_VERSION >= 1222
TEST_CASE("service advertisex")
{
    set_notification_handler(process_event);
    post("CONFIG_UPDATE");
    wait_and_check();
    REQUIRE(invocation_names.size() > 1);
    CHECK(invocation_names[0] != invocation_names[1]);
}
#endif


TEST_SUITE_END();