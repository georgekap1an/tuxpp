/** @defgroup utils utils
Various utilities. */

/** @file util.hpp
Miscellaneous utilities for use in the tuxpp library.
This file contains small functions, classes, etc. used
throughout the rest of the library.  Some are general
utilities (e.g. string functions), added to avoid
dependencies on additional libraries (e.g. boost).
Others wrap small ATMI functions. @ingroup utils */

#pragma once
#include <string>
#include <cstring>
#include <stdexcept>
#include <chrono>
#include "userlog.h"
#include "atmi.h"


//--------------------------------MACROS------------------------------
/** A macro to create a specialization of the type_name template class.
These specializations are used "stringify" template parameters
in tux::view16 and tux::view32. @ingroup utils */
#define ENABLE_TYPE_NAME(A) \
namespace tux { \
template<> \
struct type_name<A> \
{ \
    static const char* value() { return #A; } \
}; \
}

/** The namespace for this library. */
namespace tux
{

//------------------------TYPENAMES AS STRINGS-------------------------
/** A small traits class template to "stringify" template parameters.
For use in tux::view16 and tux::view32; override for views you use.
@sa ENABLE_TYPE_NAME to help write the specialization. @ingroup utils */
template <typename T>
struct type_name
{
    static const char* value() { return ""; }
};
    
//-----------------------------OPTIONAL------------------------------
/** Class template for storing either a T or nothing.
@note This is an extremely naive implementation which should be replaced by std::optional in c++17.
@ingroup utils */
template<typename T> class optional
{
private:
    T val_;
    bool is_specified_ = false;
    
public:
    optional() noexcept = default; /**< Default construct. */
    optional(optional<T> const& x) = default; /**< Copy construct. */
    optional& operator=(optional<T> const& x) = default; /**< Copy assign. */
    optional(optional<T>&& x) noexcept = default; /**< Move construct. */
    optional& operator=(optional<T>&& x) noexcept = default /**< Move assign. */;
    ~optional() noexcept = default; /**< Destruct. */
    
    optional(T const& x) : val_(x), is_specified_(true) {} /**< Construct from a T. */
    optional& operator=(T const&  x) { val_ = x; is_specified_ = true; return *this; } /**< Assign from a T. */
    optional(T&& x) noexcept : val_(std::move(x)), is_specified_(true) {} /**< Construct from a T&&. */
    optional& operator=(T&& x) noexcept { val_ = std::move(x); is_specified_ = true; return *this; } /**< Assigns from a T&&. */
    
    void reset() { val_ = T(); is_specified_ = false; } /**< Resets to default (null) state. */
    
    T* operator->() noexcept { return &val_; } /**< Accesses the T. */
    T& operator*() noexcept { return val_; } /**< Accesses the T. */
    explicit operator bool() noexcept { return is_specified_; } /**< Test for null state. */
}; 


//------------------------------NUMBERS-----------------------------
/** Returns clamped value of x.
@note This should be replaced by std::clamp in c++17. @ingroup utils */
template<typename T> T clamp(T x, T min, T max)
{
    return x < min ? min : (x > max ? max : x);
}

//------------------------------STRINGS-----------------------------
/** Erases anything after the first null terminator.
This comes in handy when atmi requires the caller to allocate a
sufficiently large buffer which it populates with a null-terminated
string. @ingroup utils */
template <typename C, typename A = std::allocator<C>>
void trim_to_null_terminator(std::basic_string<C, A>& x)
{
    auto pos = x.find('\0');
    if(pos != x.npos)
    {
        x.erase(pos);
    }  
}

/** Trims trailing spaces in place. @ingroup utils */
template <typename C, typename A = std::allocator<C>>
void rtrim(std::basic_string<C, A>& x)
{
    auto pos = x.find_last_not_of(' ');
    if(pos == x.npos)
    {
        x.clear();
    }
    else
    {
        x.erase(pos + 1);
    }  
}

/** Compares character arrays (null-safe) @ingroup utils */
int compare(const char* a, long alen, const char* b, long blen);

/** Converts char* to std::string (null-safe) @ingroup utils */
std::string cstr_to_string(const char* ptr);

//--------------------------------STRUCTS------------------------------
/** Zero-initializes bytes in struct.
Useful for "default" initializing various structs used in ATMI functions (e.g. @c TPQCTL).
@sa make_default() @ingroup utils */
template<typename S>
void init(S& s) noexcept
{
    std::memset(&s, 0, sizeof(s));
}

/** Creates a "default"-initialized struct instance.
@sa init() @ingroup utils */
template<typename S>
S make_default()
{
    S s;
    init(s);
    return s;
}

/** Copies std::string s into char[] a.
Useful for setting strings on Tuxedo structs. @ingroup utils */
template<typename A>
void set(A& a, std::string const& s)
{
    strncpy(a, s.c_str(), sizeof(a) - 1);
}

//----------------------------------BYTES------------------------------------------------
/** Returns 16-bit unsigned integer constructed from two separate bytes.
Used to read embedded size in compiled boolean/arithmetic expressions on
views and fml buffers. @ingroup utils */
uint16_t make_16bit_unsigned(uint8_t least_significant_byte, uint8_t most_significant_byte);


//----------------------------------LOGGING-----------------------------------------------
/** Writes a message to the application log [@c userlog] @ingroup utils */
template <typename... Args>  
void log(const char* format, Args const&... args) noexcept
{
    userlog(const_cast<char*>(format), args...);
}

//----------------------------------ENV------------------------------------------------
/** Gets the value of an environment variable [@c tuxgetenv]. @ingroup utils */
std::string get_env(std::string const& name);
/** Sets an environment variable [@c tuxputenv]. @ingroup utils */
void put_env(std::string const& name, std::string const& value);

//----------------------------------ERRORS---------------------------------------------
/** Returns diagnostic string suitable for logging.
@param function_name - relevant function name or other contextual information
@param code - tperrno
@param detail_code - tperrordetail
@returns a string constructed using @c tpstrerror, @c tpstrerrordetail, and @c Uunixmsg as needed.
@ingroup utils */
std::string build_error_string(std::string const& function_name, int code, int detail_code);

/** Models an error generated by an ATMI call. @ingroup utils */
class error : public std::runtime_error
{
public:
    error(); /**< Default construct. @ingroup utils */  
    error(error const& x) = default; /**< Copy construct. */ 
    error& operator=(error const& x) = default; /**< Copy assign. */ 
    error(error&& x) = default; /**< Move construct. */ 
    error& operator=(error&& x) = default; /**< Move assign. */ 
    virtual ~error() = default; /**< Destruct. */ 

    /** Construct from details.
    @param code tperrno
    @param detail_code tperrordetail
    @param what the error message to store
    @ingroup utils */
    error(int code, int detail_code, std::string const& what = "");
    
    int code() const noexcept; /**< Returns the error code. */
    int detail_code() const noexcept; /**< Returns the detail error code. */
    std::string str() const noexcept; /**< Returns the string version of the error code [@c tpstrerror]. */
    std::string detail_str() const noexcept; /**< Returns the string version of the detail error code [@c tpstrerrordetail]. */
 
private:
    int code_ = 0;
    int detail_code_ = 0;
};

/** Returns the last error generated by an ATMI call.
@param function_name - relevant function name or context to be included in result.
@returns error @ingroup utils */
error last_error(std::string const& function_name = "");

//----------------------------------BLOCKING-------------------------------------------
/** Represents the scope of the next block time adjustment.
@sa get_block_time, set_block_time
@ingroup utils */
enum class block_time_scope : long
{
    all = TPBLK_ALL, /**< Applies block time to all subsequent ATMI calls.*/
    next = TPBLK_NEXT /**< Applies block time only to the next potentially blocking ATMI call. */
};

/** Returns the global block time [@c tpgblktime]. @ingroup utils */
std::chrono::milliseconds get_block_time();

/** Returns the block time for the specified scope [@c tpgblktime]. @ingroup utils */
std::chrono::milliseconds get_block_time(block_time_scope scope);

/** Sets the block time for the specified scope [@c tpsblktime]. @ingroup utils */
void set_block_time(block_time_scope scope, std::chrono::seconds x);

/** Sets the block time for the specified scope [@c tpsblktime]. 
@note This function only works if UBB SCANUNIT is set in milliseconds. @ingroup utils */
void set_block_time(block_time_scope scope, std::chrono::milliseconds x);

//----------------------------------PRIORITY-------------------------------------------
/** Raises or lowers the current priority by amount [@c tpsprio].
@sa set_priority()
@ingroup utils */
void adjust_priority(int amount);
/** Set the current priority [@c tpsprio].
@sa adjust_priority()
@ingroup utils */
void set_priority(int priority);
/** Returns the current priority [@c tpgprio]. @ingroup utils */
int get_priority();

}
