#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include "doctest.h"
#include "tux/pub_sub.hpp"
#include "tux/unsolicited_notification.hpp"
#include "tux/message_queuing.hpp"
#include "tux/cstring.hpp"
#include "tux/context.hpp"

using namespace std;
using namespace tux;

namespace
{

string received_message;

void process_event(char* data, long len, long flags)
{
    received_message = data;
}

int wait_and_check()
{
    this_thread::sleep_for(chrono::milliseconds(5));
    return check_unsolicited();
}

}

// asan warns that tpsubscribe
// performs strcpy with overlapping
// memory segments

TEST_SUITE("pub_sub");

TEST_CASE("pub_sub unsolicited notification asan=replace_str")
{
    // set handler
    set_notification_handler(process_event);
    received_message.clear();
    
    // subscribe to "INFO" events
    subscription info_subscription("INFO", "hello");
    
    // post an event
    cstring data("hello");
    post("INFO", data.buffer());
    
    // be notified
    wait_and_check();
    CHECK(received_message == "hello");
    
    set_notification_handler(nullptr);
    
}

TEST_CASE("pub_sub queue asan=replace_str")
{
    // must be attached as admin to
    // be notified via queue
    tux::terminate();
    init_request ir;
    ir.client_name("tpsysadm");
    initialize(ir);
    
    // subscribe
    subscription error_subscription(".*ERROR.*", ".*failed.*", "myqueuespace", "REPLY1");
    
    // post some events
    post("MINOR ERROR", cstring("slow response").buffer());
    post("CONFIG UPDATE", cstring("it is not true that the config update failed").buffer());
    post("MINOR ERROR", cstring("update failed").buffer());
    
    // pull event buffer from queue
    TPQCTL qctl = make_default<TPQCTL>();
    cstring info = dequeue("myqueuespace", "REPLY1", qctl);
    CHECK(info == "update failed");
}

TEST_CASE("pub_sub service asan=replace_str")
{
    // must be attached as admin to
    // be notified via service
    tux::terminate();
    init_request ir;
    ir.client_name("tpsysadm");
    initialize(ir);
    
    // subscribe
    // (event broker will call TRIGGER_NOTIFY service,
    // which will in turn notify all clients)
    subscription error_subscription(".*ERROR.*", ".*failed.*", "TRIGGER_BROADCAST");
    
    // set handler
    set_notification_handler(process_event);
    
    // post a few events
    received_message.clear();
    post("MINOR ERROR", cstring("slow response").buffer());
    wait_and_check();
    CHECK(received_message == "");
    
    received_message.clear();
    post("CONFIG UPDATE", cstring("configs were updated").buffer());
    wait_and_check();
    CHECK(received_message == "");
    
    received_message.clear();
    post("MINOR ERROR", cstring("update failed").buffer());
    wait_and_check();
    CHECK(received_message == "broadcast message");
    
    set_notification_handler(nullptr);
}

TEST_CASE("pub_sub subscribe/unsubscribe asan=replace_str")
{
    long number_of_deleted_subscriptions = 0;
    try
    {
        subscribe("FAKE1", "");
        subscribe("FAKE2", "");
        number_of_deleted_subscriptions = unsubscribe();
        CHECK(number_of_deleted_subscriptions == 2);
    }
    catch(...)
    {
        unsubscribe(); // want to clean up even if the test fails
    }
    
}

TEST_SUITE_END();
