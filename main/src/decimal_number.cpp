#include <stdexcept>
#include <iostream>
#include "tux/decimal_number.hpp"
#include "tux/util.hpp"

using namespace std;


namespace tux
{
    
void check_rc(int x, const char* context)
{
    if(x != 0)
    {
        throw runtime_error{string{context} + " error"};
    }
}    

decimal_number::decimal_number()
{
    double x = 0.00;
    int rc = deccvdbl(x, &decimal_);
    check_rc(rc, "deccvdbl");
}

decimal_number::decimal_number(string const& x)
{
    int rc = deccvasc(const_cast<char*>(x.c_str()),
                       x.size(),
                       &decimal_);
    check_rc(rc, "deccvasc");
}

decimal_number::decimal_number(int x)
{
    int rc = deccvint(x, &decimal_);
    check_rc(rc, "deccvint");
}

decimal_number::decimal_number(long x)
{
    int rc = deccvlong(x, &decimal_);
    check_rc(rc, "deccvlong");
}

decimal_number::decimal_number(double x)
{
    int rc = deccvdbl(x, &decimal_);
    check_rc(rc, "deccvdbl");
}

decimal_number::decimal_number(float x)
{
    int rc = deccvflt(x, &decimal_);
    check_rc(rc, "deccvflt");
}

decimal_number::decimal_number(dec_t const& x) :
    decimal_(x)
{
}

//----------------- assignment ---------------------


decimal_number& decimal_number::operator=(const string& x)
{
    int rc = deccvasc(const_cast<char*>(x.c_str()),
                       x.size(),
                       &decimal_);
    check_rc(rc, "deccvasc");
    return *this;
}

decimal_number& decimal_number::operator=(int x)
{
    int rc = deccvint(x, &decimal_);
    check_rc(rc, "deccvint");
    return *this;
}

decimal_number& decimal_number::operator=(long x)
{
    int rc = deccvlong(x, &decimal_);
    check_rc(rc, "deccvlong");
    return *this;
}

decimal_number& decimal_number::operator=(double x)
{
    int rc = deccvdbl(x, &decimal_);
    check_rc(rc, "deccvdbl");
    return *this;
}

decimal_number& decimal_number::operator=(float x)
{
    int rc = deccvflt(x, &decimal_);
    check_rc(rc, "deccvflt");
    return *this;
}

decimal_number& decimal_number::operator=(dec_t const& x)
{
    decimal_ = x;
    return *this;
}
 
// ----------------------- conversions -------------------------------

string decimal_number::to_string(size_t decimal_places, size_t max_size) const
{
    string result(max_size, 0);
    int rc = dectoasc(const_cast<dec_t*>(&as_dec_t()),
                      const_cast<char*>(result.data()),
                      result.size(),
                      decimal_places);
    check_rc(rc, "dectoasc");
    rtrim(result);
    return result;
}

int decimal_number::to_int() const
{
    int result = 0;
    int rc = dectoint(const_cast<dec_t*>(&decimal_), &result);
    check_rc(rc, "dectoint");
    return result;
}

long decimal_number::to_long() const
{
    long result = 0;
    int rc = dectolong(const_cast<dec_t*>(&decimal_), &result);
    check_rc(rc, "dectolong");
    return result;
}

double decimal_number::to_double() const
{
    double result = 0;
    int rc = dectodbl(const_cast<dec_t*>(&decimal_), &result);
    check_rc(rc, "dectodbl");
    return result;
}

float decimal_number::to_float() const
{
    float result = 0;
    int rc = dectoflt(const_cast<dec_t*>(&decimal_), &result);
    check_rc(rc, "dectoflt");
    return result;
}

// ---------------- compact storage -------------
string decimal_number::store(string::size_type size) const
{
    string result(size, 0);
    stdecimal(const_cast<dec_t*>(&decimal_),
              const_cast<char*>(result.data()),
              result.size());
    //trim_right_if(result, is_null);
    return result;
}

void decimal_number::load(const string& x)
{
    int rc = lddecimal(const_cast<char*>(x.c_str()),
                       x.size(),
                       &decimal_);
    check_rc(rc, "lddecimal");
}

dec_t& decimal_number::as_dec_t() noexcept
{
    return decimal_;
}

dec_t const& decimal_number::as_dec_t() const noexcept
{
     return decimal_;
}




//---------------- comparisons ------------------------

int compare(decimal_number const& a, decimal_number const& b)
{
    dec_t* araw = const_cast<dec_t*>(&(a.as_dec_t()));
    dec_t* braw = const_cast<dec_t*>(&(b.as_dec_t()));
    int result = deccmp(araw,braw);
    if(result < -1)
    {
        throw runtime_error("deccmp error");
    }
    return result;
}


// ---------------- arithmetic -----------------

decimal_number operator+(decimal_number const& a, decimal_number const& b)
{
    decimal_number result;
    dec_t* araw = const_cast<dec_t*>(&(a.as_dec_t()));
    dec_t* braw = const_cast<dec_t*>(&(b.as_dec_t()));
    dec_t* craw = const_cast<dec_t*>(&(result.as_dec_t()));
    int rc = decadd(araw, braw, craw);
    check_rc(rc, "decadd");
    return result;
}

decimal_number operator-(decimal_number const& a, decimal_number const& b) 
{
    decimal_number result;
    dec_t* araw = const_cast<dec_t*>(&(a.as_dec_t()));
    dec_t* braw = const_cast<dec_t*>(&(b.as_dec_t()));
    dec_t* craw = const_cast<dec_t*>(&(result.as_dec_t()));
    int rc = decsub(araw, braw, craw);
    check_rc(rc, "decsub");
    return result;
}

decimal_number operator*(decimal_number const& a, decimal_number const& b) 
{
    decimal_number result;
    dec_t* araw = const_cast<dec_t*>(&(a.as_dec_t()));
    dec_t* braw = const_cast<dec_t*>(&(b.as_dec_t()));
    dec_t* craw = const_cast<dec_t*>(&(result.as_dec_t()));
    int rc = decmul(araw, braw, craw);
    check_rc(rc, "decmul");
    return result;
}

decimal_number operator/(decimal_number const& a, decimal_number const& b) 
{
    decimal_number result;
    dec_t* araw = const_cast<dec_t*>(&(a.as_dec_t()));
    dec_t* braw = const_cast<dec_t*>(&(b.as_dec_t()));
    dec_t* craw = const_cast<dec_t*>(&(result.as_dec_t()));
    int rc = decdiv(araw, braw, craw);
    check_rc(rc, "decdiv");
    return result;
}

ostream& operator<<(ostream& s, decimal_number const& x)
{  
    return s << x.to_string();
}

}
