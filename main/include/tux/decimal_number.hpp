/** @file decimal_number.hpp
@c decimal_number class and related functions. @ingroup utils */
#pragma once

#include <string>
#include <iosfwd>
#include "decimal.h"

namespace tux
{

/** Models a decimal number [@c dec_t]. @ingroup utils */
class decimal_number
{
public:
    //----------------- constructors --------------------
    decimal_number(); /**< Default construct. Initializes to 0.00 [@c deccvdbl]. */
    explicit decimal_number(std::string const& x); /**< Constructs from string [@c deccvasc]. */
    explicit decimal_number(int x); /**< Constructs from int [@c deccvint]. */
    explicit decimal_number(long x); /**< Constructs from long [@c deccvlong]. */
    explicit decimal_number(double x); /**< Constructs from double [@c deccvdbl]. */
    explicit decimal_number(float x); /**< Constructs from float [@c deccvflt]. */
    decimal_number(decimal_number const& x) = default; /**< Copy construct. */
    decimal_number(dec_t const& x); /**< Constructs from @c dec_t. */

    //----------------- assignment ---------------------
    decimal_number& operator=(std::string const& x); /**< Assigns from a string [@c deccvasc]. */
    decimal_number& operator=(int x); /**< Assigns from an int [@c deccvint]. */
    decimal_number& operator=(long x); /**< Assigns from a long [@c deccvlong]. */
    decimal_number& operator=(double x); /**< Assigns from a double [@c deccvdbl]. */
    decimal_number& operator=(float x); /**< Assigns from a float [@c deccvflt]. */
    decimal_number& operator=(decimal_number const& x) = default; /**< Copy assign. */ 
    decimal_number& operator=(dec_t const& x); /**< Assigns from a @c dec_t. */

    // ----------------------- conversions -------------------------------
    std::string to_string(size_t decimal_places = 2, size_t max_size = 18) const; /** Formats a string [@c dectoasc]. */
    int to_int() const; /**< Converts to an int [@c dectoint]. */
    long to_long() const; /**< Converts to a long [@c dectolong]. */
    double to_double() const; /**< Converts to a double [@c dectodbl]. */
    float to_float() const; /**< Converts to a float [@c dectoflt]. */

    // ---------------- compact storage --------------------------------
    std::string store(std::string::size_type size = 16) const; /**< Store in a compact representation [@c stdecimal]. */
    void load(std::string const& x); /**< Loads from a compact representation [@c lddecimal]. */
    
    // ---------------- access to dec_t ---------------------------------
    dec_t& as_dec_t() noexcept; /**< Accesses the underlying @c dec_t. */
    dec_t const& as_dec_t() const noexcept; /**< Accesses the underlying @c dec_t. */

private:
    dec_t decimal_;
};

//---------------- comparisons ------------------------
/** Compares two decimal_numbers [@c deccmp].
@relates decimal_number
@returns -1 if a < b, 0 if a == b, or 1 if a > b */
int compare(decimal_number const& a, decimal_number const& b); 
/** @relates decimal_number */ inline bool operator==(decimal_number const& a, decimal_number const& b) { return compare(a,b) == 0; }
/** @relates decimal_number */ inline bool operator!=(decimal_number const& a, decimal_number const& b) { return compare(a,b) != 0; }
/** @relates decimal_number */ inline bool operator< (decimal_number const& a, decimal_number const& b) { return compare(a,b) <  0; }
/** @relates decimal_number */ inline bool operator<=(decimal_number const& a, decimal_number const& b) { return compare(a,b) <= 0; }
/** @relates decimal_number */ inline bool operator> (decimal_number const& a, decimal_number const& b) { return compare(a,b) >  0; }
/** @relates decimal_number */ inline bool operator>=(decimal_number const& a, decimal_number const& b) { return compare(a,b) >= 0; }

// ---------------- arithmetic -----------------
/** Add two decimal_numbers [@c decadd]. @relates decimal_number */
decimal_number operator+(decimal_number const& a, decimal_number const& b);
/** Subtract two decimal_numbers [@c decsub]. @relates decimal_number */
decimal_number operator-(decimal_number const& a, decimal_number const& b);
/** Multiply two decimal_numbers [@c decmul]. @relates decimal_number */
decimal_number operator*(decimal_number const& a, decimal_number const& b);
/** Divide two decimal_numbers [@c decdiv]. @relates decimal_number */
decimal_number operator/(decimal_number const& a, decimal_number const& b); 

// ---------------- iostreams ---------------------
/** Insert into stream using decimal_number::to_string(). @relates decimal_number */
std::ostream& operator<<(std::ostream& s, decimal_number const& x); 


}
