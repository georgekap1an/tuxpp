#include <iostream>
#include <sstream>
#include "doctest.h"
#include "tux/carray.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("carray");

TEST_CASE("carray constructors")
{
    SUBCASE("default construct")
    {
        carray x;
        CHECK(x.data() == nullptr);
        CHECK((bool)x == false);
        CHECK(x.size() == 0);
    }
    
    SUBCASE("construct from std::string")
    {
        string s{"hello world"};
        carray x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
        CHECK(*x.begin() == 'h');
        CHECK(x.end() == (x.begin() + x.size()));
        CHECK(*(x.end()-1) == 'd');
        CHECK(x.buffer().type() == "CARRAY");
        CHECK(x.buffer().subtype() == "");
        CHECK(x.buffer().size() >= x.size());
    }
    
    SUBCASE("construct from char ptr")
    {
        const char* s = "foo";
        carray x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == s);
        CHECK(x.size() == 3);
        CHECK(*x.begin() == 'f');
        CHECK(*(x.begin() + 1) == 'o');
        CHECK(*(x.begin() + 2) == 'o');
    }
    
    SUBCASE("copy construct")
    {
        carray y{"a moderately long string example"};
        carray x{y};
        CHECK(y.data() != nullptr);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == y.to_string());
    }
    
    SUBCASE("move construct")
    {
        string s{"a moderately long string example"};
        carray y{s};
        carray x{move(y)};
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.to_string() == s);
    }
    
    SUBCASE("move construct from buffer")
    {
        string s("a moderately long string example");
        buffer b("CARRAY", nullptr, s.size());
        memmove(b.data(), s.c_str(), s.size());
        b.data_size(s.size());
        
        carray x{move(b)};
        
        CHECK(b.data() == nullptr);
        CHECK(b.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.to_string() == s);
        
        buffer b2;
        b2.alloc("STRING");
        CHECK_THROWS(carray(move(b2)));
    }
}

TEST_CASE("carray assignment")
{
    
    SUBCASE("assignment from std::string")
    {
        carray x{"foo"};
        string s{"hello world"};
        carray& ret = (x = s);
        CHECK(&ret == &x);
        CHECK(s == "hello world");
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("assignment from char ptr")
    {
        carray x{string{"hello world"}};
        carray& ret = (x = "foo");
        CHECK(&ret == &x);
        CHECK(x.to_string() == "foo");
        CHECK(x.size() == 3);
    }
    
    SUBCASE("copy assign")
    {
        string s{"a moderately long string example"};
        carray y{s};
        carray x;
        carray& ret = (x = y);
        CHECK(&ret == &x);
        CHECK(y.to_string() == s);
        CHECK(x.to_string() == s);
    }
    
    SUBCASE("move assign")
    {
        string s{"a moderately long string example"};
        carray y{s};
        carray x{"foo"};
        x = move(y);
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("move assign from buffer")
    {
        string s("a moderately long string example");
        buffer b("CARRAY", nullptr, s.size());
        memmove(b.data(), s.c_str(), s.size());
        b.data_size(s.size());
        
        carray x{"foo"};
        x = move(b);
        
        CHECK(b.data() == nullptr);
        CHECK(b.size() == 0);
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
        
        buffer b2;
        b2.alloc("STRING");
        CHECK_THROWS(x = move(b2));
    }
}


TEST_CASE("carray append")
{
    
    SUBCASE("append std::string")
    {
        string s1{"hello"};
        string s2{" "};
        string s3{"world"};
        carray x;
        x += s1;
        x += s2;
        x += s3;
        CHECK(x.to_string() == (s1 + s2 + s3));
    }
    
    SUBCASE("append char ptr")
    {
        carray x{"he"};
        (((x += "ll") += "o ") += "wo") += "rld";
        CHECK(x.to_string() == "hello world");
    }
    
    SUBCASE("append carray")
    {
        carray x;
        x += carray{"a moderately "};
        x += carray{"long string"};
        CHECK(x.to_string() == "a moderately long string");
    }
}

TEST_CASE("carray move buffer")
{
    carray x{"hello world"};
    buffer b = x.move_buffer();
    CHECK(x.data() == nullptr);
    CHECK(x.buffer().data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().size() == 0);
    CHECK(string(b.data(), b.data_size()) == "hello world");
    CHECK(b.type() == "CARRAY");
    CHECK(b.subtype() == "");
    CHECK(b.data_size() == strlen("hello world"));
    CHECK(b.size() >= b.data_size());
}

TEST_CASE("carray to_string")
{
    carray x("hello world");
    CHECK(x.to_string() == "hello world");
    carray n;
    CHECK(n.to_string() == "");
}

TEST_CASE("carray reserve")
{
    carray x;
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    x.reserve(1024);
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().data_size() == 0);
    CHECK(x.buffer().size() >= 1024);
    
    CHECK_THROWS(buffer("CARRAY", nullptr, 0));
    carray y;
    CHECK_NOTHROW(y.reserve(0));
    CHECK(y.size() == 0);
    CHECK(y.buffer().size() > 0);
}

TEST_CASE("carray concatenation")
{
    carray a("hello ");
    carray b("world");
    carray c = a + b;
    CHECK(a == "hello ");
    CHECK(b == "world");
    CHECK(c == "hello world");
}


TEST_CASE("carray compare")
{
    SUBCASE("carray,carray")
    {
        carray n1;
        carray n2;
        carray x{"hello"};
        carray y{"world"};
        carray z{"hello"};
        CHECK(compare(n1, n2) == 0);
        CHECK(compare(n2, n1) == 0);
        CHECK(compare(n2, x) == -1);
        CHECK(compare(x, n2) == 1);
        CHECK(compare(x,y) == -1);
        CHECK(compare(y,x) == 1);
        CHECK(compare(x,z) == 0);
        CHECK(n1 == n2);
        CHECK(n1 >= n2);
        CHECK(n2 != x);
        CHECK(n2 < x);
        CHECK(x > n2);
        CHECK(x >= n2);
        CHECK(x != y);
        CHECK(y != x);
        CHECK(x < y);
        CHECK(y > x);
        CHECK(x == z);
        CHECK(z == x);     
    }
    
    SUBCASE("carray,std::string")
    {
        carray n;
        string e;
        carray x{"hello"};
        string y{"world"};
        string z{"hello"};
        CHECK(compare(n, e) == -1);
        CHECK(compare(e, n) == 1);
        CHECK(compare(e, x) == -1);
        CHECK(compare(x, e) == 1);
        CHECK(compare(x,y) == -1);
        CHECK(compare(y,x) == 1);
        CHECK(compare(x,z) == 0);
        CHECK(e != n);
        CHECK(e != x);
        CHECK(x != y);
        CHECK(y != x);
        CHECK(x == z);
        CHECK(z == x);
    }
    
    SUBCASE("carray,char ptr")
    {
        carray n1;
        const char* n2 = nullptr;
        const char* n3 = "";
        carray x{"hello"};
        const char* y = "world";
        const char* z = "hello";
        CHECK(compare(n1, n2) == 0);
        CHECK(compare(n2, n1) == 0);
        CHECK(compare(n3, n1) == 1);
        CHECK(compare(n2, x) == -1);
        CHECK(compare(n3, x) == -1);
        CHECK(compare(x, n2) == 1);
        CHECK(compare(x, n3) == 1);
        CHECK(compare(x,y) == -1);
        CHECK(compare(y,x) == 1);
        CHECK(compare(x,z) == 0);
        CHECK(n1 == n2);
        CHECK(n1 != n3);
        CHECK(n2 != x);
        CHECK(n3 != x);
        CHECK(x != y);
        CHECK(y != x);
        CHECK(x == z);
        CHECK(z == x);
    }
}


TEST_CASE("carray stream insertion")
{
    carray x;
    carray y("hello world");
    ostringstream s;
    s << x;
    CHECK(s.str() == "");
    s << y;
    CHECK(s.str() == "hello world");
}

TEST_CASE("carray growth")
{
    carray x("hello");
    long current_buffer_size = x.buffer().size();
    string long_str(current_buffer_size * 2, 'b');
    x += long_str;
    CHECK(x.size() == (5 + long_str.size()));
    CHECK(x.buffer().size() >= x.size());
}

TEST_SUITE_END();
