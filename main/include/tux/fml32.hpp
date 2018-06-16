/** @file fml32.hpp
@c fml32 class and related functions.
@ingroup buffers*/
#pragma once
#include <string>
#include <stdexcept>
#include <vector>
#include <map>
#include <memory>
#include "fml32.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"

namespace tux
{
/** Aggregates data and encoding.
@sa fml32, packed_mbstring */       
struct unpacked_mbstring
{
    std::string data;
    std::string encoding;
};

/** When storing an mbstring in an fml32 buffer,
this enum controls whether to use the encoding
specified at the environment level (@c TPMBENC) or to
use the encoding specified at the buffer level
(via @c tpsetmbenc).
@sa packed_mbstring::packed_mbstring(std::string const&, mbpack_option),
packed_mbstring::set(std::string const&, mbpack_option) */
enum class mbpack_option : long {env = TPNOFLAGS, buffer = FBUFENC};

/** Models a mbstring, packed for
storage in an fml32 structure.  Storage
is managed via malloc/free.
*/
class packed_mbstring
{
public:
    packed_mbstring() noexcept = default; /**< Default construct (no allocation). */
    packed_mbstring(packed_mbstring const&) = default; /**< Copy construct. */
    packed_mbstring& operator=(packed_mbstring const&) = default; /**< Copy assign. */
    packed_mbstring(packed_mbstring&&) noexcept = default; /**< Move construct. */
    packed_mbstring& operator=(packed_mbstring&&) noexcept = default; /**< Move assign. */
    ~packed_mbstring() noexcept = default; /**< Destruct. */
    
    explicit operator bool() const noexcept; /**< Test for "null" state. */
    
    /** Construct a packed mbstring, using the encoding at the environment or buffer
    level [@c Fmbpack32]. @sa set(std::string const&, mbpack_option) */
    explicit packed_mbstring(std::string const& data, mbpack_option opt = mbpack_option::env);
    /** Construct a packed mbstring with an encoding specific to this
    particular string [@c Fmbpack32]. @sa set(std::string const&, std::string const&) */
    packed_mbstring(std::string const& data, std::string const& encoding);
    
    /** Set @c this, using the encoding at the environment or buffer
    level [@c Fmbpack32]. */
    void set(std::string const& data, mbpack_option opt = mbpack_option::env);
    /** Construct a packed mbstring with an encoding specific to this
    particular string [@c Fmbpack32]. */
    void set(std::string const& data, std::string const& encoding);
             
    void* data() noexcept; /**< Access the underlying buffer. */
    const void* data() const noexcept; /**< Access the underlying buffer. */
    FLDLEN32 size() const noexcept; /**< Returns the size in bytes. */
    
    void reserve(long size_in_bytes); /**< Reserve a minimum capacity. */
    
    unpacked_mbstring unpack(); /**< Unpack the mbstring [@c Fmbunpack32]. */

private:
    std::unique_ptr<void, decltype(&::free)> buffer_ = std::unique_ptr<void, decltype(&::free)>(nullptr, &::free);
    FLDLEN32 len_ = 0;
};

/** Models an "FML32" typed buffer.  FML
is a self-describing data structure
conceptually similar to JSON or
XML. fml32 support more field types, and
can store more total fields per buffer,
than fml16.
@ingroup buffers */
class fml32
{
public:
    /** Aggregates a field id and field occurrence (index). @sa next_field() */
    struct field_info
    {
        FLDID32 id = FIRSTFLDID;
        FLDOCC32 oc = 0;
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
        @param code code obtained from @c Ferror32
        @param what string to be stored as the "what" string */
        error(int code, std::string const& what = "");
        
        int code() const noexcept; /**< Returns the error code. */
        const char* str() const noexcept; /**< Returns the string version of the error code [@c Fstrerror32]. */
     
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
    static FLDID32 field_id(std::string const& name); /**< Returns the field id [@c Fldid32]. */
    static std::string field_name(FLDID32 id); /**< Returns the field name [@c Fname32]. */
    static FLDOCC32 field_number(FLDID32 id) noexcept; /**< Returns the field number [@c Fldno32]. */
    static int field_type(FLDID32 id) noexcept; /**< Returns the field type [@c Fldtype32]. */
    static std::string field_type_name(FLDID32 id); /**< Returns the field type name [@c Ftype32]. */
    static FLDID32 field_id(int type, FLDID32 number); /**< Returns the field id [@c Fmkfldid32]. */
    /** Returns the size in bytes of a buffer with field_count fields and FLD_LEN bytes of data [@c Fneeded32]. */
    static long bytes_needed(FLDOCC32 field_count, FLDLEN32 space_for_values); 
    static void unload_id_name_table() noexcept; /**< Unload field id to field name mapping [@c Fidnm_unload32]. */
    static void unload_name_id_table() noexcept; /**< Unload field name to field id mapping [@c Fnmid_unload32]. */
    
    // ---------------------constructors and assignment------------------------
    fml32() noexcept = default; /**< Default construct (no allocation). */
    fml32(fml32 const& x); /**< Copy construct [@c Fcpy32, @c Findex32]. */
    fml32& operator=(fml32 const& x); /**< Copy assign [@c Fcpy32, @c Findex32]. */
    fml32(fml32&& x) noexcept = default; /**< Move construct. */
    fml32& operator=(fml32&& x) noexcept = default; /**< Move assign. */
    ~fml32() noexcept = default; /**< Destruct. */
    
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    fml32(class buffer&& x);
    /** Assign from buffer.
    @sa fml32(buffer&&)*/
    fml32& operator=(class buffer&& x);
    
    /** Construct with initial reserved capacity.
    @sa reserve(long) */
    fml32(long capacity);
    /** Construct with initial reserved capacity [@c Fneeded32].
    @sa reserve(FLDOCC, FLDLEN), bytes_needed() */
    fml32(FLDOCC32 field_count, FLDLEN32 space_for_values);
    
    // ---------------------------------buffer access----------------------------
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    class buffer& buffer() noexcept; /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept;
    /** Reserve capacity.
    @param size size in bytes to reserve
    @sa fml32(long) */
    void reserve(long size);
    /** Reserve capacity.
    @param field_count max number of fields
    @param space_for_values total size in bytes of all data to be stored in all fields
    @sa fml32(FLDOCC, FLDLEN), bytes_needed()*/
    void reserve(FLDOCC32 field_count, FLDLEN32 space_for_values); //tpalloc/tprealloc
    operator bool() const noexcept; /**< Test for null state. */
    /** Test whether the buffer is in proper fielded format [@c Fielded32].
    Should always be true unless append has been called without a subsequent
    call to index. */
    bool is_fielded() const noexcept;
    
    FBFR32* as_fbfr() noexcept; /**< Access pointer to underlying FBFR. */
    const FBFR32* as_fbfr() const noexcept; /**< Access pointer to underlying FBFR. */
    
    /** Calculates checksum of buffer [@c Fchksum32].
    @pre The internal buffer cannot be null. */
    long checksum() const; 
    
    // ------------------------------------------file i/o-----------------------------------
    /** Print the buffer to stdout in a human readable format [@c Fprint32].
    @pre The internal buffer cannot be null. */
    void print_to_stdout() const;
    /** Load a buffer (serialized in the @c Fprint32 format) from a @c FILE* [@c Fextread32]. */
    void extread(FILE* input, long size_hint = 0); // Fextread
    /** Write the buffer to a @c FILE* [@c Fwrite32].
    @pre The internal buffer cannot be null. */
    void write(FILE* output) const;
    /** Reads a buffer (deserializes) from a @c FILE* [@c Fread32]. */
    void read(FILE* input, long size_hint = 0); 
    

    // ------------------------------------------indexing------------------------------
    /**  Index (or re-index) the data using the specified interval [@c Findex32].
    One scenario which requires this is the use of the append() methods.
    @param interval the index interval (0 tells Tuxedo to use the default) */
    void index(FLDOCC32 interval = 0); 
    /**  Drop the current index [@c Funindex32].
    @returns the index element count */
    FLDOCC32 unindex();
    /** Restore a previously dropped index [@c Frstrindex32].
    @param index_element_count the index element count returned from unindex */
    void restore_index(FLDOCC32 index_element_count); 
    
    // -----------------------------------------bytes used------------------------------
    long index_size() const; /**< Returns the number of bytes used by the index  [@c Fidxused32]. */
    long size() const; /**<  Returns the current size of the buffer [@c Fsizeof32]. */
    long unused_size() const; /**<  Returns the number of bytes currently unused (extra capacity) [@c Funused32]. */
    long used_size() const; /**<  Returns the number of bytes currently used [@c Fused32]. */
    
    // -----------------------------------field counts------------------------------------
    FLDOCC32 field_count() const; /**< Returns the current field count [@c Fnum32]. */
    FLDOCC32 count(FLDID32 id) const; /**<  Returns  the current count of field @c id [@c Foccur32]. */
    bool has(FLDID32 id, FLDOCC32 oc = 0) const; /**< Checks for the existence of a particular field [@c Fpres32]. */
    
    long field_value_size(FLDID32 id, FLDOCC32 oc = 0) const; /**< Returns the number of bytes used by the value of a particular field [@c Flen32]. */
    
    // ------------------------------------element removal-----------------------------------
    bool erase(FLDID32 id, FLDOCC32 oc); /**< Erases a particular field [@c Fdel32]. */
    bool erase(FLDID32 id); /**< Erases all occurrences of a field [@c Fdelall32]. */
    void erase(std::vector<FLDID32> ids); /**< Erases specified fields [@c Fdelete32]. */
    void erase_all_but(std::vector<FLDID32> ids); /**< Erases  all fields except those specifed [@c Fproj32]. */
    void clear(); /**< Erases all fields [@c Finit32]. */
    
    // ------------------------------operations on entire data structures-----------------------
    fml32& operator +=(fml32 const& x); /**< Append another fml32 to this one [@c Fconcat32]. */
    void join(fml32 const& x); /**< Join another fml32 to this one [@c Fjoin32]. */
    void outer_join(fml32 const& x); /**< Outer join another fml32 to this one [@c Fojoin32]. */
    fml32 project(std::vector<FLDID32> ids) const; /**< Create a projection (with only the specified fields) of this fml32 [@c Fprojcpy32]. */
    void update(fml32 const& x); /**< Update this fml32 based on field values in another fml32 [@c Fupdate32]. */
    
    // -----------------------------------mbstrings-----------------------------------------
    /** Get the encoding name at the buffer level [@c tpgetmbenc].
    @sa set_encoding_name(), clear_encoding_name(), mbpack_option */
    std::string get_encoding_name() const;
    /** Set the encoding name at the buffer level [@c tpsetmbenc].
    @sa get_encoding_name(), clear_encoding_name(), mbpack_option */
    void set_encoding_name(std::string const& encoding);
    /** Clear the encoding name at the buffer level [@c tpsetmbenc].
    @sa get_encoding_name(), set_encoding_name(), mbpack_option */
    void clear_encoding_name();
    /** Convert all mbstrings to the target encoding [@c tpconvfmb32].
    @param flags not used by Tuxedo, but could be used
    by user-supplied conversion routines. */
    void convert_mbstrings(std::string const& target_encoding, long flags = 0);
    /** Convert the mbstrings in the specified fields to the target encoding [@c tpconvfmb32].
    @param flags not used by Tuxedo, but could be used
    by user-supplied conversion routines. */
    void convert_mbstrings(std::vector<FLDID32> fields, std::string const& target_encoding, long flags = 0); // tpconvfmb32
    
    
    // -----------------------------------add fields---------------------------------------------
    void add(FLDID32 id, short x); /**< Add a short [@c CFadd32]. */
    void add(FLDID32 id, long x); /**< Add a long [@c CFadd32]. */
    void add(FLDID32 id, int x) { add(id, (long)x); } /**< Add an int [@c CFadd32]. */
    void add(FLDID32 id, char x); /**< Add a char [@c CFadd32]. */
    void add(FLDID32 id, float x); /**< Add a float [@c CFadd32]. */
    void add(FLDID32 id, double x); /**< Add a double [@c CFadd32]. */
    void add(FLDID32 id, std::string const& x); /**< Add a string [@c CFadd32]. */
    void add(FLDID32 id, packed_mbstring const& x); /**< Add an mbstring [@c Fadd32]. */
    void add(FLDID32 id, fml32 const& x); /**< Add a nested fml32 [@c Fadd32]. */
    /** Add a pointer [@c Fadd32].
    @warning FLD_PTR fields are confusing; use at your own risk.
    1.  x must point to a buffer allocated
        by tpalloc (cannot be null).
    2.  If you pass @c this to @c tpreturn,
        and @c this was allocated by application
        code in a service routine, then x will
        not (and cannot) be freed at the end
        of the service routine.  @c this
        does not own x.
    3. In all other cases, when @c this
        is destroyed, @c tpfree will be called on
        x.  Make sure no application tries to
        to free the same buffer (perhaps via buffer::~buffer()).
        @c this owns x.
    4.  @c Fmov32/@c Fcpy32/etc. will copy the pointer, but the rules
        above still apply.  If two fml32 buffers point to the
        same FLD_PTR value, double frees can result in some
        scenarios.  Functions like these make it difficult
        to (efficiently) enforce something like unique_ptr
        semantics.
    5.  If @c this was allocated by Tuxedo and passed as input
        to a service routine, and the application code in the
        service routine deletes the field, changes the value,
        or otherwise modifies the buffer so it no longer points
        to the Tuxedo allocated nested buffer, memory leaks can
        occur unless the caller frees those buffers.  @c this does
        not own x. */
    void add_ptr(FLDID32 id, const void* x);
    template <typename T> void add_view(FLDID32 id, T const& x); /**< Add a nested struct (defined in a view) [@c Fadd32]*/
    
    // -----------------------------------append fields---------------------------------------------
    void append(FLDID32 id, short x); /**< Append a short [@c Fappend32]. */
    void append(FLDID32 id, long x); /**< Append a long [@c Fappend32]. */
    void append(FLDID32 id, int x) { append(id, (long)x); } /**< Append an int [@c Fappend32]. */
    void append(FLDID32 id, char x); /**< Append a char [@c Fappend32]. */
    void append(FLDID32 id, float x); /**< Append a float [@c Fappend32]. */
    void append(FLDID32 id, double x); /**< Append a double [@c Fappend32]. */
    void append(FLDID32 id, std::string const& x); /**< Append a string [@c Fappend32]. */
    void append(FLDID32 id, packed_mbstring const& x); /**< Append an mbstring [@c Fappend32]. */
    void append(FLDID32 id, fml32 const& x); /**< Append a nested fml32 [@c Fappend32]. */
    void append_ptr(FLDID32 id, const void* x); /**< Append a pointer [@c Fappend32].  Use at your own risk. @sa add_ptr() */
    template <typename T> void append_view(FLDID32 id, T const& x); /**< Append a nested struct (defined in a view) [@c Fappend32]. */
    
    
    // -----------------------------------set fields---------------------------------------------
    void set(FLDID32 id, short x, FLDOCC32 oc = 0);  /**< Set a short [@c CFchg32]. */
    void set(FLDID32 id, long x, FLDOCC32 oc = 0); /**< Set a long [@c CFchg32]. */
    void set(FLDID32 id, int x, FLDOCC32 oc = 0) { set(id, (long)x, oc); } /**< Set an int [@c CFchg32]. */
    void set(FLDID32 id, char x, FLDOCC32 oc = 0); /**< Set a char [@c CFchg32]. */
    void set(FLDID32 id, float x, FLDOCC32 oc = 0); /**< Set a float [@c CFchg32]. */
    void set(FLDID32 id, double x, FLDOCC32 oc = 0); /**< Set a double [@c CFchg32]. */
    void set(FLDID32 id, std::string const& x, FLDOCC32 oc = 0); /**< Set a string [@c CFchg32]. */
    void set(FLDID32 id, packed_mbstring const& x, FLDOCC32 oc = 0); /**< Set an mbstring [@c Fchg32]. */
    void set(FLDID32 id, fml32 const& x, FLDOCC32 oc = 0); /**< Set a nested fml32 [@c Fchg32]. */
    void set_ptr(FLDID32 id, const void* x, FLDOCC32 oc = 0);  /**< Set a pointer [@c Fchg32]. Use at your own risk. @sa add_ptr() */
    template <typename T> void set_view(FLDID32 id, T const& x, FLDOCC32 oc = 0); /**< Set a nested struct (defined in a view) [@c Fchg32]. */
    
    // -----------------------------------get fields---------------------------------------------
    short get_short(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a short [@c CFget32]. */
    long get_long(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a long [@c CFget32]. */
    char get_char(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a char [@c CFget32]. */
    float get_float(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a float [@c CFget32]. */
    double get_double(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a double [@c CFget32]. */
    std::string get_string(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a string [@c Ffind32, @c CFget32]. */
    unpacked_mbstring get_mbstring(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get an mbstring [@c Fgetalloc32, @c Fmbunpack32]. */
    fml32 get_fml(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a nested fml32 [@c Ffind32]. */
    void get_fml(FLDID32 id, FLDOCC32 oc, fml32& output) const; /**< Get a nested fml32 [@c Ffind32]. */
    void* get_ptr(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a pointer [@c Fget32]. Use at your own risk. @sa add_ptr() */
    template <typename T> T get_view(FLDID32 id, FLDOCC32 oc = 0) const; /**< Get a nested struct (defined in a view) [@c Fgetalloc32]. */
    
    // -----------------------------------find matching occurrences----------------------------------------
    FLDOCC32 find(FLDID32 id, short x) const; /**< Find a short [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, long x) const; /**< Find a long [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, int x) const { return find(id, (long)x); } /**< Find an int [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, char x) const; /**< Find a char [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, float x) const; /**< Find a float [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, double x) const; /**< Find a double [@c CFfindocc32]. */
    FLDOCC32 find(FLDID32 id, std::string const& x) const; /**< Find a string [@c CFfindocc32]. */
    FLDOCC32 find_match(FLDID32 id, std::string const& x) const; /**< Find a string matching the given regular expression [@c Ffindocc32]. */
    FLDOCC32 find(FLDID32 id, packed_mbstring const& x) const; /**< Find an mbstring [@c Ffindocc32]. */
    // find fml appears to be broken (in Tuxedo?)
    //FLDOCC32 find(FLDID32 id, fml32 const& x) const;
    FLDOCC32 find_ptr(FLDID32 id, void* x) const; /**< Find a pointer [@c Ffindocc32]. */
    // find view appears to be broken (in Tuxedo?)
    //template<typename T> FLDOCC32 find_view(FLDID32 id, T const& x) const;
    
    // ---------------------------------------get the last occurrence----------------------------------------
    std::pair<FLDOCC32,short> get_last_short(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    std::pair<FLDOCC32,long> get_last_long(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    std::pair<FLDOCC32,char> get_last_char(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    std::pair<FLDOCC32,float> get_last_float(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    std::pair<FLDOCC32,double> get_last_double(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    std::pair<FLDOCC32,std::string> get_last_string(FLDID32 id) const; /**< Get the last occurrence of a field [@c Ffindlast32]. */
    std::pair<FLDOCC32,unpacked_mbstring> get_last_mbstring(FLDID32 id) const; /**< Get the last occurrence of a field [@c Foccur32, @c Fget32]. */
    std::pair<FLDOCC32,fml32> get_last_fml(FLDID32 id) const; /**< Get the last occurrence of a field [@c Foccur32, @c Fget32]. */
    std::pair<FLDOCC32,void*> get_last_ptr(FLDID32 id) const; /**< Get the last occurrence of a field [@c Fgetlast32]. */
    template <typename T> std::pair<FLDOCC32,T> get_last_view(FLDID32 id) const;  /**< Get the last occurrence of a field [@c Foccur32, @c Fgetalloc32]. */
    
    // ---------------------------------iterate over all fields--------------------------------------------------
    /** Get information about the next field in the buffer.
    @param x on input, this represents the current "iterator";
    on output, it is updated with info on the next field.
    @returns true if there was another field, false if there are
    no more fields in the buffer. */
    bool next_field(field_info& x) const;
    
    

    // -------------------------boolean expression---------------------------------------
    /** Models a boolean expression (string) that can be compiled
    once and applied to multiple instance of fml32.  For example:
    @code{.cpp}
    fml32 person;
    person.set(NAME, "John Doe");
    person.set(AGE, 32);
    fml32::boolean_expression is_minor("AGE < 18");
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
        
        boolean_expression(std::string const&); /**< Construct from a string [@c Fboolco32]. */
        boolean_expression& operator=(std::string const&); /**< Assign from a string [@c Fboolco32]. */
        
        /** Print a string representation of the compiled expression [Fboolpr32].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        bool evaluate(fml32 const&) const; /**< Evaluates the expression on an fml32 [@c Fboolev32]. */
        bool operator()(fml32 const&) const; /**< Evaluates the expression on an fml32 [@c Fboolev32]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
    // -------------------------arithmetic expression----------------------------------------
    /** Models an arithmetic expression (string) that can be compiled
    once and applied to multiple instance of fml32.  For example:
    @code{.cpp}
    fml32 loan;
    loan.set(PRINCIPAL, 20000);
    loan.set(INTEREST, 10000);
    fml32::arithmetic_expression calculate_amount_owed("PRINCIPAL + INTEREST");
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
        
        arithmetic_expression(std::string const&); /**< Construct from a string [@c Fboolco32]. */
        arithmetic_expression& operator=(std::string const&); /**< Assign from a string [@c Fboolco32]. */
        
        /** Print a string representation of the compiled expression [Fboolpr32].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        double evaluate(fml32 const&) const; /**< Evaluates the expression on an fml32 [@c Ffloatev32]. */
        double operator()(fml32 const&) const; /**< Evaluates the expression on an fml32 [@c Ffloatev32]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
private:
    
    static void check_field_type(FLDID32 fieldid, int expected_type);
    
    void convert_and_add(FLDID32 fieldid, const char* value, FLDLEN32 len, int type);
    void add(FLDID32 fieldid, const char* value, FLDLEN32 len);
    void get_and_convert(FLDID32 fieldid, FLDOCC32 oc, char* buf, FLDLEN32* len, int type) const;
    void get(FLDID32 fieldid, FLDOCC32 oc, char* buf, FLDLEN32* len) const;
    
    void append(FLDID32 fieldid, const char* value, FLDLEN32 len);
    
    void convert_and_set(FLDID32 fieldid, const char* value, FLDLEN32 len, FLDOCC32 oc, int type);
    void set(FLDID32 fieldid, const char* value, FLDLEN32 len, FLDOCC32 oc);
    
    
    //void init(FLDLEN32 len); // Finit
    char* find_value(FLDID32 id, FLDOCC32 oc, FLDLEN32* len) const; // Ffind .. or CFfind ?
    FLDOCC32 get_last(FLDID32 id, char* value, FLDLEN32* len) const; // Ffindlast
    FLDOCC32 find_occurence(FLDID32 id, const char* value, FLDLEN32 len) const; // Ffindocc .. or CFfindocc ?
    FLDOCC32 convert_and_find_occurrence(FLDID32 id, const char* value, FLDLEN32 len, int type) const;
    
    class buffer buffer_;
};

// TEMPLATE DEFS
template <typename T> void fml32::add_view(FLDID32 id, T const& x)
{
    using namespace std;
    check_field_type(id, FLD_VIEW32);
    auto f = make_default<FVIEWFLD>();
    tux::set(f.vname, type_name<T>::value());
    f.data = reinterpret_cast<char*>(const_cast<T*>(&x));
    add(id, reinterpret_cast<char*>(&f), sizeof(f));
}

template <typename T> void fml32::append_view(FLDID32 id, T const& x)
{
    using namespace std;
    auto f = make_default<FVIEWFLD>();
    tux::set(f.vname, type_name<T>::value());
    f.data = reinterpret_cast<char*>(const_cast<T*>(&x));
    append(id, reinterpret_cast<char*>(&f), sizeof(f));
}

template <typename T> void fml32::set_view(FLDID32 id, T const& x, FLDOCC32 oc)
{
    using namespace std;
    check_field_type(id, FLD_VIEW32);
    auto f = make_default<FVIEWFLD>();
    tux::set(f.vname, type_name<T>::value());
    f.data = reinterpret_cast<char*>(const_cast<T*>(&x));
    set(id, reinterpret_cast<char*>(&f), sizeof(f), oc);
}


template <typename T> T fml32::get_view(FLDID32 id, FLDOCC32 oc) const
{
    // TODO this is fairly inefficient
    // double allocation, allocating for intermediate FVIEWFLD
    // allocating strings, copying buffers ...
    using namespace std;
    check_field_type(id, FLD_VIEW32);
    FLDLEN32 length = 0;
    // in the case that the view is "null", asan reports
    // Fgetalloc32 triggers a heap buffer overflow during a memmove.  turning off
    // asan results in SIGSEGV.  I notice that Flen reports an insanely large value
    // for such occurrences (presumably uninitialized?)
    unique_ptr<char, decltype(&std::free)> buf { Fgetalloc32(const_cast<FBFR32*>(as_fbfr()),
                                                                id, oc, &length),
                                                &std::free};
    FVIEWFLD* f = (FVIEWFLD*)buf.get();
    std::string actual_type = f->vname;
    std::string requested_type = type_name<T>::value();
    if(actual_type != requested_type)
    {
        throw std::runtime_error("cannot extract " + requested_type + " value from a " +
                    actual_type + " field [" +
                    field_name(id) + "]");
    }
    
    T x = make_default<T>();
    memmove(&x, f->data, sizeof(x));

    return x;
}

/*template<typename T> FLDOCC32 fml32::find_view(FLDID32 id, T const& x) const
{
    using namespace std;
    check_field_type(id, FLD_VIEW32);
    auto f = make_default<FVIEWFLD>();
    tux::set(f.vname, type_name<T>::value());
    f.data = reinterpret_cast<char*>(const_cast<T*>(&x));
    return find_occurence(id, reinterpret_cast<char*>(&f), sizeof(f));
}*/

template <typename T> std::pair<FLDOCC32,T> fml32::get_last_view(FLDID32 id) const
{
    // view32, mbstrings, and fml32 present a special challenge for get_last
    // how do you know the size to allocate ?  At least without
    // first finding the index of the last occurrence ?
    // for now, I'm punting
    using namespace std;
    check_field_type(id, FLD_VIEW32);
    FLDOCC32 oc = count(id) - 1;
    auto val = get_view<T>(id, oc);
    return {oc,val};

}

/** Compare two fml32 structures [@c Fcmp32].
@relates fml32
@returns -1 if a < b, 0 if a == b, and 1 if a > b */
int compare(fml32 const& a, fml32 const& b); // Fcmp

/** @relates fml32 */ bool operator==(fml32 const& a, fml32 const& b); 
/** @relates fml32 */ bool operator!=(fml32 const& a, fml32 const& b); 
/** @relates fml32 */ bool operator< (fml32 const& a, fml32 const& b); 
/** @relates fml32 */ bool operator> (fml32 const& a, fml32 const& b); 
/** @relates fml32 */ bool operator<=(fml32 const& a, fml32 const& b); 
/** @relates fml32 */ bool operator>=(fml32 const& a, fml32 const& b);

}
