#include "doctest.h"
#include "tux/service_error.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;


TEST_SUITE("service_error");

TEST_CASE("service_error without buffer")
{
    service_error e("TOUPPER", -1);
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().data() == nullptr);
    
    // copy construct
    service_error e1(e);
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    
    CHECK(string(e1.what()).find("TOUPPER") != string::npos);
    CHECK(string(e1.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e1.user_code() == -1);
    
    // copy assign
    service_error e2("TOLOWER", 1);
    e2 = e;
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    
    CHECK(string(e2.what()).find("TOUPPER") != string::npos);
    CHECK(string(e2.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e2.user_code() == -1);
    
    // move construct
    service_error e3(move(e));
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    
    CHECK(string(e3.what()).find("TOUPPER") != string::npos);
    CHECK(string(e3.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e3.user_code() == -1);
    
    
    // move assign
    service_error e4("SVC", 9001);
    e4 = move(e);
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    
    CHECK(string(e4.what()).find("TOUPPER") != string::npos);
    CHECK(string(e4.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e4.user_code() == -1);   
}

TEST_CASE("service_error with buffer")
{
    buffer b("STRING", nullptr, 36);
    strcpy(b.data(), "error message");
    
    service_error e("TOUPPER", -1, move(b));
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().type() == "STRING");
    CHECK(e.buffer().data() == string("error message"));
    
    // copy construct
    service_error e1(e);
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().type() == "STRING");
    CHECK(e.buffer().data() == string("error message"));
    
    CHECK(string(e1.what()).find("TOUPPER") != string::npos);
    CHECK(string(e1.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e1.user_code() == -1);
    CHECK(e1.buffer().type() == "STRING");
    CHECK(e1.buffer().data() == string("error message"));
    
    // copy assign
    service_error e2("TOLOWER", 1, buffer("FML32"));
    e2 = e;
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().type() == "STRING");
    CHECK(e.buffer().data() == string("error message"));
    
    CHECK(string(e2.what()).find("TOUPPER") != string::npos);
    CHECK(string(e2.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e2.user_code() == -1);
    CHECK(e2.buffer().type() == "STRING");
    CHECK(e2.buffer().data() == string("error message"));
    
    // move construct
    service_error e3(move(e));
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().type() == "STRING");
    CHECK(e.buffer().data() == string("error message"));
    
    CHECK(string(e3.what()).find("TOUPPER") != string::npos);
    CHECK(string(e3.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e3.user_code() == -1);
    CHECK(e3.buffer().type() == "STRING");
    CHECK(e3.buffer().data() == string("error message"));
    
    
    // move assign
    service_error e4("SVC", 9001, buffer("XML", nullptr, 1024));
    e4 = move(e);
    CHECK(string(e.what()).find("TOUPPER") != string::npos);
    CHECK(string(e.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e.user_code() == -1);
    CHECK(e.buffer().type() == "STRING");
    CHECK(e.buffer().data() == string("error message"));
    
    CHECK(string(e4.what()).find("TOUPPER") != string::npos);
    CHECK(string(e4.what()).find("TPESVCFAIL") != string::npos);
    CHECK(e4.user_code() == -1);
    CHECK(e4.buffer().type() == "STRING");
    CHECK(e4.buffer().data() == string("error message"));
    
    // extract buffer
    // *buffer is stored as shared_ptr,
    //  so all copies are affected
    buffer b1 = e.move_buffer();
    CHECK(b1.type() == "STRING");
    CHECK(b1.data() == string("error message"));
    CHECK(e.buffer().data() == nullptr);
    CHECK(e1.buffer().data() == nullptr);
    CHECK(e2.buffer().data() == nullptr);
    CHECK(e3.buffer().data() == nullptr);
    CHECK(e4.buffer().data() == nullptr);
    
}


TEST_SUITE_END();