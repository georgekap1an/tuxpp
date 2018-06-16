#include <iostream>
#include <thread>
#include "doctest.h"
#include "tux/request_response.hpp"
#include "tux/cstring.hpp"
#include "tux/xml.hpp"
#include "tux/init_request.hpp"
#include "tux/context.hpp"


using namespace std;
using namespace tux;

TEST_SUITE("request_response");

TEST_CASE("request_response call")
{
    cstring request("hello");
    SUBCASE("simple")
    {
        cstring reply = call("TOUPPER", request.buffer());
        CHECK(reply == "HELLO");
    }
    
    SUBCASE("special flags")
    {
        cstring reply = call("REVERSE", request.buffer(), TPSIGRSTRT | TPNOTIME);
        CHECK(reply == "olleh");
    }
    
    SUBCASE("preallocate output")
    {
        cstring spare;
        spare.reserve(12);
        cstring reply = call("TOUPPER", request.buffer(), TPNOFLAGS, spare.move_buffer());
        CHECK(reply == "HELLO");
        CHECK(spare.buffer().data() == nullptr);
    }
    
    SUBCASE("recycle request")
    {
        cstring reply = call("REVERSE", request.buffer(), TPNOFLAGS, request.move_buffer());
        CHECK(reply == "olleh");
        CHECK(request.buffer().data() == nullptr);
    }
    
    SUBCASE("recycle request of different type")
    {
        // we're able to reuse the memory from
        // a cstring buffer for an xml reply
        xml reply = call("ECHO_XML", request.buffer(), TPNOFLAGS, request.move_buffer());
        CHECK(request.buffer().data() == nullptr);
        CHECK(reply.to_string() == "<request>hello</request>");
    }
    
    SUBCASE("recycle request on failure")
    {
        // original buffer is preserved
        CHECK_THROWS(cstring reply = call("BOGUS_SVC", request.buffer(), TPNOFLAGS, request.move_buffer()));
        CHECK(request == "hello");
    }
    
    SUBCASE("recycle request success but no reply")
    {
        // let's pretend I'm calling a service
        // which sometimes returns a null buffer
        // on success (but not always, so I can't
        // know in advance)
        cstring possible_reply = call("NO_REPLY_SVC", request.buffer(), TPNOFLAGS, request.move_buffer());
        // original buffer is preserved
        CHECK(possible_reply.buffer().data() == nullptr);
        CHECK(request == "hello");
    }
}

TEST_CASE("request_response async_call default construct")
{
    async_call acall;
    CHECK(acall.failed() == false);
    CHECK(acall.pending() == false);
    CHECK(acall.succeeded() == false);
    CHECK(acall.call_descriptor() == 0);
    CHECK(acall.service_name() == "");
    CHECK(acall.urcode() == 0);
}

TEST_CASE("request_response async_call move construct/assign")
{
    // start an async call
    cstring request("hello");
    async_call acall1("TOUPPER", request.buffer());
    CHECK(acall1.pending() == true);
    CHECK(acall1.call_descriptor() != 0);
    CHECK(acall1.service_name() == "TOUPPER");
    
    // move it (via construction)
    async_call acall2(move(acall1));
    CHECK(acall1.pending() == false);
    CHECK(acall1.call_descriptor() == 0);
    CHECK(acall1.service_name() == "");
    CHECK(acall2.pending() == true);
    CHECK(acall2.call_descriptor() != 0);
    CHECK(acall2.service_name() == "TOUPPER");
    
    // move it (via assignment)
    async_call acall3;
    acall3 = move(acall2);
    CHECK(acall2.pending() == false);
    CHECK(acall2.call_descriptor() == 0);
    CHECK(acall2.service_name() == "");
    CHECK(acall3.pending() == true);
    CHECK(acall3.call_descriptor() != 0);
    CHECK(acall3.service_name() == "TOUPPER");
    
    cstring reply = acall3.get_reply();
    CHECK(reply == "HELLO");    
}

TEST_CASE("request_response async_call status")
{
    cstring request("hello");
    async_call acall;
    acall.start("BOGUS_SVC", request.buffer());
    CHECK(acall.failed() == true);
    CHECK_THROWS(acall.get_reply());
    acall.start("TOUPPER", request.buffer());
    CHECK(acall.pending() == true);
    cstring reply = acall.get_reply();
    CHECK(acall.succeeded() == true);
    CHECK(reply == "HELLO"); 
}

TEST_CASE("request_response async_call start")
{
    cstring request("hello");
    async_call acall;
    acall.start("TOUPPER", request.buffer());
    CHECK(acall.pending() == true);
    CHECK(acall.service_name() == "TOUPPER");
    CHECK(acall.call_descriptor() != 0);
    acall.cancel();
    acall.start("TOUPPER", request.buffer(), TPNOREPLY);
    CHECK(acall.succeeded() == false);
    CHECK(acall.pending() == false);
    CHECK(acall.failed() == false);
    CHECK_NOTHROW(acall.start("TOUPPER"));
    CHECK(acall.pending() == true);
    acall.cancel();
}

TEST_CASE("request_response async_call cancel")
{
    cstring request("hello");
    async_call acall;
    acall.start("TOUPPER", request.buffer());
    CHECK(acall.pending() == true);
    acall.cancel();
    CHECK(acall.succeeded() == false);
    CHECK(acall.pending() == false);
    CHECK(acall.failed() == false);
    CHECK(acall.service_name() == "");
    CHECK(acall.call_descriptor() == 0);
    CHECK_THROWS(acall.get_reply());
}

TEST_CASE("request_response async_call clear")
{
    // clear is equivalent to cancel,
    // except that it also resets
    // any continuations
    cstring request("hello");
    async_call acall;
    acall.start("TOUPPER", request.buffer());
    CHECK(acall.pending() == true);
    acall.clear();
    CHECK(acall.succeeded() == false);
    CHECK(acall.pending() == false);
    CHECK(acall.failed() == false);
    CHECK_THROWS(acall.get_reply());
}

TEST_CASE("request_response async_call get_reply")
{
    cstring request("hello");
    async_call acall("TOUPPER", request.buffer());
    cstring reply = acall.get_reply();
    CHECK_THROWS(acall.get_reply()); // reply is moved so 2nd call fails
    CHECK(reply == "HELLO");
    CHECK(acall.urcode() == 1);
    CHECK(acall.succeeded() == true);
    CHECK(acall.call_descriptor() == 0);
    CHECK(acall.service_name() == "TOUPPER");
    
    acall.start("TOUPPER", request.buffer());
    CHECK(request == "hello");
    reply = acall.get_reply(TPNOFLAGS, request.move_buffer());
    CHECK(request.size() == 0);
    CHECK(reply == "HELLO");
    
    // fail by timeout
    acall.start("VERY_SLOW_SVC");
    set_block_time(block_time_scope::next, chrono::seconds(1));
    CHECK_THROWS(acall.get_reply());
}

TEST_CASE("request_response async_call get_reply_nonblocking")
{
    cstring request("hello");
    async_call acall("SLOW_TOUPPER", request.buffer());
    optional<buffer> reply_buf = acall.get_reply_nonblocking();
    CHECK((bool)reply_buf == false);
    while(!reply_buf)
    {
        reply_buf = acall.get_reply_nonblocking();
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    CHECK((bool)reply_buf == true);
    cstring reply = move(*reply_buf);
    CHECK(reply == "HELLO");
    
    acall.start("SLOW_TOUPPER", request.buffer());
    reply_buf = acall.get_reply_nonblocking(TPNOFLAGS, request.move_buffer());
    CHECK((bool)reply_buf == false);
    CHECK(request == "hello");
    while(!reply_buf)
    {
        reply_buf = acall.get_reply_nonblocking(TPNOFLAGS, request.move_buffer());
        this_thread::sleep_for(chrono::milliseconds(10));
    }
    CHECK(request.size() == 0);
    CHECK((bool)reply_buf == true);
    reply = move(*reply_buf);
    CHECK(reply == "HELLO");
}

TEST_CASE("request_response async_call start constructor")
{
    cstring request("hello");
    async_call acall("SLOW_TOUPPER", request.buffer());
    CHECK(acall.pending() == true);
    cstring reply = acall.get_reply();
    CHECK(reply == "HELLO");
}

TEST_CASE("request_response async_calls_pending")
{
    tux::terminate();
    init_request ir;
    ir.flags(TPMULTICONTEXTS);
    context ctx1(ir);
    CHECK(async_calls_pending() == false);
    cstring request("hello");
    async_call acall1("TOUPPER", request.buffer());
    CHECK(async_calls_pending() == true);
    
    context ctx2(ir);
    request = "world";
    CHECK(async_calls_pending() == false);
    async_call acall2("TOUPPER", request.buffer());
    CHECK(async_calls_pending() == true);
    
    ctx1.make_current();
    CHECK(async_calls_pending() == true);
    cstring reply = acall1.get_reply();
    CHECK(async_calls_pending() == false);
    CHECK(reply == "HELLO");
    
    ctx2.make_current();
    CHECK(async_calls_pending() == true);
    reply = acall2.get_reply();
    CHECK(async_calls_pending() == false);
    CHECK(reply == "WORLD");
    

    thread t1([&](){
        ctx1.make_current();
        async_call capitalize("TOUPPER",request.buffer());
        CHECK(async_calls_pending() == true);
        cstring rply = capitalize.get_reply();
        CHECK(rply == "WORLD");
    });
    
    thread t2([&](){
        ctx1.make_current();
        async_call reverse("REVERSE", request.buffer());
        CHECK(async_calls_pending() == true);
        cstring rply = reverse.get_reply();
        CHECK(rply == "dlrow");
    });
    
    t1.join();
    t2.join();
    CHECK(async_calls_pending() == false);
}

TEST_CASE("request_response get_any_reply")
{
    cstring request("hello");
    async_call capitalize("TOUPPER", request.buffer());
    async_call reverse("REVERSE", request.buffer());
    while(async_calls_pending())
    {
        async_call* next = get_any_reply();
        if(next == &capitalize)
        {
            cstring reply = next->get_reply();
            CHECK(reply == "HELLO");
        }
        else if(next == &reverse)
        {
            cstring reply = next->get_reply();
            CHECK(reply == "olleh");
        }
    }
}

TEST_CASE("request_response async_call then basic")
{
    string result1, result2;
    int urcode1 = 0;
    int urcode2 = 0;
    cstring request("hello");
    
    async_call capitalize("TOUPPER", request.buffer());
    capitalize.then([&](buffer& reply_buffer, int urcode){
        urcode1 = urcode;
        cstring reply = move(reply_buffer);
        result1 = reply.data();
    });
    
    async_call reverse("REVERSE", request.buffer());
    reverse.then([&](buffer& reply_buffer, int urcode){
        urcode2 = urcode;
        cstring reply = move(reply_buffer);
        result2 = reply.data();
    });
    
    while(async_calls_pending())
    {
        get_any_reply();
    }
    
    CHECK(urcode1 == 1);
    CHECK(result1 == "HELLO");
    CHECK(urcode2 == 2);
    CHECK(result2 == "olleh");
    
    // recycle buffer
    capitalize.start("TOUPPER", request.buffer());
    capitalize.then([&](buffer& reply_buffer, int urcode){
        urcode1 = urcode;
        cstring reply = move(reply_buffer);
        result1 = reply.data();
        reply_buffer = reply.move_buffer();
    });
    
    reverse.start("REVERSE", request.buffer());
    reverse.then([&](buffer& reply_buffer, int urcode){
        urcode2 = urcode;
        cstring reply = move(reply_buffer);
        result2 = reply.data();
        reply_buffer = reply.move_buffer();
    });
    
    while(async_calls_pending())
    {
        get_any_reply(TPNOFLAGS, request.move_buffer());
    }
    
    CHECK(urcode1 == 1);
    CHECK(result1 == "HELLO");
    CHECK(urcode2 == 2);
    CHECK(result2 == "olleh");
}

TEST_CASE("request_response async_call then basic call chaining")
{
    // this isn't pretty;
    // consider using tpforward instead
    cstring request("hello");
    cstring result;
    // note we declare both async calls in this outer scope.
    // if we only declared "second" in the lambda we pass
    // to first.then, it would go out of scope and be
    // canceled before it was completed.
    async_call first,second;
    first.start("TOUPPER", request.buffer());
    first.then([&](buffer& buf, int urcode){
        second.start("REVERSE", buf);
        second.then([&](buffer& buf, int urcode){
            result = move(buf);
        });
    });
    process_pending_async_calls();
    CHECK(result == "OLLEH");
}

TEST_CASE("request_response async_call then process replies in special order")
{
    // there may be some cases
    // where async calls can be started
    // in any order in parallel, but
    // *some* of the replies
    // must be processed in a certain
    // order.
    string concatenation;

    cstring request("hello");
    async_call capitalize("SLOW_TOUPPER", request.buffer());
    async_call reverse("REVERSE", request.buffer());
    // reverse will finish first, but we will
    // wait to process its reply until we've processed
    // the reply from capitalize
    capitalize.then([&](buffer& buf, int urcode){
        concatenation += buf.data();
        // we wait until we've processed the reply
        // from TOUPPER before attaching the
        // continuation for REVERSE
        reverse.then([&](buffer& buf, int urcode){
            concatenation += buf.data();
        });
    });
    // In this example, it would have been easier
    // to simply call capitalize.get_reply(), followed
    // by reverse.get_reply().  But suppose we have
    // 4 outstanding tpacalls, A-D.  Further suppose
    // D's reply can't be processed until B's reply is,
    // but there are no other dependencies.  In such
    // a case, this technique can be used.
    process_pending_async_calls();
    CHECK(concatenation == "HELLOolleh");
}

TEST_CASE("request_response async_call on_error")
{
    // fail by calling a fake service
    string error_message;
    async_call bad_call("BOGUS_SVC");
    bad_call.on_error([&](exception_ptr eptr){
        try
        {
            rethrow_exception(eptr);
        }
        catch(exception const& e)
        {
            error_message = e.what();
        }
    });
    CHECK(bad_call.failed() == true);
    CHECK(error_message.find("TPENOENT") != string::npos);
    
    // fail by calling a service
    // which always fails
    bad_call.start("BAD_SVC");
    bad_call.on_error([&](exception_ptr eptr){
        try
        {
            rethrow_exception(eptr);
        }
        catch(service_error const& e)
        {
            error_message = "service failed";
        }
        catch(exception const& e)
        {
            error_message = e.what();
        }
    });
    process_pending_async_calls();
    CHECK(bad_call.failed() == true);
    CHECK(error_message == "service failed");
}

TEST_CASE("request_response process_pending_async_calls")
{
    cstring request("hello");
    string capitalized, reversed;
    async_call capitalize("TOUPPER", request.buffer());
    capitalize.then([&](buffer& reply_buffer, int urcode){
        capitalized = reply_buffer.data();
    });
    async_call reverse("REVERSE", request.buffer());
    reverse.then([&](buffer& reply_buffer, int urcode){
        reversed = reply_buffer.data();
    });
    
    SUBCASE("simple")
    {
        process_pending_async_calls();
        CHECK(capitalized == "HELLO");
        CHECK(reversed == "olleh");   
    }
    
    SUBCASE("set flags")
    {
        process_pending_async_calls(TPSIGRSTRT);
        CHECK(capitalized == "HELLO");
        CHECK(reversed == "olleh");   
    }
    
    SUBCASE("recycle buffer")
    {
        process_pending_async_calls(TPNOFLAGS, request.move_buffer());
        CHECK(capitalized == "HELLO");
        CHECK(reversed == "olleh"); 
    }
    
    SUBCASE("timeout in milliseconds")
    {
        async_call slowcall("VERY_SLOW_SVC");
        CHECK(process_pending_async_calls(chrono::milliseconds(1500)) == false);
        CHECK(slowcall.pending() == true);
        CHECK(capitalized == "HELLO");
        CHECK(reversed == "olleh");
    }
    
    SUBCASE("timeout in seconds")
    {
        async_call slowcall("VERY_SLOW_SVC");
        CHECK(process_pending_async_calls(chrono::seconds(2)) == false);
        CHECK(capitalized == "HELLO");
        CHECK(reversed == "olleh");
        CHECK(slowcall.pending() == true);
    }
}


TEST_SUITE_END();