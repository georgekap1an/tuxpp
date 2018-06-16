#include <iostream>
#include <sstream>
#include "doctest.h"
#include "tux/mbstring.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

// asan warns that tpalloc for mbstring
// performs strcpy with overlapping
// memory segments

TEST_SUITE("mbstring");

TEST_CASE("mbstring constructors asan=replace_str")
{
    SUBCASE("default construct")
    {
        mbstring x;
        CHECK(x.data() == nullptr);
        CHECK((bool)x == false);
        CHECK(x.size() == 0);
    }
    
    SUBCASE("construct from std::string")
    {
        string s{"hello world"};
        mbstring x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.data() == s);
        CHECK(x.size() == s.size());
        CHECK(*x.begin() == 'h');
        CHECK(x.end() == (x.begin() + x.size()));
        //CHECK(*x.end() == (char)0);
        CHECK(x.buffer().type() == "MBSTRING");
        CHECK(x.buffer().subtype() == "");
        CHECK(x.buffer().size() >= x.size());
    }
    
    SUBCASE("construct from char ptr")
    {
        const char* s = "foo";
        mbstring x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(strcmp(x.data(), s) == 0);
        CHECK(x.size() == 3);
        CHECK(*x.begin() == 'f');
        CHECK(*(x.begin() + 1) == 'o');
        CHECK(*(x.begin() + 2) == 'o');
        CHECK(*(x.begin() + 3) == (char)0);
    }
    
    SUBCASE("copy construct")
    {
        mbstring y{"a moderately long string example"};
        mbstring x{y};
        CHECK(y.data() != nullptr);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(strcmp(x.data(), y.data()) == 0);
    }
    
    SUBCASE("move construct")
    {
        string s{"a moderately long string example"};
        mbstring y{s};
        mbstring x{move(y)};
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.data() == s);
    }
    
    SUBCASE("move construct from buffer")
    {
        string s("a moderately long string example");
        buffer b("MBSTRING", nullptr, s.size() + 1);
        memmove(b.data(), s.c_str(), s.size() + 1);
        b.data_size(s.size() + 1);
        
        mbstring x{move(b)};
        
        CHECK(b.data() == nullptr);
        CHECK(b.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.data() == s);
        
        buffer b2;
        b2.alloc("CARRAY", nullptr, 24);
        CHECK_THROWS(mbstring(move(b2)));
    }
}

TEST_CASE("mbstring assignment asan=replace_str")
{
    
    SUBCASE("assignment from std::string")
    {
        mbstring x{"foo"};
        string s{"hello world"};
        mbstring& ret = (x = s);
        CHECK(&ret == &x);
        CHECK(s == "hello world");
        CHECK(x.data() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("assignment from char ptr")
    {
        mbstring x{string{"hello world"}};
        mbstring& ret = (x = "foo");
        CHECK(&ret == &x);
        CHECK(strcmp(x.data(), "foo") == 0);
        CHECK(x.size() == 3);
    }
    
    SUBCASE("copy assign")
    {
        string s{"a moderately long string example"};
        mbstring y{s};
        mbstring x;
        mbstring& ret = (x = y);
        CHECK(&ret == &x);
        CHECK(y.data()  == s);
        CHECK(x.data() == s);
    }
    
    SUBCASE("move assign")
    {
        string s{"a moderately long string example"};
        mbstring y{s};
        mbstring x{"foo"};
        x = move(y);
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.data() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("move assign from buffer")
    {
        string s("a moderately long string example");
        buffer b("MBSTRING", nullptr, s.size() + 1);
        memmove(b.data(), s.c_str(), s.size() + 1);
        b.data_size(s.size() + 1);
        
        mbstring x{"foo"};
        x = move(b);
        
        CHECK(b.data() == nullptr);
        CHECK(b.size() == 0);
        CHECK(x.data() == s);
        CHECK(x.size() == s.size());
        
        buffer b2;
        b2.alloc("CARRAY", nullptr, 24);
        CHECK_THROWS(x = move(b2));
    }
}


TEST_CASE("mbstring append asan=replace_str")
{
    
    SUBCASE("append std::string")
    {
        string s1{"hello"};
        string s2{" "};
        string s3{"world"};
        mbstring x;
        x += s1;
        x += s2;
        x += s3;
        CHECK(x.data() == (s1 + s2 + s3));
    }
    
    SUBCASE("append char ptr")
    {
        mbstring x{"he"};
        (((x += "ll") += "o ") += "wo") += "rld";
        CHECK(strcmp(x.data(), "hello world") == 0);
    }
    
    SUBCASE("append mbstring")
    {
        mbstring x;
        x += mbstring{"a moderately "};
        x += mbstring{"long string"};
        CHECK(strcmp(x.data(), "a moderately long string") == 0);
    }
}

TEST_CASE("mbstring move buffer asan=replace_str")
{
    mbstring x{"hello world"};
    buffer b = x.move_buffer();
    CHECK(x.data() == nullptr);
    CHECK(x.buffer().data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().size() == 0);
    CHECK(strcmp(b.data(), "hello world") == 0);
    CHECK(b.type() == "MBSTRING");
    CHECK(b.subtype() == "");
    CHECK(b.data_size() == strlen("hello world") + 1);
    CHECK(b.size() >= b.data_size());
}

TEST_CASE("mbstring to_string asan=replace_str")
{
    mbstring x("hello world");
    CHECK(x.to_string() == "hello world");
    mbstring n;
    CHECK(n.to_string() == "");
}

TEST_CASE("mbstring reserve asan=replace_str")
{
    mbstring x;
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    x.reserve(1024);
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().data_size() == 0);
    CHECK(x.buffer().size() >= 1024);
}

TEST_CASE("mbstring get/set/clear encoding name asan=replace_str")
{
    mbstring x{"hello world"};
    x.set_encoding_name("EUC-JP");
    CHECK(x.get_encoding_name() == "EUC-JP");
    x.clear_encoding_name();
    CHECK(x.get_encoding_name() == "");
}

TEST_CASE("mbstring convert encoding asan=replace_str")
{
    mbstring x{"hello world"};
    x.set_encoding_name("EUC-JP");
    x.convert_encoding("SHIFT-JIS");
    CHECK(x.get_encoding_name() == "SHIFT-JIS");
}

TEST_CASE("mbstring concatenation asan=replace_str")
{
    mbstring a("hello ");
    mbstring b("world");
    mbstring c = a + b;
    CHECK(a == "hello ");
    CHECK(b == "world");
    CHECK(c == "hello world");
}

TEST_CASE("mbstring compare asan=replace_str")
{
    SUBCASE("mbstring,mbstring")
    {
        mbstring n1;
        mbstring n2;
        mbstring x{"hello"};
        mbstring y{"world"};
        mbstring z{"hello"};
        CHECK(compare(n1, n2) == 0);
        CHECK(compare(n2, n1) == 0);
        CHECK(compare(n2, x) == -1);
        CHECK(compare(x, n2) == 1);
        CHECK(compare(x,y) == -1);
        CHECK(compare(y,x) == 1);
        CHECK(compare(x,z) == 0);
        CHECK(n1 == n2);
        CHECK(n1 <= n2);
        CHECK(n1 >= n2);
        CHECK(n2 != x);
        CHECK(n2 < x);
        CHECK(n2 <= x);
        CHECK(x > n2);
        CHECK(x != y);
        CHECK(y != x);
        CHECK(x < y);
        CHECK(y > x);
        CHECK(x == z);
        CHECK(z == x);     
    }
    
    SUBCASE("mbstring,std::string")
    {
        mbstring n;
        string e;
        mbstring x{"hello"};
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
    
    SUBCASE("mbstring,char ptr")
    {
        mbstring n1;
        const char* n2 = nullptr;
        const char* n3 = "";
        mbstring x{"hello"};
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

TEST_CASE("mbstring stream insertion asan=replace_str")
{
    mbstring x;
    mbstring y("hello world");
    ostringstream s;
    s << x;
    CHECK(s.str() == "");
    s << y;
    CHECK(s.str() == "hello world");
}

TEST_CASE("mbstring growth asan=replace_str")
{
    mbstring x("hello");
    long current_buffer_size = x.buffer().size();
    string long_str(current_buffer_size * 2, 'b');
    x += long_str;
    CHECK(x.size() == (5 + long_str.size()));
    CHECK(x.buffer().size() >= x.size());
}

TEST_SUITE_END();
