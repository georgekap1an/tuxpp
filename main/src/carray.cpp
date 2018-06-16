#include <cstring>
#include <utility>
#include <iostream>
#include <cassert>
#include "tux/carray.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{
    

carray::carray(string const& x)
{
    set(x.data(), x.size());
}

carray::carray(const char* x)
{
    set(x, strlen(x));
}

carray::carray(carray const& x)
{
    set(x.data(), x.size());
}

carray::carray(class buffer&& x)
{
    if(x && x.type() != "CARRAY")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to carray");
    }
    buffer_ = move(x);
}

carray& carray::operator=(string const& x)
{
    set(x.data(), x.size());
    return *this;
}
    
carray& carray::operator=(const char* x)
{
    set(x, strlen(x));
    return *this;
}

carray& carray::operator=(carray const& x)
{
    set(x.data(), x.size());
    return *this;
}

carray& carray::operator=(class buffer&& x)
{
    if(x && x.type() != "CARRAY")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to carray");
    }
    buffer_ = move(x);
    return *this;
}
    
carray& carray::operator+=(string const& x)
{
    append(x.data(), x.size());
    return *this;
}

carray& carray::operator+=(const char* x)
{
    append(x, strlen(x));
    return *this;
}

carray& carray::operator+=(carray const&  x)
{
    append(x.data(), x.size());
    return *this;
}

class buffer const& carray::buffer() const noexcept
{
    return buffer_;
}

class buffer& carray::buffer() noexcept
{
    return buffer_;
}

class buffer&& carray::move_buffer() noexcept
{
    return move(buffer_);
}

char* carray::data() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* carray::data() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

long carray::size() const noexcept
{
    return buffer_.data_size();
}

string carray::to_string() const
{
    return buffer_.data_size() ? string(data(), size()) : string();
}

char* carray::begin() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* carray::begin() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

char* carray::end() noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size()) : nullptr;
}

const char* carray::end() const noexcept
{
    return buffer_.data_size() ? (buffer_.data() + buffer_.data_size()) : nullptr;
}

carray::operator bool() const noexcept
{
    return (bool)buffer_.data_size();
}

void carray::reserve(long new_len)
{
    assert(new_len >= 0);
    if(!buffer_)
    {
        buffer_.alloc("CARRAY", nullptr, new_len ? new_len : 1);
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

void carray::set(const char* x, long len)
{
    reserve(len);
    memmove(buffer_.data(), x, len);
    buffer_.data_size(len);
}

void carray::append(const char* x, long len)
{
    long old_len = buffer_.data_size();
    reserve(old_len + len);
    memmove(buffer_.data() + old_len, x, len);
    buffer_.data_size(old_len + len);
}

ostream& operator<<(ostream& s, carray& x)
{
    if(x)
    {
        s.write(x.data(), x.size());
    }
    return s;
}

}
