#include <algorithm>
#include <limits.h> // this may not be portable
#include "tux/fml32.hpp"
#include "Uunix.h"

using namespace std;

namespace tux
{
    
unpacked_mbstring  unpack_mbstring(void* data, FLDLEN32 data_size)
{
    unpacked_mbstring result;
    FLDLEN32 len = 20;
    result.data.resize(len);
     
    result.encoding.resize(NL_LANGMAX + 1); // may not be portable
    int rc = Fmbunpack32(data,
                         data_size,
                         const_cast<char*>(result.encoding.c_str()),
                         const_cast<char*>(result.data.c_str()),                       
                         &len,
                         TPNOFLAGS);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        result.data.resize(len);
        rc = Fmbunpack32(data,
                         data_size,
                         const_cast<char*>(result.encoding.c_str()),
                         const_cast<char*>(result.data.c_str()),                       
                         &len,
                         TPNOFLAGS);
    }
    if(rc == -1)
    {
        throw fml32::last_error("Fmbunpack32");
    }
    result.data.resize(len);
    trim_to_null_terminator(result.encoding);
    return result;
}  



packed_mbstring::operator bool() const noexcept
{
    return data();
}

packed_mbstring::packed_mbstring(std::string const& data, mbpack_option opt) 
{
    set(data, opt);
}

packed_mbstring::packed_mbstring(std::string const& data, std::string const& encoding) 
{
    set(data, encoding);
}

void packed_mbstring::set(std::string const& data, mbpack_option opt)
{
    if(data.empty())
    {
        buffer_.reset();
        len_ = 0;
        return;
    }
    FLDLEN32 newlen = data.size() + sizeof(FLDLEN32);
    reserve(newlen);
    int rc = Fmbpack32(nullptr,
                       const_cast<char*>(data.c_str()),
                       data.size(),
                       this->data(),
                       &newlen,
                       static_cast<long>(opt));
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        reserve(newlen);
        rc = Fmbpack32(nullptr,
                       const_cast<char*>(data.c_str()),
                       data.size(),
                       this->data(),
                       &newlen,
                       static_cast<long>(opt));
    }
    if(rc == -1)
    {
        throw fml32::last_error("Fmbpack32");
    }
    len_ = newlen;
}

void packed_mbstring::set(std::string const& data, std::string const& encoding)
{
    if(data.empty())
    {
        buffer_.reset();
        len_ = 0;
        return;
    }
    FLDLEN32 newlen = data.size() + sizeof(FLDLEN32);
    reserve(newlen);
    int rc = Fmbpack32(const_cast<char*>(encoding.c_str()),
                       const_cast<char*>(data.c_str()),
                       data.size(),
                       this->data(),
                       &newlen,
                       TPNOFLAGS);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        reserve(newlen);
        rc = Fmbpack32(const_cast<char*>(encoding.c_str()),
                       const_cast<char*>(data.c_str()),
                       data.size(),
                       this->data(),
                       &newlen,
                       TPNOFLAGS);
    }
    if(rc == -1)
    {
        throw fml32::last_error("Fmbpack32");
    }
    len_ = newlen;
}

void* packed_mbstring::data() noexcept
{
    return buffer_.get();
}

const void* packed_mbstring::data() const noexcept
{
    return buffer_.get();
}

FLDLEN32 packed_mbstring::size() const noexcept
{
    return len_;
}    
    
    
void packed_mbstring::reserve(long size_in_bytes)
{
    if(size_in_bytes > len_)
    {
        buffer_ = unique_ptr<void, decltype(&::free)>(malloc(size_in_bytes), &::free);
    }
}

unpacked_mbstring  packed_mbstring::unpack()
{
    return unpack_mbstring(data(), size());
}
    
//------------------ERROR---------------------------------------

fml32::error::error() :
    runtime_error("")
{
}

fml32::error::error(int code, string const& what) :
        runtime_error(what),
        code_(code)
{
}    
    
int fml32::error::code() const noexcept
{
    return code_;
}

const char* fml32::error::str() const noexcept
{
    return Fstrerror32(code_);
}

string build_fml32_error_string(string const& function_name,
                    int code)
{  
    string result;
    if(!function_name.empty())
    {
        result += function_name;
        result += ".";
    }
    const char* str = Fstrerror32(code);
    if(str)
    {
        result += str;
    }
    if(code == FEUNIX)
    {
        const char* os_str = Uunixmsg[Uunixerr];
        if(os_str)
        {
            result += " (";
            result += os_str;
            result += ")";
        }
    }
    return result;    
}
  


fml32::error fml32::last_error(string const& function_name)
{
    string what = build_fml32_error_string(function_name, Ferror32);
    return fml32::error(Ferror32, what);
}

//------------------STATIC--------------------------------------

string fml32::field_type_name_from_type(int type)
{
    switch(type)
    {
        case FLD_SHORT: return "short";
        case FLD_LONG: return "long";
        case FLD_CHAR: return "char";
        case FLD_FLOAT: return "float";
        case FLD_DOUBLE: return "double";
        case FLD_STRING: return "string";
        case FLD_CARRAY: return "carray";
        case FLD_MBSTRING: return "mbstring";
        case FLD_FML32: return "fml32";
        case FLD_PTR: return "ptr";
        case FLD_VIEW32: return "view32";
        default: return "?";
    }
}


FLDID32 fml32::field_id(const string &name)
{
    FLDID32 result = Fldid32(const_cast<char*>(name.c_str()));
    if(result == BADFLDID)
    {
        throw last_error("Fldid32");
    }
    return result;
}

string fml32::field_name(FLDID32 id)
{
    const char* result = Fname32(id);
    if(!result)
    {
        throw last_error("Fname32");
    }
    return result;
}

FLDOCC32 fml32::field_number(FLDID32 id) noexcept
{
    return Fldno32(id);
}

int fml32::field_type(FLDID32 id) noexcept
{
    return Fldtype32(id);
}

string fml32::field_type_name(FLDID32 id)
{
    const char* result = Ftype32(id);
    if(!result)
    {
        throw last_error("Ftype32");
    }
    return result;
}

FLDID32 fml32::field_id(int type, FLDID32 number)
{
    FLDID32 result = Fmkfldid32(type, number);
    if(result == BADFLDID)
    {
        throw last_error("Fldid32");
    }
    return result;
}

long fml32::bytes_needed(FLDOCC32 field_count, FLDLEN32 space_for_values)
{
    long result = Fneeded32(field_count, space_for_values);
    if(result == -1)
    {
        throw last_error("Fneeded32");
    }
    return result;
}

//long fml32::bytes_needed(std::string const& view_name); // Fvneeded
void fml32::unload_id_name_table() noexcept // Fidnm_unload
{
    Fidnm_unload32();
}

void fml32::unload_name_id_table() noexcept // Fnmid_unload
{
    Fnmid_unload32();
}


//--------------CONSTRUCTORS AND ASSIGNMENT--------------------------------
fml32::fml32(fml32 const& x) // Fcpy (or Fmove?)
{
    // docs indicate shallow copy:
    // For values of type FLD_PTR, Fcpy32() copies the buffer pointer.
    // The application programmer must manage the reallocation and freeing
    // of buffers when the associated pointer is copied.
    // Q. will tpfree blow up on two parent buffers with the same ptr value?
    // A.  I don't think so.  On closer review, I don't think tpfree will call
    // tpfree on FLD_PTR values
    reserve(x.used_size());
    int rc = Fcpy32(as_fbfr(), const_cast<FBFR32*>(x.as_fbfr()));
    if(rc == -1)
    {
        throw last_error("Fcpy32");
    }
    index(); // necessary ? 
}

fml32& fml32::operator=(fml32 const& x) // Fcpy (or Fmove?)
{
    fml32 tmp(x);
    *this = move(tmp);
    return *this;
}

fml32::fml32(class buffer&& x)
{
    if(x && x.type() != "FML32") // we could also permit malloced buffers (but that might be dangerous)
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to FML32");   
    }
    buffer_ = move(x);
}

fml32& fml32::operator=(class buffer&& x)
{
    fml32 tmp(move(x));
    *this = move(tmp);
    return *this;
}

fml32::fml32(long capacity)
{
    reserve(capacity);
}

fml32::fml32(FLDOCC32 field_count, FLDLEN32 space_for_values)
{
    reserve(field_count, space_for_values);
}


//--------------------BUFFER ACCESS----------------------------------

class buffer const& fml32::buffer() const noexcept
{
    return buffer_;
}

class buffer& fml32::buffer() noexcept
{
    return buffer_;
}

class buffer&& fml32::move_buffer() noexcept
{
    return move(buffer_);
}

void fml32::reserve(long size)
{
    if(!buffer_)
    {
        buffer_.alloc("FML32", nullptr, size);
    }
    else if(buffer_.size() < size)
    {
        buffer_.realloc(size);
    }
}

void fml32::reserve(FLDOCC32 field_count, FLDLEN32 space_for_values) //tpalloc/tprealloc
{
    reserve(bytes_needed(field_count, space_for_values));
}

fml32::operator bool() const noexcept
{
    if(!buffer_)
    {
        return false;
    }
    else
    {
        field_info info;
        return Fnext32(const_cast<FBFR32*>(as_fbfr()), &info.id, &info.oc, nullptr, nullptr) == 1;    
    }
}

FBFR32* fml32::as_fbfr() noexcept
{
    return reinterpret_cast<FBFR32*>(buffer_.data());
}

const FBFR32* fml32::as_fbfr() const noexcept
{
    return reinterpret_cast<const FBFR32*>(buffer_.data());
}

//------------------CHECKSUM----------------------------------------
long fml32::checksum() const
{
    long result = Fchksum32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fchksum32");
    }
    return result;
}

//------------------FILE I/O----------------------------------------
void fml32::print_to_stdout() const // Fprint
{
    int rc = Fprint32(const_cast<FBFR32*>(as_fbfr()));
    if(rc == -1)
    {
        throw last_error("Fprint32");
    }
}

void fml32::extread(FILE* input, long size_hint) // Fextread
{
    reserve(size_hint);
    int rc = Fextread32(as_fbfr(), input);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        reserve(size() * 2);
        rc = Fextread32(as_fbfr(), input);
    }
    if(rc == -1)
    {
        throw last_error("Fextread32");
    }
}

void fml32::write(FILE* output) const // Fwrite
{
    int rc = Fwrite32(const_cast<FBFR32*>(as_fbfr()), output);
    if(rc == -1)
    {
        throw last_error("Fwrite32");
    }
}

void fml32::read(FILE* input, long size_hint) // Fread
{
    reserve(size_hint);
    int rc = Fread32(as_fbfr(), input);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        reserve(size() * 2);
        rc = Fread32(as_fbfr(), input);
    }
    if(rc == -1)
    {
        throw last_error("Fread32");
    }
}

//------------------------INDEXING------------------------------------
void fml32::index(FLDOCC32 interval) // Findex
{
    if(buffer_)
    {
        int rc = Findex32(as_fbfr(), interval);
        if(rc == -1)
        {
            throw last_error("Findex32");
        }
    }
}

FLDOCC32 fml32::unindex() // Funindex
{
    if(!buffer_)
    {
        return 0; 
    }
    FLDOCC32 result = Funindex32(as_fbfr());
    if(result == -1)
    {
        throw last_error("Funindex32");
    }
    return result;
}

void fml32::restore_index(FLDOCC32 index_element_count) // Frstrindex
{
    if(buffer_)
    {
        int rc = Frstrindex32(as_fbfr(), index_element_count);
        if(rc == -1)
        {
            throw last_error("Frstrindex32");
        }
    }
}


//----------------------------BYTES USED-----------------------------------

long fml32::index_size() const // Fidxused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fidxused32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fidxused32");
    }
    return result;
}

long fml32::size() const // Fsizeof
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fsizeof32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fsizeof32");
    }
    return result;
}

long fml32::unused_size() const // Funused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Funused32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Funused32");
    }
    return result;
}

long fml32::used_size() const // Fused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fused32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fused32");
    }
    return result;
}

//-------------------------FIELD COUNTS--------------------------------
FLDOCC32 fml32::field_count() const // Fnum
{
    if(!buffer_)
    {
        return 0;
    }
    FLDOCC32 result = Fnum32(const_cast<FBFR32*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fnum32");
    }
    return result;
}

FLDOCC32 fml32::count(FLDID32 id) const // Foccur
{
    if(!buffer_)
    {
        return 0;
    }
    FLDOCC32 result = Foccur32(const_cast<FBFR32*>(as_fbfr()), id);
    if(result == -1)
    {
        throw last_error("Foccur32");
    }
    return result;
}

bool fml32::has(FLDID32 id, FLDOCC32 oc) const // Fpres
{
    if(!buffer_)
    {
        return false;
    }
    return Fpres32(const_cast<FBFR32*>(as_fbfr()), id, oc);
}

//--------------------FIELD VALUE SIZE------------------------------
long fml32::field_value_size(FLDID32 id, FLDOCC32 oc) const
{
    if(!buffer_)
    {
        throw error(FNOTPRES, "field_value_size - null buffer");
    }
    long result = Flen32(const_cast<FBFR32*>(as_fbfr()), id, oc);
    if(result == -1)
    {
        throw last_error("Flen32");
    }
    return result;
}

//-------------------ELEMENT REMOVAL------------------------------
bool fml32::erase(FLDID32 id, FLDOCC32 oc) // Fdel
{
    if(!buffer_)
    {
        return false;
    }

    int rc = Fdel32(as_fbfr(), id, oc);
    if(rc == -1)
    {
        if(Ferror32 == FNOTPRES)
        {
            return false;
        }
        throw last_error("Fdel32");
    }
    return true;
}

bool fml32::erase(FLDID32 id) // Fdelall
{
    if(!buffer_)
    {
        return false;
    }

    int rc = Fdelall32(as_fbfr(), id);
    if(rc == -1)
    {
        if(Ferror32 == FNOTPRES)
        {
            return false;
        }
        throw last_error("Fdelall32");
    }
    return true;
}

void fml32::erase(vector<FLDID32> ids) // Fdelete
{
    if(buffer_ && !ids.empty())
    {
        ids.push_back(BADFLDID);
        int rc = Fdelete32(as_fbfr(), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fdelete32");
        }
    }
}

void fml32::erase_all_but(vector<FLDID32> ids) // Fproj
{
    if(buffer_)
    {
        ids.push_back(BADFLDID);
        int rc = Fproj32(as_fbfr(), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fproj32");
        }
    }
}

void fml32::clear() // Finit
{
    if(buffer_)
    {
        int rc = Finit32(as_fbfr(), size());
        if(rc == -1)
        {
            throw last_error("Finit32");
        }
    }
}

//-------------------OPERATIONS ON ENTIRE DATA STRUCTURES----------------------
fml32& fml32::operator +=(fml32 const& x) // Fconcat
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fconcat32(as_fbfr(), const_cast<FBFR32*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fconcat32");
        }
    }
    return *this;
}

void fml32::join(fml32 const& x) // Fjoin
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fjoin32(as_fbfr(), const_cast<FBFR32*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fjoin32");
        }
    }
    else
    {
        clear();
    }
}

void fml32::outer_join(fml32 const& x) // Fojoin
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fojoin32(as_fbfr(), const_cast<FBFR32*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fojoin32");
        }
    }   
}

fml32 fml32::project(vector<FLDID32> ids) const // Fprojcpy
{
    fml32 result;
    if(buffer_)
    {
        ids.push_back(BADFLDID);
        result.reserve(used_size());
        int rc = Fprojcpy32(result.as_fbfr(), const_cast<FBFR32*>(as_fbfr()), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fprojcpy32");
        }
    }
    return result;
}

void fml32::update(fml32 const& x) // Fupdate
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fupdate32(as_fbfr(), const_cast<FBFR32*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fupdate32");
        }
    }
}


bool fml32::is_fielded() const noexcept
{
    return Fielded32(const_cast<FBFR32*>(as_fbfr()));
}

//------------------------MBSTRINGS----------------------------------
string fml32::get_encoding_name() const
{
    if(!buffer_)
    {
        return "";
    }
    string result;
    result.resize(NL_LANGMAX + 1); // not sure this is portable
    int rc = tpgetmbenc(const_cast<char*>(buffer_.data()),
                        const_cast<char*>(result.data()),
                        TPNOFLAGS);
    if(rc == -1)
    {
        if(tperrno == TPEPROTO)
        {
            return "";
        }
        else
        {
            throw last_error("tpgetmbenc");
        }
    }
    trim_to_null_terminator(result);
    return result;
}

void fml32::set_encoding_name(string const& encoding)
{
    if(!buffer_)
    {
        reserve(0);
    }
    int rc = tpsetmbenc(buffer_.data(),
                        const_cast<char*>(encoding.c_str()),
                        TPNOFLAGS);
    if(rc != 0)
    {
        throw last_error("tpsetmbenc");    
    }
}

void fml32::clear_encoding_name()
{
    if(buffer_)
    {
        int rc = tpsetmbenc(buffer_.data(),
                            nullptr,
                            RM_ENC);
        if(rc != 0)
        {
            throw last_error("tpsetmbenc(RM_ENC)");    
        }
    }
}


void fml32::convert_mbstrings(string const& target_encoding, long flags) // tpconvfmb32
{
    if(buffer_)
    {
        FBFR32* f = reinterpret_cast<FBFR32*>(buffer_.release());
        int rc = tpconvfmb32(&f,
                            nullptr,
                            const_cast<char*>(target_encoding.c_str()),
                            flags);
        buffer_.acquire(reinterpret_cast<char*>(f));
        if(rc == -1)
        {
            throw tux::last_error("tpconvfmb32");
        }
    }
}

void fml32::convert_mbstrings(vector<FLDID32> fields, string const& target_encoding, long flags) // tpconvfmb32
{
    if(buffer_ && !fields.empty())
    {
        fields.push_back(BADFLDID);
        FBFR32* f = reinterpret_cast<FBFR32*>(buffer_.release());
        int rc = tpconvfmb32(&f,
                            &fields[0],
                            const_cast<char*>(target_encoding.c_str()),
                            flags);
        buffer_.acquire(reinterpret_cast<char*>(f));
        if(rc == -1)
        {
            throw tux::last_error("tpconvfmb32");
        }
    }
}


//-----------------ADD----------------------------
void fml32::add(FLDID32 id, short x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_SHORT);
}

void fml32::add(FLDID32 id, long x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_LONG);
}

void fml32::add(FLDID32 id, char x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_CHAR);
}

void fml32::add(FLDID32 id, float x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_FLOAT);
}

void fml32::add(FLDID32 id, double x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_DOUBLE);
}

void fml32::add(FLDID32 id, std::string const& x)
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    convert_and_add(id, const_cast<char*>(x.data()), x.size(), type);
}

void fml32::add(FLDID32 id, packed_mbstring const& x)
{
    check_field_type(id, FLD_MBSTRING);
    add(id, reinterpret_cast<char*>(const_cast<void*>(x.data())), x.size());
}

void fml32::add(FLDID32 id, fml32 const& x)
{
    check_field_type(id, FLD_FML32);
    add(id, x.buffer().data(), x.used_size());
}

void fml32::add_ptr(FLDID32 id, const void* x)
{
    check_field_type(id, FLD_PTR);
    add(id, reinterpret_cast<char*>(const_cast<void*>(x)), sizeof(x));
}

void fml32::append(FLDID32 id, short x)
{
    check_field_type(id, FLD_SHORT);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml32::append(FLDID32 id, long x)
{
    check_field_type(id, FLD_LONG);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}  

void fml32::append(FLDID32 id, char x)
{
    check_field_type(id, FLD_CHAR);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml32::append(FLDID32 id, float x)
{
    check_field_type(id, FLD_FLOAT);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml32::append(FLDID32 id, double x)
{
    check_field_type(id, FLD_DOUBLE);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml32::append(FLDID32 id, std::string const& x) 
{
    int type = field_type(id);
    if(type != FLD_STRING && type != FLD_CARRAY)
    {
        
        throw runtime_error("string type does not match field type " +
            field_type_name(id) + " field [" +
            field_name(id) + "]");
    }
    append(id, const_cast<char*>(x.data()), x.size());
}

void fml32::append(FLDID32 id, packed_mbstring const& x)
{
    check_field_type(id, FLD_MBSTRING);
    append(id, reinterpret_cast<char*>(const_cast<void*>(x.data())), x.size());
}

void fml32::append(FLDID32 id, fml32 const& x)
{
    check_field_type(id, FLD_FML32);
    append(id, x.buffer().data(), x.used_size());
}

void fml32::append_ptr(FLDID32 id, const void* x)
{
    check_field_type(id, FLD_PTR);
    append(id, reinterpret_cast<char*>(const_cast<void*>(x)), sizeof(x));
}


void fml32::set(FLDID32 id, short x, FLDOCC32 oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_SHORT);
}
void fml32::set(FLDID32 id, long x, FLDOCC32 oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_LONG);
}
void fml32::set(FLDID32 id, char x, FLDOCC32 oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_CHAR);
}
void fml32::set(FLDID32 id, float x, FLDOCC32 oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_FLOAT);
}
void fml32::set(FLDID32 id, double x, FLDOCC32 oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_DOUBLE);
}
void fml32::set(FLDID32 id, std::string const& x, FLDOCC32 oc) // could be string or carray depending on id
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    convert_and_set(id, const_cast<char*>(x.data()), x.size(), oc, type);    
}
void fml32::set(FLDID32 id, packed_mbstring const& x, FLDOCC32 oc)
{
    check_field_type(id, FLD_MBSTRING);
    set(id, reinterpret_cast<char*>(const_cast<void*>(x.data())), x.size(), oc);
}
void fml32::set(FLDID32 id, fml32 const& x, FLDOCC32 oc)
{
    check_field_type(id, FLD_FML32);
    set(id, x.buffer().data(), x.used_size(), oc); 
}
void fml32::set_ptr(FLDID32 id, const void* x, FLDOCC32 oc)
{
    check_field_type(id, FLD_PTR);
    set(id, reinterpret_cast<char*>(const_cast<void*>(x)), sizeof(x), oc);
}



//----------------------GET--------------------------------
short fml32::get_short(FLDID32 id, FLDOCC32 oc) const
{
    short x = 0;
    FLDLEN32 len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_SHORT);
    return x;
}

long fml32::get_long(FLDID32 id, FLDOCC32 oc) const
{
    long x = 0;
    FLDLEN32 len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_LONG);
    return x;
}

char fml32::get_char(FLDID32 id, FLDOCC32 oc) const
{
    char x = 0;
    FLDLEN32 len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_CHAR);
    return x;
}

float fml32::get_float(FLDID32 id, FLDOCC32 oc) const
{
    float x = 0;
    FLDLEN32 len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_FLOAT);
    return x;
}

double fml32::get_double(FLDID32 id, FLDOCC32 oc) const
{
    double x = 0;
    FLDLEN32 len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_DOUBLE);
    return x;
}

string fml32::get_string(FLDID32 id, FLDOCC32 oc) const
{
    int type = field_type(id);
    if(type == FLD_CARRAY || type == FLD_STRING)
    {
        FLDLEN32 len = 0;
        char* loc = find_value(id, oc, &len);
        return type == FLD_CARRAY ? string(loc, len) : string(loc); 
    }
    else
    {
        string x;
        FLDLEN32 len = 18; // likely within small buffer optimization limit
        x.resize(len);
        
        int rc = CFget32(const_cast<FBFR32*>(as_fbfr()),
                         id,
                         oc,
                         const_cast<char*>(x.data()),
                         &len,
                         FLD_STRING);
        if(rc == -1 && Ferror32 == FNOSPACE)
        {
            // must be a big number
            len = 36;
            x.resize(len);
            rc = CFget32(const_cast<FBFR32*>(as_fbfr()),
                        id,
                        oc,
                        const_cast<char*>(x.data()),
                        &len,
                        FLD_STRING);
        }
        if(rc == -1)
        {
            throw last_error("CFget32");
        }
        trim_to_null_terminator(x);
        return x;
    }
}



unpacked_mbstring fml32::get_mbstring(FLDID32 id, FLDOCC32 oc) const
{
    check_field_type(id, FLD_MBSTRING);
    FLDLEN32 len = 0;
    unique_ptr<char, decltype(&std::free)> buf { Fgetalloc32(const_cast<FBFR32*>(as_fbfr()),
                                                                id, oc, &len),
                                                &std::free};
    if(!buf)
    {
        throw last_error("Fgetalloc32");
    }
    if(len == 0)
    {
        // i think this is only possible
        // via Fchg for higher index,
        // leaving a "null" in the lower
        // index
        return unpacked_mbstring();
    }
    return unpack_mbstring(buf.get(), len);
}

fml32 fml32::get_fml(FLDID32 id, FLDOCC32 oc) const
{
    fml32 x;
    get_fml(id, oc, x);
    return x;
}

void fml32::get_fml(FLDID32 id, FLDOCC32 oc, fml32& output) const
{
    check_field_type(id, FLD_FML32);
    
    // bad impl
    FLDLEN32 len = 0;
    char* loc = find_value(id, oc, &len);
    output.reserve(len);
    memcpy(output.as_fbfr(), loc, len);
    
    /*
    if(!output)
    {
        output.reserve(0);
    }
    FLDLEN32 len  = output.size();
    int rc = Fget32(const_cast<FBFR32*>(as_fbfr()), id, oc, output.buffer().data(), &len);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        len  = field_value_size(id, oc);
        output.reserve(len);
        rc = Fget32(const_cast<FBFR32*>(as_fbfr()), id, oc, output.buffer().data(), &len);
    }
    if(rc == -1)
    {
        throw last_error("Fget32");
    }
    
    */
}

void* fml32::get_ptr(FLDID32 id, FLDOCC32 oc) const
{
    check_field_type(id, FLD_PTR);
    char* x = nullptr;
    FLDLEN32 len = sizeof(x);
    get(id, oc, reinterpret_cast<char*>(&x), &len);
    return x;
}


FLDOCC32 fml32::find(FLDID32 id, short x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_SHORT);
}
FLDOCC32 fml32::find(FLDID32 id, long x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_LONG);
}
FLDOCC32 fml32::find(FLDID32 id, char x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_CHAR);
}
FLDOCC32 fml32::find(FLDID32 id, float x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_FLOAT);
}
FLDOCC32 fml32::find(FLDID32 id, double x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_DOUBLE);
}
FLDOCC32 fml32::find(FLDID32 id, std::string const& x) const
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    int size = type == FLD_CARRAY ? x.size() : 0;
    return convert_and_find_occurrence(id, const_cast<char*>(x.data()), size, type);
}
FLDOCC32 fml32::find_match(FLDID32 id, std::string const& x) const
{
    check_field_type(id, FLD_STRING);
    return find_occurence(id, x.data(), 1);
}
FLDOCC32 fml32::find(FLDID32 id, packed_mbstring const& x) const
{
    check_field_type(id, FLD_MBSTRING);
    return find_occurence(id, reinterpret_cast<char*>(const_cast<void*>(x.data())), x.size());
}
// below is broken (evidently in Tuxedo)
/*FLDOCC32 fml32::find(FLDID32 id, fml32 const& x) const
{
    check_field_type(id, FLD_FML32);
    return find_occurence(id, x.buffer().data(), 0);
}*/
FLDOCC32 fml32::find_ptr(FLDID32 id, void* x) const
{
    check_field_type(id, FLD_PTR);
    // I'd actually expect to need to pass in &x,
    // but (oddly enough), it doesn't work that way,
    // but it does work when I pass x.
    return find_occurence(id, reinterpret_cast<char*>(x), sizeof(x));
}

std::pair<FLDOCC32,short> fml32::get_last_short(FLDID32 id) const
{
    check_field_type(id, FLD_SHORT);
    short x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC32,long> fml32::get_last_long(FLDID32 id) const
{
    check_field_type(id, FLD_LONG);
    long x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC32,char> fml32::get_last_char(FLDID32 id) const
{
    check_field_type(id, FLD_CHAR);
    char x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC32,float> fml32::get_last_float(FLDID32 id) const
{
    check_field_type(id, FLD_FLOAT);
    float x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC32,double> fml32::get_last_double(FLDID32 id) const
{
    check_field_type(id, FLD_DOUBLE);
    double x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC32,std::string> fml32::get_last_string(FLDID32 id) const
{
    int type = field_type(id);
    if(type != FLD_CARRAY && type != FLD_STRING)
    {
        throw runtime_error("cannot extract string value from a " +
            field_type_name(id) + " field [" +
            field_name(id) + "]");
    }
    FLDOCC32 oc = -1;
    FLDLEN32 len = 0;
    const char* val = Ffindlast32(const_cast<FBFR32*>(as_fbfr()), id, &oc, &len);
    if(!val)
    {
        throw last_error("Ffindlast32");
    }
    
    if(type == FLD_STRING && len > 0)
    {
        --len;
    }
 
    return {oc,string{val, len}};
}
std::pair<FLDOCC32,unpacked_mbstring> fml32::get_last_mbstring(FLDID32 id) const
{
    check_field_type(id, FLD_MBSTRING);
    // mbstrings and fml32 present a special challenge for get_last
    // how do you know the size to allocate ?  At least without
    // first finding the index of the last occurrence ?
    // for now, I'm punting
    FLDOCC32 oc = count(id) - 1;
    auto val = get_mbstring(id, oc);
    return {oc,val};
}

std::pair<FLDOCC32,fml32> fml32::get_last_fml(FLDID32 id) const
{
    check_field_type(id, FLD_FML32);
    // mbstrings and fml32 present a special challenge for get_last
    // how do you know the size to allocate ?  At least without
    // first finding the index of the last occurrence ?
    // for now, I'm punting
    FLDOCC32 oc = count(id) - 1;
    auto val = get_fml(id, oc);
    return {oc,val};
}

std::pair<FLDOCC32,void*> fml32::get_last_ptr(FLDID32 id) const
{
    check_field_type(id, FLD_PTR);
    void* x = 0;
    FLDLEN32 len = sizeof(x);
    FLDOCC32 oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}

bool fml32::next_field(field_info& x) const
{
    int rc = Fnext32(const_cast<FBFR32*>(as_fbfr()), &x.id, &x.oc, nullptr, nullptr);
    if(rc == -1)
    {
        throw last_error("Fnext32");
    }
    return rc;
}


fml32::boolean_expression::boolean_expression(fml32::boolean_expression const& x)
{
    if(x.tree_)
    {
        unsigned size = x.size();
        tree_.reset(reinterpret_cast<char*>(malloc(size)));
        if(!tree_)
        {
            throw bad_alloc{};
        }
        memmove(tree_.get(), x.tree_.get(), size);
    }
}


fml32::boolean_expression& fml32::boolean_expression::operator=(fml32::boolean_expression const& x)
{
    fml32::boolean_expression tmp(x);
    *this = move(tmp);
    return *this;
}

fml32::boolean_expression::boolean_expression(std::string const& x)
{
    compile(x);
}

fml32::boolean_expression& fml32::boolean_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

void fml32::boolean_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        Fboolpr32(tree_.get(), f);
    }
}

bool fml32::boolean_expression::evaluate(fml32 const& x) const
{
    int result = Fboolev32(const_cast<FBFR32*>(x.as_fbfr()), tree_.get());
    if(result == -1)
    {
        throw last_error("Fboolev32");
    }
    return result;
}

bool fml32::boolean_expression::operator()(fml32 const& x) const
{
    return evaluate(x);
}


void fml32::boolean_expression::free() noexcept
{
    tree_.reset();
}

void fml32::boolean_expression::compile(std::string const& x)
{
    tree_.reset(Fboolco32(const_cast<char*>(x.c_str())));
    if(!tree_)
    {
        throw last_error("Fboolco32");
    }
}

unsigned fml32::boolean_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}


fml32::arithmetic_expression::arithmetic_expression(fml32::arithmetic_expression const& x)
{
    if(x.tree_)
    {
        unsigned size = x.size();
        tree_.reset(reinterpret_cast<char*>(malloc(size)));
        if(!tree_)
        {
            throw bad_alloc{};
        }
        memmove(tree_.get(), x.tree_.get(), size);
    }
}


fml32::arithmetic_expression& fml32::arithmetic_expression::operator=(fml32::arithmetic_expression const& x)
{
    fml32::arithmetic_expression tmp(x);
    *this = move(tmp);
    return *this;
}

fml32::arithmetic_expression::arithmetic_expression(std::string const& x)
{
    compile(x);
}

fml32::arithmetic_expression& fml32::arithmetic_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

void fml32::arithmetic_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        Fboolpr32(tree_.get(), f);
    }
}

double fml32::arithmetic_expression::evaluate(fml32 const& x) const
{
    double result = Ffloatev32(const_cast<FBFR32*>(x.as_fbfr()), tree_.get());
    if(result == -1)
    {
        throw last_error("Ffloatev32");
    }
    return result;
}

double fml32::arithmetic_expression::operator()(fml32 const& x) const
{
    return evaluate(x);
}

void fml32::arithmetic_expression::free() noexcept
{
    tree_.reset();
}

void fml32::arithmetic_expression::compile(std::string const& x)
{
    tree_.reset(Fboolco32(const_cast<char*>(x.c_str())));
    if(!tree_)
    {
        throw last_error("Fboolco32");
    }
}

unsigned fml32::arithmetic_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}


//---------------------PRIVATE----------------------------------------

void fml32::check_field_type(FLDID32 fieldid, int expected_type)
{
    if(field_type(fieldid) != expected_type)
    {
        
        throw runtime_error("cannot convert " + field_type_name_from_type(expected_type) + " value to/from a " +
            field_type_name(fieldid) + " field [" +
            field_name(fieldid) + "]");
    }
}

void fml32::convert_and_add(FLDID32 fieldid, const char* value, FLDLEN32 len, int type)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = CFadd32(as_fbfr(), fieldid, const_cast<char*>(value), len, type);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = CFadd32(as_fbfr(), fieldid, const_cast<char*>(value), len, type);
    }
    if(rc == -1)
    {
        throw last_error("CFadd32");
    }
}

void fml32::add(FLDID32 fieldid, const char* value, FLDLEN32 len)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fadd32(as_fbfr(), fieldid, const_cast<char*>(value), len);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fadd32(as_fbfr(), fieldid, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fadd32");
    }
}

void fml32::get_and_convert(FLDID32 fieldid, FLDOCC32 oc, char* buf, FLDLEN32* len, int type) const
{
    int rc = CFget32(const_cast<FBFR32*>(as_fbfr()), fieldid, oc, buf, len, type);
    if(rc == -1)
    {
        throw last_error("CFget32");
    }
}

void fml32::get(FLDID32 fieldid, FLDOCC32 oc, char* buf, FLDLEN32* len) const
{
    string raw(buf,*len);
    int rc = Fget32(const_cast<FBFR32*>(as_fbfr()), fieldid, oc, buf, len);
    if(rc == -1)
    {
        throw last_error("Fget32");
    }
}

void fml32::append(FLDID32 fieldid, const char* value, FLDLEN32 len)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fappend32(as_fbfr(), fieldid, const_cast<char*>(value), len);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fappend32(as_fbfr(), fieldid, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fappend32");
    }
}

void fml32::convert_and_set(FLDID32 fieldid, const char* value, FLDLEN32 len, FLDOCC32 oc, int type)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = CFchg32(as_fbfr(), fieldid, oc, const_cast<char*>(value), len, type);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = CFchg32(as_fbfr(), fieldid, oc, const_cast<char*>(value), len, type);
    }
    if(rc == -1)
    {
        throw last_error("Fchg32");
    }
}
void fml32::set(FLDID32 fieldid, const char* value, FLDLEN32 len, FLDOCC32 oc)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fchg32(as_fbfr(), fieldid, oc, const_cast<char*>(value), len);
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fchg32(as_fbfr(), fieldid, oc, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fchg32");
    }
}


char* fml32::find_value(FLDID32 id, FLDOCC32 oc, FLDLEN32* len) const
{
    char* result = Ffind32(const_cast<FBFR32*>(as_fbfr()), id, oc, len);
    if(!result)
    {
        throw last_error("Ffind32");
    }
    return result;
}

FLDOCC32 fml32::get_last(FLDID32 fieldid, char* buf, FLDLEN32* len) const
{
    FLDOCC32 oc = -1;
    int rc = Fgetlast32(const_cast<FBFR32*>(as_fbfr()), fieldid, &oc, buf, len);
    if(rc == -1)
    {
        throw last_error("Fgetlast32");
    }
    return oc;
}

FLDOCC32 fml32::find_occurence(FLDID32 id, const char* value, FLDLEN32 len) const
{
    FLDOCC32 result =  Ffindocc32(const_cast<FBFR32*>(as_fbfr()), id, const_cast<char*>(value), len);
    if(result == -1 && Ferror32 != FNOTPRES)
    {
        throw last_error("Ferror32");
    }
    return result;
}


FLDOCC32 fml32::convert_and_find_occurrence(FLDID32 id, const char* value, FLDLEN32 len, int type) const
{
    FLDOCC32 result =  CFfindocc32(const_cast<FBFR32*>(as_fbfr()), id, const_cast<char*>(value), len, type);
    if(result == -1 && Ferror32 != FNOTPRES)
    {
        throw last_error("CFfindocc32");
    }
    return result;
}




//---------------------NON-MEMBER--------------------------------------

int compare(fml32 const& a, fml32 const& b)
{
    if(!a && !b)
    {
        return 0;
    }
    else if(!a)
    {
        return -1;
    }
    else if(!b)
    {
        return 1;
    }
    else
    {
        int rc = Fcmp32(const_cast<FBFR32*>(a.as_fbfr()), const_cast<FBFR32*>(b.as_fbfr()));
        if(rc == -2)
        {
            throw last_error("Fcmp32");
        }
        return rc;
    }
}

bool operator==(fml32 const& a, fml32 const& b) { return compare(a,b) == 0; }
bool operator!=(fml32 const& a, fml32 const& b) { return compare(a,b) != 0; }
bool operator<(fml32 const& a, fml32 const& b) { return compare(a,b) < 0; }
bool operator>(fml32 const& a, fml32 const& b) { return compare(a,b) > 0; }
bool operator<=(fml32 const& a, fml32 const& b) { return compare(a,b) <= 0; }
bool operator>=(fml32 const& a, fml32 const& b) { return compare(a,b) >= 0; }



}

