#include <cstring>
#include <utility>
#include <iostream>
#include <cassert>
#include <limits.h> // this may not be portable
#include "tux/mbstring.hpp"
#include "tux/util.hpp"


using namespace std;

namespace tux
{
    

mbstring::mbstring(string const& x)
{
    set(x.data(), x.size());
}

mbstring::mbstring(const char* x)
{
    set(x, strlen(x));
}

mbstring::mbstring(mbstring const& x)
{
    set(x.data(), x.size());
}

mbstring::mbstring(class buffer&& x)
{
    if(x && x.type() != "MBSTRING")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to mbstring");
    }
    buffer_ = move(x);
}

mbstring& mbstring::operator=(string const& x)
{
    set(x.data(), x.size());
    return *this;
}
    
mbstring& mbstring::operator=(const char* x)
{
    set(x, strlen(x));
    return *this;
}

mbstring& mbstring::operator=(mbstring const& x)
{
    set(x.data(), x.size());
    return *this;
}

mbstring& mbstring::operator=(class buffer&& x)
{
    if(x && x.type() != "MBSTRING")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to mbstring");
    }
    buffer_ = move(x);
    return *this;
}
    
mbstring& mbstring::operator+=(string const& x)
{
    append(x.data(), x.size());
    return *this;
}

mbstring& mbstring::operator+=(const char* x)
{
    append(x, strlen(x));
    return *this;
}

mbstring& mbstring::operator+=(mbstring const&  x)
{
    append(x.data(), x.size());
    return *this;
}

class buffer const& mbstring::buffer() const noexcept
{
    return buffer_;
}

class buffer& mbstring::buffer() noexcept
{
    return buffer_;
}

class buffer&& mbstring::move_buffer() noexcept
{
    return move(buffer_);
}

char* mbstring::data() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* mbstring::data() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

long mbstring::size() const noexcept
{
    return buffer_.data_size() ? buffer_.data_size() - 1 : 0;
}

string mbstring::to_string() const
{
    return buffer_.data_size() ? string(data(), size()) : string();
}

char* mbstring::begin() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* mbstring::begin() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

char* mbstring::end() noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size() - 1) : nullptr;
}

const char* mbstring::end() const noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size() - 1) : nullptr;
}


mbstring::operator bool() const noexcept
{
    return (bool)buffer_.data_size();
}

void mbstring::reserve(long new_len)
{
    assert(new_len >= 0);
    ++new_len; // we'll need to allocate extra byte for null terminator
    if(!buffer_)
    {
        buffer_.alloc("MBSTRING", nullptr, new_len);
    }
    else
    {
          long current_capacity = buffer_.size();
          if(new_len > current_capacity)
          {
              buffer_.realloc(max(2 * current_capacity, new_len));
          }
    }
}

void mbstring::set(const char* x, long len)
{
    reserve(len);
    memmove(buffer_.data(), x, len + 1);
    buffer_.data_size(len + 1);
}

void mbstring::append(const char* x, long len)
{
    long old_len = size();
    reserve(old_len + len);
    memmove(buffer_.data() + old_len, x, len + 1);
    buffer_.data_size(old_len + len + 1);
}


string mbstring::get_encoding_name() const
{
    if(!buffer_)
    {
        return "";
    }
    string result;
    result.resize(NL_LANGMAX + 1); // not sure this is portable
    int rc = tpgetmbenc(const_cast<char*>(buffer_.data()),
                        const_cast<char*>(result.data()),
                        TPNOFLAGS);
    if(rc == -1)
    {
        if(tperrno == TPEPROTO)
        {
            return "";
        }
        else
        {
            throw last_error("tpgetmbenc");
        }
    }
    trim_to_null_terminator(result);
    return result;
}

void mbstring::set_encoding_name(string const& encoding)
{
    int rc = tpsetmbenc(buffer_.data(),
                        const_cast<char*>(encoding.c_str()),
                        TPNOFLAGS);
    if(rc != 0)
    {
        throw last_error("tpsetmbenc");    
    }
}

void mbstring::clear_encoding_name()
{
    int rc = tpsetmbenc(buffer_.data(),
                        nullptr,
                        RM_ENC);
    if(rc != 0)
    {
        throw last_error("tpsetmbenc(RM_ENC)");    
    }
}

void mbstring::convert_encoding(string const& encoding, long flags)
{
    int len = buffer_.data_size();
    char* bufp = buffer_.release();
    int rc = tpconvmb(&bufp,
                      &len,
                      const_cast<char*>(encoding.c_str()),
                      flags);
    buffer_.acquire(bufp, len);
    if(rc == -1)
    {
        throw last_error("tpconvmb");    
    }
}
 
ostream& operator<<(ostream& s, mbstring const& x)
{
    if(x)
    {
        s.write(x.data(), x.size());
    }
    return s;
}

}
