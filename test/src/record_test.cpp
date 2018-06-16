#if TUXEDO_VERSION >= 1222
#include "doctest.h"
#include "tux/record.hpp"
#include "tux/util.hpp"

using namespace std;
using namespace tux;

TEST_SUITE("record");

TEST_CASE("record default construct")
{
    record r;
    CHECK(r.buffer().data() == nullptr);
    CHECK(r.as_record() == nullptr);
    CHECK((bool)r == false);
    CHECK(r.type() == "");
    CHECK_THROWS(r.get_int("field_name"));
}

TEST_CASE("record copy construct")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b(a);
    CHECK(a.type() == "CUSTOMER");
    CHECK(a.get_string("NAME") == "John Doe");
    CHECK(a.get_double("BALANCE") == 55.20);
    CHECK(a.get_string("ADDRESS") == "123 Meadow Lane");
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");   
}

TEST_CASE("record copy assign")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b("TRANSACTION");
    b.set("DESC", "Store 123");
    b.set("TRAN_DATE","20170101");
    b.set("AMOUNT", 12.99);
    b = a;
    CHECK(a.type() == "CUSTOMER");
    CHECK(a.get_string("NAME") == "John Doe");
    CHECK(a.get_double("BALANCE") == 55.20);
    CHECK(a.get_string("ADDRESS") == "123 Meadow Lane");
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");
}

TEST_CASE("record move construct")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b(move(a));
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");   
}

TEST_CASE("record move assign")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b("TRANSACTION");
    b.set("DESC", "Store 123");
    b.set("TRAN_DATE","20170101");
    b.set("AMOUNT", 12.99);
    b = move(a);
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");   
}

TEST_CASE("record move construct from buffer")
{
    // normal
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b(a.move_buffer());
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");   
    
    // null buffer
    record c{buffer()};
    CHECK(c.buffer().data() == nullptr);
    
    // from wrong buffer type
    CHECK_THROWS(record(buffer("STRING")));
}

TEST_CASE("record move assign from buffer")
{
    // normal
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    record b("TRANSACTION");
    b.set("DESC", "Store 123");
    b.set("TRAN_DATE","20170101");
    b.set("AMOUNT", 12.99);
    b = move(a);
    CHECK(a.buffer().data() == nullptr);
    CHECK(b.type() == "CUSTOMER");
    CHECK(b.get_string("NAME") == "John Doe");
    CHECK(b.get_double("BALANCE") == 55.20);
    CHECK(b.get_string("ADDRESS") == "123 Meadow Lane");
    
    // null buffer
    b = buffer();
    CHECK(b.buffer().data() == nullptr);
    
    // from wrong buffer type
    CHECK_THROWS(b = buffer("FML32"));
}

TEST_CASE("record bool conversion")
{
    record r;
    CHECK((bool)r == false);
    r.init("CUSTOMER");
    CHECK((bool)r == true);
}

TEST_CASE("record type check")
{
    record r;
    CHECK(r.type() == "");
    r.init("TRANSACTION");
    CHECK(r.type() == "TRANSACTION");
}

TEST_CASE("record manual init")
{
    record r;
    r.init("CUSTOMER");
    CHECK((bool)r == true);
    CHECK(r.as_record() != nullptr);
    CHECK(r.buffer().data() != nullptr);
    CHECK(r.buffer().type() == "RECORD");
    CHECK(r.buffer().subtype() == "CUSTOMER");
    CHECK(r.type() == "CUSTOMER");
    CHECK(r.get_string("NAME").size() == 0);
    CHECK(r.get_string("NAME") == "");
    CHECK(r.get_decimal("BALANCE") == decimal_number());
    CHECK(r.get_string("ADDRESS").size() == 0);
    CHECK(r.get_string("ADDRESS") == "");
}

TEST_CASE("record get_data/set_data")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    a.set("BALANCE", 55.20);
    a.set("ADDRESS", "123 Meadow Lane");
    string data = a.get_data();
    const RECORD* r = a.as_record();
    CHECK(data == string(r->rdata, r->rsize));
    a.set_data(data.data(), data.size());
    CHECK(a.type() == "CUSTOMER");
    CHECK(a.get_string("NAME") == "John Doe");
    CHECK(a.get_double("BALANCE") == 55.20);
    CHECK(a.get_string("ADDRESS") == "123 Meadow Lane");
}

TEST_CASE("record get/set individual fields")
{
    record a("CUSTOMER");
    a.set("NAME", "John Doe");
    CHECK(a.get_string("NAME") == "John Doe");
    CHECK_THROWS(a.get_string("bad_name"));
    CHECK_THROWS(a.get_double("NAME"));
    
    a.set("BALANCE", double(55.20));
    CHECK(a.get_double("BALANCE") == doctest::Approx(55.20));
    CHECK(a.get_float("BALANCE") == doctest::Approx(55.20));
    CHECK(a.get_int("BALANCE") == 55);
    CHECK(a.get_decimal("BALANCE") == decimal_number("55.20"));
    CHECK(a.get_string("BALANCE") == "55.20000000000000");
    
    a.set("BALANCE", decimal_number("1500.98"));
    CHECK(a.get_double("BALANCE") == doctest::Approx(1500.98));
    CHECK(a.get_float("BALANCE") == doctest::Approx(1500.98));
    CHECK(a.get_long_long("BALANCE") == 1500);
    CHECK(a.get_decimal("BALANCE") == decimal_number("1500.98"));
    CHECK(a.get_string("BALANCE") == "1500.980000000000");
}

TEST_CASE("record type change")
{
    record a("CUSTOMER");
    CHECK(a.type() == "CUSTOMER");
    a.init("TRANSACTION");
    CHECK(a.type() == "TRANSACTION");
}

#endif
