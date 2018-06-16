/** @file mbstring.hpp
@c mbstring class and related functions.
@ingroup buffers*/
#pragma once
#include <iosfwd>
#include "tux/buffer.hpp"
#include "tux/util.hpp"

namespace tux
{
/** Models an "MBSTRING" typed buffer (multi-byte string).
@ingroup buffers*/     
class mbstring
{
public:
    mbstring() noexcept = default; /**< Default construct. No allocation is performed. */
    explicit mbstring(std::string const& x); /**< Construct from std::string. */
    explicit mbstring(const char* x); /**< Construct from const char*. */
    mbstring(mbstring const& x); /**< Copy construct. */
    mbstring(mbstring&& x) noexcept = default; /**< Move construct. */
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    mbstring(class buffer&& x);
    
    mbstring& operator=(std::string const& x); /**< Assign from string. */
    mbstring& operator=(const char* x); /**< Assign from const char*. */
    mbstring& operator=(mbstring const& x); /**< Copy assign. */
    mbstring& operator=(mbstring&& x) = default; /**< Move assign. */
    /** Assign from buffer.
    @sa mbstring(buffer&&)*/
    mbstring& operator=(class buffer&& x);
    
    mbstring& operator+=(std::string const& x);  /**< Appends std::string. */
    mbstring& operator+=(const char*  x); /**< Appends const char*. */
    mbstring& operator+=(mbstring const& x); /**< Appends cstring. */
    
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
    
    std::string get_encoding_name() const; /**< Returns the encoding name [@c tpgetmbenc]. */
    void set_encoding_name(std::string const& encoding); /**< Sets the encoding name [@c tpsetmbenc]. */
    void clear_encoding_name(); /**< Clear the encoding name [@c tpsetmbenc(RM_ENC)]. */
    /** Convert to another encoding [@c tpconvmb].
    Unlike get_encoding_name(), set_encoding_name(), and clear_encoding_name()
    (which simply access a private field recording the encoding name),
    this function actually modifies the string value, encoding it as requested.
    @param encoding encoding name
    @param flags unused by Tuxedo-provided conversion routines, but can be
    used by user-provided conversion routines. */
    void convert_encoding(std::string const& encoding, long flags = TPNOFLAGS); 
       
private:
    
    class buffer buffer_;
    
    void set(const char* x, long len);
    void append(const char* x, long len);
};

/** Concatenates two mbstrings. @relates mbstring */
inline mbstring operator+(mbstring x, mbstring const& y) { return (x += y); } 

/** Compares two mbstrings.
@relates mbstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b
@note This comparison is only valid if the encodings of both
strings are the same.  For more sophisticated processing,
consider using a dedicated library such as boost locale. */
inline int compare(mbstring const& a, mbstring const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates mbstring */ inline bool operator==(mbstring const& a, mbstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates mbstring */ inline bool operator!=(mbstring const& a, mbstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates mbstring */ inline bool operator< (mbstring const& a, mbstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates mbstring */ inline bool operator<=(mbstring const& a, mbstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates mbstring */ inline bool operator> (mbstring const& a, mbstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates mbstring */ inline bool operator>=(mbstring const& a, mbstring const& b) noexcept { return compare(a,b) >= 0;}

/** Compares mbstring and std::string.
@relates mbstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b
@note This comparison is only valid if the encodings of both
strings are the same.  For more sophisticated processing,
consider using a dedicated library such as boost locale. */
inline int compare(mbstring const& a, std::string const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates mbstring */ inline bool operator==(mbstring const& a, std::string const& b) noexcept { return compare(a,b) == 0;}
/** @relates mbstring */ inline bool operator!=(mbstring const& a, std::string const& b) noexcept { return compare(a,b) != 0;}
/** @relates mbstring */ inline bool operator< (mbstring const& a, std::string const& b) noexcept { return compare(a,b) <  0;}
/** @relates mbstring */ inline bool operator<=(mbstring const& a, std::string const& b) noexcept { return compare(a,b) <= 0;}
/** @relates mbstring */ inline bool operator> (mbstring const& a, std::string const& b) noexcept { return compare(a,b) >  0;}
/** @relates mbstring */ inline bool operator>=(mbstring const& a, std::string const& b) noexcept { return compare(a,b) >= 0;}

/** Compares std::string and mbstring.
@relates mbstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b
@note This comparison is only valid if the encodings of both
strings are the same.  For more sophisticated processing,
consider using a dedicated library such as boost locale. */
inline int compare(std::string const& a, mbstring const& b) noexcept
    { return compare(a.data(), a.size(), b.data(), b.size()); }
/** @relates mbstring */ inline bool operator==(std::string const& a, mbstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates mbstring */ inline bool operator!=(std::string const& a, mbstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates mbstring */ inline bool operator< (std::string const& a, mbstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates mbstring */ inline bool operator<=(std::string const& a, mbstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates mbstring */ inline bool operator> (std::string const& a, mbstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates mbstring */ inline bool operator>=(std::string const& a, mbstring const& b) noexcept { return compare(a,b) >= 0;}

/** Compares mbstring and const char*.
@relates mbstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b
@note This comparison is only valid if the encodings of both
strings are the same.  For more sophisticated processing,
consider using a dedicated library such as boost locale. */
inline int compare(mbstring const& a, const char* b) noexcept
    { return compare(a.data(), a.size(), b, b ? strlen(b) : 0); }
/** @relates mbstring */ inline bool operator==(mbstring const& a, const char* b) noexcept { return compare(a,b) == 0;}
/** @relates mbstring */ inline bool operator!=(mbstring const& a, const char* b) noexcept { return compare(a,b) != 0;}
/** @relates mbstring */ inline bool operator< (mbstring const& a, const char* b) noexcept { return compare(a,b) <  0;}
/** @relates mbstring */ inline bool operator<=(mbstring const& a, const char* b) noexcept { return compare(a,b) <= 0;}
/** @relates mbstring */ inline bool operator> (mbstring const& a, const char* b) noexcept { return compare(a,b) >  0;}
/** @relates mbstring */ inline bool operator>=(mbstring const& a, const char* b) noexcept { return compare(a,b) >= 0;}

/** Compares const char* and mbstring.
@relates mbstring
@returns -1 if a < b, 0 if a == b, and 1 if a > b
@note This comparison is only valid if the encodings of both
strings are the same.  For more sophisticated processing,
consider using a dedicated library such as boost locale. */
inline int compare(const char* a, mbstring const& b) noexcept
    { return compare(a, a ? strlen(a) : 0, b.data(), b.size()); }
/** @relates mbstring */ inline bool operator==(const char* a, mbstring const& b) noexcept { return compare(a,b) == 0;}
/** @relates mbstring */ inline bool operator!=(const char* a, mbstring const& b) noexcept { return compare(a,b) != 0;}
/** @relates mbstring */ inline bool operator< (const char* a, mbstring const& b) noexcept { return compare(a,b) <  0;}
/** @relates mbstring */ inline bool operator<=(const char* a, mbstring const& b) noexcept { return compare(a,b) <= 0;}
/** @relates mbstring */ inline bool operator> (const char* a, mbstring const& b) noexcept { return compare(a,b) >  0;}
/** @relates mbstring */ inline bool operator>=(const char* a, mbstring const& b) noexcept { return compare(a,b) >= 0;}

/** Inserts carray into std::ostream. @relates mbstring */
std::ostream& operator<<(std::ostream& s, mbstring const& x); 
    
}
