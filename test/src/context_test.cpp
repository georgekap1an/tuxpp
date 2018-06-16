#include <iostream>
#include <thread>
#include "doctest.h"
#include "tux/context.hpp"
#include "tux/unsolicited_notification.hpp"
#include "tux/pub_sub.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("context");

TEST_CASE("context context_to_string")
{
    CHECK(context_to_string(TPNULLCONTEXT) == "TPNULLCONTEXT");
    CHECK(context_to_string(TPINVALIDCONTEXT) == "TPINVALIDCONTEXT");
    CHECK(context_to_string(TPSINGLECONTEXT) == "TPSINGLECONTEXT");
    CHECK(context_to_string(45) == "45");
}

TEST_CASE("context check_authentication_level")
{
    CHECK(check_authentication_level() == TPNOAUTH);
}

TEST_CASE("context initialize/terminate")
{
    SUBCASE("no arg")
    {
        CHECK_NOTHROW(tux::initialize());
        CHECK_NOTHROW(tux::terminate());
    }
    
    SUBCASE("init_request")
    {
        init_request ir;
        ir.username("name");
        ir.client_name("client");
        ir.app_password("pwd");
        ir.flags(TPSA_FASTPATH | TPU_IGN);
        CHECK_NOTHROW(tux::initialize(ir));
        CHECK_NOTHROW(tux::terminate());        
    }
    
    // there's also initializing from appthread,
    // but that can only be tested from server context
}

TEST_CASE("context get_context")
{
    SUBCASE("single context")
    {
        CHECK(get_context() == TPNULLCONTEXT);
        initialize();
        CHECK(get_context() == TPSINGLECONTEXT);
        tux::terminate();
    }
    SUBCASE("multicontext")
    {
        CHECK(get_context() == TPNULLCONTEXT);
        init_request ir;
        ir.flags(TPMULTICONTEXTS);
        initialize(ir);
        auto ctx1 = get_context();
        CHECK(ctx1 > TPSINGLECONTEXT);
        initialize(ir);
        auto ctx2 = get_context();
        CHECK(ctx2 > TPSINGLECONTEXT);
        CHECK(ctx2 != ctx1);
        tux::terminate();
        set_context(ctx1);
        tux::terminate();
    }
}

TEST_CASE("context set_context")
{
    SUBCASE("threads")
    {
        CHECK(get_context() == TPNULLCONTEXT);
        initialize();
        auto ctx = get_context();
        thread t1([ctx](){
            CHECK_NOTHROW(set_context(ctx));
            CHECK(get_context() == ctx);
        });
        t1.join();
        tux::terminate();
    }
    
    SUBCASE("multicontext")
    {
        CHECK(get_context() == TPNULLCONTEXT);
        init_request ir;
        ir.flags(TPMULTICONTEXTS);
        initialize(ir);
        auto ctx1 = get_context();
        initialize(ir);
        auto ctx2 = get_context();
        tux::terminate();
        set_context(ctx1);
        CHECK(get_context() == ctx1);
        tux::terminate();
    }
}


TEST_CASE("context class")
{
    init_request ir;
    ir.flags(TPMULTICONTEXTS);
    context ctx1(ir);
    context ctx2(ir);
    
    CHECK(ctx1.mode() == context_mode::normal);
    CHECK(ctx1.value() > TPSINGLECONTEXT);
    CHECK(ctx2.mode() == context_mode::normal);
    CHECK(ctx2.value() > TPSINGLECONTEXT);
    CHECK(ctx2.value() != ctx1.value());    
}

namespace
{


void process_event(char* data, long len, long flags)
{
}

}

TEST_CASE("context appthr asan=replace_str")
{
    set_notification_handler(process_event);
    subscribe("100 MS PASSED", "");
    this_thread::sleep_for(chrono::milliseconds(250));
    int count = check_unsolicited();
    CHECK(count >= 2);
    unsubscribe();
}

TEST_SUITE_END();
