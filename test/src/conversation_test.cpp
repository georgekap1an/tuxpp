#include <iostream>
#include <vector>
#include "doctest.h"
#include "tux/conversation.hpp"
#include "tux/cstring.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("conversation");

TEST_CASE("conversation default construct")
{
    conversation c;
    CHECK(c.connection_descriptor() == -1);
    CHECK(c.open() == false);
    CHECK((bool)c == false);
    CHECK(c.closed_gracefully() == false);
    CHECK(c.in_send_mode() == false);
    CHECK(c.in_receive_mode() == false);
    CHECK_THROWS(c.send());
    CHECK_THROWS(c.receive());
    CHECK_THROWS(c.receive_nonblocking());
    CHECK_NOTHROW(c.disconnect());
}

TEST_CASE("conversation move construct,assign")
{
    conversation c1("TOUPPERC");
    CHECK(c1.connection_descriptor() != -1);
    CHECK(c1.open() == true);
    CHECK((bool)c1 == true);
    CHECK(c1.closed_gracefully() == false);
    CHECK(c1.in_send_mode() == true);
    
    conversation c2(move(c1));
    CHECK(c1.connection_descriptor() == -1);
    CHECK(c1.open() == false);
    CHECK((bool)c1 == false);
    CHECK(c1.closed_gracefully() == false);
    CHECK(c1.in_send_mode() == false);
    
    CHECK(c2.connection_descriptor() != -1);
    CHECK(c2.open() == true);
    CHECK((bool)c2 == true);
    CHECK(c2.closed_gracefully() == false);
    CHECK(c2.in_send_mode() == true);
    
    conversation c3;
    c3 = move(c2);
    
    CHECK(c2.connection_descriptor() == -1);
    CHECK(c2.open() == false);
    CHECK((bool)c2 == false);
    CHECK(c2.closed_gracefully() == false);
    CHECK(c2.in_send_mode() == false);
    
    CHECK(c3.connection_descriptor() != -1);
    CHECK(c3.open() == true);
    CHECK((bool)c3 == true);
    CHECK(c3.closed_gracefully() == false);
    CHECK(c3.in_send_mode() == true);
    
    c3.disconnect();
}

TEST_CASE("conversation connect construct")
{
    conversation c1("TOUPPERC", buffer(), TPSENDONLY);
    CHECK(c1.connection_descriptor() != -1);
    CHECK(c1.open() == true);
    CHECK((bool)c1 == true);
    CHECK(c1.closed_gracefully() == false);
    CHECK(c1.in_send_mode() == true);
    c1.disconnect();
    
    conversation c2("TOUPPERC", buffer(), TPRECVONLY);
    CHECK(c2.connection_descriptor() != -1);
    CHECK(c2.open() == true);
    CHECK((bool)c2 == true);
    CHECK(c2.closed_gracefully() == false);
    CHECK(c2.in_receive_mode() == true);
    c2.disconnect();
    
    cstring initial_request("hello");
    conversation c3("TOUPPERC", buffer(), TPSENDONLY);
    CHECK(c3.connection_descriptor() != -1);
    CHECK(c3.open() == true);
    CHECK((bool)c3 == true);
    CHECK(c3.closed_gracefully() == false);
    CHECK(c3.in_send_mode() == true);
    c3.disconnect();
}

TEST_CASE("conversation connect")
{
    conversation c;
    c.connect("TOUPPERC");
    CHECK(c.connection_descriptor() != -1);
    CHECK(c.open() == true);
    CHECK((bool)c == true);
    CHECK(c.closed_gracefully() == false);
    CHECK(c.in_send_mode() == true);
    c.disconnect();
}

TEST_CASE("conversation send,receive")
{
    conversation c("TOUPPERC");
    vector<cstring> msgs = {cstring("hello"), cstring("world"), cstring("foo")};
    unsigned i = 0;
    for(auto&& msg : msgs)
    {
        ++i;
        c.send(msg.buffer(), i == msgs.size() ? TPRECVONLY : TPNOFLAGS);
    }
    msgs.clear();
    while(c.in_receive_mode())
    {
        msgs.emplace_back(c.receive());
    }
    CHECK(c.open() == false);
    CHECK(c.closed_gracefully() == true);
    CHECK(msgs.size() == 3);
    CHECK(msgs.at(0) == "HELLO");
    CHECK(msgs.at(1) == "WORLD");
    CHECK(msgs.at(2) == "FOO");
    
    cstring request("hello");
    c.connect("TOUPPERC");
    c.send(request.buffer(), TPRECVONLY);
    cstring reply = c.receive(TPNOFLAGS, request.move_buffer());
    CHECK(c.closed_gracefully() == true);
    CHECK(reply == "HELLO");
    CHECK(request.buffer().data() == nullptr);
}

TEST_CASE("conversation receive_nonblocking")
{
    conversation c("TOUPPERC");
    vector<cstring> msgs = {cstring("hello"), cstring("world"), cstring("foo")};
    unsigned i = 0;
    for(auto&& msg : msgs)
    {
        ++i;
        c.send(msg.buffer(), i == msgs.size() ? TPRECVONLY : TPNOFLAGS);
    }
    msgs.clear();
    
    SUBCASE("normal")
    {
        while(c.in_receive_mode())
        {
            while(true)
            {
                optional<buffer> reply = c.receive_nonblocking();
                if(reply)
                {
                    msgs.emplace_back(move(*reply));
                    break;
                }
            }
        }
        
        CHECK(msgs.size() == 3);
        CHECK(msgs.at(0) == "HELLO");
        CHECK(msgs.at(1) == "WORLD");
        CHECK(msgs.at(2) == "FOO");
        
    }
    
    SUBCASE("recycle buffer")
    {
        buffer buf;
        while(c.in_receive_mode())
        {
            while(true)
            {
                optional<buffer> reply = c.receive_nonblocking(TPNOFLAGS, move(buf));
                if(reply)
                {
                    CHECK(reply->data() != nullptr);
                    CHECK(buf.data() == nullptr);
                    msgs.emplace_back(move(*reply));
                    break;
                }
            }
        }
        
        CHECK(msgs.size() == 3);
        CHECK(msgs.at(0) == "HELLO");
        CHECK(msgs.at(1) == "WORLD");
        CHECK(msgs.at(2) == "FOO");
    }

}


TEST_SUITE_END();
