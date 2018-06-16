#include <utility>
#include <cstring>
#include "tux/init_request.hpp"

#include <iostream>

using namespace std;

namespace tux
{
    
long calculate_buffer_size(long auth_data_len)
{
    return TPINITNEED(static_cast<unsigned>(auth_data_len));
}
    
init_request::init_request(init_request const& x)
{
    long auth_data_len = x.as_tpinit()->datalen;
    alloc(auth_data_len);
    memmove(buffer_.data(), x.buffer_.data(), calculate_buffer_size(auth_data_len));
}

init_request& init_request::operator=(init_request const& x)
{
    init_request tmp(x);
    *this = move(tmp);
    return *this;
}

void init_request::username(string const& x)
{
    alloc();
    strncpy(as_tpinit()->usrname, x.c_str(), MAXTIDENT);
}

void init_request::client_name(string const& x)
{
    alloc();
    strncpy(as_tpinit()->cltname, x.c_str(), MAXTIDENT);
}

void init_request::app_password(string const& x)
{
    alloc();
    strncpy(as_tpinit()->passwd, x.c_str(), MAXTIDENT);
}

void init_request::groupname(string const& x)
{
    alloc();
    strncpy(as_tpinit()->grpname, x.c_str(), MAXTIDENT);
}

void init_request::flags(long flags)
{
    alloc();
    as_tpinit()->flags = flags;
}

void init_request::auth_data(string const& x)
{
    alloc(x.size());
    as_tpinit()->datalen = x.size();
    memmove(reinterpret_cast<char*>(&(as_tpinit()->data)), x.data(), x.size());
}

string init_request::username() const
{
    if(as_tpinit())
    {
        return as_tpinit()->usrname;
    }
    else
    {
        return "";
    }
}
string init_request::client_name() const
{
    if(as_tpinit())
    {
        return as_tpinit()->cltname;
    }
    else
    {
        return "";
    }
}
string init_request::app_password() const
{
    if(as_tpinit())
    {
        return as_tpinit()->passwd;
    }
    else
    {
        return "";
    }
}

string init_request::groupname() const
{
    if(as_tpinit())
    {
        return as_tpinit()->grpname;
    }
    else
    {
        return "";
    }
}

long init_request::flags() const
{
    if(as_tpinit())
    {
        return as_tpinit()->flags;
    }
    else
    {
        return TPNOFLAGS;
    }
}

string init_request::auth_data() const
{
    if(as_tpinit())
    {
        return string(reinterpret_cast<char*>(const_cast<long*>(&(as_tpinit()->data))),
                      as_tpinit()->datalen);
    }
    else
    {
        return "";
    }  
}

init_request::operator bool () const noexcept
{
    return static_cast<bool>(buffer_);
}

TPINIT* init_request::as_tpinit() noexcept
{
    return reinterpret_cast<TPINIT*>(buffer_.data());
}

const TPINIT* init_request::as_tpinit() const noexcept
{
    return reinterpret_cast<const TPINIT*>(buffer_.data());
}

void init_request::alloc(long auth_data_len)
{
    long new_size = calculate_buffer_size(auth_data_len);
    if(!buffer_)
    {
        buffer_.alloc("TPINIT", nullptr, new_size);
        memset(as_tpinit(), 0, sizeof(TPINIT));
    }
    else if(buffer_.size() < new_size)
    {
        buffer_.realloc(new_size);
    }
}

bool operator==(init_request const& a, init_request const& b)
{
    const TPINIT* aptr = a.as_tpinit();
    const TPINIT* bptr = b.as_tpinit();
    if(!aptr && !bptr)
    {
        return true;
    }
    if(!aptr || !bptr)
    {
        return false;
    }
    if(aptr->datalen != bptr->datalen)
    {
        return false;
    }
    long size = calculate_buffer_size(aptr->datalen);
    return memcmp(a.buffer_.data(), b.buffer_.data(), size) == 0;
}

bool operator!=(init_request const& a, init_request const& b)
{
    return !(a == b);
}

}
