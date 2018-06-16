/** @file xml.hpp
@c xml class and related functions.
@ingroup buffers*/
#pragma once
#include <iosfwd>
#include "tux/buffer.hpp"

namespace tux
{

/** Models an "XML" typed buffer.
@note Essentially tux::xml is just a tux::carray
but with a buffer type of "XML". Users are
encouraged to use their preferred xml
processing library for manipulating
the data.
@ingroup buffers*/ 
class xml
{
public:
    xml() noexcept = default; /**< Default construct. No allocation is performed. */
    explicit xml(std::string const& x); /**< Construct from std::string. */
    explicit xml(const char* x); /**< Construct from const char*. */
    xml(xml const& x); /**< Copy construct. */
    xml(xml&& x) noexcept = default; /**< Move construct. */
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    xml(class buffer&& x); 
    
    xml& operator=(std::string const& x); /**< Assign from string. */
    xml& operator=(const char* x); /**< Assign from const char*. */
    xml& operator=(xml const& x); /**< Copy assign. */
    xml& operator=(xml&& x) = default; /**< Move assign. */
    /** Assign from buffer.
    @sa xml(buffer&&)*/
    xml& operator=(class buffer&& x);
    
    xml& operator+=(std::string const& x); /**< Appends std::string. */
    xml& operator+=(const char*  x); /**< Appends const char*. */
    xml& operator+=(xml const& x); /**< Appends cstring. */
    
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    class buffer& buffer() noexcept; /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept;

    char* data() noexcept; /**< Access underlying buffer's underlying data. */
    const char* data() const noexcept; /**< Access underlying buffer's underlying data. */
    long size() const noexcept; /**< Returns string size. */
    std::string to_string() const; /**< Converts to std::string. */
    
    explicit operator bool() const noexcept; /**< Test for null state. */
    
    void reserve(long new_data_size); /**< Reserves @c new_data_size capacity. */
private:
    
    class buffer buffer_;
    
    void set(const char* x, long len);
    void append(const char* x, long len);
};

/** Inserts xml into std::ostream. @relates xml */
std::ostream& operator<<(std::ostream& s, xml& x);
    
}
