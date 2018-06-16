#include <algorithm>
#include "tux/fml16.hpp"
#include "Uunix.h"


using namespace std;

namespace tux
{
    

    
//------------------ERROR---------------------------------------

fml16::error::error() :
    runtime_error("")
{
}

fml16::error::error(int code, string const& what) :
        runtime_error(what),
        code_(code)
{
}    
    
int fml16::error::code() const noexcept
{
    return code_;
}

const char* fml16::error::str() const noexcept
{
    return Fstrerror(code_);
}

string build_fml16_error_string(string const& function_name,
                    int code)
{  
    string result;
    if(!function_name.empty())
    {
        result += function_name;
        result += ".";
    }
    const char* str = Fstrerror(code);
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
  


fml16::error fml16::last_error(string const& function_name)
{
    string what = build_fml16_error_string(function_name, Ferror);
    return fml16::error(Ferror, what);
}

//------------------STATIC--------------------------------------

string fml16::field_type_name_from_type(int type)
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
        default: return "?";
    }
}


FLDID fml16::field_id(const string &name)
{
    FLDID result = Fldid(const_cast<char*>(name.c_str()));
    if(result == BADFLDID)
    {
        throw last_error("Fldid");
    }
    return result;
}

string fml16::field_name(FLDID id)
{
    const char* result = Fname(id);
    if(!result)
    {
        throw last_error("Fname");
    }
    return result;
}

FLDOCC fml16::field_number(FLDID id) noexcept
{
    return Fldno(id);
}

int fml16::field_type(FLDID id) noexcept
{
    return Fldtype(id);
}

string fml16::field_type_name(FLDID id)
{
    const char* result = Ftype(id);
    if(!result)
    {
        throw last_error("Ftype");
    }
    return result;
}

FLDID fml16::field_id(int type, FLDID number)
{
    FLDID result = Fmkfldid(type, number);
    if(result == BADFLDID)
    {
        throw last_error("Fldid");
    }
    return result;
}

long fml16::bytes_needed(FLDOCC field_count, FLDLEN space_for_values)
{
    long result = Fneeded(field_count, space_for_values);
    if(result == -1)
    {
        throw last_error("Fneeded");
    }
    return result;
}

//long fml16::bytes_needed(std::string const& view_name); // Fvneeded
void fml16::unload_id_name_table() noexcept // Fidnm_unload
{
    Fidnm_unload();
}

void fml16::unload_name_id_table() noexcept // Fnmid_unload
{
    Fnmid_unload();
}


//--------------CONSTRUCTORS AND ASSIGNMENT--------------------------------
fml16::fml16(fml16 const& x) // Fcpy (or Fmove?)
{
    // docs indicate shallow copy:
    // For values of type FLD_PTR, Fcpy() copies the buffer pointer.
    // The application programmer must manage the reallocation and freeing
    // of buffers when the associated pointer is copied.
    // Q. will tpfree blow up on two parent buffers with the same ptr value?
    // A.  I don't think so.  On closer review, I don't think tpfree will call
    // tpfree on FLD_PTR values
    reserve(x.used_size());
    int rc = Fcpy(as_fbfr(), const_cast<FBFR*>(x.as_fbfr()));
    if(rc == -1)
    {
        throw last_error("Fcpy");
    }
    index(); // necessary ? 
}

fml16& fml16::operator=(fml16 const& x) // Fcpy (or Fmove?)
{
    fml16 tmp(x);
    *this = move(tmp);
    return *this;
}

fml16::fml16(class buffer&& x)
{
    if(x && x.type() != "FML") // we could also permit malloced buffers (but that might be dangerous)
    {
        throw runtime_error("buffer type " + x.type() + " cannot be cast to FML");   
    }
    buffer_ = move(x);
}

fml16& fml16::operator=(class buffer&& x)
{
    fml16 tmp(move(x));
    *this = move(tmp);
    return *this;
}

fml16::fml16(long capacity)
{
    reserve(capacity);
}

fml16::fml16(FLDOCC field_count, FLDLEN space_for_values)
{
    reserve(field_count, space_for_values);
}


//--------------------BUFFER ACCESS----------------------------------

class buffer const& fml16::buffer() const noexcept
{
    return buffer_;
}

class buffer& fml16::buffer() noexcept
{
    return buffer_;
}

class buffer&& fml16::move_buffer() noexcept
{
    return move(buffer_);
}

void fml16::reserve(long size)
{
    if(!buffer_)
    {
        buffer_.alloc("FML", nullptr, size);
    }
    else if(buffer_.size() < size)
    {
        buffer_.realloc(size);
    }
}

void fml16::reserve(FLDOCC field_count, FLDLEN space_for_values) //tpalloc/tprealloc
{
    reserve(bytes_needed(field_count, space_for_values));
}

fml16::operator bool() const noexcept
{
    if(!buffer_)
    {
        return false;
    }
    else
    {
        field_info info;
        return Fnext(const_cast<FBFR*>(as_fbfr()), &info.id, &info.oc, nullptr, nullptr) == 1;    
    }
}

FBFR* fml16::as_fbfr() noexcept
{
    return reinterpret_cast<FBFR*>(buffer_.data());
}

const FBFR* fml16::as_fbfr() const noexcept
{
    return reinterpret_cast<const FBFR*>(buffer_.data());
}

//------------------CHECKSUM----------------------------------------
long fml16::checksum() const
{
    long result = Fchksum(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fchksum");
    }
    return result;
}

//------------------FILE I/O----------------------------------------
void fml16::print_to_stdout() const // Fprint
{
    int rc = Fprint(const_cast<FBFR*>(as_fbfr()));
    if(rc == -1)
    {
        throw last_error("Fprint");
    }
}

void fml16::extread(FILE* input, long size_hint) // Fextread
{
    reserve(size_hint);
    int rc = Fextread(as_fbfr(), input);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        reserve(size() * 2);
        rc = Fextread(as_fbfr(), input);
    }
    if(rc == -1)
    {
        throw last_error("Fextread");
    }
}

void fml16::write(FILE* output) const // Fwrite
{
    int rc = Fwrite(const_cast<FBFR*>(as_fbfr()), output);
    if(rc == -1)
    {
        throw last_error("Fwrite");
    }
}

void fml16::read(FILE* input, long size_hint) // Fread
{
    reserve(size_hint);
    int rc = Fread(as_fbfr(), input);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        reserve(size() * 2);
        rc = Fread(as_fbfr(), input);
    }
    if(rc == -1)
    {
        throw last_error("Fread");
    }
}

//------------------------INDEXING------------------------------------
void fml16::index(FLDOCC interval) // Findex
{
    if(buffer_)
    {
        int rc = Findex(as_fbfr(), interval);
        if(rc == -1)
        {
            throw last_error("Findex");
        }
    }
}

FLDOCC fml16::unindex() // Funindex
{
    if(!buffer_)
    {
        return 0; 
    }
    FLDOCC result = Funindex(as_fbfr());
    if(result == -1)
    {
        throw last_error("Funindex");
    }
    return result;
}

void fml16::restore_index(FLDOCC index_element_count) // Frstrindex
{
    if(buffer_)
    {
        int rc = Frstrindex(as_fbfr(), index_element_count);
        if(rc == -1)
        {
            throw last_error("Frstrindex");
        }
    }
}


//----------------------------BYTES USED-----------------------------------

long fml16::index_size() const // Fidxused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fidxused(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fidxused");
    }
    return result;
}

long fml16::size() const // Fsizeof
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fsizeof(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fsizeof");
    }
    return result;
}

long fml16::unused_size() const // Funused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Funused(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Funused");
    }
    return result;
}

long fml16::used_size() const // Fused
{
    if(!buffer_)
    {
        return 0;
    }
    long result = Fused(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fused");
    }
    return result;
}

//-------------------------FIELD COUNTS--------------------------------
FLDOCC fml16::field_count() const // Fnum
{
    if(!buffer_)
    {
        return 0;
    }
    FLDOCC result = Fnum(const_cast<FBFR*>(as_fbfr()));
    if(result == -1)
    {
        throw last_error("Fnum");
    }
    return result;
}

FLDOCC fml16::count(FLDID id) const // Foccur
{
    if(!buffer_)
    {
        return 0;
    }
    FLDOCC result = Foccur(const_cast<FBFR*>(as_fbfr()), id);
    if(result == -1)
    {
        throw last_error("Foccur");
    }
    return result;
}

bool fml16::has(FLDID id, FLDOCC oc) const // Fpres
{
    if(!buffer_)
    {
        return false;
    }
    return Fpres(const_cast<FBFR*>(as_fbfr()), id, oc);
}

//--------------------FIELD VALUE SIZE------------------------------
long fml16::field_value_size(FLDID id, FLDOCC oc) const
{
    if(!buffer_)
    {
        throw error(FNOTPRES, "field_value_size - null buffer");
    }
    long result = Flen(const_cast<FBFR*>(as_fbfr()), id, oc);
    if(result == -1)
    {
        throw last_error("Flen");
    }
    return result;
}

//-------------------ELEMENT REMOVAL------------------------------
bool fml16::erase(FLDID id, FLDOCC oc) // Fdel
{
    if(!buffer_)
    {
        return false;
    }

    int rc = Fdel(as_fbfr(), id, oc);
    if(rc == -1)
    {
        if(Ferror == FNOTPRES)
        {
            return false;
        }
        throw last_error("Fdel");
    }
    return true;
}

bool fml16::erase(FLDID id) // Fdelall
{
    if(!buffer_)
    {
        return false;
    }

    int rc = Fdelall(as_fbfr(), id);
    if(rc == -1)
    {
        if(Ferror == FNOTPRES)
        {
            return false;
        }
        throw last_error("Fdelall");
    }
    return true;
}

void fml16::erase(vector<FLDID> ids) // Fdelete
{
    if(buffer_ && !ids.empty())
    {
        ids.push_back(BADFLDID);
        int rc = Fdelete(as_fbfr(), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fdelete");
        }
    }
}

void fml16::erase_all_but(vector<FLDID> ids) // Fproj
{
    if(buffer_)
    {
        ids.push_back(BADFLDID);
        int rc = Fproj(as_fbfr(), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fproj");
        }
    }
}

void fml16::clear() // Finit
{
    if(buffer_)
    {
        int rc = Finit(as_fbfr(), size());
        if(rc == -1)
        {
            throw last_error("Finit");
        }
    }
}

//-------------------OPERATIONS ON ENTIRE DATA STRUCTURES----------------------
fml16& fml16::operator +=(fml16 const& x) // Fconcat
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fconcat(as_fbfr(), const_cast<FBFR*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fconcat");
        }
    }
    return *this;
}

void fml16::join(fml16 const& x) // Fjoin
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fjoin(as_fbfr(), const_cast<FBFR*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fjoin");
        }
    }
    else
    {
        clear();
    }
}

void fml16::outer_join(fml16 const& x) // Fojoin
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fojoin(as_fbfr(), const_cast<FBFR*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fojoin");
        }
    }   
}

fml16 fml16::project(vector<FLDID> ids) const // Fprojcpy
{
    fml16 result;
    if(buffer_)
    {
        ids.push_back(BADFLDID);
        result.reserve(used_size());
        int rc = Fprojcpy(result.as_fbfr(), const_cast<FBFR*>(as_fbfr()), &ids[0]);
        if(rc == -1)
        {
            throw last_error("Fprojcpy");
        }
    }
    return result;
}

void fml16::update(fml16 const& x) // Fupdate
{
    if(x)
    {
        reserve(used_size() + x.used_size());
        int rc = Fupdate(as_fbfr(), const_cast<FBFR*>(x.as_fbfr()));
        if(rc == -1)
        {
            throw last_error("Fupdate");
        }
    }
}


bool fml16::is_fielded() const noexcept
{
    return Fielded(const_cast<FBFR*>(as_fbfr()));
}


//-----------------ADD----------------------------
void fml16::add(FLDID id, short x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_SHORT);
}

void fml16::add(FLDID id, long x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_LONG);
}

void fml16::add(FLDID id, char x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_CHAR);
}

void fml16::add(FLDID id, float x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_FLOAT);
}

void fml16::add(FLDID id, double x)
{
    convert_and_add(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_DOUBLE);
}

void fml16::add(FLDID id, std::string const& x)
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    convert_and_add(id, const_cast<char*>(x.data()), x.size(), type);
}

void fml16::append(FLDID id, short x)
{
    check_field_type(id, FLD_SHORT);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml16::append(FLDID id, long x)
{
    check_field_type(id, FLD_LONG);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}  

void fml16::append(FLDID id, char x)
{
    check_field_type(id, FLD_CHAR);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml16::append(FLDID id, float x)
{
    check_field_type(id, FLD_FLOAT);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml16::append(FLDID id, double x)
{
    check_field_type(id, FLD_DOUBLE);
    append(id, reinterpret_cast<char*>(&x), sizeof(x));
}

void fml16::append(FLDID id, std::string const& x) 
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

void fml16::set(FLDID id, short x, FLDOCC oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_SHORT);
}
void fml16::set(FLDID id, long x, FLDOCC oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_LONG);
}
void fml16::set(FLDID id, char x, FLDOCC oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_CHAR);
}
void fml16::set(FLDID id, float x, FLDOCC oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_FLOAT);
}
void fml16::set(FLDID id, double x, FLDOCC oc)
{
    convert_and_set(id, reinterpret_cast<char*>(&x), sizeof(x), oc, FLD_DOUBLE);
}
void fml16::set(FLDID id, std::string const& x, FLDOCC oc) // could be string or carray depending on id
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    convert_and_set(id, const_cast<char*>(x.data()), x.size(), oc, type);    
}

//----------------------GET--------------------------------
short fml16::get_short(FLDID id, FLDOCC oc) const
{
    short x = 0;
    FLDLEN len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_SHORT);
    return x;
}

long fml16::get_long(FLDID id, FLDOCC oc) const
{
    long x = 0;
    FLDLEN len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_LONG);
    return x;
}

char fml16::get_char(FLDID id, FLDOCC oc) const
{
    char x = 0;
    FLDLEN len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_CHAR);
    return x;
}

float fml16::get_float(FLDID id, FLDOCC oc) const
{
    float x = 0;
    FLDLEN len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_FLOAT);
    return x;
}

double fml16::get_double(FLDID id, FLDOCC oc) const
{
    double x = 0;
    FLDLEN len = sizeof(x);
    get_and_convert(id, oc, reinterpret_cast<char*>(&x), &len, FLD_DOUBLE);
    return x;
}

string fml16::get_string(FLDID id, FLDOCC oc) const
{
    int type = field_type(id);
    if(type == FLD_CARRAY || type == FLD_STRING)
    {
        FLDLEN len = 0;
        char* loc = find_value(id, oc, &len);
        return type == FLD_CARRAY ? string(loc, len) : string(loc); 
    }
    else
    {
        string x;
        FLDLEN len = 18; // likely within small buffer optimization limit
        x.resize(len);
        
        int rc = CFget(const_cast<FBFR*>(as_fbfr()),
                         id,
                         oc,
                         const_cast<char*>(x.data()),
                         &len,
                         FLD_STRING);
        if(rc == -1 && Ferror == FNOSPACE)
        {
            // must be a big number
            len = 36;
            x.resize(len);
            rc = CFget(const_cast<FBFR*>(as_fbfr()),
                        id,
                        oc,
                        const_cast<char*>(x.data()),
                        &len,
                        FLD_STRING);
        }
        if(rc == -1)
        {
            throw last_error("CFget");
        }
        trim_to_null_terminator(x);
        return x;
    }
}


FLDOCC fml16::find(FLDID id, short x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_SHORT);
}
FLDOCC fml16::find(FLDID id, long x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_LONG);
}
FLDOCC fml16::find(FLDID id, char x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_CHAR);
}
FLDOCC fml16::find(FLDID id, float x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_FLOAT);
}
FLDOCC fml16::find(FLDID id, double x) const
{
    return convert_and_find_occurrence(id, reinterpret_cast<char*>(&x), sizeof(x), FLD_DOUBLE);
}
FLDOCC fml16::find(FLDID id, std::string const& x) const
{
    int type = field_type(id) == FLD_CARRAY ? FLD_CARRAY : FLD_STRING;
    int size = type == FLD_CARRAY ? x.size() : 0;
    return convert_and_find_occurrence(id, const_cast<char*>(x.data()), size, type);
}
FLDOCC fml16::find_match(FLDID id, std::string const& x) const
{
    check_field_type(id, FLD_STRING);
    return find_occurence(id, x.data(), 1);
}

std::pair<FLDOCC,short> fml16::get_last_short(FLDID id) const
{
    check_field_type(id, FLD_SHORT);
    short x = 0;
    FLDLEN len = sizeof(x);
    FLDOCC oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC,long> fml16::get_last_long(FLDID id) const
{
    check_field_type(id, FLD_LONG);
    long x = 0;
    FLDLEN len = sizeof(x);
    FLDOCC oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC,char> fml16::get_last_char(FLDID id) const
{
    check_field_type(id, FLD_CHAR);
    char x = 0;
    FLDLEN len = sizeof(x);
    FLDOCC oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC,float> fml16::get_last_float(FLDID id) const
{
    check_field_type(id, FLD_FLOAT);
    float x = 0;
    FLDLEN len = sizeof(x);
    FLDOCC oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC,double> fml16::get_last_double(FLDID id) const
{
    check_field_type(id, FLD_DOUBLE);
    double x = 0;
    FLDLEN len = sizeof(x);
    FLDOCC oc = get_last(id, reinterpret_cast<char*>(&x), &len);  
    return {oc,x};
}
std::pair<FLDOCC,std::string> fml16::get_last_string(FLDID id) const
{
    int type = field_type(id);
    if(type != FLD_CARRAY && type != FLD_STRING)
    {
        throw runtime_error("cannot extract string value from a " +
            field_type_name(id) + " field [" +
            field_name(id) + "]");
    }
    FLDOCC oc = -1;
    FLDLEN len = 0;
    const char* val = Ffindlast(const_cast<FBFR*>(as_fbfr()), id, &oc, &len);
    if(!val)
    {
        throw last_error("Ffindlast");
    }
    
    if(type == FLD_STRING && len > 0)
    {
        --len;
    }
 
    return {oc,string{val, len}};
}


bool fml16::next_field(field_info& x) const
{
    int rc = Fnext(const_cast<FBFR*>(as_fbfr()), &x.id, &x.oc, nullptr, nullptr);
    if(rc == -1)
    {
        throw last_error("Fnext");
    }
    return rc;
}


fml16::boolean_expression::boolean_expression(fml16::boolean_expression const& x)
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


fml16::boolean_expression& fml16::boolean_expression::operator=(fml16::boolean_expression const& x)
{
    fml16::boolean_expression tmp(x);
    *this = move(tmp);
    return *this;
}

fml16::boolean_expression::boolean_expression(std::string const& x)
{
    compile(x);
}

fml16::boolean_expression& fml16::boolean_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

void fml16::boolean_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        Fboolpr(tree_.get(), f);
    }
}

bool fml16::boolean_expression::evaluate(fml16 const& x) const
{
    int result = Fboolev(const_cast<FBFR*>(x.as_fbfr()), tree_.get());
    if(result == -1)
    {
        throw last_error("Fboolev");
    }
    return result;
}

bool fml16::boolean_expression::operator()(fml16 const& x) const
{
    return evaluate(x);
}


void fml16::boolean_expression::free() noexcept
{
    tree_.reset();
}

void fml16::boolean_expression::compile(std::string const& x)
{
    tree_.reset(Fboolco(const_cast<char*>(x.c_str())));
    if(!tree_)
    {
        throw last_error("Fboolco");
    }
}

unsigned fml16::boolean_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}


fml16::arithmetic_expression::arithmetic_expression(fml16::arithmetic_expression const& x)
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


fml16::arithmetic_expression& fml16::arithmetic_expression::operator=(fml16::arithmetic_expression const& x)
{
    fml16::arithmetic_expression tmp(x);
    *this = move(tmp);
    return *this;
}

fml16::arithmetic_expression::arithmetic_expression(std::string const& x)
{
    compile(x);
}

fml16::arithmetic_expression& fml16::arithmetic_expression::operator=(std::string const& x)
{
    compile(x);
    return *this;
}

void fml16::arithmetic_expression::print(FILE* f) const noexcept
{
    if(tree_ && f)
    {
        Fboolpr(tree_.get(), f);
    }
}

double fml16::arithmetic_expression::evaluate(fml16 const& x) const
{
    double result = Ffloatev(const_cast<FBFR*>(x.as_fbfr()), tree_.get());
    if(result == -1)
    {
        throw last_error("Ffloatev");
    }
    return result;
}

double fml16::arithmetic_expression::operator()(fml16 const& x) const
{
    return evaluate(x);
}

void fml16::arithmetic_expression::free() noexcept
{
    tree_.reset();
}

void fml16::arithmetic_expression::compile(std::string const& x)
{
    tree_.reset(Fboolco(const_cast<char*>(x.c_str())));
    if(!tree_)
    {
        throw last_error("Fboolco");
    }
}

unsigned fml16::arithmetic_expression::size() const
{
    if(!tree_)
    {
        return 0;
    }
    return make_16bit_unsigned(tree_.get()[0], tree_.get()[1]);
}


//---------------------PRIVATE----------------------------------------

void fml16::check_field_type(FLDID fieldid, int expected_type)
{
    if(field_type(fieldid) != expected_type)
    {
        
        throw runtime_error("cannot convert " + field_type_name_from_type(expected_type) + " value to/from a " +
            field_type_name(fieldid) + " field [" +
            field_name(fieldid) + "]");
    }
}

void fml16::convert_and_add(FLDID fieldid, const char* value, FLDLEN len, int type)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = CFadd(as_fbfr(), fieldid, const_cast<char*>(value), len, type);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = CFadd(as_fbfr(), fieldid, const_cast<char*>(value), len, type);
    }
    if(rc == -1)
    {
        throw last_error("CFadd");
    }
}

void fml16::add(FLDID fieldid, const char* value, FLDLEN len)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fadd(as_fbfr(), fieldid, const_cast<char*>(value), len);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fadd(as_fbfr(), fieldid, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fadd");
    }
}

void fml16::get_and_convert(FLDID fieldid, FLDOCC oc, char* buf, FLDLEN* len, int type) const
{
    int rc = CFget(const_cast<FBFR*>(as_fbfr()), fieldid, oc, buf, len, type);
    if(rc == -1)
    {
        throw last_error("CFget");
    }
}

void fml16::get(FLDID fieldid, FLDOCC oc, char* buf, FLDLEN* len) const
{
    string raw(buf,*len);
    int rc = Fget(const_cast<FBFR*>(as_fbfr()), fieldid, oc, buf, len);
    if(rc == -1)
    {
        throw last_error("Fget");
    }
}

void fml16::append(FLDID fieldid, const char* value, FLDLEN len)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fappend(as_fbfr(), fieldid, const_cast<char*>(value), len);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fappend(as_fbfr(), fieldid, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fappend");
    }
}

void fml16::convert_and_set(FLDID fieldid, const char* value, FLDLEN len, FLDOCC oc, int type)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = CFchg(as_fbfr(), fieldid, oc, const_cast<char*>(value), len, type);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = CFchg(as_fbfr(), fieldid, oc, const_cast<char*>(value), len, type);
    }
    if(rc == -1)
    {
        throw last_error("Fchg");
    }
}
void fml16::set(FLDID fieldid, const char* value, FLDLEN len, FLDOCC oc)
{
    if(!buffer_)
    {
        reserve(1, len);
    }
    int rc = Fchg(as_fbfr(), fieldid, oc, const_cast<char*>(value), len);
    if(rc == -1 && Ferror == FNOSPACE)
    {
        long current_size = size();
        reserve(max(current_size + bytes_needed(1, len), 2 * current_size));
        rc = Fchg(as_fbfr(), fieldid, oc, const_cast<char*>(value), len);
    }
    if(rc == -1)
    {
        throw last_error("Fchg");
    }
}


char* fml16::find_value(FLDID id, FLDOCC oc, FLDLEN* len) const
{
    char* result = Ffind(const_cast<FBFR*>(as_fbfr()), id, oc, len);
    if(!result)
    {
        throw last_error("Ffind");
    }
    return result;
}

FLDOCC fml16::get_last(FLDID fieldid, char* buf, FLDLEN* len) const
{
    FLDOCC oc = -1;
    int rc = Fgetlast(const_cast<FBFR*>(as_fbfr()), fieldid, &oc, buf, len);
    if(rc == -1)
    {
        throw last_error("Fgetlast");
    }
    return oc;
}

FLDOCC fml16::find_occurence(FLDID id, const char* value, FLDLEN len) const
{
    FLDOCC result =  Ffindocc(const_cast<FBFR*>(as_fbfr()), id, const_cast<char*>(value), len);
    if(result == -1 && Ferror != FNOTPRES)
    {
        throw last_error("Ferror");
    }
    return result;
}


FLDOCC fml16::convert_and_find_occurrence(FLDID id, const char* value, FLDLEN len, int type) const
{
    FLDOCC result =  CFfindocc(const_cast<FBFR*>(as_fbfr()), id, const_cast<char*>(value), len, type);
    if(result == -1 && Ferror != FNOTPRES)
    {
        throw last_error("CFfindocc");
    }
    return result;
}




//---------------------NON-MEMBER--------------------------------------

int compare(fml16 const& a, fml16 const& b)
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
        int rc = Fcmp(const_cast<FBFR*>(a.as_fbfr()), const_cast<FBFR*>(b.as_fbfr()));
        if(rc == -2)
        {
            throw last_error("Fcmp");
        }
        return rc;
    }
}

bool operator==(fml16 const& a, fml16 const& b) { return compare(a,b) == 0; }
bool operator!=(fml16 const& a, fml16 const& b) { return compare(a,b) != 0; }
bool operator<(fml16 const& a, fml16 const& b) { return compare(a,b) < 0; }
bool operator>(fml16 const& a, fml16 const& b) { return compare(a,b) > 0; }
bool operator<=(fml16 const& a, fml16 const& b) { return compare(a,b) <= 0; }
bool operator>=(fml16 const& a, fml16 const& b) { return compare(a,b) >= 0; }

}
