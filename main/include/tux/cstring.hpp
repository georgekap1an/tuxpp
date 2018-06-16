/** @file cstring.hpp
@c cstring class and related functions.
@ingroup buffers*/
#pragma once
#include <iosfwd>
#include "tux/buffer.hpp"
#include "tux/util.hpp"

namespace tux
{    
/** Models a "STRING" typed buffer (null-terminated string).
@ingroup buffers*/
class cstring
{
public:
    cstring() noexcept = default; /**< Default construct. No allocation is performed. */
    explicit cstring(std::string const& x); /**< Construct from std::string. */
    explicit cstring(const char* x); /**< Construct from const char*. */
    cstring(cstring const& x); /**< Copy construct. */
    cstring(cstring&& x) noexcept = default; /**< Move construct. */
    /** Construct from a buffer.
    @note Tuxedo tends to be a very "dynamic" framework,
    and this includes its approach to typed buffers.
    Services receive and return generic buffers, and nothing
    prohibits a service from, say, returning different
    buffer types from invocation to invocation.  This library
    aims to support that kind of flexibility, but also to support
    simple constructs when buffer types are known in advance (a
    common scenario).  This intentionally implicit conversion
    exists to support code snippets like:
        @code cstring reply = call("MY_SERVICE", request); @endcode
    call() technically returns a generic \ref buffer, but if it
    is known in advance the service always returns buffers of type
    "STRING", code like the above can safely be written.  (Internally, the
    buffer type is checked at runtime to make sure it matches; if not
    an exception is thrown.)   An alternative scheme might have involved
    class inheritance, with \ref buffer as the base class and cstring as
    a subclass.  I rejected that approach for a number of reasons:
    \arg Past experience
    \arg There's a growing skepticism regarding class inheritance in the c++ community
    \arg Conceptually, a string is not a kind of "buffer" any more than vectors and std::strings
    are kinds of buffers.
    */
    cstring(class buffer&& x);
    
    cstring& operator=(std::string const& x); /**< Assign from string. */
    cstring& operator=(const char* x); /**< Assign from const char*. */
    cstring& operator=(cstring const& x); /**< Copy assign. */
    cstring& operator=(cstring&& x) = default; /**< Move assign. */
    /** Assign from buffer.
    @sa cstring(buffer&&)*/
    cstring& operator=(class buffer&& x); 
    
    cstring& operator+=(std::string const& x); /**< Appends std::string. */
    cstring& operator+=(const char*  x); /**< Appends const char*. */
    cstring& operator+=(cstring const& x); /**< Appends cstring. */
    
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    class buffer& buffer() noexcept; /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept; 

    char* data() noexcept; /**< Access underlying buffer's underlying data. */
    const char* data() const noexcept; /**< Access underlying buffer's underlying data. */
    long size() const noexcept; /**< Returns string size. */
    std::string to_string() const; /**< Converts to std::string. */
    
    char* begin() noexcept; /**< Returns begin iterator. */
    const char* begin() const noexcept; /**< Returns begin iterator. */
    char* end() noexcept; /**< Returns end iterator. */
    const char* end() const noexcept; /**< Returns end iterator. */
    
    explicit operator bool() const noexcept; /**< Test for null state. */
    
    void reserve(long new_data_size); /**< Reserves @c new_data_size capacity. */
       
private:
    
    class buffer buffer_;
        
    void set(const char* x, long len);
    void append(const char* x, long len);
};

/** Concatenates two cstrings. @relates cstring */
inline cstring operator+(cstring x, cstring const& y) { return (x += y); } 

/** Compares two cstrings.
@relates cstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(cstring const& a, cstring const& b) noexcept 
    { return compare(a.data(), a.size(), b.data(), b.size()); } 
/** @relates cstring */ inline bool operator==(cstring const& a, cstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates cstring */ inline bool operator!=(cstring const& a, cstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates cstring */ inline bool operator< (cstring const& a, cstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates cstring */ inline bool operator<=(cstring const& a, cstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates cstring */ inline bool operator> (cstring const& a, cstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates cstring */ inline bool operator>=(cstring const& a, cstring const& b) noexcept { return compare(a,b) >= 0;}

/** Compares cstring and std::string.
@relates cstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(cstring const& a, std::string const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); } 
/** @relates cstring */ inline bool operator==(cstring const& a, std::string const& b) noexcept { return compare(a,b) == 0;}
/** @relates cstring */ inline bool operator!=(cstring const& a, std::string const& b) noexcept { return compare(a,b) != 0;}
/** @relates cstring */ inline bool operator< (cstring const& a, std::string const& b) noexcept { return compare(a,b) <  0;}
/** @relates cstring */ inline bool operator<=(cstring const& a, std::string const& b) noexcept { return compare(a,b) <= 0;}
/** @relates cstring */ inline bool operator> (cstring const& a, std::string const& b) noexcept { return compare(a,b) >  0;}
/** @relates cstring */ inline bool operator>=(cstring const& a, std::string const& b) noexcept { return compare(a,b) >= 0;}

/** Compares std::string and cstring.
@relates cstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(std::string const& a, cstring const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); } 
/** @relates cstring */ inline bool operator==(std::string const& a, cstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates cstring */ inline bool operator!=(std::string const& a, cstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates cstring */ inline bool operator< (std::string const& a, cstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates cstring */ inline bool operator<=(std::string const& a, cstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates cstring */ inline bool operator> (std::string const& a, cstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates cstring */ inline bool operator>=(std::string const& a, cstring const& b) noexcept { return compare(a,b) >= 0;}

/** Compares cstring and const char*.
@relates cstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(cstring const& a, const char* b) noexcept
    { return compare(a.data(), a.size(), b, b ? strlen(b) : 0); } 
/** @relates cstring */ inline bool operator==(cstring const& a, const char* b) noexcept { return compare(a,b) == 0;}
/** @relates cstring */ inline bool operator!=(cstring const& a, const char* b) noexcept { return compare(a,b) != 0;}
/** @relates cstring */ inline bool operator< (cstring const& a, const char* b) noexcept { return compare(a,b) <  0;}
/** @relates cstring */ inline bool operator<=(cstring const& a, const char* b) noexcept { return compare(a,b) <= 0;}
/** @relates cstring */ inline bool operator> (cstring const& a, const char* b) noexcept { return compare(a,b) >  0;}
/** @relates cstring */ inline bool operator>=(cstring const& a, const char* b) noexcept { return compare(a,b) >= 0;}

/** Compares const char* and cstring.
@relates cstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(const char* a, cstring const& b) noexcept
    { return compare(a, a ? strlen(a) : 0, b.data(), b.size()); } 
/** @relates cstring */ inline bool operator==(const char* a, cstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates cstring */ inline bool operator!=(const char* a, cstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates cstring */ inline bool operator< (const char* a, cstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates cstring */ inline bool operator<=(const char* a, cstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates cstring */ inline bool operator> (const char* a, cstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates cstring */ inline bool operator>=(const char* a, cstring const& b) noexcept { return compare(a,b) >= 0;}

/** Inserts cstring into std::ostream. @relates cstring */
std::ostream& operator<<(std::ostream& s, cstring const& x); 
    
}
