#include "doctest.h"
#include "tux/util.hpp"
#include "Uunix.h"

using namespace std;
using namespace tux;

TEST_SUITE("util");

TEST_CASE("util clamp")
{
    CHECK(clamp(35.5, 1.0, 100.0) == doctest::Approx(35.5));
    CHECK(clamp(40, 50, 60) == 50);
    CHECK(clamp(30, 1, 10) == 10);
}

TEST_CASE("util trim_to_null_terminator")
{
    string s = "abcdefghijklmnop     ";
    string tmp;
    
    tmp = s;
    tmp[0] = 0;
    trim_to_null_terminator(tmp);
    CHECK(tmp == "");
    
    tmp = s;
    tmp[5] = 0;
    trim_to_null_terminator(tmp);
    CHECK(tmp == "abcde");
    
    tmp = s;
    tmp[18] = 0;
    trim_to_null_terminator(tmp);
    CHECK(tmp == "abcdefghijklmnop  ");    
}

TEST_CASE("util rtrim")
{
    string s;
    
    s = "hello    ";
    rtrim(s);
    CHECK(s == "hello");
    
    s = "    hello    ";
    rtrim(s);
    CHECK(s == "    hello");
    
    s = "hello";
    rtrim(s);
    CHECK(s == "hello");
    
    s = "";
    rtrim(s);
    CHECK(s == "");
    
    s = "           ";
    rtrim(s);
    CHECK(s == "");
}

TEST_CASE("util compare")
{
    string a,b;
    int result = 0;
    
    SUBCASE("hello == hello")
    {
        a = "hello";
        b = a;
        result = compare(a.data(), a.size(), b.data(), b.size());
        CHECK(result == 0);
    }
    SUBCASE("hello > Hello")
    {
        a = "hello";
        b = "Hello";
        result = compare(a.data(), a.size(), b.data(), b.size());
        CHECK(result == 1);      
    }
    SUBCASE("Z > A")
    {
        a = "Z";
        b = "A";
        result = compare(a.data(), a.size(), b.data(), b.size());
        CHECK(result == 1);
    }
    SUBCASE("123 < 345")
    {
        a = "123";
        b = "345";
        result = compare(a.data(), a.size(), b.data(), b.size());
        CHECK(result == -1);
    }
    SUBCASE("null < \"\"")
    {
        b = "";
        result = compare(nullptr, 0, b.data(), b.size());
        CHECK(result == -1);
    }
    SUBCASE("a > null")
    {
        a = "a";
        result = compare(a.data(), a.size(), nullptr, 0);
        CHECK(result == 1);
    }
    SUBCASE("null == null")
    {
        result = compare(nullptr, 0, nullptr, 0);
        CHECK(result == 0);
    }
}

TEST_CASE("util cstr_to_string")
{
    CHECK(cstr_to_string("hello world") == string{"hello world"});
    CHECK(cstr_to_string(nullptr) == string{});
}

TEST_CASE("util init")
{
    struct my_type
    {
        char c;
        double d;
        int i;
        char str[10];
    };
    
    my_type x;
    init(x);
    CHECK(x.c == 0);
    CHECK(x.d == 0.00);
    CHECK(x.i == 0);
    int len = strlen(x.str);
    CHECK(len == 0);
}

TEST_CASE("util make_default")
{
    struct my_type
    {
        char c;
        double d;
        int i;
        char str[10];
    };
    
    my_type x;
    init(x);
    my_type y = make_default<my_type>();
    int result = memcmp(&x, &y, sizeof(x));
    CHECK(result == 0);
}

TEST_CASE("util set")
{
    char str[6] = {0,0,0,0,0,0};
    string s;
    
    s = "hello";
    set(str, s);
    CHECK(str == s);

    s = "world";
    set(str, s);
    CHECK(str == s);
    
    s = "i'm just a poor boy though my story's seldom told";
    set(str, s);
    string post = "i'm j";
    CHECK(str == post);
}

TEST_CASE("util make_16bit_unsigned")
{
    uint16_t i16 = 1;
    uint8_t* i8 = reinterpret_cast<uint8_t*>(&i16);
    bool is_little_endian = i8[0] == 1;
    size_t lsb = is_little_endian ? 0 : 1;
    size_t msb = is_little_endian ? 1 : 0;
    uint16_t result = 0;

    i16 = 0;
    result = make_16bit_unsigned(i8[lsb], i8[msb]);
    CHECK(result == i16);
    
    i16 = 15;
    result = make_16bit_unsigned(i8[lsb], i8[msb]);
    CHECK(result == i16);
    
    i16 = 2048;
    result = make_16bit_unsigned(i8[lsb], i8[msb]);
    CHECK(result == i16);
    
    i16 = 60000;
    result = make_16bit_unsigned(i8[lsb], i8[msb]);
    CHECK(result == i16);
    
}

TEST_CASE("util env")
{
    put_env("HELLO", "WORLD");
    CHECK(get_env("HELLO") == "WORLD");
}

TEST_CASE("util build_error_string")
{
    Uunixerr = UOPEN;
    string result = build_error_string("my_app_function", TPEOS, TPED_DOMAINUNREACHABLE);
    CHECK(result.find("my_app_function") != result.npos);
    CHECK(result.find("TPEOS") != result.npos);
    CHECK(result.find(tpstrerror(TPEOS)) != result.npos);
    CHECK(result.find("TPED_DOMAINUNREACHABLE") != result.npos);
    CHECK(result.find(tpstrerror(TPEOS)) != result.npos);
    CHECK(result.find(Uunixmsg[UOPEN]) != result.npos); 
}

TEST_CASE("util error")
{
    error e(TPEINVAL, 0, "msg");
    CHECK(e.code() == TPEINVAL);
    CHECK(e.detail_code() == 0);
    
    string str = e.str();
    CHECK(str.find("TPEINVAL") != str.npos);
    CHECK(str == cstr_to_string(tpstrerror(TPEINVAL)));
    
    string dtlstr = e.detail_str();
    CHECK(dtlstr == "");
    CHECK(dtlstr == cstr_to_string(tpstrerrordetail(0, TPNOFLAGS)));
    
    string what = e.what();
    CHECK(what.find("msg") != what.npos);      
}

TEST_CASE("util last_error")
{
    tperrno = TPEABORT;
    error e = last_error("my context");
    CHECK(e.code() == TPEABORT);
    string what = e.what();
    CHECK(what.find("TPEABORT") != what.npos);
    CHECK(what.find("my context") != what.npos);
}

TEST_CASE("util get/set block_time")
{
    // reset
    set_block_time(block_time_scope::all, chrono::seconds{0});
    CHECK(get_block_time(block_time_scope::all) == chrono::seconds{0});
    
    set_block_time(block_time_scope::next, chrono::seconds{0});
    CHECK(get_block_time(block_time_scope::next) == chrono::seconds{0});
    
    
    SUBCASE("throws on huge block time")
    {
        CHECK_THROWS(set_block_time(block_time_scope::all, chrono::seconds{numeric_limits<int>::max()}));
    }
    
    
    SUBCASE("set all")
    {
        SUBCASE("in seconds")
        {
            set_block_time(block_time_scope::all, chrono::seconds{2});
            CHECK(tpgblktime(TPBLK_ALL | TPBLK_MILLISECOND) == 2000);
            CHECK(get_block_time(block_time_scope::all) == chrono::seconds{2});
            CHECK(get_block_time(block_time_scope::next) == chrono::seconds{0});
            CHECK(get_block_time() == chrono::seconds{2});
        }
        
        SUBCASE("in milliseconds")
        {
            set_block_time(block_time_scope::all, chrono::milliseconds{500});
            CHECK(tpgblktime(TPBLK_ALL | TPBLK_MILLISECOND) == 500);
            CHECK(get_block_time(block_time_scope::all) == chrono::milliseconds{500});
            CHECK(get_block_time(block_time_scope::next) == chrono::seconds{0});
            CHECK(get_block_time() == chrono::milliseconds{500});
        }
    }
    
    // reset
    set_block_time(block_time_scope::all, chrono::seconds{0});
    set_block_time(block_time_scope::next, chrono::seconds{0});

    SUBCASE("set next")
    {
        SUBCASE("in seconds")
        {
            set_block_time(block_time_scope::next, chrono::seconds{2});
            CHECK(tpgblktime(TPBLK_NEXT | TPBLK_MILLISECOND) == 2000);
            CHECK(get_block_time(block_time_scope::all) == chrono::seconds{0});
            CHECK(get_block_time(block_time_scope::next) == chrono::seconds{2});
            CHECK(get_block_time() == chrono::seconds{2});
        }
        
        SUBCASE("in milliseconds")
        {
            set_block_time(block_time_scope::next, chrono::milliseconds{500});
            CHECK(tpgblktime(TPBLK_NEXT | TPBLK_MILLISECOND) == 500);
            CHECK(get_block_time(block_time_scope::all) == chrono::seconds{0});
            CHECK(get_block_time(block_time_scope::next) == chrono::milliseconds{500});
            CHECK(get_block_time() == chrono::milliseconds{500});
        }
    }
    
    // reset
    set_block_time(block_time_scope::all, chrono::seconds{0});
    set_block_time(block_time_scope::next, chrono::seconds{0});

    SUBCASE("set both")
    {
        set_block_time(block_time_scope::all, chrono::milliseconds{500});
        set_block_time(block_time_scope::next, chrono::seconds{2});
        CHECK(get_block_time(block_time_scope::all) == chrono::milliseconds{500});
        CHECK(get_block_time(block_time_scope::next) == chrono::seconds{2});
        CHECK(get_block_time() == chrono::seconds{2});
    }
}

TEST_CASE("util set/adjust/get priority")
{
    // priority adjustments/sets apply to the next call
    // default priority is 50
    // have to make a call to get the priority back
    set_priority(55);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 55);
    CHECK(get_priority() == tpgprio());
    
    adjust_priority(10);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 60);
    CHECK(get_priority() == tpgprio());
    
    adjust_priority(-20);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 30);
    CHECK(get_priority() == tpgprio());
    
    // adjust is not cumulative
    // (in below context)
    adjust_priority(10);
    adjust_priority(10);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 60);
    CHECK(get_priority() == tpgprio());
    
    // value should be 1 - 100
    // over-adjustments are clamped (by tuxedo)
    adjust_priority(200);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 100);
    CHECK(get_priority() == tpgprio());
    
    adjust_priority(-75);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 1);
    CHECK(get_priority() == tpgprio());
    
    // sets with bad values are clamped (by tuxpp)
    set_priority(150);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 100);
    CHECK(get_priority() == tpgprio());
    
    set_priority(-8000);
    tpacall(const_cast<char*>(".TMIB"), nullptr, 0, TPNOREPLY);
    CHECK(get_priority() == 1);
    CHECK(get_priority() == tpgprio());
    
    tpterm();
}

TEST_SUITE_END();

// TODO
// integrate C api calls into unit tests?
// proves C++ api works, and also
// demonstrates what the C++ api is doing

// TODO
// benchmarks
// there are some frameworks (google benchmark, celero, hayai)
// might consider something handrolled to avoid dependencies?