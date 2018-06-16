/** @defgroup comm comm
Communication routines
*/

/** @file context.hpp
@c context class and related functions (for clients). @ingroup comm */
#pragma once
#include "atmi.h"
#include "tux/init_request.hpp"

namespace tux
{

/** Represents the type of Tuxedo context (normal or application thread).
@sa initialize, context::context() @ingroup comm */
enum class context_mode {normal, appthread};

/** Serialize a TPCONTEXT_T (primarily for logging). @ingroup comm */
std::string context_to_string(TPCONTEXT_T c);
    
/** Check the authentication level [@c tpchkauth].
@returns TPNOAUTH, TPSYSAUTH, or TPAPPAUTH @ingroup comm */
int check_authentication_level();

/** connect to a domain [@c tpinit].
@param x supply a populated object to specify password, options, etc.
@param mode normal or appthread
@sa context::context()
@ingroup comm */    
void initialize(init_request const& x = init_request(),
                context_mode mode = context_mode::normal);

/** disconnect from a domain [@c tpterm].
@param mode normal or appthread
@sa context::~context()
@ingroup comm */
void terminate(context_mode mode = context_mode::normal);



/** Get the current context [@c tpgetctxt]. @sa context::value() @ingroup comm */
TPCONTEXT_T get_context();

/** Set the current context [@c tpsetctxt]. @sa context::make_current() @ingroup comm */
void set_context(TPCONTEXT_T x);

/** RAII wrapper for initializing, terminating, and swapping
contexts. @ingroup comm */
class context
{
public:
    /** Construct a context [@c tpinit]. @sa initialize() */
    explicit context(init_request const& x = init_request(),
                     context_mode mode = context_mode::normal);
    /** Destruct a context [@c tpterm]. @sa terminate() */
    ~context() noexcept;
    /** Make @c this the current context [@c tpsetctxt]. @sa  set_context() */
    void make_current();
    /** Returns the mode of this context. */
    context_mode mode() const noexcept;
    /** Returns the id for this context. */
    TPCONTEXT_T value() const noexcept;
private:
    TPCONTEXT_T ctx_ = TPNULLCONTEXT;
    context_mode mode_ = context_mode::normal;
    
};

}
