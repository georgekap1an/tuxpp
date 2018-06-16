#include <iostream>
#include <sstream>
#include "doctest.h"
#include "tux/decimal_number.hpp"
#include "tux/util.hpp"


using namespace std;
using namespace tux;

TEST_SUITE("decimal_number");

TEST_CASE("decimal_number default construct")
{
    decimal_number x;
    CHECK(x.to_double() == doctest::Approx(0.00));    
}

TEST_CASE("decimal_number string constructor")
{
    decimal_number x("878.23");
    CHECK(x.to_double() == doctest::Approx(878.23));
}

TEST_CASE("decimal_number int constructor")
{
    int i = 4082;
    decimal_number x(i);
    CHECK(x.to_double() == doctest::Approx(i));
}

TEST_CASE("decimal_number long constructor")
{
    long l = -20801;
    decimal_number x(l);
    CHECK(x.to_double() == doctest::Approx(l));
}

TEST_CASE("decimal_number double constructor")
{
    double d = 11762.52;
    decimal_number x(d);
    CHECK(x.to_double() == doctest::Approx(d));
}

TEST_CASE("decimal_number float constructor")
{
    float f = 11762.52;
    decimal_number x(f);
    CHECK(x.to_double() == doctest::Approx(f));
}

TEST_CASE("decimal_number copy constructor")
{
    decimal_number x("-115123.11");
    decimal_number y(x);
    CHECK(x.to_double() == doctest::Approx(-115123.11));
    CHECK(y.to_double() == doctest::Approx(-115123.11));
}

TEST_CASE("decimal_number dec_t constructor")
{
    dec_t raw;
    deccvdbl(8.2, &raw);
    decimal_number x(raw);
    CHECK(x.to_double() == doctest::Approx(8.2));
}

TEST_CASE("decimal_number string assignment")
{
    decimal_number x;
    x = "800.1234";
    CHECK(x.to_double() == doctest::Approx(800.1234));
}

TEST_CASE("decimal_number int assignment")
{
    int i = 3000;
    decimal_number x;
    x = i;
    CHECK(x.to_double() == doctest::Approx(i));
}

TEST_CASE("decimal_number long assignment")
{
    long l = 3000;
    decimal_number x;
    x = l;
    CHECK(x.to_double() == doctest::Approx(l));
}

TEST_CASE("decimal_number double assignment")
{
    double d = -2.5;
    decimal_number x;
    x = d;
    CHECK(x.to_double() == doctest::Approx(d));
}

TEST_CASE("decimal_number float assignment")
{
    float f = -2.5;
    decimal_number x;
    x = f;
    CHECK(x.to_double() == doctest::Approx(f));
}

TEST_CASE("decimal_number copy assignment")
{
    double d = 750.10;
    decimal_number x(d);
    decimal_number y;
    y = x;
    CHECK(x.to_double() == doctest::Approx(d));
    CHECK(y.to_double() == doctest::Approx(d));
}

TEST_CASE("decimal_number dec_t assignment")
{
    dec_t raw;
    deccvdbl(8.2, &raw);
    decimal_number x;
    x = raw;
    CHECK(x.to_double() == doctest::Approx(8.2));
}

TEST_CASE("decimal_number to_string")
{
    decimal_number x(-00050.0100);
    CHECK(x.to_string() == "-50.01");
    CHECK(x.to_string(4) == "-50.0100");
    x = "99999999999999999999.99";
    // rounding
    CHECK(x.to_string(1,50) == "100000000000000000000.0");
}

TEST_CASE("decimal_number to_int")
{
    decimal_number x(455.99);
    CHECK(x.to_int() == 455);
}

TEST_CASE("decimal_number to_long")
{
    decimal_number x(-4055.99);
    CHECK(x.to_long() == -4055);
}

TEST_CASE("decimal_number to_double")
{
    decimal_number x("60.99");
    CHECK(x.to_double() == doctest::Approx(60.99));
}

TEST_CASE("decimal_number to_float")
{
    decimal_number x(7);
    CHECK(x.to_float() == doctest::Approx(7));
}

TEST_CASE("decimal_number comparisons")
{
    decimal_number x("150.99");
    decimal_number y("150.99");
    decimal_number z("151.00");
    
    CHECK(compare(x,y) == 0);
    CHECK(compare(y,x) == 0);
    CHECK(compare(x,z) < 0);
    CHECK(compare(z,x) > 0);
    CHECK(x == y);
    CHECK(x >= y);
    CHECK(x < z);
    CHECK(x != z);
    CHECK(z > x);
}

TEST_CASE("decimal_number addition")
{
    decimal_number a,b,c;
    
    a = "150.99";
    b = "0.01";
    c = "151.00";
    CHECK(a + b == c);
    
    a = "150.99";
    b = "0.00";
    c = "150.99";
    CHECK(a + b == c);
    
    a = "150.99";
    b = "10.49";
    c = "161.48";
    CHECK(a + b == c);
    
    a = "-150.99";
    b = "-10.49";
    c = "-161.48";
    CHECK(a + b == c);
    
    a = "-150.99";
    b = "10.49";
    c = "-140.50";
    CHECK(a + b == c); 
}
    
TEST_CASE("decimal_number subtraction")
{
    decimal_number a,b,c;
    
    a = "150.99";
    b = "0.01";
    c = "150.98";
    CHECK(a - b == c);
    
    a = "150.99";
    b = "0.00";
    c = "150.99";
    CHECK(a - b == c);
    
    a = "150.99";
    b = "10.49";
    c = "140.50";
    CHECK(a - b == c);
    
    a = "-150.99";
    b = "-10.49";
    c = "-140.50";
    CHECK(a - b == c);
    
    a = "-150.99";
    b = "10.49";
    c = "-161.48";
    CHECK(a - b == c); 
}

TEST_CASE("decimal_number multiplication")
{
    try
    {
    decimal_number a,b,c;
    
    a = "150.99";
    b = "0.01";
    c = "1.5099";
    CHECK(a * b == c);
    
    a = "150.99";
    b = "0.00";
    c = "0.00";
    CHECK(a * b == c);
    
    a = "150.99";
    b = "10.49";
    c = "1583.8851";
    CHECK(a * b == c);
    
    a = "-150.99";
    b = "-10.49";
    c = "1583.8851";
    CHECK(a * b == c);
    
    a = "-150.99";
    b = "10.49";
    c = "-1583.8851";
    CHECK(a * b == c);
    
    }
    catch(exception const& e)
    {
        cerr << e.what() << endl;
    }
}

TEST_CASE("decimal_number division")
{
    decimal_number a,b,c;
    
    a = "150.99";
    b = "0.01";
    c = "15099";
    CHECK(a / b == c);
    
    a = "150.99";
    b = "0.00";
    CHECK_THROWS(a / b);
    
    a = "150.99";
    b = "10.49";
    c = "14.393708293612964728312678741659";
    CHECK(a / b == c);
    
    a = "-150.99";
    b = "-10.49";
    c = "14.393708293612964728312678741659";
    CHECK(a / b == c);
    
    a = "-150.99";
    b = "10.49";
    c = "-14.393708293612964728312678741659";
    CHECK(a / b == c); 
}

TEST_CASE("decimal_number stream insertion")
{
    decimal_number x("4082.52");
    ostringstream s;
    s << x;
    CHECK(s.str() == "4082.52");
}

TEST_SUITE_END();
