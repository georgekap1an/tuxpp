#include "doctest.h"
#include "tux/view16.hpp"
#include "tux/util.hpp"
#include "views16.h"

using namespace std;
using namespace tux;
template<typename T> using view = view16<T>;

TEST_SUITE("view16");

TEST_CASE("view16 size by name")
{
    CHECK(view_size16("my_struct") == sizeof(my_struct));
    CHECK(view_size16("string_info") == sizeof(string_info));
    CHECK(view_size16(string("string_info")) == sizeof(string_info));
}

// refresh definitions
// ... kind of tough to test ... would have to run system commands?
// (edit file, recompile ...)
// maybe not worth testing as the wrapper is trivial

// set mapping options (this should be tested when we test conversions later)

TEST_CASE("view16 default constructor")
{
    view<string_info> v;
    CHECK(v.buffer().data() == nullptr);
    CHECK((bool)v == false);
}

TEST_CASE("view16 copy constructor")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2(v1);
    CHECK(v1->ascii_sum == 3);
    CHECK(v1->byte_count == 10);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
}

TEST_CASE("view16 move constructor")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2(move(v1));
    CHECK(v1.buffer().data() == nullptr);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
}

TEST_CASE("view16 copy assign")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2;
    v2 = v1;
    CHECK(v1->ascii_sum == 3);
    CHECK(v1->byte_count == 10);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
}

TEST_CASE("view16 move assign")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2;
    v2 = move(v1);
    CHECK(v1.buffer().data() == nullptr);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
}

TEST_CASE("view16 move construct from buffer")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2(v1.move_buffer());
    CHECK(v1.buffer().data() == nullptr);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
    buffer b;
    CHECK(b.data() == nullptr);
    // any null buffer can be moved
    CHECK_NOTHROW(view<string_info>(move(b)));
    view<my_struct> v3;
    CHECK_NOTHROW(view<string_info>(v3.move_buffer()));
    
    // buffer of the wrong type or subtype throws
    b.alloc("STRING");
    CHECK_THROWS(view<string_info>(move(b)));
    v3.alloc();
    CHECK_THROWS(view<string_info>(v3.move_buffer()));
}

TEST_CASE("view16 move assign from buffer")
{
    view<string_info> v1;
    v1->ascii_sum = 3;
    v1->byte_count = 10;
    view<string_info> v2;
    v2 = v1.move_buffer();
    CHECK(v1.buffer().data() == nullptr);
    CHECK(v2->ascii_sum == 3);
    CHECK(v2->byte_count == 10);
    buffer b;
    CHECK(b.data() == nullptr);
    // any null buffer can be moved
    CHECK_NOTHROW(v2 = move(b));
    CHECK(v2.buffer().data() == nullptr);
    view<my_struct> v3;
    CHECK_NOTHROW(v2 = v3.move_buffer());
    CHECK(v2.buffer().data() == nullptr);
    
    // buffer of the wrong type or subtype throws
    b.alloc("STRING");
    CHECK_THROWS(v2 = move(b));
    v3.alloc();
    CHECK_THROWS(v2 = v3.move_buffer());
}

TEST_CASE("view16 manual alloc and free")
{
    view16<string_info> v;
    CHECK(v.buffer().data() == nullptr);
    v.alloc();
    CHECK(v.buffer().data() != nullptr);
    CHECK(v.buffer().data_size() == sizeof(string_info));
    CHECK(v.buffer().type() == "VIEW");
    CHECK(v.buffer().subtype() == "string_info");
    v.free();
    CHECK(v.buffer().data() == nullptr);
    CHECK(v.buffer().data_size() == 0);
    CHECK(v.buffer().type() == "");
    CHECK(v.buffer().subtype() == "");
}

TEST_CASE("view16 clear")
{
    view16<string_info> v;
    v->ascii_sum = 25;
    v->byte_count = 50;
    set(v->original_string,"hello world");
    
    SUBCASE("clear all")
    {
        v.clear();
        CHECK(v->ascii_sum == 0);
        CHECK(v->byte_count == 0);
        CHECK(v->original_string == string());
    }
    
    SUBCASE("clear one field")
    {
        v.clear("ascii_sum");
        CHECK(v->ascii_sum == 0);
        CHECK(v->byte_count == 50);
        CHECK(v->original_string == string("hello world"));
    }
    
    SUBCASE("check for null")
    {
        CHECK(v.is_null("ascii_sum") == false);
        CHECK(v.is_null("byte_count") == false);
        CHECK(v.is_null("original_string") == false);
        CHECK(v.is_null("most_frequent_char") == true);
        CHECK_THROWS(v.is_null("bad field name"));
    }
}

TEST_CASE("view16::boolean_expression")
{
    view<string_info>::boolean_expression e;
    string_info s = make_default<string_info>();
    
    CHECK_THROWS(e(s));
    
    // basic numeric check
    e = "byte_count > 50";
    s.byte_count = 51;
    CHECK(e(s) == true);
    s.byte_count = 50;
    CHECK(e(s) == false);
    e = "byte_count == 5";
    s.byte_count = 5;
    CHECK(e(s) == true);
    
    // basic string check
    e = "original_string == 'hello world'";
    set(s.original_string, "hello world");
    CHECK(e(s) == true);
    set(s.original_string, "hello WORLD");
    CHECK(e(s) == false);
    
    e = "original_string > 'apple'";
    set(s.original_string, "orange");
    CHECK(e(s) == true);
    
    // regex
    e = "original_string %% 'hello.*'";
    set(s.original_string, "hello world");
    CHECK(e(s) == true);
    set(s.original_string, "hello Tuxedo");
    CHECK(e(s) == true);
    e = "original_string %% 'Jack|Jill'";
    set(s.original_string, "Jack");
    CHECK(e(s) == true);
    set(s.original_string, "Jill");
    CHECK(e(s) == true);
    set(s.original_string, "John");
    CHECK(e(s) == false);
    
    // arithmetic
    e = "byte_count + ascii_sum == 10";
    s.byte_count = 5;
    s.ascii_sum = 4;
    CHECK(e(s) == false);
    s.byte_count = 0;
    s.ascii_sum = 10;
    CHECK(e(s) == true);
    s.byte_count = 5;
    s.ascii_sum = 5;
    CHECK(e(s) == true);
    
    e = "ascii_sum - byte_count <= 2";
    s.ascii_sum = 40;
    s.byte_count = 36;
    CHECK(e(s) == false);
    s.ascii_sum = 38;
    s.byte_count = 36;
    CHECK(e(s) == true);
    
    e = "ascii_sum * byte_count > 25";
    s.ascii_sum = 5;
    s.byte_count = 4;
    CHECK(e(s) == false);
    s.ascii_sum = 6;
    s.byte_count = 6;
    CHECK(e(s) == true);
    
    e = "ascii_sum / byte_count < 3";
    s.ascii_sum = 100;
    s.byte_count = 25;
    CHECK(e(s) == false);
    s.ascii_sum = 100;
    s.byte_count = 50;
    CHECK(e(s) == true);
    
    // parenthesis
    e = "ascii_sum == 50 || byte_count == 4";
    s.ascii_sum = 49;
    s.byte_count = 3;
    CHECK(e(s) == false);
    s.ascii_sum = 50;
    s.byte_count = 3;
    CHECK(e(s) == true);
    s.ascii_sum = 49;
    s.byte_count = 4;
    CHECK(e(s) == true);
    
    e = "original_string == 'hello world' && (ascii_sum == 50 || byte_count == 4)";
    set(s.original_string, "hello world");
    s.ascii_sum = 49;
    s.byte_count = 3;
    CHECK(e(s) == false);
    s.ascii_sum = 50;
    CHECK(e(s) == true);
    
    CHECK_THROWS(e = "bad_name == 2");
    
    // use with views
    view<string_info> v;
    v->ascii_sum = 235;
    e = "ascii_sum == 235";
    CHECK(e(*v) == true);
}

TEST_CASE("view16::arithmetic_expression")
{
    view<string_info>::arithmetic_expression e;
    string_info s = make_default<string_info>();
    
    s.ascii_sum = 200;
    s.byte_count = 47;
    e = "ascii_sum + byte_count";
    CHECK((int)e(s) == 247);
    e = "ascii_sum / byte_count";
    CHECK((int)e(s) == 4);
    
    view<my_struct>::arithmetic_expression e1;
    my_struct s1 = make_default<my_struct>();
    
    s1.d = 100.00;
    e1 = "d / 3";
    CHECK(e1(s1) == doctest::Approx(33.33333));
}

TEST_CASE("view16 comparison")
{
    view<string_info> v1;
    view<string_info> v2;
    CHECK(v1 == v2);
    
    v1.alloc();
    v2.alloc();
    CHECK(v1 == v2);
    
    v1->ascii_sum = 2;
    CHECK(v1 != v2);
    
    v2->ascii_sum = 2;
    CHECK(v1 == v2);
}


TEST_SUITE_END(); 
