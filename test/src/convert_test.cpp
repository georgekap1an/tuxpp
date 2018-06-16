#include <iostream>
#include <sstream>
#include "doctest.h"
#include "tux/convert.hpp"
#include "tux/util.hpp"
#include "fields16.hpp"
#include "fields32.hpp"
#include "views32.h"


using namespace std;
using namespace tux;

TEST_SUITE("convert");

TEST_CASE("convert fml16->fml32")
{
    fml16 f16;
    f16.add(field16::A_SHORT_FIELD, 2);
    f16.add(field16::A_LONG_FIELD, 100);
    f16.add(field16::A_CHAR_FIELD, 'b');
    f16.add(field16::A_FLOAT_FIELD, 13.7);
    f16.add(field16::A_DOUBLE_FIELD, 4096.32);
    f16.add(field16::A_STRING_FIELD, "hello world");
    f16.add(field16::A_CARRAY_FIELD, "hello dolly");
    
    fml32 f32 = to_fml32(f16);
    CHECK(f32.get_short(field32::A_SHORT_FIELD) == 2);
    CHECK(f32.get_long(field32::A_LONG_FIELD) == 100);
    CHECK(f32.get_char(field32::A_CHAR_FIELD) == 'b');
    CHECK(f32.get_float(field32::A_FLOAT_FIELD) == doctest::Approx(13.7));
    CHECK(f32.get_double(field32::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
    CHECK(f32.get_string(field32::A_STRING_FIELD) == "hello world");
    CHECK(f32.get_string(field32::A_CARRAY_FIELD) == "hello dolly");
}

TEST_CASE("convert fml32->fml16")
{
    fml32 f32;
    f32.add(field32::A_SHORT_FIELD, 2);
    f32.add(field32::A_LONG_FIELD, 100);
    f32.add(field32::A_CHAR_FIELD, 'b');
    f32.add(field32::A_FLOAT_FIELD, 13.7);
    f32.add(field32::A_DOUBLE_FIELD, 4096.32);
    f32.add(field32::A_STRING_FIELD, "hello world");
    f32.add(field32::A_CARRAY_FIELD, "hello dolly");
    
    fml16 f16 = to_fml16(f32);
    CHECK(f16.field_count() == 7); 
    CHECK(f16.get_short(field16::A_SHORT_FIELD) == 2);
    CHECK(f16.get_long(field16::A_LONG_FIELD) == 100);
    CHECK(f16.get_char(field16::A_CHAR_FIELD) == 'b');
    CHECK(f16.get_float(field16::A_FLOAT_FIELD) == doctest::Approx(13.7));
    CHECK(f16.get_double(field16::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
    CHECK(f16.get_string(field16::A_STRING_FIELD) == "hello world");
    CHECK(f16.get_string(field16::A_CARRAY_FIELD) == "hello dolly");
}

TEST_CASE("convert fml16->struct")
{
    fml16 f;
    f.add(field16::A_LONG_FIELD, 100);
    f.add(field16::A_FLOAT_FIELD, 13.7);
    f.add(field16::A_DOUBLE_FIELD, 4096.32);
    
    auto s = to_struct<my_struct>(f);
    CHECK(s.l == 100);
    CHECK(s.f == doctest::Approx(13.7));
    CHECK(s.d == doctest::Approx(4096.32));
}


TEST_CASE("convert fml32->struct")
{
    fml32 f;
    f.add(field32::ORIGINAL_STRING, "hello world");
    f.add(field32::ASCII_SUM, 500);
    f.add(field32::BYTE_COUNT, 50);
    f.add(field32::MOST_FREQUENT_CHAR, 'l');
    
    auto s = to_struct<string_info>(f);
    CHECK(s.original_string == string("hello world"));
    CHECK(s.ascii_sum == 500);
    CHECK(s.byte_count == 50);
    CHECK(s.most_frequent_char == 'l');
}

// asan issues memcpy-param-overlap warning on Fvstof 
TEST_CASE("convert struct->fml16(existing) asan=replace_intrin")
{
    fml16 f;
    f.add(field16::A_LONG_FIELD, 100);
    f.add(field16::A_FLOAT_FIELD, 13.7);
    f.add(field16::A_DOUBLE_FIELD, 4096.32);
    f.add(field16::ASCII_SUM, 100);
    f.add(field16::BYTE_COUNT, 2);
    
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.ascii_sum = 500;
    s.byte_count = 50;
    s.most_frequent_char = 'l';
    
    SUBCASE("update")
    {
        convert(s, f, FUPDATE);
        CHECK(f.field_count() == 7);
        CHECK(f.get_long(field16::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field16::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field16::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field16::ASCII_SUM) == 500);
        CHECK(f.get_long(field16::BYTE_COUNT) == 50);
        CHECK(f.get_string(field16::ORIGINAL_STRING) == "hello world");
        CHECK(f.get_char(field16::MOST_FREQUENT_CHAR) == 'l');
    }
    
    SUBCASE("join")
    {
        convert(s, f, FJOIN);
        CHECK(f.field_count() == 2);
        CHECK(f.get_long(field16::ASCII_SUM) == 500);
        CHECK(f.get_long(field16::BYTE_COUNT) == 50);
    }
    
    SUBCASE("ojoin")
    {
        convert(s, f, FOJOIN);
        CHECK(f.field_count() == 5);
        CHECK(f.get_long(field16::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field16::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field16::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field16::ASCII_SUM) == 500);
        CHECK(f.get_long(field16::BYTE_COUNT) == 50);
    }
    
    SUBCASE("concat")
    {
        convert(s, f, FCONCAT);
        CHECK(f.field_count() == 9);
        CHECK(f.get_long(field16::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field16::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field16::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field16::ASCII_SUM, 0) == 100);
        CHECK(f.get_long(field16::ASCII_SUM, 1) == 500);
        CHECK(f.get_long(field16::BYTE_COUNT, 0) == 2);
        CHECK(f.get_long(field16::BYTE_COUNT, 1) == 50);
        CHECK(f.get_string(field16::ORIGINAL_STRING) == "hello world");
        CHECK(f.get_char(field16::MOST_FREQUENT_CHAR) == 'l');
    }
    
}

// asan issues memcpy-param-overlap warning on Fvstof32 
TEST_CASE("convert struct to fml32(existing) asan=replace_intrin")
{
    fml32 f;
    f.add(field32::A_LONG_FIELD, 100);
    f.add(field32::A_FLOAT_FIELD, 13.7);
    f.add(field32::A_DOUBLE_FIELD, 4096.32);
    f.add(field32::ASCII_SUM, 100);
    f.add(field32::BYTE_COUNT, 2);
    
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.ascii_sum = 500;
    s.byte_count = 50;
    s.most_frequent_char = 'l';
    
    SUBCASE("update")
    {
        convert(s, f, FUPDATE);
        CHECK(f.field_count() == 7);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field32::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field32::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field32::ASCII_SUM) == 500);
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
        CHECK(f.get_string(field32::ORIGINAL_STRING) == "hello world");
        CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');
    }
    
    SUBCASE("join")
    {
        convert(s, f, FJOIN);
        CHECK(f.field_count() == 2);
        CHECK(f.get_long(field32::ASCII_SUM) == 500);
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    }
    
    SUBCASE("ojoin")
    {
        convert(s, f, FOJOIN);
        CHECK(f.field_count() == 5);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field32::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field32::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field32::ASCII_SUM) == 500);
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    }
    
    SUBCASE("concat")
    {
        convert(s, f, FCONCAT);
        CHECK(f.field_count() == 9);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 100);
        CHECK(f.get_float(field32::A_FLOAT_FIELD) == doctest::Approx(13.7));
        CHECK(f.get_double(field32::A_DOUBLE_FIELD) == doctest::Approx(4096.32));
        CHECK(f.get_long(field32::ASCII_SUM, 0) == 100);
        CHECK(f.get_long(field32::ASCII_SUM, 1) == 500);
        CHECK(f.get_long(field32::BYTE_COUNT, 0) == 2);
        CHECK(f.get_long(field32::BYTE_COUNT, 1) == 50);
        CHECK(f.get_string(field32::ORIGINAL_STRING) == "hello world");
        CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');
    }    
}

// asan issues memcpy-param-overlap warning on Fvstof
TEST_CASE("convert struct->fml16 asan=replace_intrin")
{
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.ascii_sum = 500;
    s.byte_count = 50;
    s.most_frequent_char = 'l';
    
    fml16 f = to_fml16(s);
    CHECK(f.field_count() == 4);
    CHECK(f.get_long(field16::ASCII_SUM) == 500);
    CHECK(f.get_long(field16::BYTE_COUNT) == 50);
    CHECK(f.get_string(field16::ORIGINAL_STRING) == "hello world");
    CHECK(f.get_char(field16::MOST_FREQUENT_CHAR) == 'l');
}

// asan issues memcpy-param-overlap warning on Fvstof32 
TEST_CASE("convert struct->fml32 asan=replace_intrin")
{
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.ascii_sum = 500;
    s.byte_count = 50;
    s.most_frequent_char = 'l';
    
    fml32 f = to_fml32(s);
    CHECK(f.field_count() == 4);
    CHECK(f.get_long(field32::ASCII_SUM) == 500);
    CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    CHECK(f.get_string(field32::ORIGINAL_STRING) == "hello world");
    CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');
}

string trim_right(string x)
{
    auto pos = x.find_last_not_of(' ');
    if(pos == x.npos)
    {
        x.clear();
    }
    else
    {
        x.erase(pos + 1);
    }
    return x;
}

#if TUXEDO_VERSION >= 1222
TEST_CASE("convert record->fml32(existing)")
{
    record r("STRING_INFO");
    r.set("ORIGINAL_STRING", "hello world");
    r.set("BYTE_COUNT", 50);
    r.set("ASCII_SUM", 500);
    r.set("MOST_FREQUENT_CHAR", 'l');
    
    fml32 f;
    f.add(field32::A_LONG_FIELD, 50);
    f.add(field32::ORIGINAL_STRING, "ab");
    f.add(field32::BYTE_COUNT, 2);
    
    SUBCASE("update")
    {
        convert(r, f, FUPDATE);
        CHECK(f.field_count() == 5);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 50);
        CHECK(trim_right(f.get_string(field32::ORIGINAL_STRING)) == "hello world");
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
        CHECK(f.get_long(field32::ASCII_SUM) == 500);
        CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');  
    }
    
    SUBCASE("join")
    {
        convert(r, f, FJOIN);
        CHECK(f.field_count() == 2);
        CHECK(trim_right(f.get_string(field32::ORIGINAL_STRING)) == "hello world");
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    }
    
    SUBCASE("ojoin")
    {
        convert(r, f, FOJOIN);
        CHECK(f.field_count() == 3);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 50);
        CHECK(trim_right(f.get_string(field32::ORIGINAL_STRING)) == "hello world");
        CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    }
    
    SUBCASE("concat")
    {
        convert(r, f, FCONCAT);
        CHECK(f.field_count() == 7);
        CHECK(f.get_long(field32::A_LONG_FIELD) == 50);
        CHECK(f.get_string(field32::ORIGINAL_STRING, 0) == "ab");
        CHECK(trim_right(f.get_string(field32::ORIGINAL_STRING, 1)) == "hello world");
        CHECK(f.get_long(field32::BYTE_COUNT, 0) == 2);
        CHECK(f.get_long(field32::BYTE_COUNT, 1) == 50);
        CHECK(f.get_long(field32::ASCII_SUM) == 500);
        CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');  
    }
}

TEST_CASE("convert record->fml32")
{
    record r("STRING_INFO");
    r.set("ORIGINAL_STRING", "hello world");
    r.set("BYTE_COUNT", 50);
    r.set("ASCII_SUM", 500);
    r.set("MOST_FREQUENT_CHAR", 'l');
    
    fml32 f = to_fml32(r);
    
    CHECK(f.field_count() == 4);
    CHECK(trim_right(f.get_string(field32::ORIGINAL_STRING)) == "hello world");
    CHECK(f.get_long(field32::BYTE_COUNT) == 50);
    CHECK(f.get_long(field32::ASCII_SUM) == 500);
    CHECK(f.get_char(field32::MOST_FREQUENT_CHAR) == 'l');
}

TEST_CASE("convert fml32->record(existing)")
{
    fml32 f;
    f.add(field32::ORIGINAL_STRING, "hello world");
    f.add(field32::ASCII_SUM, 500);
    f.add(field32::BYTE_COUNT, 50);
    f.add(field32::MOST_FREQUENT_CHAR, 'l');
    
    record r("STRING_INFO");
    
    convert(f, r);
    
    CHECK(r.get_string("ORIGINAL_STRING") == "hello world");
    CHECK(r.get_long("ASCII_SUM") == 500);
    CHECK(r.get_long("BYTE_COUNT") == 50);
    CHECK(r.get_char("MOST_FREQUENT_CHAR") == 'l');
}

TEST_CASE("convert fml32->record")
{
    fml32 f;
    f.add(field32::ORIGINAL_STRING, "hello world");
    f.add(field32::ASCII_SUM, 500);
    f.add(field32::BYTE_COUNT, 50);
    f.add(field32::MOST_FREQUENT_CHAR, 'l');
    
    record r = to_record(f, "STRING_INFO");
    
    CHECK(r.get_string("ORIGINAL_STRING") == "hello world");
    CHECK(r.get_long("ASCII_SUM") == 500);
    CHECK(r.get_long("BYTE_COUNT") == 50);
    CHECK(r.get_char("MOST_FREQUENT_CHAR") == 'l');
}

TEST_CASE("convert struct->record")
{
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.byte_count = 50;
    s.ascii_sum = 500;
    s.most_frequent_char = 'l';
    
    record r = to_record(s, "STRING_INFO");
    
    CHECK(r.get_string("ORIGINAL_STRING") == "hello world");
    CHECK(r.get_long("ASCII_SUM") == 500);
    CHECK(r.get_long("BYTE_COUNT") == 50);
    CHECK(r.get_char("MOST_FREQUENT_CHAR") == 'l');
    
}

TEST_CASE("convert record->struct")
{
    record r("STRING_INFO");
    r.set("ORIGINAL_STRING", "hello world");
    r.set("BYTE_COUNT", 50);
    r.set("ASCII_SUM", 500);
    r.set("MOST_FREQUENT_CHAR", 'l');
    
    auto s = to_struct<string_info>(r);
    
    CHECK(trim_right(s.original_string) == "hello world");
    CHECK(s.ascii_sum == 500);
    CHECK(s.byte_count == 50);
    CHECK(s.most_frequent_char == 'l');
}


/*TEST_CASE("convert view16_to_record")
{
    try
    {
    auto s = make_default<string_info>();
    set(s.original_string, "hello world");
    s.byte_count = 50;
    s.ascii_sum = 500;
    s.most_frequent_char = 'l';
    
    //record r("STRING_INFO", nullptr, 0 ,TPENC_EBCDIC | TPENC_MAINFRAME_FLOAT);
    //string raw = r.get_data(); // properly sized buffer
    //cout << "size: " << raw.size() << endl;
    string raw;
    raw.resize(1024);
    view32_to_record(s, const_cast<char*>(raw.data()), raw.size());
    record r("STRING_INFO", raw.data(), 100, TPENC_EBCDIC | TPENC_MAINFRAME_FLOAT);

    
    CHECK(r.get_string("ORIGINAL_STRING") == "hello world");
    CHECK(r.get_long("ASCII_SUM") == 500);
    CHECK(r.get_long("BYTE_COUNT") == 50);
    CHECK(r.get_char("MOST_FREQUENT_CHAR") == 'l');
    }
    catch(exception const& e)
    {
        cerr << e.what() << endl;
    }
    
}

TEST_CASE("record_to_view16")
{
    record r("STRING_INFO");
    r.set("ORIGINAL_STRING", "hello world");
    r.set("BYTE_COUNT", 50);
    r.set("ASCII_SUM", 500);
    r.set("MOST_FREQUENT_CHAR", 'l');
    
    auto s = make_default<string_info>();
    string raw = r.get_data();
    record_to_view16(raw.data(), s);
    
    CHECK(trim_right(s.original_string) == "hello world");
    CHECK(s.ascii_sum == 500);
    CHECK(s.byte_count == 50);
    CHECK(s.most_frequent_char == 'l');
}*/

TEST_CASE("convert record(raw data)->struct")
{
    record r("STRING_INFO");
    r.set("ORIGINAL_STRING", "hello world");
    r.set("BYTE_COUNT", 50);
    r.set("ASCII_SUM", 500);
    r.set("MOST_FREQUENT_CHAR", 'l');
    
    auto s = to_struct<string_info>(r);
    
    CHECK(trim_right(s.original_string) == "hello world");
    CHECK(s.ascii_sum == 500);
    CHECK(s.byte_count == 50);
    CHECK(s.most_frequent_char == 'l');
}
#endif

TEST_CASE("convert fml16->xml")
{
    fml16 f;
    f.add(field16::A_LONG_FIELD, 100);
    f.add(field16::A_STRING_FIELD, "hello");
    f.add(field16::A_STRING_FIELD, "world");
    
    xml x = to_xml(f, "CUSTOM_ROOT");
    
    CHECK(x.to_string() == R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?><CUSTOM_ROOT Type="FML"><A_LONG_FIELD>100</A_LONG_FIELD><A_STRING_FIELD>hello</A_STRING_FIELD><A_STRING_FIELD>world</A_STRING_FIELD></CUSTOM_ROOT>)");   
}

TEST_CASE("convert fml32->xml")
{
    fml32 f;
    f.add(field32::A_LONG_FIELD, 100);
    f.add(field32::A_STRING_FIELD, "hello");
    f.add(field32::A_STRING_FIELD, "world");
    
    xml x = to_xml(f);
    
    CHECK(x.to_string() == R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?><FML32><A_LONG_FIELD>100</A_LONG_FIELD><A_STRING_FIELD>hello</A_STRING_FIELD><A_STRING_FIELD>world</A_STRING_FIELD></FML32>)");   
}

TEST_CASE("convert xml->fml16")
{
    xml x(R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?><CUSTOM_ROOT Type="FML"><A_LONG_FIELD>100</A_LONG_FIELD><A_STRING_FIELD>hello</A_STRING_FIELD><A_STRING_FIELD>world</A_STRING_FIELD></CUSTOM_ROOT>)");
    fml16 f;
    string root;
    tie(f,root) = to_fml16(x);
    CHECK(root == "CUSTOM_ROOT");
    CHECK(f.get_long(field16::A_LONG_FIELD) == 100);
    CHECK(f.get_string(field16::A_STRING_FIELD, 0) == "hello");
    CHECK(f.get_string(field16::A_STRING_FIELD, 1) == "world");
}


TEST_CASE("convert xml->fml32")
{
    xml x(R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?><CUSTOM_ROOT Type="FML"><A_LONG_FIELD>100</A_LONG_FIELD><A_STRING_FIELD>hello</A_STRING_FIELD><A_STRING_FIELD>world</A_STRING_FIELD></CUSTOM_ROOT>)");
    fml32 f;
    string root;
    tie(f,root) = to_fml32(x);
    CHECK(root == "CUSTOM_ROOT");
    CHECK(f.get_long(field32::A_LONG_FIELD) == 100);
    CHECK(f.get_string(field32::A_STRING_FIELD, 0) == "hello");
    CHECK(f.get_string(field32::A_STRING_FIELD, 1) == "world");
}



TEST_SUITE_END();
