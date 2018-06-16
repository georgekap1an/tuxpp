#include <iostream>
#include <vector>
#include <thread>
#include "doctest.h"
#include "tux/unsolicited_notification.hpp"
#include "tux/cstring.hpp"
#include "tux/context.hpp"
#include "tux/request_response.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("unsolicited_notification");

namespace
{
    
string received_message;

int wait_and_check()
{
    this_thread::sleep_for(chrono::milliseconds(5));
    return check_unsolicited();
}

}

TEST_CASE("unsolicited_notification notify")
{
    // set handler
    set_notification_handler([](char* data, long len, long flags){
        received_message = data;
	});
    
    received_message.clear();
    // call service that will notify me
    call("TRIGGER_NOTIFY");
    wait_and_check();
    CHECK(received_message == "notification message");
    
    set_notification_handler(nullptr);
}

TEST_CASE("unsolicited_notification broadcast")
{
    tux::terminate();
    init_request ir;
    ir.flags(TPMULTICONTEXTS);
    context ctx1(ir);
    // set handler for ctx1
    set_notification_handler([](char* data, long len, long flags){
        received_message = data;
	});
        
    context ctx2(ir);
    // set same handler for ctx2
    set_notification_handler([](char* data, long len, long flags){
        received_message = data;
	});
    
    received_message.clear();
    call("TRIGGER_BROADCAST");
    wait_and_check();
    CHECK(received_message == "broadcast message");
    
    // switch back to ctx1
    ctx1.make_current();
    received_message.clear();
    int count = wait_and_check();;
    CHECK(count == 1);
    CHECK(received_message == "broadcast message");
    
    set_notification_handler(nullptr);
}

TEST_CASE("unsolicited_notification set_notification_handler")
{
    auto handler_a = [](char* data, long len, long flags) { received_message = data; };
    auto handler_b = [](char* data, long len, long flags) { received_message = "ignore";};
    
    set_notification_handler(handler_a);
    received_message.clear();
    call("TRIGGER_NOTIFY");
    wait_and_check();;
    CHECK(received_message == "notification message");
    auto last_handler = set_notification_handler(handler_b);
    CHECK(last_handler == handler_a);
    received_message.clear();
    call("TRIGGER_NOTIFY");
    wait_and_check();;
    CHECK(received_message == "ignore");
    
    set_notification_handler(nullptr);
}

TEST_CASE("unsolicited_notification check_unsolicited")
{
    received_message.clear();
    set_notification_handler([](char* data, long len, long flags) { received_message += "recvd";});
    async_call call1("TRIGGER_NOTIFY_TWICE", buffer(),TPNOREPLY);
    int count = wait_and_check();
    CHECK(count == 2);
    CHECK(received_message == "recvdrecvd");
    
    set_notification_handler(nullptr);  
}

TEST_SUITE_END();