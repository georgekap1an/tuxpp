/** @file carray.hpp
@c carray class and related functions.
@ingroup buffers */
#pragma once
#include <iosfwd>
#include "tux/buffer.hpp"
#include "tux/util.hpp"

namespace tux
{
/** Models a "CARRAY" typed buffer.
Can be used to represent non null-terminated string.
Can also be used as an opaque byte array.
@ingroup buffers */    
class carray
{
public:
    carray() noexcept = default; /**< Default construct. No allocation is performed. */
    explicit carray(std::string const& x); /**< Construct from std::string. */
    explicit carray(const char* x); /**< Construct from const char*. */
    carray(carray const& x); /**< Copy construct. */
    carray(carray&& x) noexcept = default; /**< Move construct. */
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    carray(class buffer&& x);
    
    carray& operator=(std::string const& x); /**< Assign from string. */
    carray& operator=(const char* x); /**< Assign from const char*. */
    carray& operator=(carray const& x); /**< Copy assign. */
    carray& operator=(carray&& x) = default; /**< Move assign. */
    /** Assign from buffer.
    @sa carray(buffer&&)*/
    carray& operator=(class buffer&& x);
    
    carray& operator+=(std::string const& x); /**< Appends std::string. */
    carray& operator+=(const char*  x); /**< Appends const char*. */
    carray& operator+=(carray const& x); /**< Appends cstring. */
    
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

/** Concatenates two carrays. @relates carray */
inline carray operator+(carray x, carray const& y) { return (x += y); } 

/** Compares two carrays.
@relates carray
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(carray const& a, carray const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates carray */ inline bool operator==(carray const& a, carray const& b) noexcept { return compare(a,b) == 0;} 
/** @relates carray */ inline bool operator!=(carray const& a, carray const& b) noexcept { return compare(a,b) != 0;} 
/** @relates carray */ inline bool operator< (carray const& a, carray const& b) noexcept { return compare(a,b) <  0;} 
/** @relates carray */ inline bool operator<=(carray const& a, carray const& b) noexcept { return compare(a,b) <= 0;} 
/** @relates carray */ inline bool operator> (carray const& a, carray const& b) noexcept { return compare(a,b) >  0;} 
/** @relates carray */ inline bool operator>=(carray const& a, carray const& b) noexcept { return compare(a,b) >= 0;} 

/** Compares carray and std::string.
@relates carray
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(carray const& a, std::string const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates carray */ inline bool operator==(carray const& a, std::string const& b) noexcept { return compare(a,b) == 0;} 
/** @relates carray */ inline bool operator!=(carray const& a, std::string const& b) noexcept { return compare(a,b) != 0;} 
/** @relates carray */ inline bool operator< (carray const& a, std::string const& b) noexcept { return compare(a,b) <  0;} 
/** @relates carray */ inline bool operator<=(carray const& a, std::string const& b) noexcept { return compare(a,b) <= 0;} 
/** @relates carray */ inline bool operator> (carray const& a, std::string const& b) noexcept { return compare(a,b) >  0;} 
/** @relates carray */ inline bool operator>=(carray const& a, std::string const& b) noexcept { return compare(a,b) >= 0;} 

/** Compares std::string and carray.
@relates carray
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(std::string const& a, carray const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates carray */ inline bool operator==(std::string const& a, carray const& b) noexcept { return compare(a,b) == 0;} 
/** @relates carray */ inline bool operator!=(std::string const& a, carray const& b) noexcept { return compare(a,b) != 0;} 
/** @relates carray */ inline bool operator< (std::string const& a, carray const& b) noexcept { return compare(a,b) <  0;} 
/** @relates carray */ inline bool operator<=(std::string const& a, carray const& b) noexcept { return compare(a,b) <= 0;} 
/** @relates carray */ inline bool operator> (std::string const& a, carray const& b) noexcept { return compare(a,b) >  0;} 
/** @relates carray */ inline bool operator>=(std::string const& a, carray const& b) noexcept { return compare(a,b) >= 0;} 

/** Compares carray and const char*.
@relates carray
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(carray const& a, const char* b) noexcept
    { return compare(a.data(), a.size(), b, b ? strlen(b) : 0); }
/** @relates carray */ inline bool operator==(carray const& a, const char* b) noexcept { return compare(a,b) == 0;} 
/** @relates carray */ inline bool operator!=(carray const& a, const char* b) noexcept { return compare(a,b) != 0;} 
/** @relates carray */ inline bool operator< (carray const& a, const char* b) noexcept { return compare(a,b) <  0;} 
/** @relates carray */ inline bool operator<=(carray const& a, const char* b) noexcept { return compare(a,b) <= 0;} 
/** @relates carray */ inline bool operator> (carray const& a, const char* b) noexcept { return compare(a,b) >  0;} 
/** @relates carray */ inline bool operator>=(carray const& a, const char* b) noexcept { return compare(a,b) >= 0;} 

/** Compares const char* and carray.
@relates carray
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
inline int compare(const char* a, carray const& b) noexcept
    { return compare(a, a ? strlen(a) : 0, b.data(), b.size()); }
/** @relates carray */ inline bool operator==(const char* a, carray const& b) noexcept { return compare(a,b) == 0;} 
/** @relates carray */ inline bool operator!=(const char* a, carray const& b) noexcept { return compare(a,b) != 0;} 
/** @relates carray */ inline bool operator< (const char* a, carray const& b) noexcept { return compare(a,b) <  0;} 
/** @relates carray */ inline bool operator<=(const char* a, carray const& b) noexcept { return compare(a,b) <= 0;} 
/** @relates carray */ inline bool operator> (const char* a, carray const& b) noexcept { return compare(a,b) >  0;} 
/** @relates carray */ inline bool operator>=(const char* a, carray const& b) noexcept { return compare(a,b) >= 0;} 

/** Inserts carray into std::ostream. @relates carray */
std::ostream& operator<<(std::ostream& s, carray& x); 
    
}
