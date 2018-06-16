#if TUXEDO_VERSION >= 1222
#include <stdexcept>
#include "tux/record.hpp"
#include "tux/fml32.hpp"

using namespace std;

namespace tux
{
    
record::record(record const& x)
{
    if(x)
    {
        const RECORD* r = x.as_record();
        init(r->rname, r->rdata, r->rsize, r->flag);
    }
}

record& record::operator=(record const& x)
{
    if(&x != this)
    {
        if(!x)
        {
            buffer_.free();
        }
        else
        {
            const RECORD* r = x.as_record();
            init(r->rname, r->rdata, r->rsize, r->flag);
        }
    }
    return *this;
}
    
    
record::record(class buffer&& x)
{
    if(x && x.type() != "RECORD")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to record");
    }
    buffer_ = move(x);
}

record& record::operator=(class buffer&& x)
{
    if(x && x.type() != "RECORD")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to record");
    }
    buffer_ = move(x);
    return *this;
}

class buffer const& record::buffer() const noexcept
{
    return buffer_;
}

class buffer& record::buffer() noexcept
{
    return buffer_;
}

class buffer&& record::move_buffer() noexcept
{
    return move(buffer_);
}

record::operator bool() const noexcept
{
    return (bool)buffer_;
}

string record::type() const
{
    if(buffer_)
    {
        return as_record()->rname;
    }
    else
    {
       return ""; 
    }
}

void record::init(string const& record_type, const char* data, int len, int flags)
{
    alloc(record_type);
    int rc = Rinit(as_record(), const_cast<char*>(data), len, flags);
    if(rc == -1)
    {
        throw fml32::last_error("Rinit");
    }
}

record::record(std::string const& record_type, const char* data, int len, int flags)
{
    init(record_type, data, len, flags);
}

string record::get_data(long flags) const
{
    static const int record_datatype = 0;
    string result;
    if(buffer_)
    {
        result.resize(as_record()->rsize);
        int len = result.size();
        int rc = Rget(const_cast<RECORD*>(as_record()),
                      nullptr,
                      const_cast<char*>(result.data()),
                      record_datatype,
                      &len,
                      flags);
        if(rc == -1)
        {
            throw fml32::last_error("Rget");
        }
    }
    return result;
    
}

void record::set_data(const char* data, int len, int flags)
{
    static const int record_datatype = 0;
    int rc = Rset(as_record(),
                  nullptr,
                  const_cast<char*>(data),
                  record_datatype,
                  len,
                  flags);
    if(rc == -1)
    {
        throw fml32::last_error("Rset");
    }
}

short record::get_short(string const& name)
{
    short x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_SHORT);
    return x;
}

void record::set(string const& name, short x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_SHORT);
}

long record::get_long(string const& name)
{
    long x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_LONG);
    return x;
}

void record::set(string const& name, long x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_LONG);
}

char record::get_char(string const& name)
{
    char x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_CHAR);
    return x;
}

void record::set(string const& name, char x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_CHAR);
}

float record::get_float(string const& name)
{
    float x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_FLOAT);
    return x;
}

void record::set(string const& name, float x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_FLOAT);
}

double record::get_double(string const& name)
{
    double x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_DOUBLE);
    return x;
}

void record::set(string const& name, double x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_DOUBLE);
}

string record::get_string(string const& name, bool trim_spaces, bool binary, int size_hint)
{
    string x;
    int len = size_hint;
    x.resize(len); 
    int type = binary ? C_CARRAY : C_STRING;
    bool sufficient_space = try_get_field(name, const_cast<char*>(x.data()), len, type);
    if(!sufficient_space)
    {
        len = as_record()->rsize;
        x.resize(len);
        get_field(name, const_cast<char*>(x.data()), len, type);
    }
    x.resize(len);
    if(type == C_STRING)
    {
        trim_to_null_terminator(x);
    }
    if(trim_spaces)
    {
        rtrim(x);
    }
    return x;
}

void record::set(string const& name, string const& x, bool binary)
{
    set_field(name,
              const_cast<char*>(x.data()),
              x.size(),
              binary ? C_CARRAY : C_STRING);
}

int record::get_int(std::string const& name)
{
    int x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_INT);
    return x;
}

void record::set(std::string const& name, int x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_INT);
}

decimal_number record::get_decimal(std::string const& name)
{
    decimal_number x;
    int len = sizeof(x.as_dec_t());
    get_field(name,
              reinterpret_cast<char*>(&x.as_dec_t()),
              len,
              C_DECIMAL);
    return x;
}

void record::set(std::string const& name, decimal_number const& x)
{
    dec_t* d = const_cast<dec_t*>(&x.as_dec_t());
    set_field(name,
              reinterpret_cast<char*>(d),
              sizeof(*d),
              C_DECIMAL);
}


unsigned int record::get_unsigned_int(std::string const& name)
{
    unsigned int x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_UINT);
    return x;
}

void record::set(std::string const& name, unsigned int x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_UINT);
}

unsigned long record::get_unsigned_long(std::string const& name)
{
    unsigned long x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_ULONG);
    return x;
}

void record::set(std::string const& name, unsigned long x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_ULONG);
}

long long record::get_long_long(std::string const& name)
{
    long long x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_LLONG);
    return x;
}

void record::set(std::string const& name, long long x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_LLONG);
}

unsigned short record::get_unsigned_short(std::string const& name)
{
    unsigned short x = 0;
    int len = sizeof(x);
    get_field(name,
              reinterpret_cast<char*>(&x),
              len,
              C_USHORT);
    return x;
}

void record::set(std::string const& name, unsigned short x)
{
    set_field(name,
              reinterpret_cast<char*>(&x),
              sizeof(x),
              C_USHORT);
}
    
RECORD* record::as_record() noexcept
{
    return reinterpret_cast<RECORD*>(buffer_.data());
}

const RECORD* record::as_record() const noexcept
{
    return reinterpret_cast<const RECORD*>(buffer_.data());
}
    
    
void record::set_field(std::string const& name, const char* data, int len, int datatype)
{
    int rc = Rset(as_record(),
                  const_cast<char*>(name.c_str()),
                  const_cast<char*>(data),
                  datatype,
                  len,
                  TPNOFLAGS);
    if(rc == -1)
    {
        throw fml32::last_error("Rset");
    }
}
    
void record::get_field(std::string const& name, char* data, int& len, int datatype) const
{
    int rc = Rget(const_cast<RECORD*>(as_record()),
                  const_cast<char*>(name.c_str()),
                  data,
                  datatype,
                  &len,
                  TPNOFLAGS);
    if(rc == -1)
    {
        throw fml32::last_error("Rget");
    }
}
    
bool record::try_get_field(string const& name, char* data, int& len, int datatype)
{
    int rc = Rget(as_record(),
                  const_cast<char*>(name.c_str()),
                  data,
                  datatype,
                  &len,
                  TPNOFLAGS);
    if(rc == -1)
    {
        if(Ferror32 == FNOSPACE)
        {
            return false;
        }
        else
        {
            throw fml32::last_error("Rget");
        }
    }
    else
    {
        return true;
    }
}



void record::alloc(string const& record_type)
{
    if(!buffer_ || type() != record_type)
    {
        char* type = const_cast<char*>(record_type.c_str());
        buffer_.alloc("RECORD", type, Frneeded(type));
    } 
}
    
}
#endif
