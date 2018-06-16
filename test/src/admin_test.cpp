#include <iostream>
#include <sstream>
#include <unistd.h>
#include "doctest.h"
#include "tux/admin.hpp"
#include "tux/context.hpp"
#include "tux/transaction.hpp"
#include "tux/request_response.hpp"
#include "tux/cstring.hpp"

using namespace std;
using namespace tux;
using fml = fml32;

TEST_SUITE("admin");

TEST_CASE("admin clientid<->string")
{
    // connect as admin
    init_request ir;
    ir.client_name("tpsysadm");
    context ctx(ir);
    
    // don't know how to get client id as client
    // ask a service for it.
    string clientid = cstr_to_string(call("ECHO_CLIENTID").data());
    // service will check conversions for us and fail if they
    // don't work as expected
    
    // inquire using client id
    fml request;
    request.add(TA_OPERATION, "GET");
    request.add(TA_CLASS, "T_CLIENT");
    request.add(TA_LMID, "this");
    request.add(TA_CLIENTID, clientid);
    fml reply = admin_call(request);
    
    CHECK(reply.get_string(TA_CLTNAME) == "tpsysadm");
    CHECK(reply.get_long(TA_PID) == getpid());
}

TEST_CASE("admin tranid<->string")
{
    // connect as admin
    init_request ir;
    ir.client_name("tpsysadm");
    context ctx(ir);
    
    // start a transaction
    transaction tx;
    
    // suspend the transaction
    tx.suspend();
    TPTRANID tranid = *tx.suspended_tranid();
    string tranid_str = to_string(tranid);
    
    // check the conversions
    CHECK(tranid_str != "");
    TPTRANID generated_tranid = to_tranid(tranid_str);
    CHECK(memcmp(&generated_tranid, &tranid, sizeof(tranid)) == 0);
    
    // use the string to inquire on the transaction
    fml request;
    request.add(TA_OPERATION, "GET");
    request.add(TA_CLASS, "T_TRANSACTION");
    request.add(TA_COORDLMID, "this");
    request.add(TA_LMID, "this");
    request.add(TA_TPTRANID, tranid_str);
    fml reply = admin_call(request);

    CHECK(reply.get_string(TA_TPTRANID) == tranid_str);
    CHECK(reply.get_string(TA_STATE) == "SUSPENDED");
    
    // resume and commit
    tx.resume();
    tx.commit();
}

#if TUXEDO_VERSION >= 1213
TEST_CASE("admin set/get call_info")
{
    cstring msg("hello");
    fml metadata;
    
    SUBCASE("HTTP header")
    {
        metadata.add(TA_HTTP_HEADER_NAME, "Accept");
        metadata.add(TA_HTTP_HEADER_VALUE, "text/plain");
        metadata.add(TA_HTTP_HEADER_NAME, "Accept-Charset");
        metadata.add(TA_HTTP_HEADER_VALUE, "utf-8");
        set_call_info(metadata, msg.buffer());
        fml retrieved_metadata = get_call_info(msg.buffer());
        CHECK(retrieved_metadata == metadata);
    }

#if TUXEDO_VERSION >= 1222   
    SUBCASE("SOAP header")
    {
        // weird soap header, but just a sample
        fml soap_header;
        soap_header.add(TA_MONCORRID, "a correlation id");
        soap_header.add(TA_MONMSGTYPE, "a message type");
        metadata.add(TA_SOAP_HEADER, soap_header);
        set_call_info(metadata, msg.buffer());
        fml retrieved_metadata = get_call_info(msg.buffer());
        CHECK(retrieved_metadata == metadata); 
    }
    
    SUBCASE("msg tag")
    {
        metadata.add(TA_MSGTAG, "a message tag");
        set_call_info(metadata, msg.buffer());
        fml retrieved_metadata = get_call_info(msg.buffer());
        CHECK(retrieved_metadata == metadata); 
    }
#endif    
}
#endif

TEST_CASE("admin admin_call")
{
    fml request;
    request.add(TA_OPERATION, "GET");
    request.add(TA_CLASS, "T_MACHINE");
    request.add(TA_STATUS, "ACTIVE"); 
    request.add(TA_FILTER, (long)TA_LMID);

    SUBCASE("normal")
    {
        fml reply = admin_call(request);
        CHECK(reply.get_string(TA_LMID) == "this");
    }
    
    SUBCASE("recycle buffer")
    {
        admin_call(request, request);
        CHECK(request.get_string(TA_LMID) == "this");
        
    }
    
}

string get_appdir()
{
    fml request;
    request.add(TA_OPERATION, "GET");
    request.add(TA_CLASS, "T_MACHINE");
    request.add(TA_STATUS, "ACTIVE");
    request.add(TA_FILTER, (long)TA_APPDIR);
    fml reply = admin_call(request);
    return reply.get_string(TA_APPDIR);
}

TEST_CASE("admin get/set repository_info asan=replace_intrin")
{
    string path = get_appdir() + "/repository";
    
    // get
    fml request;
    request.add(TA_OPERATION, "GET");
    request.add(TA_STATE, "VAL");
    request.add(TA_REPOSSERVICE, "*");
    fml reply = get_repository_info(path, request);
    fml svc_info = reply.get_fml(TA_REPOSEMBED);
    CHECK(svc_info.get_string(TA_REPOSSERVICE) == "TO_UPPER");
    CHECK(svc_info.get_string(TA_REPOSSERVICETYPE) == "service");

    
    // set (without actually changing anything)
    request = svc_info;
    request.set(TA_OPERATION, "SET");
    request.set(TA_STATE, "");
    reply = set_repository_info(path, request);
    CHECK(reply.get_fml(TA_REPOSEMBED).get_long(TA_ERROR) == TAOK);
}


TEST_SUITE_END();
