#include <iostream>
#include <sstream>
#include "doctest.h"
#include "tux/xml.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("xml");

TEST_CASE("xml constructors")
{
    SUBCASE("default construct")
    {
        xml x;
        CHECK(x.data() == nullptr);
        CHECK((bool)x == false);
        CHECK(x.size() == 0);
    }
    
    SUBCASE("construct from std::string")
    {
        string s{"<tag>hello world</tag>"};
        xml x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
        CHECK(x.to_string() == s);
        CHECK(x.buffer().type() == "XML");
        CHECK(x.buffer().subtype() == "");
        CHECK(x.buffer().size() >= x.size());
    }
    
    SUBCASE("construct from char ptr")
    {
        const char* s = "<root><tag>foo</tag></root>";
        xml x{s};
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == s);
        CHECK(x.size() == strlen(s));
        CHECK(x.to_string() == s);
    }
    
    SUBCASE("copy construct")
    {
        xml y{"<example>a moderately long string example</example>"};
        xml x{y};
        CHECK(y.data() != nullptr);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.to_string() == y.to_string());
    }
    
    SUBCASE("move construct")
    {
        string s{"<example>a moderately long string example</example>"};
        xml y{s};
        xml x{move(y)};
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.to_string() == s);
    }
    
    SUBCASE("move construct from buffer")
    {
        string s("<example>a moderately long string example</example>");
        buffer b("XML", nullptr, s.size() + 1);
        memmove(b.data(), s.c_str(), s.size() + 1);
        b.data_size(s.size() + 1);
        
        xml x{move(b)};
        
        CHECK(b.data() == nullptr);
        CHECK(b.size() == 0);
        CHECK(x.data() != nullptr);
        CHECK((bool)x == true);
        CHECK(x.size() == s.size());
        CHECK(x.to_string() == s);
        
        buffer b2;
        b2.alloc("STRING");
        CHECK_THROWS(xml(move(b2)));
    }
}

TEST_CASE("xml assignment")
{
    
    SUBCASE("assignment from std::string")
    {
        xml x{"foo"};
        string s{"<root>hello world</root>"};
        xml& ret = (x = s);
        CHECK(&ret == &x);
        CHECK(s == "<root>hello world</root>");
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("assignment from char ptr")
    {
        xml x{string{"<root>hello world</root>"}};
        xml& ret = (x = "<root>foo</root>");
        CHECK(&ret == &x);
        CHECK(x.to_string() == "<root>foo</root>");
        CHECK(x.size() == strlen("<root>foo</root>"));
    }
    
    SUBCASE("copy assign")
    {
        string s{"<example>a moderately long string example</example>"};
        xml y{s};
        xml x;
        xml& ret = (x = y);
        CHECK(&ret == &x);
        CHECK(y.to_string() == s);
        CHECK(x.to_string() == s);
    }
    
    SUBCASE("move assign")
    {
        string s{"<example>a moderately long string example</example>"};
        xml y{s};
        xml x{"<root>foo</root>"};
        x = move(y);
        CHECK(y.data() == nullptr);
        CHECK(y.size() == 0);
        CHECK(x.to_string() == s);
        CHECK(x.size() == s.size());
    }
    
    SUBCASE("move assign from buffer")
    {
        string s("<example>a moderately long string example</example>");
        buffer b("XML", nullptr, s.size() + 1);
        memmove(b.data(), s.c_str(), s.size() + 1);
        b.data_size(s.size() + 1);
        
        xml x{"<root>foo</root>"};
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


TEST_CASE("xml append")
{
    
    SUBCASE("append std::string")
    {
        string s1{"<root>hello"};
        string s2{" "};
        string s3{"world</root>"};
        xml x;
        x += s1;
        x += s2;
        x += s3;
        CHECK(x.to_string() == (s1 + s2 + s3));
    }
    
    SUBCASE("append char ptr")
    {
        xml x{"<root>he"};
        (((x += "ll") += "o ") += "wo") += "rld</root>";
        CHECK(x.to_string() == "<root>hello world</root>");
    }
    
    SUBCASE("append xml")
    {
        xml x;
        x += xml{"<root>a moderately "};
        x += xml{"long string</root>"};
        CHECK(x.to_string() == "<root>a moderately long string</root>");
    }
}

TEST_CASE("xml move buffer")
{
    xml x{"<root>hello world</root>"};
    buffer b = x.move_buffer();
    CHECK(x.data() == nullptr);
    CHECK(x.buffer().data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().size() == 0);
    CHECK(string(b.data(), b.data_size() - 1) == "<root>hello world</root>");
    CHECK(b.type() == "XML");
    CHECK(b.subtype() == "");
    CHECK(b.data_size() - 1 == strlen("<root>hello world</root>"));
    CHECK(b.size() >= b.data_size());
}

TEST_CASE("xml to_string")
{
    xml x("<root>hello world</root>");
    CHECK(x.to_string() == "<root>hello world</root>");
    xml n;
    CHECK(n.to_string() == "");
}

TEST_CASE("xml reserve")
{
    xml x;
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    x.reserve(1024);
    CHECK(x.data() == nullptr);
    CHECK(x.size() == 0);
    CHECK(x.buffer().data_size() == 0);
    CHECK(x.buffer().size() >= 1024);
    
    CHECK_THROWS(buffer("XML", nullptr, 0));
    xml y;
    CHECK_NOTHROW(y.reserve(0));
    CHECK(y.size() == 0);
    CHECK(y.buffer().size() > 0);
}


TEST_CASE("xml stream insertion")
{
    xml x;
    xml y("<root>hello world</root>");
    ostringstream s;
    s << x;
    CHECK(s.str() == "");
    s << y;
    CHECK(s.str() == "<root>hello world</root>");
}

TEST_CASE("xml growth")
{
    xml x("hello");
    long current_buffer_size = x.buffer().size();
    string long_str(current_buffer_size * 2, 'b');
    x += long_str;
    CHECK(x.size() == (5 + long_str.size()));
    CHECK(x.buffer().size() >= x.size());
}

TEST_SUITE_END();