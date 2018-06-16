/** @file view16.hpp
@c view16 class and related functions.
@ingroup buffers*/
#pragma once
#include <string>
#include <utility>
#include <memory>
#include "atmi.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"
#include "tux/fml16.hpp" 

namespace tux
{
/** Returns the size of the view (struct) given its name.
@ingroup buffers*/    
inline long view_size16(const char* view_name)
{
    long result = Fvneeded(const_cast<char*>(view_name));
    if(result == -1)
    {
        throw fml16::last_error("Fvneeded");
    }
    return result;
}

/** Returns the size of the view (struct) given its name. */
inline long view_size16(std::string const& view_name)
{
    return view_size16(view_name.c_str());
}

/** Mapping options between views(structs) and fml buffers.
@sa view16::set_mapping_option(). */
enum class view16_mapping_option : int { none = F_OFF, /**< No fml to struct mapping support. */
                                        from_fml = F_FTOS, /**< Support fml to struct mapping. */
                                        to_fml = F_STOF, /**< Support struct to fml mapping. */
                                        both = F_BOTH /**< Support both. */
                                        }; 
                                        
/** Refreshes view definitions */
inline void refresh_view16_definitions() noexcept { Fvrefresh(); }
                                     
/** Models a "VIEW" typed buffer (C struct).
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
*/
template <typename T>
class view16
{
public:
    /** Controls support for mapping between view16 buffers and fml16 buffers. */
    static void set_mapping_option(std::string const& field_name, view16_mapping_option option);
    
    view16() noexcept = default; /**< Default construct; no allocation. */
    view16(view16<T> const& x); /**< Copy construct. */
    view16<T>& operator=(view16<T> const& x); /**< Copy assign. */
    view16(view16<T>&& x) = default; /**< Move construct. */
    view16<T>& operator=(view16<T>&& x) = default; /**< Move assign. */
    ~view16() = default; /**< Destruct. */
    
    /** Construct from a buffer.
    @sa cstring::cstring(buffer&&). */
    view16(class buffer&& x);
    /** Assign from buffer.
    @sa view16(buffer&&)*/
    view16<T>& operator=(class buffer&& x);
    
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
    
    /** Clears all members of T [@c Fvsinit].   Any custom
    "null" values specified in the view definition
    file will be used.*/
    void clear();
    /** Clears a value on T [@c Fvselinit].   Any custom
    "null" values specified in the view definition
    file will be used.*/
    void clear(std::string const& field_name);
    /** Checks if a field on T has a "null" value [@c Fvnull].
    Any custom "null" values specified in the view definition
    file will be used.*/
    bool is_null(std::string const& field_name, FLDOCC oc = 0);
    
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
    view16<my_struct>::boolean_expression int_is_bigger("my_integer > my_double");
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
        
        boolean_expression(std::string const&); /**< Construct from a string [@c Fvboolco]. */
        boolean_expression& operator=(std::string const&); /**< Assign from a string [@c Fvboolco]. */
        
        /** Print a string representation of the compiled expression [Fvboolpr].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept; 
        
        bool evaluate(T const&) const; /**< Evaluates the expression on a T [@c Fvboolev]. */
        bool operator()(T const&) const; /**< Evaluates the expression on a T [@c Fvboolev]. */
        
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
    view16<my_struct>::arithmetic_expression add_parts("int_1 + int_2");
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
        
        arithmetic_expression(std::string const&); /**< Construct from a string [@c Fvboolco]. */
        arithmetic_expression& operator=(std::string const&); /**< Assign from a string [@c Fvboolco]. */
        
        /** Print a string representation of the compiled expression [Fvboolpr].
        This may not be the exact string that was used to construct @c this. */
        void print(FILE* f) const noexcept;
        
        double evaluate(T const&) const; /**< Evaluates the expression on a T [@c Ffloatev]. */
        double operator()(T const&) const; /**< Evaluates the expression on a T [@c Ffloatev]. */
        
    private:
        void free() noexcept;
        void compile(std::string const&);
        unsigned size() const;
        std::unique_ptr<char, decltype(&::free)> tree_ = std::unique_ptr<char, decltype(&::free)>(nullptr, &::free);
    };
    
private:
    class buffer buffer_;
};

/** Test for equality. @relates view16 */
template <typename T>
bool operator==(view16<T> const& a, view16<T> const& b);

/** Test for inequality. @relates view16 */
template <typename T>
bool operator!=(view16<T> const& a, view16<T> const& b); 




// --- DEFS ----------------------------------------
template<typename T>
void view16<T>::set_mapping_option(std::string const& field_name, view16_mapping_option option)
{
    int rc = Fvopt(const_cast<char*>(field_name.c_str()),
                    static_cast<int>(option),
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvopt");
    }
}

template<typename T>
view16<T>::view16(view16<T> const& x)
{
    if(x)
    {
        alloc();
        memcpy(get(), x.buffer().data(), sizeof(T));
    }
}

template<typename T>
view16<T>& view16<T>::operator=(view16<T> const& x)
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
view16<T>::view16(class buffer&& x)
{
    if(x && (x.type() != "VIEW" || x.subtype() != type_name<T>::value()))
    {
        throw std::runtime_error(x.type() + "." + x.subtype() +
                                 " buffer cannot be cast to VIEW." +
                                 type_name<T>::value() +  " buffer");
    }
    buffer_ = std::move(x);
}

template<typename T>
view16<T>& view16<T>::operator=(class buffer&& x)
{
    if(x && (x.type() != "VIEW" || x.subtype() != type_name<T>::value()))
    {
        throw std::runtime_error(x.type() + "." + x.subtype() +
                                 " buffer cannot be cast to VIEW." +
                                 type_name<T>::value() +  " buffer");
    }
    buffer_ = std::move(x);
    return *this;
}

template<typename T>
T& view16<T>::operator*()
{ 
    alloc();
    return reinterpret_cast<T&>(*buffer_.data());
} 

template<typename T>
const T& view16<T>::operator*() const
{ 
    alloc();
    return reinterpret_cast<T const&>(*buffer_.data());
}

template<typename T>
T* view16<T>::operator->()
{ 
    alloc();
    return reinterpret_cast<T*>(buffer_.data());
}

template<typename T>
const T* view16<T>::operator->() const
{ 
    alloc();
    return reinterpret_cast<const T*>(buffer_.data());
}

template<typename T>
T* view16<T>::get()
{ 
    alloc();
    return reinterpret_cast<T*>(buffer_.data());
}

template<typename T>
const T* view16<T>::get() const
{
    alloc();
    return reinterpret_cast<const T*>(buffer_.data());
}

template<typename T>
class buffer& view16<T>::buffer() noexcept
{ 
    return buffer_;
}

template<typename T>
class buffer const& view16<T>::buffer() const noexcept
{ 
    return buffer_;
}

template<typename T>
class buffer&& view16<T>::move_buffer() noexcept
{ 
    return std::move(buffer_);
}


template<typename T>
view16<T>::operator bool() const noexcept
{ 
    return (bool)buffer_;
}


template<typename T>
void view16<T>::alloc()
{
    if(!buffer_)
    {
        const char* view_name = type_name<T>::value();
        buffer_.alloc("VIEW",
                      const_cast<char*>(view_name),
                      view_size16(view_name));
        buffer_.data_size(sizeof(T));
        clear();
    }
}

template<typename T>
void view16<T>::free()
{
    buffer_.free();
}

template<typename T>
void view16<T>::clear()
{
    memset(buffer_.data(), 0, sizeof(T));
    int rc = Fvsinit(buffer_.data(),
                       const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvsinit");
    }
}

template<typename T>
void view16<T>::clear(std::string const& field_name)
{
    int rc = Fvselinit(buffer_.data(),
                       const_cast<char*>(field_name.c_str()),
                       const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvselinit");
    }
}

template<typename T>
bool view16<T>::is_null(std::string const& field_name, FLDOCC oc)
{
    int rc = Fvnull(buffer_.data(),
                    const_cast<char*>(field_name.c_str()),
                    oc,
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvnull");
    }
    return rc == 1;
}



template<typename T>
view16<T>::boolean_expression::boolean_expression(view16<T>::boolean_expression const& x)
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
typename view16<T>::boolean_expression& view16<T>::boolean_expression::operator=(view16<T>::boolean_expression const& x)
{
    view16<T>::boolean_expression tmp(x);
    *this = move(tmp);
    return *this;
}

template<typename T>
view16<T>::boolean_expression::boolean_expression(std::string const& x)
{
    compile(x);
}

template<typename T>
typename view16<T>::boolean_expression& view16<T>::boolean_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

template<typename T>
void view16<T>::boolean_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        int rc = Fvboolpr(tree_.get(), f, const_cast<char*>(type_name<T>::value()));
        if(rc == -1)
        {
            throw fml16::last_error("Fvboolpr");
        }
    }
}

template<typename T>
bool view16<T>::boolean_expression::evaluate(T const& x) const
{

    int result = Fvboolev(reinterpret_cast<char*>(const_cast<T*>(&x)),
                            tree_.get(),
                            const_cast<char*>(type_name<T>::value()));
    if(result == -1)
    {
        throw fml16::last_error("Fvboolev");
    }
    return result;
}

template<typename T>
bool view16<T>::boolean_expression::operator()(T const& x) const
{
    return evaluate(x);
}

template<typename T>
void view16<T>::boolean_expression::free() noexcept
{
    tree_.reset();
}

template<typename T>
void view16<T>::boolean_expression::compile(std::string const& x)
{
    tree_.reset(Fvboolco(const_cast<char*>(x.c_str()), const_cast<char*>(type_name<T>::value())));
    if(!tree_)
    {
        throw fml16::last_error("Fvboolco");
    }
}

template<typename T>
unsigned view16<T>::boolean_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}

template<typename T>
view16<T>::arithmetic_expression::arithmetic_expression(view16<T>::arithmetic_expression const& x)
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
typename view16<T>::arithmetic_expression& view16<T>::arithmetic_expression::operator=(view16<T>::arithmetic_expression const& x)
{
    view16<T>::arithmetic_expression tmp(x);
    *this = move(tmp);
    return *this;
}

template<typename T>
view16<T>::arithmetic_expression::arithmetic_expression(std::string const& x)
{
    compile(x);
}

template<typename T>
typename view16<T>::arithmetic_expression& view16<T>::arithmetic_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

template<typename T>
void view16<T>::arithmetic_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        int rc = Fvboolpr(tree_.get(), f, const_cast<char*>(type_name<T>::value()));
        if(rc == -1)
        {
            throw fml16::last_error("Fvboolpr");
        }
    }
}

template<typename T>
double view16<T>::arithmetic_expression::evaluate(T const& x) const
{
    double result = Fvfloatev(reinterpret_cast<char*>(const_cast<T*>(&x)), tree_.get(), const_cast<char*>(type_name<T>::value()));
    if(result == -1)
    {
        throw fml16::last_error("Fvfloatev");
    }
    return result;
}

template<typename T>
double view16<T>::arithmetic_expression::operator()(T const& x) const
{
    return evaluate(x);
}

template<typename T>
void view16<T>::arithmetic_expression::free() noexcept
{
    tree_.reset();
}

template<typename T>
void view16<T>::arithmetic_expression::compile(std::string const& x)
{
    tree_.reset(Fvboolco(const_cast<char*>(x.c_str()), const_cast<char*>(type_name<T>::value())));
    if(!tree_)
    {
        throw fml16::last_error("Fvboolco");
    }
}

template<typename T>
unsigned view16<T>::arithmetic_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}



template <typename T>
bool operator==(view16<T> const& a, view16<T> const& b)
{
    if(!a && !b) { return true; }
    if(a && b) { return memcmp(a.buffer().data(), b.buffer().data(), sizeof(T)) == 0; }
    return false;
}

template <typename T>
bool operator!=(view16<T> const& a, view16<T> const& b)
{
     return !(a == b);   
}

}
