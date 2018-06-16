#include "atmi.h"
#include "tux/admin.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{
    
string to_string(CLIENTID const& clt)
{
    string str(TPCONVMAXSTR, 0);
    int rc = tpconvert(const_cast<char*>(str.data()),
                       reinterpret_cast<char*>(const_cast<CLIENTID*>(&clt)),
                       TPCONVCLTID | TPTOSTRING);
    if(rc == -1)
    {
        throw last_error("tpconvert(TPCONVCLTID | TPTOSTRING)");
    }
    trim_to_null_terminator(str);
    return str;
}
    
CLIENTID to_clientid(string const& str)
{
    CLIENTID clt;
    int rc = tpconvert(const_cast<char*>(str.data()),
                       reinterpret_cast<char*>(const_cast<CLIENTID*>(&clt)),
                       TPCONVCLTID);
    if(rc == -1)
    {
        throw last_error("tpconvert(TPCONVCLTID)");
    }
    return clt;
}    

string to_string(TPTRANID const& tranid)
{
    string str(TPCONVMAXSTR, 0);
    int rc = tpconvert(const_cast<char*>(str.data()),
                       reinterpret_cast<char*>(const_cast<TPTRANID*>(&tranid)),
                       TPCONVTRANID | TPTOSTRING);
    if(rc == -1)
    {
        throw last_error("tpconvert(TPCONVTRANID | TPTOSTRING)");
    }
    trim_to_null_terminator(str);
    return str;
}

TPTRANID to_tranid(string const& str)
{
    TPTRANID tranid;
    int rc = tpconvert(const_cast<char*>(str.data()),
                       reinterpret_cast<char*>(const_cast<TPTRANID*>(&tranid)),
                       TPCONVTRANID);
    if(rc == -1)
    {
        throw last_error("tpconvert(TPCONVTRANID)");
    }
    return tranid;
}

#if TUXEDO_VERSION >= 1213    
void set_call_info(fml32 const& call_info, buffer& dest)
{
    int rc = tpsetcallinfo(dest.data(),
                           const_cast<FBFR32*>(call_info.as_fbfr()),
                           TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpsetcallinfo");
    }
}

fml32 get_call_info(buffer const& x)
{
    fml32 f;
    f.reserve(0);
    FBFR32* fbfr = reinterpret_cast<FBFR32*>(f.buffer().release());
    int rc = tpgetcallinfo(const_cast<char*>(x.data()),
                           &fbfr,
                           TPNOFLAGS);
    f.buffer().acquire(reinterpret_cast<char*>(fbfr));
    if(rc == -1)
    {
        throw last_error("tpgetcallinfo");
    }
    return f;
}
#endif

void admin_call(fml32 const& request, fml32& response)
{
    if(!response)
    {
        response.reserve(0);
    }
    const FBFR32* in = request.as_fbfr();
    FBFR32* out = reinterpret_cast<FBFR32*>(response.buffer().release());
    int rc = tpadmcall(const_cast<FBFR32*>(in), &out, TPNOFLAGS);
    response.buffer().acquire(reinterpret_cast<char*>(out));
    if(rc == -1)
    {
        throw last_error("tpadmcall");
    }
}

fml32 admin_call(fml32 const& request)
{
    fml32 result;
    admin_call(request, result);
    return result;
}

fml32 set_repository_info(std::string const& filename, fml32 const& input)
{
    fml32 output;
    output.reserve(0);
    const FBFR32* in = input.as_fbfr();
    FBFR32* out = reinterpret_cast<FBFR32*>(output.buffer().release());
    int rc = tpsetrepos(const_cast<char*>(filename.c_str()), const_cast<FBFR32*>(in), &out);
    output.buffer().acquire(reinterpret_cast<char*>(out));
    if(rc == -1)
    {
        throw last_error("tpsetrepos");
    }
    return output;
}

fml32 get_repository_info(std::string const& filename, fml32 const& input)
{
    fml32 output;
    output.reserve(0);
    const FBFR32* in = input.as_fbfr();
    FBFR32* out = reinterpret_cast<FBFR32*>(output.buffer().release());
    int rc = tpgetrepos(const_cast<char*>(filename.c_str()), const_cast<FBFR32*>(in), &out);
    output.buffer().acquire(reinterpret_cast<char*>(out));
    if(rc == -1)
    {
        throw last_error("tpgetrepos");
    }
    return output;
}



}
