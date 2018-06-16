/** @file view32.hpp
@c view32 class and related functions.
@ingroup buffers*/
#pragma once
#include <string>
#include <utility>
#include <memory>
#include "atmi.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"
#include "tux/fml32.hpp"


namespace tux
{

/** Returns the size of the view (struct) given its name.
@ingroup buffers*/
inline long view_size32(const char* view_name)
{
    long result = Fvneeded32(const_cast<char*>(view_name));
    if(result == -1)
    {
        throw fml32::last_error("Fvneeded32");
    }
    return result;
}

/** Returns the size of the view (struct) given its name. */
inline long view_size32(std::string const& view_name)
{
    return view_size32(view_name.c_str());
}

/** Mapping options between views(structs) and fml buffers.
@sa view16::set_mapping_option(). */
enum class view32_mapping_option : int { none = F_OFF,
                                        from_fml = F_FTOS,
                                        to_fml = F_STOF,
                                        both = F_BOTH };

/** Refreshes view definitions */
inline void refresh_view32_definitions() noexcept { Fvrefresh32(); }

/** Models a "VIEW32" typed buffer (C struct).
This template class acts a bit like a smart
pointer.  struct members can be accessed
via overloaded -> and * operators.

Given a struct definition from a view like:
@code{.cpp}
struct my_struct
{
    int my_integer;
    double my_double;
    char my_string[30];
}
@endcode

Members can be accessed like:
@code{.cpp}
view16<my_struct> request;
request->my_integer = 9;
request->my_double = 3.4;
set(my_struct->my_string, "hello world");
my_struct request_copy = *request;
@endcode

Internally the struct is allocated via tpalloc so
it can be used in ATMI functions.
@sa view16
*/    
template <typename T>
class view32
{
public:
    /** Controls support for mapping between view32 buffers and fml32 buffers. */
    static void set_mapping_option(std::string const& field_name, view32_mapping_option option);
    
    view32() noexcept = default; /**< Default construct; no allocation. */
    view32(view32<T> const& x); /**< Copy construct. */
    view32<T>& operator=(view32<T> const& x); /**< Copy assign. */
    view32(view32<T>&& x) = default; /**< Move construct. */
    view32<T>& operator=(view32<T>&& x) = default; /**< Move assign. */
    ~view32() = default; /**< Destruct. */
    
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    view32(class buffer&& x);
    /** Assign from buffer.
    @sa view32(buffer&&)*/
    view32<T>& operator=(class buffer&& x);
    
    T& operator*(); /**< Returns a reference to the current T (allocates if currently in null state). */
    const T& operator*() const; /**< Returns a reference to the current T (allocates if currently in null state). */
    T* operator->(); /**< Returns a pointer to the current T (allocates if currently in null state). */
    const T* operator->() const; /**< Returns a pointer to the current T (allocates if currently in null state). */
    T* get(); /**< Returns a pointer to the current T (allocates if currently in null state). */
    const T* get() const; /**< Returns a pointer to the current T (allocates if currently in null state). */
    
    class buffer& buffer() noexcept; /**< Access underlying buffer. */
    class buffer const& buffer() const noexcept; /**< Access underlying buffer. */
    /** Move underlying buffer.
    This can (and usually will) leave @c this in a default (null) state. */
    class buffer&& move_buffer() noexcept;
    
    explicit operator bool() const noexcept; /**< Test for null state. */
    
    void alloc(); /**< Explicitly allocate storage for T [@c Fvsinit]. */
    void free(); /**< Explicitly free storage for T. */
    
    /** Clears all members of T [@c Fvsinit32].   Any custom
    "null" values specified in the view definition
    file will be used.*/
    void clear();
    /** Clears a value on T [@c Fvselinit32].   Any custom
    "null" values specified in the view definition
    file will be used.*/
    void clear(std::string const& field_name);
    /** Checks if a field on T has a "null" value [@c Fvnull32].
    Any custom "null" values specified in the view definition
    file will be used.*/
    bool is_null(std::string const& field_name, FLDOCC32 oc = 0);
    
    /** Models a boolean expression (string) that can be compiled
    once and applied to multiple instances of T.
    Given a struct definition from a view like:
    @code{.cpp}
    struct my_struct
    {
        int my_integer;
        double my_double;
        char my_string[30];
    }
    @endcode
    A boolean expression could be used as follows.
    @code
    view32<my_struct>::boolean_expression int_is_bigger("my_integer > my_double");
    my_struct data = make_default<my_struct>();
    data.my_integer = 5;
    data.my_double = 4.5;
    assert(int_is_bigger(data) == true);
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
        
        boolean_expression(std::string const&); /**< Construct from a string [@c Fvboolco32]. */
        boolean_expression& operator=(std::string const&); /**< Assign from a string [@c Fvboolco32]. */
        
        /** Print a string representation of the compiled expression [Fvboolpr32].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        bool evaluate(T const&) const; /**< Evaluates the expression on a T [@c Fvboolev32]. */
        bool operator()(T const&) const; /**< Evaluates the expression on a T [@c Fvboolev32]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
    /** Models an arithmetic expression (string) that can be compiled
    once and applied to multiple instances of T.
    Given a struct definition from a view like:
    @code{.cpp}
    struct my_struct
    {
        int int_1;
        int int_2;
    }
    @endcode
    An arithmetic expression could be used as follows.
    @code
    view32<my_struct>::arithmetic_expression add_parts("int_1 + int_2");
    my_struct data = make_default<my_struct>();
    data.int_1 = 5;
    data.int_2 = 4;
    assert(add_parts(data) == 9);
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
        
        arithmetic_expression(std::string const&); /**< Construct from a string [@c Fvboolco32]. */
        arithmetic_expression& operator=(std::string const&); /**< Assign from a string [@c Fvboolco32]. */
        
        /** Print a string representation of the compiled expression [Fvboolpr32].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        double evaluate(T const&) const; /**< Evaluates the expression on a T [@c Ffloatev32]. */
        double operator()(T const&) const; /**< Evaluates the expression on a T [@c Ffloatev32]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
private:
    class buffer buffer_;
};

/** Test for equality. @relates view32 */
template <typename T>
bool operator==(view32<T> const& a, view32<T> const& b); 

/** Test for inequality. @relates view32 */
template <typename T>
bool operator!=(view32<T> const& a, view32<T> const& b); 




// --- DEFS ----------------------------------------

template<typename T>
void view32<T>::set_mapping_option(std::string const& field_name, view32_mapping_option option)
{
    int rc = Fvopt32(const_cast<char*>(field_name.c_str()),
                    static_cast<int>(option),
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvopt32");
    }
}

template<typename T>
view32<T>::view32(view32<T> const& x)
{
    if(x)
    {
        alloc();
        memcpy(get(), x.buffer().data(), sizeof(T));
    }
}

template<typename T>
view32<T>& view32<T>::operator=(view32<T> const& x)
{
    if(x)
    {
        alloc();
        memcpy(get(), x.buffer().data(), sizeof(T));
    }
    else
    {
        free();
    }
    return *this;
}

template<typename T>
view32<T>::view32(class buffer&& x)
{
    if(x && (x.type() != "VIEW32" || x.subtype() != type_name<T>::value()))
    {
        throw std::runtime_error(x.type() + "." + x.subtype() +
                                 " buffer cannot be cast to VIEW32." +
                                 type_name<T>::value() +  " buffer");
    }
    buffer_ = std::move(x);
}

template<typename T>
view32<T>& view32<T>::operator=(class buffer&& x)
{
    if(x && (x.type() != "VIEW32" || x.subtype() != type_name<T>::value()))
    {
        throw std::runtime_error(x.type() + "." + x.subtype() +
                                 " buffer cannot be cast to VIEW32." +
                                 type_name<T>::value() +  " buffer");
    }
    buffer_ = std::move(x);
    return *this;
}

template<typename T>
T& view32<T>::operator*()
{ 
    alloc();
    return reinterpret_cast<T&>(*buffer_.data());
} 

template<typename T>
const T& view32<T>::operator*() const
{ 
    alloc();
    return reinterpret_cast<T const&>(*buffer_.data());
}

template<typename T>
T* view32<T>::operator->()
{ 
    alloc();
    return reinterpret_cast<T*>(buffer_.data());
}

template<typename T>
const T* view32<T>::operator->() const
{ 
    alloc();
    return reinterpret_cast<const T*>(buffer_.data());
}

template<typename T>
T* view32<T>::get()
{ 
    alloc();
    return reinterpret_cast<T*>(buffer_.data());
}

template<typename T>
const T* view32<T>::get() const
{
    alloc();
    return reinterpret_cast<const T*>(buffer_.data());
}

template<typename T>
class buffer& view32<T>::buffer() noexcept
{ 
    return buffer_;
}

template<typename T>
class buffer const& view32<T>::buffer() const noexcept
{ 
    return buffer_;
}

template<typename T>
class buffer&& view32<T>::move_buffer() noexcept
{ 
    return std::move(buffer_);
}


template<typename T>
view32<T>::operator bool() const noexcept
{ 
    return (bool)buffer_;
}


template<typename T>
void view32<T>::alloc()
{
    if(!buffer_)
    {
        const char* view_name = type_name<T>::value();
        buffer_.alloc("VIEW32",
                      const_cast<char*>(view_name),
                      view_size32(view_name));
        buffer_.data_size(sizeof(T));
        clear();
    }
}

template<typename T>
void view32<T>::free()
{
    buffer_.free();
}

template<typename T>
void view32<T>::clear()
{
    memset(buffer_.data(), 0, sizeof(T));
    int rc = Fvsinit32(buffer_.data(),
                       const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvsinit32");
    }
}

template<typename T>
void view32<T>::clear(std::string const& field_name)
{
    int rc = Fvselinit32(buffer_.data(),
                       const_cast<char*>(field_name.c_str()),
                       const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvselinit32");
    }
}

template<typename T>
bool view32<T>::is_null(std::string const& field_name, FLDOCC32 oc)
{
    int rc = Fvnull32(buffer_.data(),
                    const_cast<char*>(field_name.c_str()),
                    oc,
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvnull32");
    }
    return rc == 1;
}


template<typename T>
view32<T>::boolean_expression::boolean_expression(view32<T>::boolean_expression const& x)
{
    if(x.tree_)
    {
        unsigned size = x.size();
        tree_.reset(reinterpret_cast<char*>(malloc(size)));
        if(!tree_)
        {
            throw std::bad_alloc{};
        }
        memmove(tree_.get(), x.tree_.get(), size);
    }
}


template<typename T>
typename view32<T>::boolean_expression& view32<T>::boolean_expression::operator=(view32<T>::boolean_expression const& x)
{
    view32<T>::boolean_expression tmp(x);
    *this = move(tmp);
    return *this;
}

template<typename T>
view32<T>::boolean_expression::boolean_expression(std::string const& x)
{
    compile(x);
}

template<typename T>
typename view32<T>::boolean_expression& view32<T>::boolean_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

template<typename T>
void view32<T>::boolean_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        int rc = Fvboolpr32(tree_.get(), f, const_cast<char*>(type_name<T>::value()));
        if(rc == -1)
        {
            throw fml32::last_error("Fvboolpr32");
        }
    }
}

template<typename T>
bool view32<T>::boolean_expression::evaluate(T const& x) const
{

    int result = Fvboolev32(reinterpret_cast<char*>(const_cast<T*>(&x)),
                            tree_.get(),
                            const_cast<char*>(type_name<T>::value()));
    if(result == -1)
    {
        throw fml32::last_error("Fvboolev32");
    }
    return result;
}

template<typename T>
bool view32<T>::boolean_expression::operator()(T const& x) const
{
    return evaluate(x);
}

template<typename T>
void view32<T>::boolean_expression::free() noexcept
{
    tree_.reset();
}

template<typename T>
void view32<T>::boolean_expression::compile(std::string const& x)
{
    free();
    tree_.reset(Fvboolco32(const_cast<char*>(x.c_str()), const_cast<char*>(type_name<T>::value())));
    if(!tree_)
    {
        throw fml32::last_error("Fvboolco32");
    }
}

template<typename T>
unsigned view32<T>::boolean_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}

template<typename T>
view32<T>::arithmetic_expression::arithmetic_expression(view32<T>::arithmetic_expression const& x)
{
    if(x.tree_)
    {
        unsigned size = x.size();
        tree_ = reinterpret_cast<char*>(malloc(size));
        if(!tree_)
        {
            throw std::bad_alloc{};
        }
        memmove(tree_.get(), x.tree_.get(), size);
    }
}

template<typename T>
typename view32<T>::arithmetic_expression& view32<T>::arithmetic_expression::operator=(view32<T>::arithmetic_expression const& x)
{
    view32<T>::arithmetic_expression tmp(x);
    *this = move(tmp);
    return *this;
}

template<typename T>
view32<T>::arithmetic_expression::arithmetic_expression(std::string const& x)
{
    compile(x);
}

template<typename T>
typename view32<T>::arithmetic_expression& view32<T>::arithmetic_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

template<typename T>
void view32<T>::arithmetic_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        int rc = Fvboolpr32(tree_.get(), f, const_cast<char*>(type_name<T>::value()));
        if(rc == -1)
        {
            throw fml32::last_error("Fvboolpr32");
        }
    }
}

template<typename T>
double view32<T>::arithmetic_expression::evaluate(T const& x) const
{
    double result = Fvfloatev32(reinterpret_cast<char*>(const_cast<T*>(&x)), tree_.get(), const_cast<char*>(type_name<T>::value()));
    if(result == -1)
    {
        throw fml32::last_error("Fvfloatev32");
    }
    return result;
}

template<typename T>
double view32<T>::arithmetic_expression::operator()(T const& x) const
{
    return evaluate(x);
}

template<typename T>
void view32<T>::arithmetic_expression::free() noexcept
{
    tree_.reset();
}

template<typename T>
void view32<T>::arithmetic_expression::compile(std::string const& x)
{
    tree_.reset(Fvboolco32(const_cast<char*>(x.c_str()), const_cast<char*>(type_name<T>::value())));
    if(!tree_)
    {
        throw fml32::last_error("Fvboolco32");
    }
}

template<typename T>
unsigned view32<T>::arithmetic_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}



template <typename T>
bool operator==(view32<T> const& a, view32<T> const& b)
{
    if(!a && !b) { return true; }
    if(a && b) { return memcmp(a.buffer().data(), b.buffer().data(), sizeof(T)) == 0; }
    return false;
}

template <typename T>
bool operator!=(view32<T> const& a, view32<T> const& b)
{
     return !(a == b);   
}

}
