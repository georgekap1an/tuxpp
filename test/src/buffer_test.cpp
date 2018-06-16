#include "doctest.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("buffer");

TEST_CASE("buffer default construct")
{
    buffer b;
    CHECK(b.data() == nullptr);
    CHECK((bool)b == false);
    CHECK(b.size() == 0);
    CHECK(b.data_size() == 0);
    CHECK(b.type() == "");
    CHECK(b.subtype() == "");
}

TEST_CASE("buffer manually alloc/realloc/free")
{
    buffer b;
    b.alloc("STRING", "MYSUBTYPE", 512);
    CHECK(b.data() != nullptr);
    CHECK((bool)b == true);
    CHECK(b.size() >= 512);
    CHECK(b.data_size() == 0);
    CHECK(b.type() == "STRING");
    CHECK(b.subtype() == "MYSUBTYPE");
    
    long original_size = b.size();
    b.realloc(original_size * 2);
    CHECK(b.size() >= original_size * 2);

    b.free();
    CHECK(b.data() == nullptr);
    CHECK((bool)b == false);
    CHECK(b.size() == 0);
    CHECK(b.data_size() == 0);
    CHECK(b.type() == "");
    CHECK(b.subtype() == "");  
}


TEST_CASE("buffer set data_size")
{
    buffer b;
    b.alloc("STRING", nullptr, 512);
    CHECK(b.size() >= 512);
    CHECK(b.data_size() == 0);

    b.data_size(24);
    CHECK(b.data_size() == 24);
}

TEST_CASE("buffer manually acquire/release")
{
    buffer b;
    CHECK(b.data() == nullptr);
    
    char* raw_buf_ptr = tpalloc(const_cast<char*>("STRING"), nullptr, 0);
    if(!raw_buf_ptr)
    {
        throw last_error("tpalloc");
    }
    
    // b now owns the buffer
    b.acquire(raw_buf_ptr);
    CHECK(b.data() == raw_buf_ptr);
    CHECK(b.type() == "STRING");
    CHECK(b.subtype() == "");
    CHECK(b.data_size() == 0);
    
    char* raw_buf_ptr_2 = b.release();
    CHECK(b.data() == nullptr);
    CHECK(b.size() == 0);
    CHECK(b.type() == "");
    CHECK(b.subtype() == "");
    
    tpfree(raw_buf_ptr_2);
}

TEST_CASE("buffer convenience constructors")
{
    // allocate buffer
    buffer a("XML", nullptr, 32);
    CHECK(a.data() != nullptr);
    CHECK(a.size() >= 32);
    CHECK(a.type() == "XML");
    CHECK(a.subtype() == "");
    CHECK(a.data_size() == 0);
    
    
    char* raw_buf_ptr = tpalloc(const_cast<char*>("FML32"), nullptr, 0);
    if(!raw_buf_ptr)
    {
        throw last_error("tpalloc");
    }
    // acquire previously allocated buffer
    buffer b(raw_buf_ptr);
    CHECK(b.data() == raw_buf_ptr);
    CHECK(b.size() >= 0);
    CHECK(b.type() == "FML32");
    CHECK(b.subtype() == "");
}

TEST_CASE("buffer default allocation")
{
    buffer a;
    a.alloc_default();
    CHECK(a.type() == buffer::default_type);
    CHECK(a.subtype() == cstr_to_string(buffer::default_subtype));
    CHECK(a.size() >= buffer::default_size);
    
    buffer::default_type = "STRING";
    buffer::default_subtype = nullptr;
    buffer::default_size = 2048;
    
    a.alloc_default();
    CHECK(a.type() == "STRING");
    CHECK(a.subtype() == "");
    CHECK(a.size() >= 2048);
    
    // reset
    buffer::default_type = "FML32";
    buffer::default_subtype = nullptr;
    buffer::default_size = 0;  
}

TEST_CASE("buffer move operations")
{
    buffer a;
    CHECK(a.data() == nullptr);
    
    buffer b("CARRAY", nullptr, 64);
    CHECK(b.data() != nullptr);
    
    char* data_ptr = b.data();
    
    a = move(b);
    
    CHECK(a.data() != nullptr);
    CHECK(a.data() == data_ptr);
    CHECK(a.type() == "CARRAY");
    
    CHECK(b.data() == nullptr);
    CHECK(b.type() == "");
    
    buffer c{move(a)};
    
    CHECK(c.data() != nullptr);
    CHECK(c.data() == data_ptr);
    CHECK(c.type() == "CARRAY");
    
    CHECK(b.data() == nullptr);
    CHECK(b.type() == "");
    CHECK(a.data() == nullptr);
    CHECK(a.type() == "");
}

TEST_CASE("buffer binary export/import")
{
    buffer a("STRING", nullptr, 32);
    strcpy(a.data(), "hello world");
    
    string exported = export_buffer(a, export_mode::binary);
    CHECK(exported.size() >= 0);

    
    buffer b = import_buffer(exported, export_mode::binary);
    CHECK(b.type() == "STRING");
    CHECK(b.size() >= 11);
    CHECK(string(b.data()) == "hello world");
    
    // use pre-allocated string and buffer
    strcpy(b.data(), "foo");
    string pre_allocated_string = "other nonsense";
    exported = export_buffer(b, export_mode::binary, move(pre_allocated_string));
    CHECK(exported.size() > 0);
    CHECK(pre_allocated_string.size() == 0);
    
    buffer c = import_buffer(exported, export_mode::binary, move(b));
    CHECK(b.data() == nullptr);
    CHECK(c.data() != nullptr);
    CHECK(c.size() >= 3);
    CHECK(string(c.data()) == "foo");
    
}

TEST_CASE("buffer base64 export/import")
{
    buffer a("STRING", nullptr, 32);
    strcpy(a.data(), "hello world");
    
    string exported = export_buffer(a, export_mode::base64);
    CHECK(exported.size() >= 0);

    
    buffer b = import_buffer(exported, export_mode::base64);
    CHECK(b.type() == "STRING");
    CHECK(b.size() >= 11);
    CHECK(string(b.data()) == "hello world");
    
    // use pre-allocated string and buffer
    strcpy(b.data(), "foo");
    string pre_allocated_string = "other nonsense";
    exported = export_buffer(b, export_mode::base64, move(pre_allocated_string));
    CHECK(exported.size() > 0);
    CHECK(pre_allocated_string.size() == 0);
    
    buffer c = import_buffer(exported, export_mode::base64, move(b));
    CHECK(b.data() == nullptr);
    CHECK(c.data() != nullptr);
    CHECK(c.size() >= 3);
    CHECK(string(c.data()) == "foo");
}

TEST_CASE("buffer export/import null buffer")
{
    buffer a;
    
    string exported = export_buffer(a);
    CHECK(exported.size() == 0);

    buffer b = import_buffer(exported);
    CHECK(b.type() == "");
    CHECK(b.size() == 0);
    CHECK(b.data() == nullptr);
    
    // use pre-allocated string and buffer
    // since there is nothing to do,
    // resources aren't pilfered
    string pre_allocated_string = "other nonsense";
    exported = export_buffer(b, export_mode::base64, move(pre_allocated_string));
    CHECK(exported.size() == 0);
    CHECK(pre_allocated_string == "other nonsense");
    
    b.alloc("STRING");
    buffer c = import_buffer(exported, export_mode::base64, move(b));
    CHECK(b.data() != nullptr);
    CHECK(b.type() == "STRING");
    CHECK(b.size() > 0);
    CHECK(c.data() == nullptr);
    CHECK(c.size() == 0);
}

TEST_SUITE_END();


