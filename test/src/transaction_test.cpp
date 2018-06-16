#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <unistd.h>
#include "doctest.h"
#include "tux/message_queuing.hpp"
#include "tux/cstring.hpp"
#include "tux/transaction.hpp"

using namespace std;
using namespace tux;

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

TEST_SUITE("transaction");

TEST_CASE("transaction transaction_in_progress")
{
    CHECK(transaction_in_progress() == false);
    begin_transaction();
    CHECK(transaction_in_progress() == true);
    commit_transaction();
    CHECK(transaction_in_progress() == false);
    
    CHECK(transaction_in_progress() == false);
    begin_transaction();
    CHECK(transaction_in_progress() == true);
    abort_transaction();
    CHECK(transaction_in_progress() == false);
    
    CHECK(transaction_in_progress() == false);
    begin_transaction();
    CHECK(transaction_in_progress() == true);
    TPTRANID tranid = suspend_current_transaction();
    CHECK(transaction_in_progress() == false);
    resume(tranid);
    CHECK(transaction_in_progress() == true);
    commit_transaction();
    CHECK(transaction_in_progress() == false);
}

TEST_CASE("transaction begin/commit/abort")
{
    CHECK(transaction_in_progress() == false);
    
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    begin_transaction();
    
    SUBCASE("commit")
    {
        enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
        commit_transaction();
        qctl.flags = TPQGETBYCORRID;
        cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
        CHECK(reply == "HELLO");
    }
    
    SUBCASE("abort")
    {
        enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
        abort_transaction(); // rolls back enqueue
        qctl.flags = TPQGETBYCORRID;
        CHECK_THROWS(dequeue("myqueuespace", "REPLY1", qctl));
    }
}

TEST_CASE("transaction suspend/resume")
{
    
    CHECK(transaction_in_progress() == false);
    
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    begin_transaction();
    enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
    auto id1 = suspend_current_transaction();
    
    begin_transaction();
    enqueue("myqueuespace", "REVERSE", qctl, request.buffer());
    commit_transaction();
    
    resume(id1);
    abort_transaction();
    
    cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
    CHECK(reply == "olleh"); 
}

TEST_CASE("transaction class")
{
    CHECK(transaction_in_progress() == false);
    
    cstring request("hello");
    auto qctl = make_default<TPQCTL>();
    set(qctl.replyqueue, "REPLY1");
    set(qctl.corrid, make_correlation_id());
    qctl.flags = TPQCORRID | TPQREPLYQ;
    
    SUBCASE("commit")
    {
        transaction tx;
        enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
        tx.commit();
        qctl.flags = TPQGETBYCORRID;
        cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
        CHECK(reply == "HELLO");
    }
    
    SUBCASE("explicit abort")
    {
        transaction tx;
        enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
        tx.abort(); // rolls back enqueue
        qctl.flags = TPQGETBYCORRID;
        CHECK_THROWS(dequeue("myqueuespace", "REPLY1", qctl));
    }
    
    SUBCASE("implicit abort")
    {
        {
            transaction tx;
            enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
            // implicit abort when tx is destroyed
        }
        
        qctl.flags = TPQGETBYCORRID;
        CHECK_THROWS(dequeue("myqueuespace", "REPLY1", qctl));
    }
    
    SUBCASE("suspend and resume")
    {
        transaction tx;
        enqueue("myqueuespace", "TOUPPER", qctl, request.buffer());
        tx.suspend();
        CHECK(transaction_in_progress() == false);
        CHECK((bool)tx.suspended_tranid() == true);
        tx.resume();
        tx.commit();
        qctl.flags = TPQGETBYCORRID;
        cstring reply = dequeue("myqueuespace", "REPLY1", qctl);
        CHECK(reply == "HELLO");         
    }
}



TEST_CASE("transaction set_commit_return_mode")
{
    CHECK_NOTHROW(set_commit_return_mode(TP_CMT_LOGGED));
    CHECK_NOTHROW(set_commit_return_mode(TP_CMT_COMPLETE));
}

TEST_SUITE_END();