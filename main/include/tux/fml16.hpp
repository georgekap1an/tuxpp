/** @file fml16.hpp
@c fml16 class and related functions.
@ingroup buffers*/
#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include "fml.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"



namespace tux
{

/** Models an "FML" typed buffer.  FML
is a self-describing data structure
conceptually similar to JSON or
XML.
@ingroup buffers*/       
class fml16
{
public:
    /** Aggregates a field id and field occurrence (index); used
    when traversing all fields in a buffer. */
    struct field_info
    {
        FLDID id = FIRSTFLDID;
        FLDOCC oc = 0;
    };
    
    /** Models an error originating from the FML library. */
    class error : public std::runtime_error
    {
    public:
        error(); /**< Default construct. */  
        error(error const& x) = default; /**< Copy construct. */
        error& operator=(error const& x) = default; /**< Copy assign. */
        error(error&& x) = default; /**< Move construct. */
        error& operator=(error&& x) = default; /**< Move assign. */
        virtual ~error() = default; /**< Destruct. */
        
        /** Construct from a code and an optional string.
        @param code code obtained from @c Ferror
        @param what string to be stored as the "what" string */
        error(int code, std::string const& what = "");
        
        int code() const noexcept; /**< Returns the error code. */
        const char* str() const noexcept; /**< Returns the string version of the error code [@c Fstrerror]. */
     
    private:
        int code_ = 0;
    };
    
    /** Returns the last FML error.
    @param function_name function name or other contextual information
    to be included in the "what" string. */
    static error last_error(std::string const& function_name = "");
    
    /** Returns the field type name given the integer type code.
    @param type e.g. FLD_SHORT, FLD_LONG
    @returns a string representing the type e.g. "short", "long" */
    static std::string field_type_name_from_type(int type);
    static FLDID field_id(std::string const& name); /**< Returns the field id [@c Fldid]. */
    static std::string field_name(FLDID id); /**< Returns the field name [@c Fname]. */
    static FLDOCC field_number(FLDID id) noexcept; /**< Returns the field number [@c Fldno]. */
    static int field_type(FLDID id) noexcept; /**< Returns the field type [@c Fldtype]. */
    static std::string field_type_name(FLDID id); /**< Returns the field type name [@c Ftype]. */
    static FLDID field_id(int type, FLDID number); /**< Returns the field id [@c Fmkfldid]. */
    /** Returns the size in bytes of a buffer with field_count fields and FLD_LEN bytes of data [@c Fneeded]. */
    static long bytes_needed(FLDOCC field_count, FLDLEN space_for_values); 
    static void unload_id_name_table() noexcept; /**< Unload field id to field name mapping [@c Fidnm_unload]. */
    static void unload_name_id_table() noexcept; /**< Unload field name to field id mapping [@c Fnmid_unload]. */
    
    // ---------------constructors and assignment-------------------------------------
    fml16() noexcept = default; /**< Default construct (no allocation). */
    fml16(fml16 const& x); /**< Copy construct [@c Fcpy, @c Findex]. */
    fml16& operator=(fml16 const& x); /**< Copy assign [@c Fcpy, @c Findex]. */
    fml16(fml16&& x) noexcept = default; /**< Move construct. */
    fml16& operator=(fml16&& x) noexcept = default; /**< Move assign. */
    ~fml16() noexcept = default; /**< Destruct. */
    
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    fml16(class buffer&& x);
    /** Assign from buffer.
    @sa fml16(buffer&&)*/
    fml16& operator=(class buffer&& x);
    
    /** Construct with initial reserved capacity.
    @sa reserve(long) */
    fml16(long capacity);
    /** Construct with initial reserved capacity [@c Fneeded].
    @sa reserve(FLDOCC, FLDLEN), bytes_needed() */
    fml16(FLDOCC field_count, FLDLEN space_for_values); 
    
    // -------------------------buffer access-------------------------------------------
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    class buffer& buffer() noexcept;  /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept;
    /** Reserve capacity.
    @param size size in bytes to reserve
    @sa fml16(long)
    */
    void reserve(long size);
    /** Reserve capacity.
    @param field_count max number of fields
    @param space_for_values total size in bytes of all data to be stored in all fields
    @sa fml16(FLDOCC, FLDLEN), bytes_needed()*/
    void reserve(FLDOCC field_count, FLDLEN space_for_values); 
    operator bool() const noexcept; /**< Test for null state. */
    /** Test whether the buffer is in proper fielded format [@c Fielded].
    Should always be true unless append has been called without a subsequent
    call to index. */
    bool is_fielded() const noexcept;
    
    FBFR* as_fbfr() noexcept; /**< Access pointer to underlying FBFR. */
    const FBFR* as_fbfr() const noexcept; /**< Access pointer to underlying FBFR. */
    
    /** Calculates checksum of buffer [@c Fchksum].
    @pre The internal buffer cannot be null. */
    long checksum() const;
    
    // ---------------------------file i/o-------------------------------------------
    /** Print the buffer to stdout in a human readable format [@c Fprint].
    @pre The internal buffer cannot be null. */
    void print_to_stdout() const;
    /** Load a buffer (serialized in the @c Fprint format) from a @c FILE* [@c Fextread]. */
    void extread(FILE* input, long size_hint = 0);
    /** Write the buffer to a @c FILE* [@c Fwrite].
    @pre The internal buffer cannot be null. */
    void write(FILE* output) const;
    /** Reads a buffer (deserializes) from a @c FILE* [@c Fread]. */
    void read(FILE* input, long size_hint = 0);
    
    // ---------------------------indexing-----------------------------------------------
    /**  Index (or re-index) the data using the specified interval [@c Findex].
    One scenario which requires this is the use of the append() methods.
    @param interval the index interval (0 tells Tuxedo to use the default) */
    void index(FLDOCC interval = 0); 
    /**  Drop the current index [@c Funindex].
    @returns the index element count */
    FLDOCC unindex();
    /** Restore a previously dropped index [@c Frstrindex].
    @param index_element_count the index element count returned from unindex */
    void restore_index(FLDOCC index_element_count); 
    
    // --------------------------bytes used----------------------------------------------
    long index_size() const; /**< Returns the number of bytes used by the index  [@c Fidxused]. */
    long size() const; /**<  Returns the current size of the buffer [@c Fsizeof]. */
    long unused_size() const; /**<  Returns the number of bytes currently unused (extra capacity) [@c Funused]. */
    long used_size() const; /**<  Returns the number of bytes currently used [@c Fused]. */
    
    // -------------------------field counts---------------------------------------------
    FLDOCC field_count() const; /**< Returns the current field count [@c Fnum]. */
    FLDOCC count(FLDID id) const; /**<  Returns  the current count of field @c id [@c Foccur]. */
    bool has(FLDID id, FLDOCC oc = 0) const; /**< Checks for the existence of a particular field [@c Fpres]. */
    
    long field_value_size(FLDID id, FLDOCC oc = 0) const; /**< Returns the number of bytes used by the value of a particular field [@c Flen]. */
    
    // ------------------------element removal--------------------------------------------
    bool erase(FLDID id, FLDOCC oc); /**< Erases a particular field [@c Fdel]. */
    bool erase(FLDID id); /**< Erases all occurrences of a field [@c Fdelall]. */
    void erase(std::vector<FLDID> ids); /**< Erases specified fields [@c Fdelete]. */
    void erase_all_but(std::vector<FLDID> ids); /**< Erases  all fields except those specifed [@c Fproj]. */
    void clear(); /**< Erases all fields [@c Finit]. */
    
    // --------------operations on entire data structures----------------------------------
    fml16& operator +=(fml16 const& x); /**< Append another fml16 to this one [@c Fconcat]. */
    void join(fml16 const& x); /**< Join another fml16 to this one [@c Fjoin]. */
    void outer_join(fml16 const& x); /**< Outer join another fml16 to this one [@c Fojoin]. */
    fml16 project(std::vector<FLDID> ids) const; /**< Create a projection (with only the specified fields) of this fml16 [@c Fprojcpy]. */
    void update(fml16 const& x); /**< Update this fml16 based on field values in another fml16 [@c Fupdate]. */
    
    // -----------------------------add fields---------------------------------------------
    void add(FLDID id, short x); /**< Add a short [@c CFadd]. */
    void add(FLDID id, long x); /**< Add a long [@c CFadd]. */
    void add(FLDID id, int x) { add(id, (long)x); } /**< Add an int [@c CFadd]. */
    void add(FLDID id, char x); /**< Add a char [@c CFadd]. */
    void add(FLDID id, float x); /**< Add a float [@c CFadd]. */
    void add(FLDID id, double x); /**< Add a double [@c CFadd]. */
    void add(FLDID id, std::string const& x); /**< Add a string [@c CFadd]. */

    
    // --------------------------------append fields----------------------------------------------
    void append(FLDID id, short x);  /**< Append a short [@c Fappend]. */
    void append(FLDID id, long x); /**< Append a long [@c Fappend]. */
    void append(FLDID id, int x) { append(id, (long)x); } /**< Append an int [@c Fappend]. */
    void append(FLDID id, char x); /**< Append a char [@c Fappend]. */
    void append(FLDID id, float x); /**< Append a float [@c Fappend]. */
    void append(FLDID id, double x); /**< Append a double [@c Fappend]. */
    void append(FLDID id, std::string const& x); /**< Append a string [@c Fappend]. */

    
    // ------------------------------set fields--------------------------------------------
    void set(FLDID id, short x, FLDOCC oc = 0);  /**< Set a short [@c CFchg]. */
    void set(FLDID id, long x, FLDOCC oc = 0); /**< Set a long [@c CFchg]. */
    void set(FLDID id, int x, FLDOCC oc = 0) { set(id, (long)x, oc); } /**< Set an int [@c CFchg]. */
    void set(FLDID id, char x, FLDOCC oc = 0); /**< Set a char [@c CFchg]. */
    void set(FLDID id, float x, FLDOCC oc = 0); /**< Set a float [@c CFchg]. */
    void set(FLDID id, double x, FLDOCC oc = 0); /**< Set a double [@c CFchg]. */
    void set(FLDID id, std::string const& x, FLDOCC oc = 0); /**< Set a string [@c CFchg]. */

    
    // -------------------------------get fields--------------------------------------------
    short get_short(FLDID id, FLDOCC oc = 0) const; /**< Get a short [@c CFget]. */
    long get_long(FLDID id, FLDOCC oc = 0) const; /**< Get a long [@c CFget]. */
    char get_char(FLDID id, FLDOCC oc = 0) const; /**< Get a char [@c CFget]. */
    float get_float(FLDID id, FLDOCC oc = 0) const; /**< Get a float [@c CFget]. */
    double get_double(FLDID id, FLDOCC oc = 0) const; /**< Get a double [@c CFget]. */
    std::string get_string(FLDID id, FLDOCC oc = 0) const; /**< Get a string [@c Ffind, @c CFget]. */
    
    // ------------------------------find matching occurrence-------------------------------
    FLDOCC find(FLDID id, short x) const; /**< Find a short [@c CFfindocc]. */
    FLDOCC find(FLDID id, long x) const; /**< Find a long [@c CFfindocc]. */
    FLDOCC find(FLDID id, int x) const { return find(id, (long)x); } /**< Find an int [@c CFfindocc]. */
    FLDOCC find(FLDID id, char x) const; /**< Find a char [@c CFfindocc]. */
    FLDOCC find(FLDID id, float x) const; /**< Find a float [@c CFfindocc]. */
    FLDOCC find(FLDID id, double x) const; /**< Find a double [@c CFfindocc]. */
    FLDOCC find(FLDID id, std::string const& x) const; /**< Find a string [@c CFfindocc]. */
    FLDOCC find_match(FLDID id, std::string const& x) const; /**< Find a string matching the given regular expression [@c Ffindocc]. */

    
    // ---------------------------------------get the last occurrence----------------------------------------
    std::pair<FLDOCC,short> get_last_short(FLDID id) const; /**< Get the last occurrence of a field [@c Fgetlast]. */
    std::pair<FLDOCC,long> get_last_long(FLDID id) const; /**< Get the last occurrence of a field [@c Fgetlast]. */
    std::pair<FLDOCC,char> get_last_char(FLDID id) const; /**< Get the last occurrence of a field [@c Fgetlast]. */
    std::pair<FLDOCC,float> get_last_float(FLDID id) const; /**< Get the last occurrence of a field [@c Fgetlast]. */
    std::pair<FLDOCC,double> get_last_double(FLDID id) const; /**< Get the last occurrence of a field [@c Fgetlast]. */
    std::pair<FLDOCC,std::string> get_last_string(FLDID id) const; /**< Get the last occurrence of a field [@c Ffindlast]. */
    
    // ---------------------------------iterate over all fields--------------------------------------------------
    /** Get information about the next field in the buffer.
    @param x on input, this represents the current "iterator";
    on output, it is updated with info on the next field.
    @returns true if there was another field, false if there are
    no more fields in the buffer. */
    bool next_field(field_info& x) const;
    
    // -------------------------boolean expression---------------------------------------
    /** Models a boolean expression (string) that can be compiled
    once and applied to multiple instance of fml16.  For example:
    @code{.cpp}
    fml16 person;
    person.set(NAME, "John Doe");
    person.set(AGE, 32);
    fml16::boolean_expression is_minor("AGE < 18");
    assert(is_minor(person) == false);
    @endcode
    @sa arithmetic_expression
    */
    class boolean_expression
    {
    public:
        boolean_expression() noexcept = default; /**< Default construct (no allocation). */
        boolean_expression(boolean_expression const&); /**< Copy construct. */
        boolean_expression& operator=(boolean_expression const&); /**< Copy assign. */
        boolean_expression(boolean_expression&&) noexcept = default; /**< Move construct. */
        boolean_expression& operator=(boolean_expression&&) noexcept = default; /**< Move assign. */
        ~boolean_expression() noexcept = default; /**< Destruct. */
        
        boolean_expression(std::string const&); /**< Construct from a string [@c Fboolco]. */
        boolean_expression& operator=(std::string const&); /**< Assign from a string [@c Fboolco]. */
        
        /** Print a string representation of the compiled expression [Fboolpr].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        bool evaluate(fml16 const&) const; /**< Evaluates the expression on an fml16 [@c Fboolev]. */
        bool operator()(fml16 const&) const; /**< Evaluates the expression on an fml16 [@c Fboolev]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
    // -------------------------arithmetic expression----------------------------------------
    
    /** Models an arithmetic expression (string) that can be compiled
    once and applied to multiple instance of fml16.  For example:
    @code{.cpp}
    fml16 loan;
    loan.set(PRINCIPAL, 20000);
    loan.set(INTEREST, 10000);
    fml16::arithmetic_expression calculate_amount_owed("PRINCIPAL + INTEREST");
    assert(calculate_amount_owed(loan) == 30000);
    @endcode
    @sa boolean_expression
    */
    class arithmetic_expression
    {
    public:
        arithmetic_expression() noexcept = default; /**< Default construct (no allocation). */
        arithmetic_expression(arithmetic_expression const&); /**< Copy construct. */
        arithmetic_expression& operator=(arithmetic_expression const&); /**< Copy assign. */
        arithmetic_expression(arithmetic_expression&&) noexcept = default; /**< Move construct. */
        arithmetic_expression& operator=(arithmetic_expression&&) noexcept = default; /**< Move assign. */
        ~arithmetic_expression() noexcept = default; /**< Destruct. */
        
        arithmetic_expression(std::string const&); /**< Construct from a string [@c Fboolco]. */
        arithmetic_expression& operator=(std::string const&); /**< Assign from a string [@c Fboolco]. */
        
        /** Print a string representation of the compiled expression [Fboolpr].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        double evaluate(fml16 const&) const; /**< Evaluates the expression on an fml16 [@c Ffloatev]. */
        double operator()(fml16 const&) const; /**< Evaluates the expression on an fml16 [@c Ffloatev]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
private:
    
    static void check_field_type(FLDID fieldid, int expected_type);
    
    void convert_and_add(FLDID fieldid, const char* value, FLDLEN len, int type);
    void add(FLDID fieldid, const char* value, FLDLEN len);
    void get_and_convert(FLDID fieldid, FLDOCC oc, char* buf, FLDLEN* len, int type) const;
    void get(FLDID fieldid, FLDOCC oc, char* buf, FLDLEN* len) const;
    
    void append(FLDID fieldid, const char* value, FLDLEN len);
    
    void convert_and_set(FLDID fieldid, const char* value, FLDLEN len, FLDOCC oc, int type);
    void set(FLDID fieldid, const char* value, FLDLEN len, FLDOCC oc);
    
    char* find_value(FLDID id, FLDOCC oc, FLDLEN* len) const;
    FLDOCC get_last(FLDID id, char* value, FLDLEN* len) const;
    FLDOCC find_occurence(FLDID id, const char* value, FLDLEN len) const;
    FLDOCC convert_and_find_occurrence(FLDID id, const char* value, FLDLEN len, int type) const;
    
    class buffer buffer_;
};

/** Compare two fml16 structures [@c Fcmp].
@relates fml16
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
int compare(fml16 const& a, fml16 const& b); 

/** @relates fml16 */ bool operator==(fml16 const& a, fml16 const& b);
/** @relates fml16 */ bool operator!=(fml16 const& a, fml16 const& b);
/** @relates fml16 */ bool operator< (fml16 const& a, fml16 const& b);
/** @relates fml16 */ bool operator> (fml16 const& a, fml16 const& b);
/** @relates fml16 */ bool operator<=(fml16 const& a, fml16 const& b);
/** @relates fml16 */ bool operator>=(fml16 const& a, fml16 const& b);

}
