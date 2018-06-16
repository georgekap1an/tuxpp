#include <cstring>
#include <iostream>
#include "Uunix.h"
#include "tux/util.hpp"

using namespace std;
using namespace std::chrono;

namespace tux
{

//---------------------------STRINGS-------------------------------    

int compare(const char* a, long alen, const char* b, long blen)
{
    if(!a && !b)
    {
        return 0;
    }
    if(!a)
    {
        return -1;
    }
    if(!b)
    {
        return 1;
    }
    if(!alen && !blen)
    {
        return 0;
    }
    if(!alen)
    {
        return -1;
    }
    if(!blen)
    {
        return 1;
    }
    const char* aend = a + alen;
    const char* bend = b + blen;
    while(a != aend && b != bend)
    {
        if(*a < *b)
        {
            return -1;
        }
        if(*a > *b)
        {
            return 1;
        }
        ++a;
        ++b;
    }
    if(a == aend && b == bend)
    {
        return 0;
    }
    if(a == aend)
    {
        return -1;
    }
    return 1;
}


string cstr_to_string(const char* ptr)
{
    return ptr ? string{ptr} : string{};
}

//---------------------------BYTES-------------------------------  
uint16_t make_16bit_unsigned(uint8_t least_significant_byte, uint8_t most_significant_byte)
{
    return uint16_t((most_significant_byte << 8) | least_significant_byte);
}

//----------------------------------ENV------------------------------------------------
std::string get_env(std::string const& name)
{
    return cstr_to_string(tuxgetenv(const_cast<char*>(name.c_str())));
}

void put_env(std::string const& name, std::string const& value)
{
    string s(name);
    s += '=';
    s += value;
    int rc = tuxputenv(const_cast<char*>(s.c_str()));
    if(rc != 0)
    {
        throw runtime_error("tuxputenv [" + s + "] failed");
    }
}


//---------------------------ERRORS-------------------------------  
error::error() :
    runtime_error("")
{
}

error::error(int code, int detail_code, string const& what) :
        runtime_error(what),
        code_(code),
        detail_code_(detail_code)
{
}    
    
int error::code() const noexcept
{
    return code_;
}

int error::detail_code() const noexcept
{
    return detail_code_;
}

string error::str() const noexcept
{
    return cstr_to_string(tpstrerror(code_));
}

string error::detail_str() const noexcept
{
    return cstr_to_string(tpstrerrordetail(detail_code_, TPNOFLAGS));
}

string build_error_string(string const& function_name,
                    int code,
                    int detail_code)
{  
    string result;
    if(!function_name.empty())
    {
        result += function_name;
        result += ".";
    }
    const char* str = tpstrerror(code);
    if(str)
    {
        result += str;
    }
    const char* dtlstr =   tpstrerrordetail(detail_code, TPNOFLAGS);
    if(dtlstr && strlen(dtlstr) > 0)
    {
        result += " (";
        result += dtlstr;
        result += ")";
    }
    if(code == TPEOS)
    {
        const char* os_str = Uunixmsg[Uunixerr];
        if(os_str)
        {
            result += " (";
            result += os_str;
            result += ")";
        }
    }
    return result;    
}

error last_error(string const& function_name)
{
    static const long flags = 0;
    int err = tperrno;
    int errdtl = tperrordetail(flags);
    string what = build_error_string(function_name, err, errdtl);
    return error(err, errdtl, what);
}

//---------------------------BLOCKING-------------------------------------
milliseconds get_block_time()
{
    int result = tpgblktime(TPBLK_MILLISECOND);
    if(result == -1)
    {
        throw last_error("tpgblktime");
    }
    return milliseconds(result);
}

milliseconds get_block_time(block_time_scope scope)
{
    int result = tpgblktime(TPBLK_MILLISECOND | static_cast<long>(scope));
    if(result == -1)
    {
        throw last_error("tpgblktime");
    }
    return milliseconds(result);
}

void set_block_time(block_time_scope scope, std::chrono::seconds x)
{
    int rc = tpsblktime(x.count(), TPBLK_SECOND | static_cast<long>(scope));
    if(rc == -1)
    {
        throw last_error("tpsblktime");
    }    
}

void set_block_time(block_time_scope scope, std::chrono::milliseconds x)
{
    int rc = tpsblktime(x.count(), TPBLK_MILLISECOND | static_cast<long>(scope));
    if(rc == -1)
    {
        throw last_error("tpsblktime");
    }
}

//----------------------------------PRIORITY-------------------------------------------
// core set - private
void private_set_priority(int val, long flags)
{
    int rc = tpsprio(val, flags);
    if(rc == -1)
    {
        throw last_error("tpsprio");
    }
}

void adjust_priority(int amount)
{
    private_set_priority(amount, TPNOFLAGS);
}

void set_priority(int priority)
{
    private_set_priority(clamp(priority, 1, 100), TPABSOLUTE);
}

int get_priority()
{
    int result = tpgprio();
    if(result == -1)
    {
        throw last_error("tpgprio");
    }
    return result;
}

}
