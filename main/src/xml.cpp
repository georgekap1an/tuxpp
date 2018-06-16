#include <cstring>
#include <utility>
#include <iostream>
#include <cassert>
#include "tux/xml.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{
    

xml::xml(string const& x)
{
    set(x.data(), x.size());
}

xml::xml(const char* x)
{
    set(x, strlen(x));
}

xml::xml(xml const& x)
{
    set(x.data(), x.size());
}

xml::xml(class buffer&& x)
{
    if(x && x.type() != "XML")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to xml");
    }
    buffer_ = move(x);
}

xml& xml::operator=(string const& x)
{
    set(x.data(), x.size());
    return *this;
}
    
xml& xml::operator=(const char* x)
{
    set(x, strlen(x));
    return *this;
}

xml& xml::operator=(xml const& x)
{
    set(x.data(), x.size());
    return *this;
}

xml& xml::operator=(class buffer&& x)
{
    if(x && x.type() != "XML")
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to xml");
    }
    buffer_ = move(x);
    return *this;
}
    
xml& xml::operator+=(string const& x)
{
    append(x.data(), x.size());
    return *this;
}

xml& xml::operator+=(const char* x)
{
    append(x, strlen(x));
    return *this;
}

xml& xml::operator+=(xml const&  x)
{
    append(x.data(), x.size());
    return *this;
}

class buffer const& xml::buffer() const noexcept
{
    return buffer_;
}

class buffer& xml::buffer() noexcept
{
    return buffer_;
}

class buffer&& xml::move_buffer() noexcept
{
    return move(buffer_);
}

char* xml::data() noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

const char* xml::data() const noexcept
{
    return buffer_.data_size() ? buffer_.data() : nullptr;
}

long xml::size() const noexcept
{
    return buffer_.data_size() ? buffer_.data_size() - 1 : 0;
}

string xml::to_string() const
{
    return buffer_.data_size() ? string(data(), buffer_.data_size() - 1) : string();
}

xml::operator bool() const noexcept
{
    return (bool)buffer_.data_size();
}

void xml::reserve(long new_len)
{
    assert(new_len >= 0);
    ++new_len; // we'll need to allocate extra byte for null terminator
    if(!buffer_)
    {
        buffer_.alloc("XML", nullptr, new_len);
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

void xml::set(const char* x, long len)
{
    reserve(len);
    memmove(buffer_.data(), x, len + 1);
    buffer_.data_size(len + 1);
}

void xml::append(const char* x, long len)
{
    long old_len = size();
    reserve(old_len + len);
    memmove(buffer_.data() + old_len, x, len + 1);
    buffer_.data_size(old_len + len + 1);
}

ostream& operator<<(ostream& s, xml& x)
{
    if(x)
    {
        s.write(x.data(), x.size());
    }
    return s;
}

}
