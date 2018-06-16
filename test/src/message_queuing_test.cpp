#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>
#include "doctest.h"
#include "tux/message_queuing.hpp"
#include "tux/cstring.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("message_queuing");

namespace
{
string make_correlation_id()
{
	static thread_local int i = 0;
	++i;
	stringstream ss;
	ss << getpid() << this_thread::get_id() << i;
	return ss.str();
}
}

TEST_CASE("message_queuing enqueue")
{
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
    
    qctl.flags = TPQGETBYCORRID;
    
    cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
    
    CHECK(reply == "HELLO");
}

TEST_CASE("message_queuing dequeue_nonblocking")
{
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
    
    qctl.flags = TPQGETBYCORRID;
    

    SUBCASE("normal")
    {
        optional<buffer> reply_buffer;
        while(!reply_buffer)
        {
            reply_buffer = dequeue_nonblocking("myqueuespace", "REPLY1", qctl);
        } 
        cstring reply = move(*reply_buffer);
        CHECK(reply == "HELLO");
    }
    
    SUBCASE("recycle buffer")
    {
        optional<buffer> reply_buffer;
        while(!reply_buffer)
        {
            reply_buffer = dequeue_nonblocking("myqueuespace", "REPLY1", qctl, TPNOFLAGS, request.move_buffer());
        } 
        cstring reply = move(*reply_buffer);
        CHECK(reply == "HELLO");
    }
}

TEST_CASE("message_queuing dequeue")
{
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
    
    qctl.flags = TPQGETBYCORRID;
    

    SUBCASE("normal")
    {
        cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
        CHECK(reply == "HELLO");
    }
    
    SUBCASE("recycle buffer")
    {
        cstring reply = dequeue("myqueuespace", "REPLY1", qctl, TPNOFLAGS, request.move_buffer());
        CHECK(reply == "HELLO");
    }
}

TEST_SUITE_END();
