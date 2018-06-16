#include <iostream>
#include <fstream>
#include "doctest.h"
#include "tux/fml32.hpp"
#include "tux/util.hpp"
#include "tux/cstring.hpp"
#include "fields32.h"
#include "views32.h"


using namespace std;
using namespace tux;
using fml = fml32;

TEST_SUITE("fml32");

class ptr_field_guard
{
public:
ptr_field_guard(fml32& f, std::vector<FLDID32> ids) :
    f_(f),
    ids_(std::move(ids)) {}
    ~ptr_field_guard()
    {
        try
        {
            f_.erase(ids_);
        }
        catch(...)
        {
            
        }
    }
    
    
private:
    fml32& f_;
    std::vector<FLDID32> ids_;
};

TEST_CASE("fml32 packed_mbstring")
{
    unpacked_mbstring unpacked;
    
    packed_mbstring x;
    CHECK(x.data() == nullptr);
    CHECK((bool)x == false);
    CHECK(x.size() == 0);
    put_env("TPMBENC", "UTF-16LE");
    x.set("mbstring1");
    CHECK(x.data() != nullptr);
    CHECK(x.size() >= 9);
    
    unpacked = x.unpack();
    CHECK(unpacked.data == "mbstring1");
    CHECK(unpacked.encoding == "UTF-16LE");
    CHECK(x.size() >= 9);
    
    x.set("mbstring2", mbpack_option::buffer);
    unpacked = x.unpack();
    CHECK(unpacked.data == "mbstring2");
    CHECK(unpacked.encoding == "");
    CHECK(x.size() >= 9);
    
    x.set("mbstr3", "SHIFT-JIS");
    unpacked = x.unpack();
    CHECK(unpacked.data == "mbstr3");
    CHECK(unpacked.encoding == "SHIFT-JIS");
    CHECK(x.size() >= 6);
    
    put_env("TPMBENC", "");
    CHECK_THROWS(x.set("mbstring1"));
    
}

TEST_CASE("fml32::error")
{
    fml::error empty;
    CHECK(empty.what() == string());
    CHECK(empty.str() == string());
    CHECK(empty.code() == 0);
    
    fml::error e(FNOSPACE, "my message");
    CHECK(e.code() == FNOSPACE);
    CHECK(string(e.str()).find(Fstrerror32(FNOSPACE)) != string::npos);
    CHECK(string(e.what()) == "my message");
    
    Ferror32 = FNOSPACE;
    fml::error le = fml::last_error("my message");
    CHECK(le.code() == FNOSPACE);
    CHECK(string(le.str()).find(Fstrerror32(FNOSPACE)) != string::npos);
    CHECK(string(le.what()).find(Fstrerror32(FNOSPACE)) != string::npos);
    CHECK(string(le.what()).find("my message") != string::npos); 
}

TEST_CASE("fml32::field_type_name_from_type")
{
    CHECK(fml::field_type_name_from_type(FLD_SHORT) == "short");
    CHECK(fml::field_type_name_from_type(FLD_LONG) == "long");
    CHECK(fml::field_type_name_from_type(FLD_CHAR) == "char");
    CHECK(fml::field_type_name_from_type(FLD_FLOAT) == "float");
    CHECK(fml::field_type_name_from_type(FLD_DOUBLE) == "double");
    CHECK(fml::field_type_name_from_type(FLD_STRING) == "string");
    CHECK(fml::field_type_name_from_type(FLD_CARRAY) == "carray");
    CHECK(fml::field_type_name_from_type(FLD_MBSTRING) == "mbstring");
    CHECK(fml::field_type_name_from_type(FLD_FML32) == "fml32");
    CHECK(fml::field_type_name_from_type(FLD_PTR) == "ptr");
    CHECK(fml::field_type_name_from_type(FLD_VIEW32) == "view32");
    CHECK(fml::field_type_name_from_type(-57) == "?");
}

TEST_CASE("fml32::field_id")
{
    CHECK(fml::field_id("A_SHORT_FIELD") == A_SHORT_FIELD);
    CHECK(fml::field_id("A_LONG_FIELD") == A_LONG_FIELD);
    CHECK(fml::field_id("A_CHAR_FIELD") == A_CHAR_FIELD);
    CHECK(fml::field_id("A_FLOAT_FIELD") == A_FLOAT_FIELD);
    CHECK(fml::field_id("A_DOUBLE_FIELD") == A_DOUBLE_FIELD);
    CHECK(fml::field_id("A_STRING_FIELD") == A_STRING_FIELD);
    CHECK(fml::field_id("A_CARRAY_FIELD") == A_CARRAY_FIELD);
    CHECK(fml::field_id("AN_MBSTRING_FIELD") == AN_MBSTRING_FIELD);
    CHECK(fml::field_id("AN_FML32_FIELD") == AN_FML32_FIELD);
    CHECK(fml::field_id("A_PTR_FIELD") == A_PTR_FIELD);
    CHECK(fml::field_id("A_VIEW32_FIELD") == A_VIEW32_FIELD);
    
    CHECK_THROWS(fml::field_id("BAD_NAME"));
}

TEST_CASE("fml32::field_name")
{
    CHECK(fml::field_name(A_SHORT_FIELD) == "A_SHORT_FIELD");
    CHECK(fml::field_name(A_LONG_FIELD) == "A_LONG_FIELD");
    CHECK(fml::field_name(A_CHAR_FIELD) == "A_CHAR_FIELD");
    CHECK(fml::field_name(A_FLOAT_FIELD) == "A_FLOAT_FIELD");
    CHECK(fml::field_name(A_DOUBLE_FIELD) == "A_DOUBLE_FIELD");
    CHECK(fml::field_name(A_STRING_FIELD) == "A_STRING_FIELD");
    CHECK(fml::field_name(A_CARRAY_FIELD) == "A_CARRAY_FIELD");
    CHECK(fml::field_name(AN_MBSTRING_FIELD) == "AN_MBSTRING_FIELD");
    CHECK(fml::field_name(AN_FML32_FIELD) == "AN_FML32_FIELD");
    CHECK(fml::field_name(A_PTR_FIELD) == "A_PTR_FIELD");
    CHECK(fml::field_name(A_VIEW32_FIELD) == "A_VIEW32_FIELD");
    
    CHECK_THROWS(fml::field_name(-800));
}

TEST_CASE("fml32::field_number")
{
    CHECK(fml::field_number(A_SHORT_FIELD)     == 1001);
    CHECK(fml::field_number(A_LONG_FIELD)      == 1002);
    CHECK(fml::field_number(A_CHAR_FIELD)      == 1003);
    CHECK(fml::field_number(A_FLOAT_FIELD)     == 1004);
    CHECK(fml::field_number(A_DOUBLE_FIELD)    == 1005);
    CHECK(fml::field_number(A_STRING_FIELD)    == 1006);
    CHECK(fml::field_number(A_CARRAY_FIELD)    == 1007);
    CHECK(fml::field_number(AN_MBSTRING_FIELD) == 1008);
    CHECK(fml::field_number(AN_FML32_FIELD)    == 1009);
    CHECK(fml::field_number(A_PTR_FIELD)       == 1010);
    CHECK(fml::field_number(A_VIEW32_FIELD)    == 1011);
}

TEST_CASE("fml32::field_type")
{
    CHECK(fml::field_type(A_SHORT_FIELD)     == FLD_SHORT);
    CHECK(fml::field_type(A_LONG_FIELD)      == FLD_LONG);
    CHECK(fml::field_type(A_CHAR_FIELD)      == FLD_CHAR);
    CHECK(fml::field_type(A_FLOAT_FIELD)     == FLD_FLOAT);
    CHECK(fml::field_type(A_DOUBLE_FIELD)    == FLD_DOUBLE);
    CHECK(fml::field_type(A_STRING_FIELD)    == FLD_STRING);
    CHECK(fml::field_type(A_CARRAY_FIELD)    == FLD_CARRAY);
    CHECK(fml::field_type(AN_MBSTRING_FIELD) == FLD_MBSTRING);
    CHECK(fml::field_type(AN_FML32_FIELD)    == FLD_FML32);
    CHECK(fml::field_type(A_PTR_FIELD)       == FLD_PTR);
    CHECK(fml::field_type(A_VIEW32_FIELD)    == FLD_VIEW32);
}

TEST_CASE("fml32::field_type_name")
{
    CHECK(fml::field_type_name(A_SHORT_FIELD)     == "short");
    CHECK(fml::field_type_name(A_LONG_FIELD)      == "long");
    CHECK(fml::field_type_name(A_CHAR_FIELD)      == "char");
    CHECK(fml::field_type_name(A_FLOAT_FIELD)     == "float");
    CHECK(fml::field_type_name(A_DOUBLE_FIELD)    == "double");
    CHECK(fml::field_type_name(A_STRING_FIELD)    == "string");
    CHECK(fml::field_type_name(A_CARRAY_FIELD)    == "carray");
    CHECK(fml::field_type_name(AN_MBSTRING_FIELD) == "mbstring");
    CHECK(fml::field_type_name(AN_FML32_FIELD)    == "fml32");
    CHECK(fml::field_type_name(A_PTR_FIELD)       == "ptr");
    CHECK(fml::field_type_name(A_VIEW32_FIELD)    == "view32");
    
    CHECK_THROWS(fml::field_type_name(-800));
}

TEST_CASE("fml32::field_id")
{
    CHECK(fml::field_id(fml::field_type(A_SHORT_FIELD), fml::field_number(A_SHORT_FIELD))  == A_SHORT_FIELD);
    CHECK(fml::field_id(fml::field_type(A_LONG_FIELD), fml::field_number(A_LONG_FIELD))  == A_LONG_FIELD);
    CHECK(fml::field_id(fml::field_type(A_CHAR_FIELD), fml::field_number(A_CHAR_FIELD))  == A_CHAR_FIELD);
    CHECK(fml::field_id(fml::field_type(A_FLOAT_FIELD), fml::field_number(A_FLOAT_FIELD))  == A_FLOAT_FIELD);
    CHECK(fml::field_id(fml::field_type(A_DOUBLE_FIELD), fml::field_number(A_DOUBLE_FIELD))  == A_DOUBLE_FIELD);
    CHECK(fml::field_id(fml::field_type(A_STRING_FIELD), fml::field_number(A_STRING_FIELD))  == A_STRING_FIELD);
    CHECK(fml::field_id(fml::field_type(A_CARRAY_FIELD), fml::field_number(A_CARRAY_FIELD))  == A_CARRAY_FIELD);
    CHECK(fml::field_id(fml::field_type(AN_MBSTRING_FIELD), fml::field_number(AN_MBSTRING_FIELD))  == AN_MBSTRING_FIELD);
    CHECK(fml::field_id(fml::field_type(AN_FML32_FIELD), fml::field_number(AN_FML32_FIELD))  == AN_FML32_FIELD);
    CHECK(fml::field_id(fml::field_type(A_PTR_FIELD), fml::field_number(A_PTR_FIELD))  == A_PTR_FIELD);
    CHECK(fml::field_id(fml::field_type(A_VIEW32_FIELD), fml::field_number(A_VIEW32_FIELD))  == A_VIEW32_FIELD);
}

TEST_CASE("fml32::bytes_needed")
{
    CHECK(fml::bytes_needed(64, 512) == Fneeded32(64,512));
    CHECK_THROWS(fml::bytes_needed(-1, 90000));
}

TEST_CASE("fml32 default construct")
{
    fml b;
    CHECK(b.as_fbfr() == nullptr);
    CHECK((bool)b == false);
    CHECK(b.size() == 0);
}

TEST_CASE("fml32 copy construct")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    fml b(a);
    CHECK(a == b);
    CHECK(a.get_long(A_LONG_FIELD) == 2);
    CHECK(a.get_string(A_STRING_FIELD) == "hello world");
    CHECK(b.get_long(A_LONG_FIELD) == 2);
    CHECK(b.get_string(A_STRING_FIELD) == "hello world");
}

TEST_CASE("fml32 copy assign")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    fml b;
    b.add(A_DOUBLE_FIELD, 57.9);
    b.add(A_STRING_FIELD, "hello dolly");
    b = a;
    
    CHECK(a == b);
    CHECK(a.get_long(A_LONG_FIELD) == 2);
    CHECK(a.get_string(A_STRING_FIELD) == "hello world");
    CHECK(b.get_long(A_LONG_FIELD) == 2);
    CHECK(b.get_string(A_STRING_FIELD) == "hello world");
}

TEST_CASE("fml32 move construct")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    fml b(move(a));
    
    CHECK(a != b);
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.get_long(A_LONG_FIELD) == 2);
    CHECK(b.get_string(A_STRING_FIELD) == "hello world");
}

TEST_CASE("fml32 move assign")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    fml b;
    b.add(A_DOUBLE_FIELD, 57.9);
    b.add(A_STRING_FIELD, "hello dolly");
    b = move(a);
    
    CHECK(a != b);
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.get_long(A_LONG_FIELD) == 2);
    CHECK(b.get_string(A_STRING_FIELD) == "hello world");
}

TEST_CASE("fml32 move construct from buffer")
{
    // move from null buffer
    buffer buf;
    fml a(move(buf));
    CHECK(a.buffer().data() == nullptr);
    
    // move from fml buffer
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    fml b(a.move_buffer());
    CHECK(a != b);
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.get_long(A_LONG_FIELD) == 2);
    CHECK(b.get_string(A_STRING_FIELD) == "hello world");
    
    // move from other buffer types
    CHECK_THROWS(fml(buffer("FML"))); // instead of FML32
    CHECK_THROWS(fml(buffer("STRING"))); // instead of FML32
}

TEST_CASE("fml32 move assign from buffer")
{
    // move from null buffer
    buffer buf;
    fml f1;
    f1 = move(buf);
    CHECK(f1.buffer().data() == nullptr);
    
    // move from fml buffer
    f1.add(A_LONG_FIELD, 2);
    f1.add(A_STRING_FIELD, "hello world");
    fml f2 = move(f1);
    CHECK(f1 != f2);
    CHECK(f1.buffer().data() == nullptr);
    CHECK(f2.get_long(A_LONG_FIELD) == 2);
    CHECK(f2.get_string(A_STRING_FIELD) == "hello world");
    
    // move from other buffer types
    CHECK_THROWS(f1 = buffer("FML")); // instead of FML32
    CHECK_THROWS(f1 = buffer("STRING")); // instead of FML32
}

TEST_CASE("fml32 construct with capacity")
{
    fml f(8000);
    CHECK(f.buffer().size() >= 8000);
    CHECK(f.size() >= 8000);
    CHECK(f.used_size() < 8000);
}

TEST_CASE("fml32 construct with capacity info")
{
    fml f(10, 8000);
    CHECK(f.buffer().size() >= Fneeded32(10, 8000));
    CHECK(f.size() >= Fneeded32(10, 8000));
    CHECK(f.used_size() < Fneeded32(10, 8000));
}

TEST_CASE("fml32 move buffer")
{
    fml f(10, 8000);
    buffer b = f.move_buffer();
    CHECK(f.buffer().data() == nullptr);
    CHECK(b.type() == "FML32");
    CHECK(b.size() >= Fneeded32(10, 8000));
}

TEST_CASE("fml32 reserve")
{
    fml f;
    f.reserve(4096);
    CHECK(f.buffer().size() >= 4096);
    CHECK(f.size() >= 4096);
    CHECK(f.used_size() < 4096);
    
    f.reserve(100,8000);
    CHECK(f.buffer().size() >= Fneeded32(100, 8000));
    CHECK(f.size() >= Fneeded32(100, 8000));
    CHECK(f.used_size() < Fneeded32(100, 8000));
}

TEST_CASE("fml32 boolean conversion")
{
    fml f;
    CHECK((bool)f == false);
    f.reserve(4096);
    CHECK((bool)f == false);
    f.add(A_LONG_FIELD, 2);
    CHECK((bool)f == true);
}

TEST_CASE("fml32 is_fielded")
{
    fml f;
    CHECK(f.is_fielded() == false);
    f.reserve(4096);
    CHECK(f.is_fielded() == true);
    f.add(A_LONG_FIELD, 2);
    CHECK(f.is_fielded() == true);
}

TEST_CASE("fml32 as_fbfr")
{
    fml f;
    CHECK(f.as_fbfr() == nullptr);
    f.reserve(4096);
    CHECK(f.as_fbfr() != nullptr);
    f.add(A_LONG_FIELD, 2);
    CHECK(f.as_fbfr() != nullptr);
}

TEST_CASE("fml32 checksum")
{
    fml f;
    CHECK_THROWS(f.checksum());
    f.reserve(4096);
    CHECK(f.checksum() == Fchksum32(f.as_fbfr()));
    f.add(A_LONG_FIELD, 2);
    CHECK(f.checksum() == Fchksum32(f.as_fbfr()));
}

TEST_CASE("fml32 print_to_stdout")
{
    fml f;
    CHECK_THROWS(f.print_to_stdout());
    f.add(A_STRING_FIELD, "hello world");
    CHECK_NOTHROW(f.print_to_stdout());
    
    const char* fname = "tmp_extrw_test";
    ofstream os(fname);
    os << "A_DOUBLE_FIELD\t13.7\nA_STRING_FIELD\thello world\n\n";
    os.close();

    FILE* file = fopen(fname, "r");
    f.extread(file);
    fclose(file);
    remove(fname);
    CHECK(f.get_double(A_DOUBLE_FIELD) == doctest::Approx(13.7));
    CHECK(f.get_string(A_STRING_FIELD) == "hello world");
}

TEST_CASE("fml32 write/read")
{
    fml f1;
    f1.add(A_SHORT_FIELD, 2);
    f1.add(A_LONG_FIELD, 100);
    f1.add(A_CHAR_FIELD, 'b');
    f1.add(A_FLOAT_FIELD, 13.7);
    f1.add(A_DOUBLE_FIELD, 4096.32);
    f1.add(A_STRING_FIELD, "hello world");
    f1.add(A_CARRAY_FIELD, "hello dolly");
    
    const char* fname = "tmp_rw_test";
    FILE* outfile = fopen(fname, "w");
    f1.write(outfile);
    fclose(outfile);
    
    FILE* infile = fopen(fname, "r");
    fml f2;
    f2.read(infile);
    fclose(infile);
    remove(fname);
    CHECK(f2.get_short(A_SHORT_FIELD) == 2);
    CHECK(f2.get_long(A_LONG_FIELD) == 100);
    CHECK(f2.get_char(A_CHAR_FIELD) == 'b');
    CHECK(f2.get_float(A_FLOAT_FIELD) == doctest::Approx(13.7));
    CHECK(f2.get_double(A_DOUBLE_FIELD) == doctest::Approx(4096.32));
    CHECK(f2.get_string(A_STRING_FIELD) == "hello world");
    CHECK(f2.get_string(A_CARRAY_FIELD) == "hello dolly");
    CHECK(f2 == f1);
}

TEST_CASE("fml32 indexing")
{
    fml f;
    for(int i=0; i<100; ++i)
    {
        f.add(A_DOUBLE_FIELD, 4096.32);
        f.add(A_STRING_FIELD, "hello world");
    }
    CHECK(f.index_size() > 0);
    FLDOCC32 index_element_count = f.unindex();
    CHECK(f.index_size() == 0);
    f.restore_index(index_element_count);
    CHECK(f.index_size() > 0);
    f.unindex();
    CHECK(f.index_size() == 0);
    f.index();
    CHECK(f.index_size() > 0);  
}

TEST_CASE("fml32 bytes used")
{
    fml f;
    CHECK(f.index_size() == 0);
    CHECK(f.unused_size() == 0);
    CHECK(f.used_size() == 0);
    CHECK(f.size() == 0);
    for(int i=0; i<100; ++i)
    {
        f.add(A_DOUBLE_FIELD, 4096.32);
        f.add(A_STRING_FIELD, "hello world");
    }
    long index_size = f.index_size();
    long unused_size = f.unused_size();
    long used_size = f.used_size();
    long size = f.size();
    CHECK(size > 0);
    CHECK(index_size > 0);
    CHECK(index_size < size);
    CHECK(unused_size >= 0);
    CHECK(unused_size < size);
    CHECK(used_size > 0);
    CHECK(used_size <= size);
}

TEST_CASE("fml32 field counts")
{
    fml f;
    for(int i=0; i<100; ++i)
    {
        f.add(A_DOUBLE_FIELD, 4096.32);
        f.add(A_STRING_FIELD, "hello world");
    }
    CHECK(f.field_count() == 200);
    CHECK(f.count(A_DOUBLE_FIELD) == 100);
    CHECK(f.count(A_STRING_FIELD) == 100);
    CHECK(f.has(A_STRING_FIELD) == true);
    CHECK(f.has(A_SHORT_FIELD) == false);
}

TEST_CASE("fml32 field_value_size")
{
    
    string s1 = "hello world";
    string s2 = "hello";
    fml f;
    f.add(A_STRING_FIELD, s1);
    f.add(A_STRING_FIELD, s2);
    f.add(A_DOUBLE_FIELD, 38.20);
    CHECK(f.field_value_size(A_STRING_FIELD, 0) == s1.size() + 1);
    CHECK(f.field_value_size(A_STRING_FIELD, 1) == s2.size() + 1);
    CHECK(f.field_value_size(A_DOUBLE_FIELD) == sizeof(double));
}

TEST_CASE("fml32 element removal")
{
    fml f;
    CHECK(f.erase(A_STRING_FIELD, 0) == false);
    CHECK(f.erase(A_STRING_FIELD) == false);
    CHECK_NOTHROW(f.erase({A_STRING_FIELD, A_DOUBLE_FIELD}));
    CHECK_NOTHROW(f.erase_all_but({A_STRING_FIELD, A_DOUBLE_FIELD}));
    
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_DOUBLE_FIELD, 38.20);
    f.add(A_DOUBLE_FIELD, 55.99);
    f.add(A_DOUBLE_FIELD, 69.01);
    f.add(A_SHORT_FIELD, 3);
    f.add(A_LONG_FIELD, 32000);
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_CARRAY_FIELD, "asdasd");
    
    CHECK(f.erase(A_DOUBLE_FIELD, 20) == false);
    CHECK(f.erase(A_STRING_FIELD, 1) == true);
    CHECK(f.has(A_STRING_FIELD, 1) == false);
    CHECK(f.has(A_STRING_FIELD, 0) == true);
    CHECK(f.has(A_STRING_FIELD) == true);
    CHECK(f.count(A_DOUBLE_FIELD) == 3);
    CHECK(f.erase(A_DOUBLE_FIELD, 0) == true);
    CHECK(f.count(A_DOUBLE_FIELD) == 2);
    CHECK(f.has(A_DOUBLE_FIELD,0) == true);
    CHECK(f.has(A_DOUBLE_FIELD,1) == true);
    CHECK(f.has(A_DOUBLE_FIELD,2) == false);
    CHECK(f.get_double(A_DOUBLE_FIELD,0) == 55.99);
    CHECK(f.get_double(A_DOUBLE_FIELD,1) == 69.01);
    CHECK(f.erase(A_DOUBLE_FIELD) == true);
    CHECK(f.has(A_DOUBLE_FIELD) == false);
    f.erase_all_but({A_CHAR_FIELD, A_CARRAY_FIELD});
    CHECK(f.has(A_LONG_FIELD) == false);
    CHECK(f.has(A_CHAR_FIELD) == true);
    CHECK(f.has(A_CARRAY_FIELD) == true);
    
    f.clear();
    CHECK(f.field_count() == 0);
}

// asan warns that Fdelete32 performs memcpy with
// overlapping memory segments
TEST_CASE("fml32 erase asan=replace_intrin")
{
    fml f;
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_DOUBLE_FIELD, 38.20);
    f.add(A_DOUBLE_FIELD, 55.99);
    f.add(A_DOUBLE_FIELD, 69.01);
    f.add(A_SHORT_FIELD, 3);
    f.add(A_LONG_FIELD, 32000);
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_CARRAY_FIELD, "asdasd");
    
    f.erase({A_STRING_FIELD, A_SHORT_FIELD});
    CHECK(f.has(A_STRING_FIELD) == false);
    CHECK(f.has(A_SHORT_FIELD) == false);
    CHECK(f.has(A_LONG_FIELD) == true);
    CHECK(f.has(A_CHAR_FIELD) == true);
    CHECK(f.has(A_CARRAY_FIELD) == true);
}

TEST_CASE("fml32 +=")
{
    fml f1;
    f1.add(A_STRING_FIELD, "hello");
    f1.add(A_STRING_FIELD, "world");
    f1.add(A_DOUBLE_FIELD, 38.20);
    f1.add(A_DOUBLE_FIELD, 55.99);
    
    fml f2;
    f2.add(A_DOUBLE_FIELD, 69.01);
    f2.add(A_SHORT_FIELD, 3);
    f2.add(A_LONG_FIELD, 32000);
    f2.add(A_CHAR_FIELD, 'b');
    f2.add(A_CARRAY_FIELD, "asdasd");
    
    f1 += f2;
    
    CHECK(f2.field_count() == 5);
    CHECK(f1.field_count() == 9);
    CHECK(f1.get_string(A_STRING_FIELD, 0) == "hello");
    CHECK(f1.get_string(A_STRING_FIELD, 1) == "world");
    CHECK(f1.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(38.20));
    CHECK(f1.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(55.99));
    CHECK(f1.get_double(A_DOUBLE_FIELD, 2) == doctest::Approx(69.01));
    CHECK(f1.get_short(A_SHORT_FIELD) == 3);
    CHECK(f1.get_long(A_LONG_FIELD) == 32000);
    CHECK(f1.get_char(A_CHAR_FIELD) == 'b');
    CHECK(f1.get_string(A_CARRAY_FIELD) == "asdasd");
}

TEST_CASE("fml32 join")
{
    fml f1;
    f1.add(A_STRING_FIELD, "hello");
    f1.add(A_STRING_FIELD, "world");
    f1.add(A_DOUBLE_FIELD, 38.20);
    f1.add(A_DOUBLE_FIELD, 55.99);
    
    fml f2;
    f2.add(A_STRING_FIELD, "goodbye");
    f2.add(A_STRING_FIELD, "planet");
    f2.add(A_STRING_FIELD, "...");
    f2.add(A_DOUBLE_FIELD, 69.01);
    f2.add(A_SHORT_FIELD, 3);
    f2.add(A_LONG_FIELD, 32000);
    f2.add(A_CHAR_FIELD, 'b');
    f2.add(A_CARRAY_FIELD, "asdasd");
    
    f1.join(f2);
    
    CHECK(f2.field_count() == 8);
    CHECK(f1.field_count() == 3);
    CHECK(f1.get_string(A_STRING_FIELD,0) == "goodbye");
    CHECK(f1.get_string(A_STRING_FIELD,1) == "planet");
    CHECK(f1.get_double(A_DOUBLE_FIELD) == doctest::Approx(69.01));
}

TEST_CASE("fml32 outer_join")
{
    fml f1;
    f1.add(A_STRING_FIELD, "hello");
    f1.add(A_STRING_FIELD, "world");
    f1.add(A_DOUBLE_FIELD, 38.20);
    f1.add(A_DOUBLE_FIELD, 55.99);
    
    fml f2;
    f2.add(A_STRING_FIELD, "goodbye");
    f2.add(A_STRING_FIELD, "planet");
    f2.add(A_STRING_FIELD, "...");
    f2.add(A_DOUBLE_FIELD, 69.01);
    f2.add(A_SHORT_FIELD, 3);
    f2.add(A_LONG_FIELD, 32000);
    f2.add(A_CHAR_FIELD, 'b');
    f2.add(A_CARRAY_FIELD, "asdasd");
    
    f1.outer_join(f2);
    
    CHECK(f2.field_count() == 8);
    CHECK(f1.field_count() == 4);
    CHECK(f1.get_string(A_STRING_FIELD,0) == "goodbye");
    CHECK(f1.get_string(A_STRING_FIELD,1) == "planet");
    CHECK(f1.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(69.01));
    CHECK(f1.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(55.99));
}

TEST_CASE("fml32 project")
{
    fml f1;
    f1.add(A_STRING_FIELD, "goodbye");
    f1.add(A_STRING_FIELD, "planet");
    f1.add(A_STRING_FIELD, "...");
    f1.add(A_DOUBLE_FIELD, 69.01);
    f1.add(A_SHORT_FIELD, 3);
    f1.add(A_LONG_FIELD, 32000);
    f1.add(A_CHAR_FIELD, 'b');
    f1.add(A_CARRAY_FIELD, "asdasd");
    
    fml f2 = f1.project({A_LONG_FIELD, A_CHAR_FIELD});
    
    CHECK(f1.field_count() == 8);
    CHECK(f2.field_count() == 2);
    CHECK(f2.get_long(A_LONG_FIELD) == 32000);
    CHECK(f2.get_char(A_CHAR_FIELD) == 'b');
}

TEST_CASE("fml32 update")
{
    fml f1;
    f1.add(A_STRING_FIELD, "hello");
    f1.add(A_STRING_FIELD, "world");
    f1.add(A_DOUBLE_FIELD, 38.20);
    f1.add(A_DOUBLE_FIELD, 55.99);
    f1.add(A_FLOAT_FIELD, 2.5);
    
    fml f2;
    f2.add(A_STRING_FIELD, "goodbye");
    f2.add(A_STRING_FIELD, "planet");
    f2.add(A_STRING_FIELD, "...");
    f2.add(A_DOUBLE_FIELD, 69.01);
    f2.add(A_SHORT_FIELD, 3);
    f2.add(A_LONG_FIELD, 32000);
    f2.add(A_CHAR_FIELD, 'b');
    f2.add(A_CARRAY_FIELD, "asdasd");
    
    f1.update(f2);
    
    CHECK(f2.field_count() == 8);
    CHECK(f1.field_count() == 10);
    CHECK(f1.get_string(A_STRING_FIELD,0) == "goodbye");
    CHECK(f1.get_string(A_STRING_FIELD,1) == "planet");
    CHECK(f1.get_string(A_STRING_FIELD,2) == "...");
    CHECK(f1.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(69.01));
    CHECK(f1.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(55.99));
    CHECK(f1.get_float(A_FLOAT_FIELD) == doctest::Approx(2.5));
    CHECK(f1.get_short(A_SHORT_FIELD) == 3);
    CHECK(f1.get_long(A_LONG_FIELD) == 32000);
    CHECK(f1.get_char(A_CHAR_FIELD) == 'b');
    CHECK(f1.get_string(A_CARRAY_FIELD) == "asdasd");
}

TEST_CASE("fml32 get/set/clear encoding_name")
{
    fml f;
    CHECK(f.get_encoding_name() == "");
    f.set_encoding_name("SHIFT-JIS");
    CHECK(f.get_encoding_name() == "SHIFT-JIS");
    f.clear_encoding_name();
    CHECK(f.get_encoding_name() == "");
}

// asan warns that tpconvfmb32 performs strcpy with
// overlapping memory segments
TEST_CASE("fml32 convert_mbstrings asan=replace_str")
{
    fml f;
    f.add(AN_MBSTRING_FIELD, packed_mbstring{"mbstring1","SHIFT-JIS"});
    f.add(MBSTRING_FIELD_2, packed_mbstring{"mbstring2","SHIFT-JIS"});
    f.add(MBSTRING_FIELD_3, packed_mbstring{"mbstring3","SHIFT-JIS"});
    
    f.convert_mbstrings({AN_MBSTRING_FIELD, MBSTRING_FIELD_2}, "EUC-JP");
    
    CHECK(f.get_mbstring(AN_MBSTRING_FIELD).encoding == "EUC-JP");
    CHECK(f.get_mbstring(MBSTRING_FIELD_2).encoding == "EUC-JP");
    CHECK(f.get_mbstring(MBSTRING_FIELD_3).encoding == "SHIFT-JIS");
    
    f.convert_mbstrings("EUC-JP");
    
    CHECK(f.get_mbstring(AN_MBSTRING_FIELD).encoding == "EUC-JP");
    CHECK(f.get_mbstring(MBSTRING_FIELD_2).encoding == "EUC-JP");
    CHECK(f.get_mbstring(MBSTRING_FIELD_3).encoding == "EUC-JP");
    
    f.clear();
    
    f.set_encoding_name("SHIFT-JIS");
    put_env("TPMBENC", "EUC-JP");
    f.add(AN_MBSTRING_FIELD, packed_mbstring{"mbstring1", mbpack_option::env});
    f.add(MBSTRING_FIELD_2, packed_mbstring{"mbstring2", mbpack_option::buffer});
    f.add(MBSTRING_FIELD_3, packed_mbstring{"mbstring3","SHIFT-JIS"});
    
    f.convert_mbstrings("EUC-JP");
    
    CHECK(f.get_mbstring(AN_MBSTRING_FIELD).encoding == "EUC-JP");
    CHECK(f.get_mbstring(MBSTRING_FIELD_2).encoding == "");
    CHECK(f.get_mbstring(MBSTRING_FIELD_3).encoding == "EUC-JP");    
}

TEST_CASE("fml32 add short")
{
    fml f;
    f.add(A_SHORT_FIELD, short(3));
    f.add(A_SHORT_FIELD, long(100));
    f.add(A_SHORT_FIELD, char(60));
    f.add(A_SHORT_FIELD, float(20.2));
    f.add(A_SHORT_FIELD, double(100.01));
    f.add(A_SHORT_FIELD, "27");
    CHECK(f.get_short(A_SHORT_FIELD, 0) == 3);
    CHECK(f.get_short(A_SHORT_FIELD, 1) == 100);
    CHECK(f.get_short(A_SHORT_FIELD, 2) == 60);
    CHECK(f.get_short(A_SHORT_FIELD, 3) == 20);
    CHECK(f.get_short(A_SHORT_FIELD, 4) == 100);
    CHECK(f.get_short(A_SHORT_FIELD, 5) == 27);
}

TEST_CASE("fml32 add long")
{
    fml f;
    f.add(A_LONG_FIELD, short(3));
    f.add(A_LONG_FIELD, long(30000));
    f.add(A_LONG_FIELD, char(60));
    f.add(A_LONG_FIELD, float(20.2));
    f.add(A_LONG_FIELD, double(100.01));
    f.add(A_LONG_FIELD, "27");
    CHECK(f.get_long(A_LONG_FIELD, 0) == 3);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 30000);
    CHECK(f.get_long(A_LONG_FIELD, 2) == 60);
    CHECK(f.get_long(A_LONG_FIELD, 3) == 20);
    CHECK(f.get_long(A_LONG_FIELD, 4) == 100);
    CHECK(f.get_long(A_LONG_FIELD, 5) == 27);
}

TEST_CASE("fml32 add int")
{
    fml f;
    f.add(A_LONG_FIELD, int(3));
    CHECK(f.get_long(A_LONG_FIELD, 0) == 3);
}

TEST_CASE("fml32 add char")
{
    fml fb;
    short s = 'a';
    long l = 'b';
    char c = 'C';
    float f = 'd';
    double d = 'e';
    string str = "foo";
    
    fb.add(A_CHAR_FIELD, s);
    fb.add(A_CHAR_FIELD, l);
    fb.add(A_CHAR_FIELD, c);
    fb.add(A_CHAR_FIELD, f);
    fb.add(A_CHAR_FIELD, d);
    fb.add(A_CHAR_FIELD, str);
    CHECK(fb.get_char(A_CHAR_FIELD, 0) == 'a');
    CHECK(fb.get_char(A_CHAR_FIELD, 1) == 'b');
    CHECK(fb.get_char(A_CHAR_FIELD, 2) == 'C');
    CHECK(fb.get_char(A_CHAR_FIELD, 3) == 'd');
    CHECK(fb.get_char(A_CHAR_FIELD, 4) == 'e');
    CHECK(fb.get_char(A_CHAR_FIELD, 5) == 'f');
}

TEST_CASE("fml32 add float")
{
    fml f;
    f.add(A_FLOAT_FIELD, short(3));
    f.add(A_FLOAT_FIELD, long(30000));
    f.add(A_FLOAT_FIELD, char(60));
    f.add(A_FLOAT_FIELD, float(20.2));
    f.add(A_FLOAT_FIELD, double(100.01));
    f.add(A_FLOAT_FIELD, "27.1");
    CHECK(f.get_float(A_FLOAT_FIELD, 0) == doctest::Approx(3));
    CHECK(f.get_float(A_FLOAT_FIELD, 1) == doctest::Approx(30000));
    CHECK(f.get_float(A_FLOAT_FIELD, 2) == doctest::Approx(60));
    CHECK(f.get_float(A_FLOAT_FIELD, 3) == doctest::Approx(20.2));
    CHECK(f.get_float(A_FLOAT_FIELD, 4) == doctest::Approx(100.01));
    CHECK(f.get_float(A_FLOAT_FIELD, 5) == doctest::Approx(27.1));
}

TEST_CASE("fml32 add double")
{
    fml f;
    f.add(A_DOUBLE_FIELD, short(3));
    f.add(A_DOUBLE_FIELD, long(30000));
    f.add(A_DOUBLE_FIELD, char(60));
    f.add(A_DOUBLE_FIELD, float(20.2));
    f.add(A_DOUBLE_FIELD, double(100.01));
    f.add(A_DOUBLE_FIELD, "27.1");
    CHECK(f.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(3));
    CHECK(f.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(30000));
    CHECK(f.get_double(A_DOUBLE_FIELD, 2) == doctest::Approx(60));
    CHECK(f.get_double(A_DOUBLE_FIELD, 3) == doctest::Approx(20.2));
    CHECK(f.get_double(A_DOUBLE_FIELD, 4) == doctest::Approx(100.01));
    CHECK(f.get_double(A_DOUBLE_FIELD, 5) == doctest::Approx(27.1));
}

namespace {
string rtrim_zeroes(string x)
{
    x.erase(x.find_last_not_of('0') + 1);
    return x; 
}
}

TEST_CASE("fml32 add string")
{
    fml f;
    f.add(A_STRING_FIELD, short(3));
    f.add(A_STRING_FIELD, long(30000));
    f.add(A_STRING_FIELD, char('Z'));
    f.add(A_STRING_FIELD, float(20.2));
    f.add(A_STRING_FIELD, double(100.01));
    f.add(A_STRING_FIELD, "hello world");
    CHECK(f.get_string(A_STRING_FIELD, 0) == "3");
    CHECK(f.get_string(A_STRING_FIELD, 1) == "30000");
    CHECK(f.get_string(A_STRING_FIELD, 2) == "Z");
    CHECK(rtrim_zeroes(f.get_string(A_STRING_FIELD, 3)) == "20.2");
    CHECK(rtrim_zeroes(f.get_string(A_STRING_FIELD, 4)) == "100.01");
    CHECK(f.get_string(A_STRING_FIELD, 5) == "hello world");
}

TEST_CASE("fml32 add string (carray)")
{
    fml f;
    f.add(A_CARRAY_FIELD, short(3));
    f.add(A_CARRAY_FIELD, long(30000));
    f.add(A_CARRAY_FIELD, char('Z'));
    f.add(A_CARRAY_FIELD, float(20.2));
    f.add(A_CARRAY_FIELD, double(100.01));
    f.add(A_CARRAY_FIELD, "hello world");
    CHECK(f.get_string(A_CARRAY_FIELD, 0) == "3");
    CHECK(f.get_string(A_CARRAY_FIELD, 1) == "30000");
    CHECK(f.get_string(A_CARRAY_FIELD, 2) == "Z");
    CHECK(rtrim_zeroes(f.get_string(A_CARRAY_FIELD, 3)) == "20.2");
    CHECK(rtrim_zeroes(f.get_string(A_CARRAY_FIELD, 4)) == "100.01");
    CHECK(f.get_string(A_CARRAY_FIELD, 5) == "hello world");
}

TEST_CASE("fml32 add mbstring")
{
    fml f;
    CHECK_THROWS(f.add(AN_MBSTRING_FIELD, 3));
    CHECK_THROWS(f.add(AN_MBSTRING_FIELD, "hello world"));
    
    put_env("TPMBENC", "UTF-16LE");
    f.set_encoding_name("SHIFT-JIS");
    
    f.add(AN_MBSTRING_FIELD, packed_mbstring("mbstring1"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("mbstring2", mbpack_option::buffer));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("mbstring3", "EUC-JP"));
    
    unpacked_mbstring unpacked;
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 0);
    CHECK(unpacked.data == "mbstring1");
    CHECK(unpacked.encoding == "UTF-16LE");
    
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 1);
    CHECK(unpacked.data == "mbstring2");
    CHECK(unpacked.encoding == "");
    CHECK(f.get_encoding_name() == "SHIFT-JIS");
    
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 2);
    CHECK(unpacked.data == "mbstring3");
    CHECK(unpacked.encoding == "EUC-JP");
}

TEST_CASE("fml32 add fml32")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    
    fml b;
    CHECK_THROWS(b.add(AN_FML32_FIELD, 35));
    CHECK_THROWS(b.add(AN_FML32_FIELD, "hello world"));
    b.add(AN_FML32_FIELD, a);
    
    a.clear();
    a.add(A_DOUBLE_FIELD, 35.05);
    a.add(A_STRING_FIELD, "hello");
    a.add(A_STRING_FIELD, "world");
    
    b.add(AN_FML32_FIELD, a);
    
    fml c = b.get_fml(AN_FML32_FIELD, 0);
    CHECK(c.get_long(A_LONG_FIELD) == 2);
    CHECK(c.get_string(A_STRING_FIELD) == "hello world");
    
    fml d = b.get_fml(AN_FML32_FIELD, 1);
    CHECK(d.get_double(A_DOUBLE_FIELD) == doctest::Approx(35.05));
    CHECK(d.get_string(A_STRING_FIELD, 0) == "hello");
    CHECK(d.get_string(A_STRING_FIELD, 1) == "world");
}

TEST_CASE("fml32 add_ptr")
{
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    cstring s("hello world");
    buffer b("CARRAY", nullptr, 24);
    
    f.add_ptr(A_PTR_FIELD, s.data());
    f.add_ptr(A_PTR_FIELD, b.data());
    
    CHECK((const char*)f.get_ptr(A_PTR_FIELD, 0) == string("hello world"));
    CHECK(f.get_ptr(A_PTR_FIELD, 0) == s.data());
    CHECK(f.get_ptr(A_PTR_FIELD, 1) == b.data());             
}

TEST_CASE("fml32 add_view")
{
    my_struct s1;
    s1.d = 35.5;
    s1.l = 200;
    s1.f = 53.2;
    
    string_info s2;
    s2.ascii_sum = 500;
    s2.byte_count = 35;
    s2.most_frequent_char = 'c';
    set(s2.original_string, "hello world");
    
    struct random_struct
    {
        int i;
    };
    random_struct rs;
    
    fml f;
    CHECK_THROWS(f.add_view(A_VIEW32_FIELD, rs)); // random_struct has no view file
    f.add_view(A_VIEW32_FIELD, s1);
    f.add_view(A_VIEW32_FIELD, s2);
    
    CHECK_THROWS(f.get_view<string_info>(A_VIEW32_FIELD, 0)); // types can't mismatch
    CHECK_THROWS(f.get_view<my_struct>(A_VIEW32_FIELD, 1)); // types can't mismatch
    
    auto s3 = f.get_view<my_struct>(A_VIEW32_FIELD, 0);
    auto s4 = f.get_view<string_info>(A_VIEW32_FIELD, 1);
    
    CHECK(s3.d == doctest::Approx(35.5));
    CHECK(s3.l == 200);
    CHECK(s3.f == doctest::Approx(53.2));
    
    CHECK(s4.ascii_sum == 500);
    CHECK(s4.byte_count == 35);
    CHECK(s4.most_frequent_char == 'c');
    CHECK(s4.original_string == string("hello world"));
}

TEST_CASE("fml32 append short")
{
    fml f;
    CHECK_THROWS(f.append(A_SHORT_FIELD, long(100)));
    CHECK_THROWS(f.append(A_SHORT_FIELD, char(60)));
    CHECK_THROWS(f.append(A_SHORT_FIELD, float(20.2)));
    CHECK_THROWS(f.append(A_SHORT_FIELD, double(100.01)));
    CHECK_THROWS(f.append(A_SHORT_FIELD, "27"));
    f.append(A_SHORT_FIELD, short(3));
    f.append(A_SHORT_FIELD, short(4));
    f.append(A_SHORT_FIELD, short(5));
    CHECK_THROWS(f.get_short(A_SHORT_FIELD, 0));
    f.index();
    CHECK(f.get_short(A_SHORT_FIELD, 0) == 3);
    CHECK(f.get_short(A_SHORT_FIELD, 1) == 4);
    CHECK(f.get_short(A_SHORT_FIELD, 2) == 5);
}

TEST_CASE("fml32 append long")
{
    fml f;
    CHECK_THROWS(f.append(A_LONG_FIELD, short(100)));
    CHECK_THROWS(f.append(A_LONG_FIELD, char(60)));
    f.append(A_LONG_FIELD, long(3));
    f.append(A_LONG_FIELD, long(4));
    f.append(A_LONG_FIELD, long(5));
    f.unindex();
    CHECK(f.get_long(A_LONG_FIELD, 0) == 3);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 4);
    CHECK(f.get_long(A_LONG_FIELD, 2) == 5);
}

TEST_CASE("fml32 append int")
{
    fml f;
    f.append(A_LONG_FIELD, int(3));
    f.append(A_LONG_FIELD, int(4));
    f.append(A_LONG_FIELD, int(5));
    f.index();
    CHECK(f.get_long(A_LONG_FIELD, 0) == 3);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 4);
    CHECK(f.get_long(A_LONG_FIELD, 2) == 5);
}

TEST_CASE("fml32 append char")
{
    fml f;
    CHECK_THROWS(f.append(A_CHAR_FIELD, short(100)));
    CHECK_THROWS(f.append(A_CHAR_FIELD, long(60)));
    f.append(A_CHAR_FIELD, 'a');
    f.append(A_CHAR_FIELD, 'b');
    f.append(A_CHAR_FIELD, 'c');
    f.unindex();
    CHECK(f.get_char(A_CHAR_FIELD, 0) == 'a');
    CHECK(f.get_char(A_CHAR_FIELD, 1) == 'b');
    CHECK(f.get_char(A_CHAR_FIELD, 2) == 'c');
}

TEST_CASE("fml32 append float")
{
    fml f;
    CHECK_THROWS(f.append(A_FLOAT_FIELD, double(0.5)));
    CHECK_THROWS(f.append(A_FLOAT_FIELD, long(0)));
    f.append(A_FLOAT_FIELD, float(0.5));
    f.append(A_FLOAT_FIELD, float(1.6));
    f.append(A_FLOAT_FIELD, float(2.7));
    f.index();
    CHECK(f.get_float(A_FLOAT_FIELD, 0) == doctest::Approx(0.5));
    CHECK(f.get_float(A_FLOAT_FIELD, 1) == doctest::Approx(1.6));
    CHECK(f.get_float(A_FLOAT_FIELD, 2) == doctest::Approx(2.7));
}

TEST_CASE("fml32 append double")
{
    fml f;
    CHECK_THROWS(f.append(A_DOUBLE_FIELD, float(0.5)));
    CHECK_THROWS(f.append(A_DOUBLE_FIELD, long(0)));
    f.append(A_DOUBLE_FIELD, double(0.5));
    f.append(A_DOUBLE_FIELD, double(1.6));
    f.append(A_DOUBLE_FIELD, double(2.7));
    f.unindex();
    CHECK(f.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(0.5));
    CHECK(f.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(1.6));
    CHECK(f.get_double(A_DOUBLE_FIELD, 2) == doctest::Approx(2.7));
}

TEST_CASE("fml32 append string")
{
    fml f;
    CHECK_THROWS(f.append(A_STRING_FIELD, float(0.5)));
    CHECK_THROWS(f.append(A_STRING_FIELD, long(0)));
    f.append(A_STRING_FIELD, "hello");
    f.append(A_STRING_FIELD, "world");
    f.append(A_STRING_FIELD, "!");
    f.index();
    CHECK(f.get_string(A_STRING_FIELD, 0) == "hello");
    CHECK(f.get_string(A_STRING_FIELD, 1) == "world");
    CHECK(f.get_string(A_STRING_FIELD, 2) == "!");
}

TEST_CASE("fml32 append string(carray)")
{
    fml f;
    CHECK_THROWS(f.append(A_CARRAY_FIELD, float(0.5)));
    CHECK_THROWS(f.append(A_CARRAY_FIELD, long(0)));
    f.append(A_CARRAY_FIELD, "hello");
    f.append(A_CARRAY_FIELD, "world");
    f.append(A_CARRAY_FIELD, "!");
    f.unindex();
    CHECK(f.get_string(A_CARRAY_FIELD, 0) == "hello");
    CHECK(f.get_string(A_CARRAY_FIELD, 1) == "world");
    CHECK(f.get_string(A_CARRAY_FIELD, 2) == "!");
}

TEST_CASE("fml32 append mbstring")
{
    fml f;
    CHECK_THROWS(f.append(AN_MBSTRING_FIELD, float(0.5)));
    CHECK_THROWS(f.append(AN_MBSTRING_FIELD, "hello world"));
    f.append(AN_MBSTRING_FIELD, packed_mbstring("hello", "SHIFT-JIS"));
    f.append(AN_MBSTRING_FIELD, packed_mbstring("world", "EUC-JP"));
    f.append(AN_MBSTRING_FIELD, packed_mbstring("!", "SHIFT-JIS"));
    f.index();
    unpacked_mbstring unpacked;
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 0);
    CHECK(unpacked.data == "hello");
    CHECK(unpacked.encoding == "SHIFT-JIS");
    
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 1);
    CHECK(unpacked.data == "world");
    CHECK(unpacked.encoding == "EUC-JP");
    
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 2);
    CHECK(unpacked.data == "!");
    CHECK(unpacked.encoding == "SHIFT-JIS");
}

TEST_CASE("fml32 append fml32")
{
    fml a;
    a.add(A_LONG_FIELD, 2);
    a.add(A_STRING_FIELD, "hello world");
    
    fml b;
    CHECK_THROWS(b.append(AN_FML32_FIELD, 35));
    CHECK_THROWS(b.append(AN_FML32_FIELD, "hello world"));
    b.append(AN_FML32_FIELD, a);
    
    a.clear();
    a.add(A_DOUBLE_FIELD, 35.05);
    a.add(A_STRING_FIELD, "hello");
    a.add(A_STRING_FIELD, "world");
    
    b.append(AN_FML32_FIELD, a);
    
    b.unindex();
    
    fml c = b.get_fml(AN_FML32_FIELD, 0);
    CHECK(c.get_long(A_LONG_FIELD) == 2);
    CHECK(c.get_string(A_STRING_FIELD) == "hello world");
    
    fml d = b.get_fml(AN_FML32_FIELD, 1);
    CHECK(d.get_double(A_DOUBLE_FIELD) == doctest::Approx(35.05));
    CHECK(d.get_string(A_STRING_FIELD, 0) == "hello");
    CHECK(d.get_string(A_STRING_FIELD, 1) == "world");
}

TEST_CASE("fml32 append_ptr")
{
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    buffer b("STRING");
    cstring s1("hello");
    cstring s2("world");
    
    
    f.append_ptr(A_PTR_FIELD, b.data());
    f.append_ptr(A_PTR_FIELD, s1.data());
    f.append_ptr(A_PTR_FIELD, s2.data());
    f.index();
    
    CHECK(f.get_ptr(A_PTR_FIELD, 0) == b.data());
    CHECK(f.get_ptr(A_PTR_FIELD, 1) == s1.data());
    CHECK(f.get_ptr(A_PTR_FIELD, 2) == s2.data());
}

TEST_CASE("fml32 append_view")
{
    my_struct s1;
    s1.d = 35.5;
    s1.l = 200;
    s1.f = 53.2;
    
    string_info s2;
    s2.ascii_sum = 500;
    s2.byte_count = 35;
    s2.most_frequent_char = 'c';
    set(s2.original_string, "hello world");
    
    struct random_struct
    {
        int i;
    };
    random_struct rs;
    
    fml f;
    CHECK_THROWS(f.append_view(A_VIEW32_FIELD, rs)); // random_struct has no view file
    f.append_view(A_VIEW32_FIELD, s1);
    f.append_view(A_VIEW32_FIELD, s2);
    f.unindex();
    
    auto s3 = f.get_view<my_struct>(A_VIEW32_FIELD, 0);
    auto s4 = f.get_view<string_info>(A_VIEW32_FIELD, 1);
    
    CHECK(s3.d == doctest::Approx(35.5));
    CHECK(s3.l == 200);
    CHECK(s3.f == doctest::Approx(53.2));
    
    CHECK(s4.ascii_sum == 500);
    CHECK(s4.byte_count == 35);
    CHECK(s4.most_frequent_char == 'c');
    CHECK(s4.original_string == string("hello world"));
}

TEST_CASE("fml32 set short")
{
    fml f;
    f.set(A_SHORT_FIELD, short(3), 1);
    CHECK(f.get_short(A_SHORT_FIELD, 0) == 0);
    CHECK(f.get_short(A_SHORT_FIELD, 1) == 3);
    
    f.set(A_SHORT_FIELD, long(100), 0);
    f.set(A_SHORT_FIELD, char(60), 2);
    f.set(A_SHORT_FIELD, float(20.2), 3);
    f.set(A_SHORT_FIELD, double(100.01), 4);
    f.set(A_SHORT_FIELD, "27", 5);
    
    CHECK(f.get_short(A_SHORT_FIELD, 0) == 100);
    CHECK(f.get_short(A_SHORT_FIELD, 1) == 3);
    CHECK(f.get_short(A_SHORT_FIELD, 2) == 60);
    CHECK(f.get_short(A_SHORT_FIELD, 3) == 20);
    CHECK(f.get_short(A_SHORT_FIELD, 4) == 100);
    CHECK(f.get_short(A_SHORT_FIELD, 5) == 27);
}

TEST_CASE("fml32 set long")
{
    fml f;
    f.set(A_LONG_FIELD, long(3), 1);
    CHECK(f.get_long(A_LONG_FIELD, 0) == 0);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 3);
    
    f.set(A_LONG_FIELD, short(100), 0);
    f.set(A_LONG_FIELD, char(60), 2);
    f.set(A_LONG_FIELD, float(20.2), 3);
    f.set(A_LONG_FIELD, double(100.01), 4);
    f.set(A_LONG_FIELD, "27", 5);
    
    CHECK(f.get_long(A_LONG_FIELD, 0) == 100);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 3);
    CHECK(f.get_long(A_LONG_FIELD, 2) == 60);
    CHECK(f.get_long(A_LONG_FIELD, 3) == 20);
    CHECK(f.get_long(A_LONG_FIELD, 4) == 100);
    CHECK(f.get_long(A_LONG_FIELD, 5) == 27);
}

TEST_CASE("fml32 set int")
{
    fml f;
    f.set(A_LONG_FIELD, int(3), 1);
    CHECK(f.get_long(A_LONG_FIELD, 0) == 0);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 3);
    
    f.set(A_LONG_FIELD, short(100), 0);
    f.set(A_LONG_FIELD, char(60), 2);
    f.set(A_LONG_FIELD, float(20.2), 3);
    f.set(A_LONG_FIELD, double(100.01), 4);
    f.set(A_LONG_FIELD, "27", 5);
    
    CHECK(f.get_long(A_LONG_FIELD, 0) == 100);
    CHECK(f.get_long(A_LONG_FIELD, 1) == 3);
    CHECK(f.get_long(A_LONG_FIELD, 2) == 60);
    CHECK(f.get_long(A_LONG_FIELD, 3) == 20);
    CHECK(f.get_long(A_LONG_FIELD, 4) == 100);
    CHECK(f.get_long(A_LONG_FIELD, 5) == 27);
}

TEST_CASE("fml32 set char")
{
    fml f;
    f.set(A_CHAR_FIELD, 'a', 1);
    CHECK(f.get_char(A_CHAR_FIELD, 0) == char(0));
    CHECK(f.get_char(A_CHAR_FIELD, 1) == 'a');
    
    f.set(A_CHAR_FIELD, long(100), 0);
    f.set(A_CHAR_FIELD, char(60), 2);
    f.set(A_CHAR_FIELD, float(20.2), 3);
    f.set(A_CHAR_FIELD, double(100.01), 4);
    f.set(A_CHAR_FIELD, "hello", 5);
    
    CHECK(f.get_char(A_CHAR_FIELD, 0) == 100);
    CHECK(f.get_char(A_CHAR_FIELD, 1) == 'a');
    CHECK(f.get_char(A_CHAR_FIELD, 2) == 60);
    CHECK(f.get_char(A_CHAR_FIELD, 3) == 20);
    CHECK(f.get_char(A_CHAR_FIELD, 4) == 100);
    CHECK(f.get_char(A_CHAR_FIELD, 5) == 'h');
}

TEST_CASE("fml32 set float")
{
    fml f;
    f.set(A_FLOAT_FIELD, float(3.2), 1);
    CHECK(f.get_float(A_FLOAT_FIELD, 0) == doctest::Approx(0));
    CHECK(f.get_float(A_FLOAT_FIELD, 1) == doctest::Approx(3.2));
    
    f.set(A_FLOAT_FIELD, long(100), 0);
    f.set(A_FLOAT_FIELD, char(60), 2);
    f.set(A_FLOAT_FIELD, float(20.2), 3);
    f.set(A_FLOAT_FIELD, double(100.01), 4);
    f.set(A_FLOAT_FIELD, "38.5", 5);
    
    CHECK(f.get_float(A_FLOAT_FIELD, 0) == doctest::Approx(100));
    CHECK(f.get_float(A_FLOAT_FIELD, 1) == doctest::Approx(3.2));
    CHECK(f.get_float(A_FLOAT_FIELD, 2) == doctest::Approx(60));
    CHECK(f.get_float(A_FLOAT_FIELD, 3) == doctest::Approx(20.2));
    CHECK(f.get_float(A_FLOAT_FIELD, 4) == doctest::Approx(100.01));
    CHECK(f.get_float(A_FLOAT_FIELD, 5) == doctest::Approx(38.5));
}

TEST_CASE("fml32 set double")
{
    fml f;
    f.set(A_DOUBLE_FIELD, double(3.2), 1);
    CHECK(f.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(0));
    CHECK(f.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(3.2));
    
    f.set(A_DOUBLE_FIELD, long(100), 0);
    f.set(A_DOUBLE_FIELD, char(60), 2);
    f.set(A_DOUBLE_FIELD, float(20.2), 3);
    f.set(A_DOUBLE_FIELD, double(100.01), 4);
    f.set(A_DOUBLE_FIELD, "38.5", 5);
    
    CHECK(f.get_double(A_DOUBLE_FIELD, 0) == doctest::Approx(100));
    CHECK(f.get_double(A_DOUBLE_FIELD, 1) == doctest::Approx(3.2));
    CHECK(f.get_double(A_DOUBLE_FIELD, 2) == doctest::Approx(60));
    CHECK(f.get_double(A_DOUBLE_FIELD, 3) == doctest::Approx(20.2));
    CHECK(f.get_double(A_DOUBLE_FIELD, 4) == doctest::Approx(100.01));
    CHECK(f.get_double(A_DOUBLE_FIELD, 5) == doctest::Approx(38.5));
}

TEST_CASE("fml32 set string")
{
    fml f;
    f.set(A_STRING_FIELD, "hello", 1);
    CHECK(f.get_string(A_STRING_FIELD, 0) == "");
    CHECK(f.get_string(A_STRING_FIELD, 1) == "hello");
    
    f.set(A_STRING_FIELD, "world", 0);
    f.set(A_STRING_FIELD, "!", 2);
    f.set(A_STRING_FIELD, 'a', 3);
    f.set(A_STRING_FIELD, double(100.01), 4);
    f.set(A_STRING_FIELD, 6000, 5);
    
    CHECK(f.get_string(A_STRING_FIELD, 0) == "world");
    CHECK(f.get_string(A_STRING_FIELD, 1) == "hello");
    CHECK(f.get_string(A_STRING_FIELD, 2) == "!");
    CHECK(f.get_string(A_STRING_FIELD, 3) == "a");
    CHECK(rtrim_zeroes(f.get_string(A_STRING_FIELD, 4)) == "100.01");
    CHECK(f.get_string(A_STRING_FIELD, 5) == "6000"); 
}

TEST_CASE("fml32 set string(carray)")
{
    fml f;
    f.set(A_CARRAY_FIELD, "hello", 1);
    CHECK(f.get_string(A_CARRAY_FIELD, 0) == "");
    CHECK(f.get_string(A_CARRAY_FIELD, 1) == "hello");
    
    f.set(A_CARRAY_FIELD, "world", 0);
    f.set(A_CARRAY_FIELD, "!", 2);
    f.set(A_CARRAY_FIELD, 'a', 3);
    f.set(A_CARRAY_FIELD, double(100.01), 4);
    f.set(A_CARRAY_FIELD, 6000, 5);
    
    CHECK(f.get_string(A_CARRAY_FIELD, 0) == "world");
    CHECK(f.get_string(A_CARRAY_FIELD, 1) == "hello");
    CHECK(f.get_string(A_CARRAY_FIELD, 2) == "!");
    CHECK(f.get_string(A_CARRAY_FIELD, 3) == "a");
    CHECK(rtrim_zeroes(f.get_string(A_CARRAY_FIELD, 4)) == "100.01");
    CHECK(f.get_string(A_CARRAY_FIELD, 5) == "6000"); 
}

TEST_CASE("fml32 set mbstring")
{
    fml f;
    unpacked_mbstring unpacked;
    put_env("TPMBENC", "UTF-16LE");
    f.set_encoding_name("SHIFT-JIS");
    
    // you can't explicitly insert an empty mbstring
    // into an fml data structure
    CHECK_THROWS(f.add(AN_MBSTRING_FIELD, packed_mbstring("")));
    CHECK_THROWS(f.set(AN_MBSTRING_FIELD, packed_mbstring(""), 0));
    
    // but you can implicitly do so by setting a
    // higher index
    f.set(AN_MBSTRING_FIELD, packed_mbstring("hello"), 1);
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 0);
    CHECK(unpacked.data == "");
    CHECK(unpacked.encoding == "");
    unpacked = f.get_mbstring(AN_MBSTRING_FIELD, 1);
    CHECK(unpacked.data == "hello");
    CHECK(unpacked.encoding == "UTF-16LE");
}

TEST_CASE("fml32 set fml32")
{
    fml f1;

    fml f2;
    f2.add(A_STRING_FIELD, "hello");
    f2.add(A_LONG_FIELD, 50);
    
    f1.set(AN_FML32_FIELD, f2, 1);
    
    fml a = f1.get_fml(AN_FML32_FIELD, 0);
    fml b = f1.get_fml(AN_FML32_FIELD, 1);
    
    CHECK(a.field_count() == 0);
    CHECK(b.get_string(A_STRING_FIELD) == "hello");
    CHECK(b.get_long(A_LONG_FIELD) == 50);
}

TEST_CASE("fml32 set_ptr")
{
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    cstring s("hello");
    f.set_ptr(A_PTR_FIELD, s.data(), 1);
    
    CHECK(f.get_ptr(A_PTR_FIELD, 0) == nullptr);
    CHECK(f.get_ptr(A_PTR_FIELD, 1) == s.data());
}

TEST_CASE("fml32 set_view")
{
    string_info s;
    s.ascii_sum = 500;
    s.byte_count = 50;
    s.most_frequent_char = 'n';
    set(s.original_string, "hello world");
    
    fml f;
    f.set_view(A_VIEW32_FIELD, s, 1);
    
    // there appears to be a catastrophic bug in Tuxedo
    // that prevents the follow code from working (at all)
    /*auto s1 = f.get_view<string_info>(A_VIEW32_FIELD, 0);
    CHECK(s1.ascii_sum == 0);
    CHECK(s1.byte_count == 0);
    CHECK(s1.most_frequent_char == 0);
    CHECK(s1.original_string == string());*/
    
    auto s2 = f.get_view<string_info>(A_VIEW32_FIELD, 1);
    CHECK(s2.ascii_sum == 500);
    CHECK(s2.byte_count == 50);
    CHECK(s2.most_frequent_char == 'n');
    CHECK(s2.original_string == string("hello world")); 
}

TEST_CASE("fml32 get_short")
{
    fml f;
    f.add(A_SHORT_FIELD, 5);
    f.add(A_LONG_FIELD, 20);
    f.add(A_CHAR_FIELD, 'a');
    f.add(A_FLOAT_FIELD, 20.5);
    f.add(A_DOUBLE_FIELD, 50.99);
    f.add(A_STRING_FIELD, "17");
    
    CHECK(f.get_short(A_SHORT_FIELD) == 5);
    CHECK(f.get_short(A_LONG_FIELD) == 20);
    CHECK(f.get_short(A_CHAR_FIELD) == 'a');
    CHECK(f.get_short(A_FLOAT_FIELD) == 20);
    CHECK(f.get_short(A_DOUBLE_FIELD) == 50);
    CHECK(f.get_short(A_STRING_FIELD) == 17);
}

TEST_CASE("fml32 get_long")
{
    fml f;
    f.add(A_SHORT_FIELD, 5);
    f.add(A_LONG_FIELD, 20);
    f.add(A_CHAR_FIELD, 'a');
    f.add(A_FLOAT_FIELD, 20.5);
    f.add(A_DOUBLE_FIELD, 50.99);
    f.add(A_STRING_FIELD, "17");
    
    CHECK(f.get_long(A_SHORT_FIELD) == 5);
    CHECK(f.get_long(A_LONG_FIELD) == 20);
    CHECK(f.get_long(A_CHAR_FIELD) == 'a');
    CHECK(f.get_long(A_FLOAT_FIELD) == 20);
    CHECK(f.get_long(A_DOUBLE_FIELD) == 50);
    CHECK(f.get_long(A_STRING_FIELD) == 17);
}

TEST_CASE("fml32 get_char")
{
    fml f;
    f.add(A_SHORT_FIELD, 'a');
    f.add(A_LONG_FIELD, 'b');
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_FLOAT_FIELD, 'd');
    f.add(A_DOUBLE_FIELD, 'e');
    f.add(A_STRING_FIELD, "foo");
    
    CHECK(f.get_char(A_SHORT_FIELD) == 'a');
    CHECK(f.get_char(A_LONG_FIELD) == 'b');
    CHECK(f.get_char(A_CHAR_FIELD) == 'c');
    CHECK(f.get_char(A_FLOAT_FIELD) == 'd');
    CHECK(f.get_char(A_DOUBLE_FIELD) == 'e');
    CHECK(f.get_char(A_STRING_FIELD) == 'f');
}

TEST_CASE("fml32 get_float")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_FLOAT_FIELD, 3.5);
    f.add(A_DOUBLE_FIELD, 4.6);
    f.add(A_STRING_FIELD, "5.7");
    
    CHECK(f.get_float(A_SHORT_FIELD) == doctest::Approx(1));
    CHECK(f.get_float(A_LONG_FIELD) == doctest::Approx(2));
    CHECK(f.get_float(A_CHAR_FIELD) == doctest::Approx('c'));
    CHECK(f.get_float(A_FLOAT_FIELD) == doctest::Approx(3.5));
    CHECK(f.get_float(A_DOUBLE_FIELD) == doctest::Approx(4.6));
    CHECK(f.get_float(A_STRING_FIELD) == doctest::Approx(5.7));
}

TEST_CASE("fml32 get_double")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_FLOAT_FIELD, 3.5);
    f.add(A_DOUBLE_FIELD, 4.6);
    f.add(A_STRING_FIELD, "5.7");
    
    CHECK(f.get_double(A_SHORT_FIELD) == doctest::Approx(1));
    CHECK(f.get_double(A_LONG_FIELD) == doctest::Approx(2));
    CHECK(f.get_double(A_CHAR_FIELD) == doctest::Approx('c'));
    CHECK(f.get_double(A_FLOAT_FIELD) == doctest::Approx(3.5));
    CHECK(f.get_double(A_DOUBLE_FIELD) == doctest::Approx(4.6));
    CHECK(f.get_double(A_STRING_FIELD) == doctest::Approx(5.7));
}

TEST_CASE("fml32 get_string")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_FLOAT_FIELD, 3.5);
    f.add(A_DOUBLE_FIELD, 4.6);
    f.add(A_STRING_FIELD, "hello");
    f.add(A_CARRAY_FIELD, "world");
    
    CHECK(f.get_string(A_SHORT_FIELD) == "1");
    CHECK(f.get_string(A_LONG_FIELD) == "2");
    CHECK(f.get_string(A_CHAR_FIELD) == "c");
    CHECK(rtrim_zeroes(f.get_string(A_FLOAT_FIELD)) == "3.5");
    CHECK(rtrim_zeroes(f.get_string(A_DOUBLE_FIELD)) == "4.6");
    CHECK(f.get_string(A_STRING_FIELD) == "hello");
    CHECK(f.get_string(A_CARRAY_FIELD) == "world");
}

TEST_CASE("fml32 get_mbstring")
{
    fml f;
    f.add(A_SHORT_FIELD, 3);
    f.add(AN_MBSTRING_FIELD, packed_mbstring("hello", "SHIFT-JIS"));

    CHECK_THROWS(f.get_mbstring(A_SHORT_FIELD));
    CHECK_THROWS(f.get_mbstring(AN_MBSTRING_FIELD, 10));
    auto unpacked = f.get_mbstring(AN_MBSTRING_FIELD);
    CHECK(unpacked.data == "hello");
    CHECK(unpacked.encoding == "SHIFT-JIS");
}

TEST_CASE("fml32 get_fml")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_FLOAT_FIELD, 3.5);
    f.add(A_DOUBLE_FIELD, 4.6);
    f.add(A_STRING_FIELD, "hello");
    f.add(A_CARRAY_FIELD, "world");
    
    fml f1;
    f1.add(AN_FML32_FIELD, f);
    f1.add(A_STRING_FIELD, "hello");
    
    CHECK_THROWS(f1.get_fml(A_STRING_FIELD));
    CHECK_THROWS(f1.get_fml(AN_FML32_FIELD, 1));
    
    fml f2 = f1.get_fml(AN_FML32_FIELD);
    CHECK(f2 == f);  
}

TEST_CASE("fml32 get_ptr")
{
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    cstring s("hello");
    fml f1;
    f1.add(A_SHORT_FIELD, 3);
    
    f.add(A_CHAR_FIELD, 'a');
    f.add_ptr(A_PTR_FIELD, s.data());
    f.add_ptr(A_PTR_FIELD, f1.as_fbfr());
    
    CHECK_THROWS(f.get_ptr(A_CHAR_FIELD));
    CHECK((const char*)f.get_ptr(A_PTR_FIELD, 0) == string("hello"));
    CHECK(f.get_ptr(A_PTR_FIELD, 1) == f1.as_fbfr());
}

TEST_CASE("fml32 get_view")
{
    fml f;
    auto s1 = make_default<my_struct>();
    s1.d = 50.5;
    s1.f = 3.2;
    s1.l = -2;
    
    auto s2 = make_default<string_info>();
    s2.ascii_sum = 500;
    
    f.add_view(A_VIEW32_FIELD, s1);
    f.add_view(A_VIEW32_FIELD, s2);
    f.add(A_LONG_FIELD, 90);
    
    CHECK_THROWS(f.get_view<my_struct>(A_LONG_FIELD));
    CHECK_THROWS(f.get_view<my_struct>(A_VIEW32_FIELD, 1));
    CHECK_THROWS(f.get_view<string_info>(A_VIEW32_FIELD, 0));
    
    s1 = f.get_view<my_struct>(A_VIEW32_FIELD, 0);
    s2 = f.get_view<string_info>(A_VIEW32_FIELD, 1);
    
    CHECK(s1.d == doctest::Approx(50.5));
    CHECK(s1.f == doctest::Approx(3.2));
    CHECK(s1.l == -2);
    CHECK(s2.ascii_sum == 500);
}

TEST_CASE("fml32 find short")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 3);
    
    f.add(A_LONG_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_LONG_FIELD, 3);
    
    f.add(A_STRING_FIELD, "1");
    f.add(A_STRING_FIELD, "2");
    f.add(A_STRING_FIELD, "3");
    
    CHECK(f.find(A_SHORT_FIELD, short(1)) == 0);
    CHECK(f.find(A_SHORT_FIELD, short(2)) == 1);
    CHECK(f.find(A_SHORT_FIELD, short(3)) == 2);
    
    CHECK(f.find(A_LONG_FIELD, short(2)) == 1);
    
    CHECK(f.find(A_STRING_FIELD, short(2)) == 1);
    
    CHECK(f.find(A_SHORT_FIELD, short(30)) == -1);
    CHECK(f.find(A_CHAR_FIELD, short(30)) == -1);
}

TEST_CASE("fml32 find long")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 3);
    
    f.add(A_LONG_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_LONG_FIELD, 3);
    
    f.add(A_STRING_FIELD, "1");
    f.add(A_STRING_FIELD, "2");
    f.add(A_STRING_FIELD, "3");
    
    CHECK(f.find(A_SHORT_FIELD, long(1)) == 0);
    CHECK(f.find(A_SHORT_FIELD, long(2)) == 1);
    CHECK(f.find(A_SHORT_FIELD, long(3)) == 2);
    
    CHECK(f.find(A_LONG_FIELD, long(2)) == 1);
    
    CHECK(f.find(A_STRING_FIELD, long(2)) == 1);
    
    CHECK(f.find(A_SHORT_FIELD, long(30)) == -1);
    CHECK(f.find(A_CHAR_FIELD, long(30)) == -1);
}

TEST_CASE("fml32 find int")
{
    fml f;
    f.add(A_SHORT_FIELD, 1);
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 3);
    
    f.add(A_LONG_FIELD, 1);
    f.add(A_LONG_FIELD, 2);
    f.add(A_LONG_FIELD, 3);
    
    f.add(A_STRING_FIELD, "1");
    f.add(A_STRING_FIELD, "2");
    f.add(A_STRING_FIELD, "3");
    
    CHECK(f.find(A_SHORT_FIELD, int(1)) == 0);
    CHECK(f.find(A_SHORT_FIELD, int(2)) == 1);
    CHECK(f.find(A_SHORT_FIELD, int(3)) == 2);
    
    CHECK(f.find(A_LONG_FIELD, int(2)) == 1);
    
    CHECK(f.find(A_STRING_FIELD, int(2)) == 1);
    
    CHECK(f.find(A_SHORT_FIELD, int(30)) == -1);
    CHECK(f.find(A_CHAR_FIELD, int(30)) == -1);
}

TEST_CASE("fml32 find char")
{
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_CHAR_FIELD, 'c');
    
    f.add(A_LONG_FIELD, 'a');
    f.add(A_LONG_FIELD, 'b');
    f.add(A_LONG_FIELD, 'c');
    
    f.add(A_STRING_FIELD, "a");
    f.add(A_STRING_FIELD, "b");
    f.add(A_STRING_FIELD, "c");
    
    CHECK(f.find(A_CHAR_FIELD, 'a') == 0);
    CHECK(f.find(A_CHAR_FIELD, 'b') == 1);
    CHECK(f.find(A_CHAR_FIELD, 'c') == 2);
    
    CHECK(f.find(A_LONG_FIELD, 'b') == 1);
    
    CHECK(f.find(A_STRING_FIELD, 'b') == 1);
    
    CHECK(f.find(A_CHAR_FIELD, 'd') == -1);
    CHECK(f.find(A_SHORT_FIELD, 'd') == -1);
}

TEST_CASE("fml32 find float")
{
    fml f;
    f.add(A_FLOAT_FIELD, -1.5f);
    f.add(A_FLOAT_FIELD, 0.6f);
    f.add(A_FLOAT_FIELD, 1.7f);
    
    f.add(A_LONG_FIELD, -1);
    f.add(A_LONG_FIELD, 0);
    f.add(A_LONG_FIELD, 1);
    
    f.add(A_STRING_FIELD, "-1.5000000");
    f.add(A_STRING_FIELD, "0.6000000");
    f.add(A_STRING_FIELD, "1.7000000");
    
    CHECK(f.find(A_FLOAT_FIELD, -1.5f) == 0);
    CHECK(f.find(A_FLOAT_FIELD, 0.6f) == 1);
    CHECK(f.find(A_FLOAT_FIELD, 1.7f) == 2);
    CHECK(f.find(A_LONG_FIELD, 0) == 1);
    CHECK(f.find(A_STRING_FIELD, 0.6f) == 1);
    CHECK(f.find(A_FLOAT_FIELD, 2.8f) == -1);
    CHECK(f.find(A_SHORT_FIELD, 0.6f) == -1);
}

TEST_CASE("fml32 find double")
{
    fml f;
    f.add(A_DOUBLE_FIELD, -1.5);
    f.add(A_DOUBLE_FIELD, 0.6);
    f.add(A_DOUBLE_FIELD, 1.7);
    
    f.add(A_LONG_FIELD, -1);
    f.add(A_LONG_FIELD, 0);
    f.add(A_LONG_FIELD, 1);
    
    f.add(A_STRING_FIELD, "-1.5000000000000000");
    f.add(A_STRING_FIELD, "0.6000000000000000");
    f.add(A_STRING_FIELD, "1.7000000000000000");
    
    CHECK(f.find(A_DOUBLE_FIELD, -1.5) == 0);
    CHECK(f.find(A_DOUBLE_FIELD, 0.6) == 1);
    CHECK(f.find(A_DOUBLE_FIELD, 1.7) == 2);
    CHECK(f.find(A_LONG_FIELD, 0) == 1);
    CHECK(f.find(A_STRING_FIELD, 0.6) == 1);
    CHECK(f.find(A_DOUBLE_FIELD, 2.8) == -1);
    CHECK(f.find(A_SHORT_FIELD, 0.6) == -1);
}

TEST_CASE("fml32 find string")
{
    fml f;
    f.add(A_CHAR_FIELD, 'a');

    f.add(A_LONG_FIELD, -1);
    f.add(A_LONG_FIELD, 0);
    f.add(A_LONG_FIELD, 1);
    
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_STRING_FIELD, "!");
    
    f.add(A_CARRAY_FIELD, "hello");
    f.add(A_CARRAY_FIELD, "world");
    f.add(A_CARRAY_FIELD, "!");
    
    CHECK(f.find(A_CHAR_FIELD, "a") == 0);
    CHECK(f.find(A_LONG_FIELD, "1") == 2);
    CHECK(f.find(A_STRING_FIELD, "hello") == 0);
    CHECK(f.find(A_STRING_FIELD, "world") == 1);
    CHECK(f.find(A_STRING_FIELD, "!") == 2);
    CHECK(f.find(A_CARRAY_FIELD, "hello") == 0);
    CHECK(f.find(A_CARRAY_FIELD, "world") == 1);
    CHECK(f.find(A_CARRAY_FIELD, "!") == 2);
    
    CHECK(f.find(A_DOUBLE_FIELD, "") == -1);
    CHECK(f.find(A_STRING_FIELD, "") == -1);
}

TEST_CASE("fml32 find_match")
{
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(A_STRING_FIELD, "Chicago");
    f.add(A_STRING_FIELD, "Houston");
    f.add(A_STRING_FIELD, "1234");
    
    CHECK_THROWS(f.find_match(A_CHAR_FIELD, ""));
    
    
    CHECK(f.find_match(A_STRING_FIELD, "Houston|Dallas") == 1);
    CHECK(f.find_match(A_STRING_FIELD, "New York|Chicago") == 0);
    CHECK(f.find_match(A_STRING_FIELD, "H.*") == 1);
    CHECK(f.find_match(A_STRING_FIELD, ".*ago") == 0);
    CHECK(f.find_match(A_STRING_FIELD, "[0-9]+") == 2);
    CHECK(f.find_match(A_STRING_FIELD, "Z.*") == -1);
}

TEST_CASE("fml32 find mbstring")
{
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(AN_MBSTRING_FIELD, packed_mbstring("hello", "EUC-JP"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("world", "EUC-JP"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("!", "SHIFT-JIS"));
    
    CHECK_THROWS(f.find(A_CHAR_FIELD, packed_mbstring("hello", "EUC-JP")));
    CHECK(f.find(AN_MBSTRING_FIELD, packed_mbstring("hello", "EUC-JP")) == 0);
    CHECK(f.find(AN_MBSTRING_FIELD, packed_mbstring("world", "EUC-JP")) == 1);
    CHECK(f.find(AN_MBSTRING_FIELD, packed_mbstring("!", "SHIFT-JIS")) == 2);
    CHECK(f.find(AN_MBSTRING_FIELD, packed_mbstring("hello", "SHIFT-JIS")) == -1);
    CHECK(f.find(AN_MBSTRING_FIELD, packed_mbstring("!!", "SHIFT-JIS")) == -1);
}

// find fml32 appears to be broken (in Tuxedo?)
/*
TEST_CASE("fml32 find fml32")
{
    fml f1,f2,f3;
    f1.add(A_SHORT_FIELD, -25);
    f1.add(A_STRING_FIELD, "hello");
    
    f2.add(A_LONG_FIELD, 200);
    f2.add(A_CARRAY_FIELD, "world");
    
    f3.add(A_CHAR_FIELD, 'T');
    
    
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(AN_FML32_FIELD, f1);
    f.add(AN_FML32_FIELD, f2);
    f.add(AN_FML32_FIELD, f3);
    
    CHECK_THROWS(f.find(A_CHAR_FIELD, f1));
    fml f4,f5,f6;
    
    f4 = f1;
    f5 = f2;
    f6 = f3;
    CHECK(f.find(AN_FML32_FIELD, f4) == 0);
    CHECK(f.find(AN_FML32_FIELD, f5) == 1);
    CHECK(f.find(AN_FML32_FIELD, f6) == 2);
    f1.clear();
    CHECK(f.find(AN_FML32_FIELD, f1) == -1);

}*/

TEST_CASE("fml32 find_ptr")
{
    cstring s1("hello");
    cstring s2("world");
    fml a;
    a.add(A_SHORT_FIELD, 1);
    
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    f.add(A_CHAR_FIELD, 'a');
    
    f.add_ptr(A_PTR_FIELD, s1.data());
    f.add_ptr(A_PTR_FIELD, s2.data());
    f.add_ptr(A_PTR_FIELD, a.as_fbfr());
    
    CHECK_THROWS(f.find_ptr(A_CHAR_FIELD, s1.data()));
    CHECK(f.find_ptr(A_PTR_FIELD, s1.data()) == 0);
    CHECK(f.find_ptr(A_PTR_FIELD, s2.data()) == 1);
    CHECK(f.find_ptr(A_PTR_FIELD, a.as_fbfr()) == 2);
}

// find_view appears to be broken (in Tuxedo?)
/*
TEST_CASE("fml32 find_view")
{
    auto ms1 = make_default<my_struct>();
    ms1.d = 20.5;
    ms1.f = 3.2;
    ms1.l = -1;
    auto ms2 = make_default<my_struct>();
    
    auto si1 = make_default<string_info>();
    si1.ascii_sum = 500;
    si1.byte_count = 50;
    si1.most_frequent_char = 'n';
    set(si1.original_string, "hello");
    auto si2 = make_default<string_info>();
    
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add_view(A_VIEW32_FIELD, ms1);
    f.add_view(A_VIEW32_FIELD, ms2);
    f.add_view(A_VIEW32_FIELD, si1);
    f.add_view(A_VIEW32_FIELD, si2);
    
    CHECK(f.find_view(A_VIEW32_FIELD, ms1) == 0);
    CHECK(f.find_view(A_VIEW32_FIELD, ms2) == 1);
    CHECK(f.find_view(A_VIEW32_FIELD, si1) == 2);
    CHECK(f.find_view(A_VIEW32_FIELD, si2) == 3);
    
    CHECK_THROWS(f.find_view(A_CHAR_FIELD, ms1));
    
    ms2.d = .1;
    
    CHECK(f.find_view(A_VIEW32_FIELD, ms2) == -1);    
}*/

TEST_CASE("fml32 get_last_short")
{ 
    fml f;
    f.add(A_SHORT_FIELD, -1);
    f.add(A_SHORT_FIELD, 100);
    f.add(A_SHORT_FIELD, 25);
    f.add(A_SHORT_FIELD, 50);
    
    FLDOCC32 oc = 0;
    short val = 0;
    tie(oc,val) = f.get_last_short(A_SHORT_FIELD);
    CHECK(oc == 3);
    CHECK(val == 50);
    
    CHECK_THROWS(f.get_last_short(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_long")
{ 
    fml f;
    f.add(A_LONG_FIELD, -1);
    f.add(A_LONG_FIELD, 100);
    f.add(A_LONG_FIELD, 25);
    f.add(A_LONG_FIELD, 50);
    
    FLDOCC32 oc = 0;
    long val = 0;
    tie(oc,val) = f.get_last_long(A_LONG_FIELD);
    CHECK(oc == 3);
    CHECK(val == 50);
    
    CHECK_THROWS(f.get_last_long(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_char")
{ 
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_CHAR_FIELD, 'c');
    f.add(A_CHAR_FIELD, 'C');
    
    FLDOCC32 oc = 0;
    char val = ' ';
    tie(oc,val) = f.get_last_char(A_CHAR_FIELD);
    CHECK(oc == 3);
    CHECK(val == 'C');
    
    CHECK_THROWS(f.get_last_char(A_LONG_FIELD));
}

TEST_CASE("fml32 get_last_float")
{ 
    fml f;
    f.add(A_FLOAT_FIELD, -10.1);
    f.add(A_FLOAT_FIELD, -1.2);
    f.add(A_FLOAT_FIELD, 0.3);
    f.add(A_FLOAT_FIELD, 1.4);
    
    FLDOCC32 oc = 0;
    float val = 0;
    tie(oc,val) = f.get_last_float(A_FLOAT_FIELD);
    CHECK(oc == 3);
    CHECK(val == doctest::Approx(1.4));
    
    CHECK_THROWS(f.get_last_float(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_double")
{ 
    fml f;
    f.add(A_DOUBLE_FIELD, -10.1);
    f.add(A_DOUBLE_FIELD, -1.2);
    f.add(A_DOUBLE_FIELD, 0.3);
    f.add(A_DOUBLE_FIELD, 1.4);
    
    FLDOCC32 oc = 0;
    double val = 0;
    tie(oc,val) = f.get_last_double(A_DOUBLE_FIELD);
    CHECK(oc == 3);
    CHECK(val == doctest::Approx(1.4));
    
    CHECK_THROWS(f.get_last_double(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_string")
{ 
    fml f;
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_STRING_FIELD, "foo");
    f.add(A_STRING_FIELD, "!");
    
    FLDOCC32 oc = 0;
    string val;
    tie(oc,val) = f.get_last_string(A_STRING_FIELD);
    CHECK(oc == 3);
    CHECK(val == "!");
    
    CHECK_THROWS(f.get_last_string(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_mbstring")
{ 
    fml f;
    f.add(AN_MBSTRING_FIELD, packed_mbstring("hello", "SHIFT-JIS"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("world", "EUC-JP"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("foo", "SHIFT-JIS"));
    f.add(AN_MBSTRING_FIELD, packed_mbstring("!", "EUC-JP"));
    
    FLDOCC32 oc = 0;
    unpacked_mbstring val;
    tie(oc,val) = f.get_last_mbstring(AN_MBSTRING_FIELD);
    CHECK(oc == 3);
    CHECK(val.data == "!");
    CHECK(val.encoding == "EUC-JP");
    
    CHECK_THROWS(f.get_last_mbstring(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last fml")
{
    fml f1,f2,f3;
    f1.add(A_SHORT_FIELD, -25);
    f1.add(A_STRING_FIELD, "hello");
    
    f2.add(A_LONG_FIELD, 200);
    f2.add(A_CARRAY_FIELD, "world");
    
    f3.add(A_CHAR_FIELD, 'T');
    
    
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add(AN_FML32_FIELD, f1);
    f.add(AN_FML32_FIELD, f2);
    f.add(AN_FML32_FIELD, f3);
    
    FLDOCC32 oc = 0;
    fml val;
    tie(oc,val) = f.get_last_fml(AN_FML32_FIELD);
    CHECK(oc == 2);
    CHECK(val == f3);
    
    CHECK_THROWS(f.get_last_fml(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_ptr")
{
    cstring s1("hello");
    cstring s2("world");
    fml a;
    a.add(A_SHORT_FIELD, 1);
    
    fml f;
    ptr_field_guard guard(f, {A_PTR_FIELD});
    f.add(A_CHAR_FIELD, 'a');
    
    f.add_ptr(A_PTR_FIELD, s1.data());
    f.add_ptr(A_PTR_FIELD, s2.data());
    f.add_ptr(A_PTR_FIELD, a.as_fbfr());
    
    FLDOCC32 oc = 0;
    void* val;
    tie(oc,val) = f.get_last_ptr(A_PTR_FIELD);
    CHECK(oc == 2);
    CHECK(val == a.as_fbfr());
    
    CHECK_THROWS(f.get_last_ptr(A_CHAR_FIELD));
}

TEST_CASE("fml32 get_last_view")
{
    auto si1 = make_default<string_info>();
    auto si2 = make_default<string_info>();
    si2.ascii_sum = 500;
    si2.byte_count = 50;
    si2.most_frequent_char = 'n';
    set(si2.original_string, "hello");
    
    fml f;
    f.add(A_CHAR_FIELD, 'a');
    f.add_view(A_VIEW32_FIELD, si1);
    f.add_view(A_VIEW32_FIELD, si2);
    
    FLDOCC32 oc = 0;
    string_info val;
    tie(oc,val) = f.get_last_view<string_info>(A_VIEW32_FIELD);
    CHECK(oc == 1);
    CHECK(memcmp(&si2, &val, sizeof(val)) == 0);
    
    CHECK_THROWS(f.get_last_view<string_info>(A_CHAR_FIELD));
}

TEST_CASE("fml32 next_field")
{
    fml f;
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 5);
    f.add(A_LONG_FIELD, 100);
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_FLOAT_FIELD, 13.7);
    f.add(A_DOUBLE_FIELD, 4096.32);
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_CARRAY_FIELD, "hello dolly");
    
    fml::field_info field;
    vector<fml::field_info> fields;
    
    while(f.next_field(field))
    {
        fields.push_back(field);
    }
    
    CHECK(fields[0].id == A_SHORT_FIELD);
    CHECK(fields[0].oc == 0);
    
    CHECK(fields[1].id == A_SHORT_FIELD);
    CHECK(fields[1].oc == 1);
    
    CHECK(fields[2].id == A_LONG_FIELD);
    CHECK(fields[2].oc == 0);
    
    CHECK(fields[3].id == A_CHAR_FIELD);
    CHECK(fields[3].oc == 0);
    
    CHECK(fields[4].id == A_FLOAT_FIELD);
    CHECK(fields[4].oc == 0);
    
    CHECK(fields[5].id == A_DOUBLE_FIELD);
    CHECK(fields[5].oc == 0);
    
    CHECK(fields[6].id == A_STRING_FIELD);
    CHECK(fields[6].oc == 0);
    
    CHECK(fields[7].id == A_STRING_FIELD);
    CHECK(fields[7].oc == 1);
    
    CHECK(fields[8].id == A_CARRAY_FIELD);
    CHECK(fields[8].oc == 0);
}

TEST_CASE("fml32 boolean_expression")
{
    fml f;
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 5);
    f.add(A_LONG_FIELD, 100);
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_FLOAT_FIELD, 13.7);
    f.add(A_DOUBLE_FIELD, 4096.32);
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_CARRAY_FIELD, "hello dolly");
    
    // simple
    fml::boolean_expression first_short_is_negative("A_SHORT_FIELD < 0");
    fml::boolean_expression second_short_is_five("A_SHORT_FIELD[1] == 5");
    fml::boolean_expression any_short_is_greater_than_three("A_SHORT_FIELD[?] > 3");
    
    CHECK(first_short_is_negative(f) == false);
    CHECK(second_short_is_five(f) == true);
    CHECK(any_short_is_greater_than_three(f) == true);
    
    // compound
    fml::boolean_expression first_short_is_three_or_second_short_is_five("A_SHORT_FIELD[0] == 3 || A_SHORT_FIELD[1] == 5");
    CHECK(first_short_is_three_or_second_short_is_five(f) == true);
    
    // compound across multiple fields
    fml::boolean_expression a_short_or_long_is_large("A_SHORT_FIELD[?] > 75 || A_LONG_FIELD[?] > 75");
    fml::boolean_expression float_and_double_are_large("A_FLOAT_FIELD > 2000 && A_DOUBLE_FIELD > 2000");
    CHECK(a_short_or_long_is_large(f) == true);
    CHECK(float_and_double_are_large(f) == false);
    
    // parenthesis
    fml::boolean_expression complex_expression("(A_FLOAT_FIELD < 0 || A_FLOAT_FIELD > 13) && A_CARRAY_FIELD == 'hello dolly'");
    CHECK(complex_expression(f) == true);
    
    // arithmetic
    fml::boolean_expression short_sum_is_seven("A_SHORT_FIELD[0] + A_SHORT_FIELD[1] == 7");
    CHECK(short_sum_is_seven(f) == true);
    
    fml::boolean_expression short_product_is_twenty("A_SHORT_FIELD[0] * A_SHORT_FIELD[1] == 20");
    CHECK(short_product_is_twenty(f) == false);
    
    
    // regex
    fml::boolean_expression a_string_ends_in_rld("A_STRING_FIELD[?] %% '.*rld'");
    CHECK(a_string_ends_in_rld(f) == true);
    
    fml::boolean_expression first_string_is_numeric("A_STRING_FIELD %% '[0-9]+'");
    CHECK(first_string_is_numeric(f) == false);
    
    fml::boolean_expression second_string_is_world_or_planet("A_STRING_FIELD[1] %% 'world|planet'");
    CHECK(second_string_is_world_or_planet(f) == true);
    
    // copy-construct
    fml::boolean_expression ex(short_sum_is_seven);
    CHECK(ex(f) == true);

    // copy-assign
    ex = first_string_is_numeric;
    CHECK(ex(f) == false);
    
    // assign from string
    ex = "A_CHAR_FIELD == 'b'";
    CHECK(ex(f) == true);
    
    // move
    ex = move(first_short_is_negative);
    CHECK_THROWS(first_short_is_negative(f));
    CHECK(ex(f) == false);
    
    // print
    CHECK_NOTHROW(ex.print(stdout));
    
    // evaluate
    CHECK(ex.evaluate(f) == false);
}

TEST_CASE("fml32 arithmetic_expression")
{
    fml f;
    f.add(A_SHORT_FIELD, 2);
    f.add(A_SHORT_FIELD, 5);
    f.add(A_LONG_FIELD, 100);
    f.add(A_CHAR_FIELD, 'b');
    f.add(A_FLOAT_FIELD, 13.7);
    f.add(A_DOUBLE_FIELD, 4096.32);
    f.add(A_STRING_FIELD, "hello");
    f.add(A_STRING_FIELD, "world");
    f.add(A_CARRAY_FIELD, "hello dolly");
    
    // simple
    fml::arithmetic_expression sum_first_two_shorts("A_SHORT_FIELD[0] + A_SHORT_FIELD[1]");
    CHECK((int)sum_first_two_shorts(f) == 7);
    fml::arithmetic_expression divide_long_by_short("A_LONG_FIELD / A_SHORT_FIELD");
    CHECK((int)divide_long_by_short(f) == 50);
    
    fml::arithmetic_expression multiply_float_and_double("A_FLOAT_FIELD * A_DOUBLE_FIELD");
    CHECK(multiply_float_and_double(f) == doctest::Approx(56119.584));
    
    
    // complex
    fml::arithmetic_expression complex("A_SHORT_FIELD[0] + A_SHORT_FIELD[1] - A_LONG_FIELD");
    CHECK((int)complex(f) == -93);
    

    // parenthesis
    fml::arithmetic_expression paren("A_LONG_FIELD / (A_SHORT_FIELD[1] + A_SHORT_FIELD[1])");
    CHECK((int)paren(f) == 10);
    
    // copy-construct
    fml::arithmetic_expression ex(sum_first_two_shorts);
    CHECK((int)ex(f) == 7);

    // copy-assign
    ex = divide_long_by_short;
    CHECK((int)ex(f) == 50);
    
    // assign from string
    ex = "A_LONG_FIELD * 2";
    CHECK((int)ex(f) == 200);
    
    // move
    ex = move(paren);
    CHECK_THROWS(paren(f));
    CHECK((int)ex(f) == 10);
    
    // print
    CHECK_NOTHROW(ex.print(stdout));
    
    // evaluate
    CHECK((int)ex.evaluate(f) == 10);
}


TEST_CASE("fml32 comparisons")
{
    fml a,b;
    CHECK(a == b);
    a.add(A_SHORT_FIELD, 10);
    CHECK(a != b);
    CHECK(a > b);
    CHECK(a >= b);
    CHECK(b < a);
    CHECK(b <= a);
    b.add(A_SHORT_FIELD, 10);
    CHECK(a == b);
    CHECK(a >= b);
    CHECK(a <= b);
    b.set(A_SHORT_FIELD, 20);
    CHECK(b > a);
    b.set(A_SHORT_FIELD, 20);
    a.add(A_STRING_FIELD, "apple");
    CHECK(a != b);
    b.add(A_STRING_FIELD, "banana");
    CHECK(a < b);
}

TEST_CASE("fml32 growth")
{
    fml f;
    f.reserve(0);
    long original_buffer_size = f.buffer().size();
    
    SUBCASE("add")
    {
        while(f.buffer().size() <= original_buffer_size)
        {
            f.add(A_DOUBLE_FIELD, 50.2);
        }
        CHECK(f.buffer().size() > original_buffer_size);
    }
    
    // append
    SUBCASE("append")
    {
        while(f.buffer().size() <= original_buffer_size)
        {
            f.append(A_STRING_FIELD, "hello world");
        }
        f.index();
        CHECK(f.buffer().size() > original_buffer_size);
    }
    
    // set
    SUBCASE("set")
    {
        for(int i=0;
            f.buffer().size() <= original_buffer_size;
            ++i)
        {
            f.set(A_LONG_FIELD, 2000, i);
        }
        CHECK(f.buffer().size() > original_buffer_size);
    }

    // get
    SUBCASE("get_fml")
    {
        while(f.buffer().size() <= original_buffer_size)
        {
            f.add(A_DOUBLE_FIELD, 50.2);
        }
        CHECK(f.buffer().size() > original_buffer_size);
        
        fml parent;
        parent.add(AN_FML32_FIELD, f);
        CHECK(parent.buffer().size() > original_buffer_size);
        fml child = parent.get_fml(AN_FML32_FIELD);
        CHECK(child.buffer().size() > original_buffer_size);
        CHECK(child == f);
    }
}

TEST_SUITE_END();
