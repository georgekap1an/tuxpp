/** @file record.hpp
@c record class
@since Tuxedo 12.2.2
@ingroup buffers*/
#pragma once

#if TUXEDO_VERSION >= 1222

#include <string>
#include <stdexcept>
#include "fml32.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"
#include "tux/decimal_number.hpp"

#include <iostream>

namespace tux
{
/** Models a "RECORD" typed buffer (COBOL copybook).
@since Tuxedo 12.2.2
@note This class is only supported for Tuxedo 12.2.2
or later.  The Tuxedo reference docs state RECORD
features are available in 12.1.3, and I do see cpy2record
and references to the RECORD buffer type.
However, I don't see Rinit, Rget, etc.
seriously hampering its utility.
@ingroup buffers*/    
class record
{
public:
    record() noexcept = default; /**< Default construct. No allocation is performed. */
    record(record const& x); /**< Copy construct. */
    record& operator=(record const& x); /**< Copy assign. */
    record(record&& x) noexcept = default; /**< Move construct. */
    record& operator=(record&& x) noexcept = default; /**< Move assign. */
    ~record() noexcept = default; /**< Destruct. */
    
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    record(class buffer&& x);
    /** Assign from buffer.
    @sa record(buffer&&)*/
    record& operator=(class buffer&& x);
    
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    class buffer& buffer() noexcept; /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept;
    
    operator bool() const noexcept; /**< Test for null state. */
    
    std::string type() const; /**< Returns record type [@c RECORD::rname]. */
    
    /** Initialize [@c Rinit].
    @param record_type copybook name
    @param data raw data
    @param len length of raw data
    @flags TPENC_EBCDIC, TPENC_ASCII, TPENC_BIG_ENDIAN, TPENC_LITTLE_ENDIAN, TPENC_MAINFRAME_FLOAT */
    void init(std::string const& record_type, const char* data = nullptr, int len = 0, int flags = TPNOFLAGS);
    
    /** Construct with initial type and optional data.
    @sa init() */
    explicit record(std::string const& record_type, const char* data = nullptr, int len = 0, int flags = TPNOFLAGS);
    
    // entire record
    std::string get_data(long flags = TPNOFLAGS) const; /**< Returns raw data for entire record [@c Rget]. */
    void set_data(const char* data, int len, int flags = TPNOFLAGS); /**< Sets raw data for entire record [@c Rset]. */
    
    // short
    short get_short(std::string const& name); /**< Returns short value of field @c name [@c Rget]. */
    void set(std::string const& name, short x); /**< Sets field @c name to short value [@c Rset]. */
    
    // long (int32_t)
    long get_long(std::string const& name); /**< Returns long value of field @c name [@c Rget]. */
    void set(std::string const& name, long x); /**< Sets field @c name to long value [@c Rset]. */
    
    // char
    char get_char(std::string const& name); /**< Returns char value of field @c name [@c Rget]. */
    void set(std::string const& name, char x); /**< Sets field @c name to char value [@c Rset]. */
    
    // float
    float get_float(std::string const& name); /**< Returns float value of field @c name [@c Rget]. */
    void set(std::string const& name, float x); /**< Sets field @c name to float value [@c Rset]. */
    
    // double
    double get_double(std::string const& name); /**< Returns double value of field @c name [@c Rget]. */
    void set(std::string const& name, double x); /**< Sets field @c name to double value [@c Rset]. */
    
    // string or carray
    /** Returns string value of field @c name [@c Rget].
    @param name field name
    @param trim_spaces if set to true, trailing spaces will be trimmed from the result
    @param binary if set to false, the result will be trimmed to the null character
    @param size_hint hint to help pre-allocate a buffer of the correct size */
    std::string get_string(std::string const& name, bool trim_spaces = true, bool binary = false, int size_hint = 20);
    /** Sets field @c name to string value [@c Rget].
    @param name field name
    @param binary if true, value will be inserted as C_CARRAY; if false, value will be inserted as C_STRING */
    void set(std::string const& name, std::string const& x, bool binary = false);
    
    // int
    int get_int(std::string const& name); /**< Returns int value of field @c name [@c Rget]. */
    void set(std::string const& name, int x); /**< Sets field @c name to int value [@c Rset]. */
    
    // decimal
    decimal_number get_decimal(std::string const& name); /**< Returns decimal value of field @c name [@c Rget]. */
    void set(std::string const& name, decimal_number const& x); /**< Sets field @c name to decimal value [@c Rset]. */
    
    // unsigned int
    unsigned int get_unsigned_int(std::string const& name); /**< Returns unsigned int value of field @c name [@c Rget]. */
    void set(std::string const& name, unsigned int x); /**< Sets field @c name to unsigned int value [@c Rset]. */
    
    // unsigned long (uint32_t)
    unsigned long get_unsigned_long(std::string const& name); /**< Returns unsigned long value of field @c name [@c Rget]. */
    void set(std::string const& name, unsigned long x); /**< Sets field @c name to unsigned long value [@c Rset]. */
    
    // long long (int64_t)
    long long get_long_long(std::string const& name); /**< Returns long long value of field @c name [@c Rget]. */
    void set(std::string const& name, long long x); /**< Sets field @c name to long long value [@c Rset]. */
    
    // unsigned short
    unsigned short get_unsigned_short(std::string const& name); /**< Returns short value of field @c name [@c Rget]. */
    void set(std::string const& name, unsigned short x); /**< Sets field @c name to short value [@c Rset]. */
    
    RECORD* as_record() noexcept; /**< Access underlying buffer data. */
    const RECORD* as_record() const noexcept; /**< Access underlying buffer data. */
    
private:
    void set_field(std::string const& name, const char* data, int len, int datatype);
    void get_field(std::string const& name, char* data, int& len, int datatype) const;
    bool try_get_field(std::string const& name, char* data, int& len, int datatype);

    void alloc(std::string const& record_type);
    
    class buffer buffer_;
};

}

#endif
