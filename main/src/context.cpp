#include "tux/context.hpp"
#include "tux/util.hpp"
#include <iostream>

using namespace std;

namespace tux
{
    
string context_to_string(TPCONTEXT_T c)
{
	switch(c)
	{
		case TPNULLCONTEXT: return "TPNULLCONTEXT";
		case TPINVALIDCONTEXT: return "TPINVALIDCONTEXT";
		case TPSINGLECONTEXT: return "TPSINGLECONTEXT";
		default: return to_string(c);
	}
}

int check_authentication_level()
{
    int result = tpchkauth();
    if(result == -1)
    {
        throw last_error("tpchkauth");
    }
    return result;
}
    
void initialize(init_request const& x,
                context_mode mode)
{
    if(mode == context_mode::appthread)
    {
        int rc = tpappthrinit(const_cast<TPINIT*>(x.as_tpinit()));
        if(rc == -1)
        {
            throw last_error("tpappthrinit");
        }
    }
    else
    {
        int rc = tpinit(const_cast<TPINIT*>(x.as_tpinit()));
        if(rc == -1)
        {
            throw last_error("tpinit");
        }
    }
}

void terminate(context_mode mode)
{
    if(mode == context_mode::appthread)
    {
        int rc = tpappthrterm();
        if(rc == -1)
        {
            throw last_error("tpappthrterm");
        }
    }
    else
    {    
        int rc = tpterm();
        if(rc == -1)
        {
            throw last_error("tpterm");
        }
    }
}

TPCONTEXT_T get_context()
{
    TPCONTEXT_T x;
    int rc = tpgetctxt(&x, TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpgetctxt");
    }
    return x;
}

void set_context(TPCONTEXT_T x)
{
    int rc = tpsetctxt(x, TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpgetctxt");
    }    
}


context::context(init_request const& x,
                 context_mode mode)
: mode_(mode)
{
    initialize(x, mode_);
    ctx_ = get_context();
}

context::~context() noexcept
{
    try
    {
        make_current();
        tux::terminate(mode_);
    }
    catch(exception& e)
    {
        log("WARN: %s [~context()]", e.what());
    }
}

void context::make_current()
{
    set_context(ctx_);
}

context_mode context::mode() const noexcept
{
    return mode_;
}

TPCONTEXT_T context::value() const noexcept
{
    return ctx_;
}


}
