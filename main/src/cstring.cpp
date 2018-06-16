#include <cstring>
#include <utility>
#include <iostream>
#include <cassert>
#include "tux/cstring.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{

cstring::cstring(string const& x)
{
    set(x.data(), x.size());
}

cstring::cstring(const char* x) 
{
    set(x, strlen(x));
}

cstring::cstring(cstring const& x)
{
    set(x.data(), x.size());
}

cstring::cstring(class buffer&& x)
{
    if(x && x.type() != "STRING")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to cstring");
    }
    buffer_ = move(x);
}

cstring& cstring::operator=(string const& x)
{
    set(x.data(), x.size());
    return *this;
}
    
cstring& cstring::operator=(const char* x)
{
    set(x, strlen(x));
    return *this;
}

cstring& cstring::operator=(cstring const& x)
{
    set(x.data(), x.size());
    return *this;
}

cstring& cstring::operator=(class buffer&& x)
{
    if(x && x.type() != "STRING")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to cstring");
    }
    buffer_ = move(x);
    return *this;
}
    
cstring& cstring::operator+=(string const& x)
{
    append(x.data(), x.size());
    return *this;
}

cstring& cstring::operator+=(const char* x)
{
    append(x, strlen(x));
    return *this;
}

cstring& cstring::operator+=(cstring const&  x)
{
    append(x.data(), x.size());
    return *this;
}

class buffer const& cstring::buffer() const noexcept
{
    return buffer_;
}

class buffer& cstring::buffer() noexcept
{
    return buffer_;
}

class buffer&& cstring::move_buffer() noexcept
{
    return move(buffer_);
}

char* cstring::data() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* cstring::data() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

long cstring::size() const noexcept
{
    return buffer_.data_size() ? buffer_.data_size() - 1 : 0;
}

string cstring::to_string() const
{
    return buffer_.data_size() ? string(data(), size()) : string();
}

char* cstring::begin() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* cstring::begin() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

char* cstring::end() noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size() - 1) : nullptr;
}

const char* cstring::end() const noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size() - 1) : nullptr;
}


cstring::operator bool() const noexcept
{
    return (bool)buffer_.data_size();
}

void cstring::reserve(long new_len)
{
    assert(new_len >= 0);
    ++new_len; // we'll need to allocate extra byte for null terminator
    if(!buffer_)
    {
        buffer_.alloc("STRING", nullptr, new_len);
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

void cstring::set(const char* x, long len)
{
    reserve(len);
    memmove(buffer_.data(), x, len + 1);
    buffer_.data_size(len + 1);
}

void cstring::append(const char* x, long len)
{
    long old_len = size();
    reserve(old_len + len);
    memmove(buffer_.data() + old_len, x, len + 1);
    buffer_.data_size(old_len + len + 1);
}
 
ostream& operator<<(ostream& s, cstring const& x)
{
    if(x)
    {
        s.write(x.data(), x.size());
    }
    return s;
}

}
