/** @file convert.hpp
Routines for converting between various buffer types. @ingroup buffers */
#pragma once

#include <string>
// 32 (fml32.h) header must come before 16 (fml16.h)
#include "tux/fml32.hpp" 
#include "tux/fml16.hpp"
#include "tux/record.hpp"
#include "tux/xml.hpp"


namespace tux
{
    

fml32 to_fml32(fml16 const&); /**< Convert an fml16 to an fml32 [@c F16to32]. @ingroup buffers */
fml16 to_fml16(fml32 const&); /**< Convert an fml32 to an fml16 [@c F32to16]. @ingroup buffers */

// fml <-> view    
template <typename T> T to_struct(fml16 const& x); /**< Convert an fml16 to a struct (defined in a view) [@c Fvftos]. @ingroup buffers */
template <typename T> T to_struct(fml32 const& x); /**< Convert an fml32 to a struct (defined in a view) [@c Fvftos32]. @ingroup buffers */
/** Convert a struct (defined in a view) to an fml16 [@c Fvstof].
@param dest existing fml16
@param mode valid options include FUPDATE, FJOIN, FOJOIN, FCONCAT
@ingroup buffers */
template <typename T> void convert(T const& src, fml16& dest, int mode);
/** Convert a struct (defined in a view) to an fml32 [@c Fvstof32].
@param dest existing fml32
@param mode valid options include FUPDATE, FJOIN, FOJOIN, FCONCAT
@ingroup buffers */
template <typename T> void convert(T const& src, fml32& dest, int mode);
template <typename T> fml16 to_fml16(T const& x); /**< Convert a struct (defined in a view) to an fml16 [@c Fvstof] @ingroup buffers */
template <typename T> fml32 to_fml32(T const& x); /**< Convert a struct (defined in a view) to an fml32 [@c Fvstof32] @ingroup buffers */

#if TUXEDO_VERSION >= 1222
/** Convert a record to an fml32 [@c Fvrtof32].
@ since Tuxedo 12.2.2
@param dest existing fml32
@param mode valid options include FUPDATE, FJOIN, FOJOIN, FCONCAT
@ingroup buffers */
void convert(record const& src, fml32& dest, int mode, std::string const& field_name = ""); 
/** Convert a record to an fml32 [@c Fvrtof32].
@since Tuxedo 12.2.2
@ingroup buffers */
fml32 to_fml32(record const& x, std::string const& field_name = "");
/** Convert an fml32 to a record [@c Fvrtof32].
@since Tuxedo 12.2.2
@param dest the destination record
@param field_name optional name of item to be transferred (defaults to the entire record)
@pre @c dest must already be allocated.
@ingroup buffers */
void convert(fml32 const& src, record& dest, std::string const& field_name = "");
/** Convert an fml32 to a record [@c Fvrtof32].
@since Tuxedo 12.2.2
@param record_type struct name
@param field_name optional name of item to be transferred (defaults to the entire record)
@ingroup buffers */
record to_record(fml32 const& x, std::string const& record_type, std::string const& field_name = "");

/** Convert from a struct (defined in a view) to a record [@c Fvstor32].
@since Tuxedo 12.2.2
@param record_type struct name
@param field_name optional name of item to be transferred (defaults to the entire record)
@ingroup buffers */
template<typename T> record to_record(T const& x, std::string const& record_type, std::string const& field_name = "");
/** Convert from a record to a struct (defined in a view)  [@c Fvrtos32].
@since Tuxedo 12.2.2
@param field_name optional name of item to be transferred (defaults to the entire record)
@ingroup buffers */
template<typename T> T to_struct(record const& x, std::string const& field_name = "");

// view <-> record(raw data)
// UNTESTED -- beginning here.
//template<typename T> void view16_to_record(T const& src, char* dest, long destlen);
//template<typename T> void view32_to_record(T const& src, char* dest, long destlen);

//template<typename T> void record_to_view16(char* src, T const& dest);
//template<typename T> void record_to_view32(char* src, T const& dest);

//void set_record_codeset_16(char* translation_table);
//void set_record_codeset_32(char* translation_table);
// UNTESTED -- end here.
#endif

/** Convert an fml16 to an xml [@c tpfmltoxml].
@param root_name optional root element name for result
@ingroup buffers */
xml to_xml(fml16 const& x, std::string const& root_name = "");
/** Convert an fml32 to an xml [@c tpfml32toxml].
@param root_name optional root element name for result
@ingroup buffers */
xml to_xml(fml32 const& x, std::string const& root_name = "");
/** Convert xml to fml16 [@c tpxmltofml].
@param flags valid flags include
@arg TPXPARSNEVER
@arg TPXPARSALWAYS
@arg TPXPARSSCHFULL
@arg TPXPARSCONFATAL
@arg TPXPARSNSPACE
@arg TPXPARSDOSCH
@arg TPXPARSEREFN
@arg TPXPARSNOEXIT
@arg TPXPARSNOINCWS
@arg TPXPARSCACHESET
@arg TPXPARSCACHERESET
@returns the fml16 and root element name
@ingroup buffers */
std::pair<fml16, std::string> to_fml16(xml const& x, long flags = TPNOFLAGS);
/** Convert xml to fml32 [@c tpxmltofml32].
@param flags valid flags include
@arg TPXPARSNEVER
@arg TPXPARSALWAYS
@arg TPXPARSSCHFULL
@arg TPXPARSCONFATAL
@arg TPXPARSNSPACE
@arg TPXPARSDOSCH
@arg TPXPARSEREFN
@arg TPXPARSNOEXIT
@arg TPXPARSNOINCWS
@arg TPXPARSCACHESET
@arg TPXPARSCACHERESET
@returns the fml16 and root element name
@ingroup buffers */
std::pair<fml32, std::string> to_fml32(xml const& x, long flags = TPNOFLAGS);



//----------------TEMPLATE DEFS ---------------------------

//----------------FML TO STRUCT ---------------------------
template <typename T>
T to_struct(fml16 const& x)
{
    T result;
    int rc = Fvftos(const_cast<FBFR*>(x.as_fbfr()),
                     reinterpret_cast<char*>(&result),
                     const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvftos");
    }
    return result;
}

template <typename T>
T to_struct(fml32 const& x)
{
    T result;
    int rc = Fvftos32(const_cast<FBFR32*>(x.as_fbfr()),
                     reinterpret_cast<char*>(&result),
                     const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvftos32");
    }
    return result;
}    


//---------------STRUCT TO FML----------------------------
template <typename T>
void convert(T const& src, fml16& dest, int mode) // mode = FUPDATE, FJOIN, FOJOIN, FCONCAT 
{
    if(!dest)
    {
        dest.reserve(0);
    }
    int rc = Fvstof(dest.as_fbfr(),
                    reinterpret_cast<char*>(const_cast<T*>(&src)),
                    mode,
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1 && Ferror == FNOSPACE)
    {
        dest.reserve(sizeof(T) * 4);
        rc = Fvstof(dest.as_fbfr(),
                    reinterpret_cast<char*>(const_cast<T*>(&src)),
                    mode,
                    const_cast<char*>(type_name<T>::value()));
    }
    if(rc == -1)
    {
        throw fml16::last_error("Fvstof");
    }   
}

template <typename T>
void convert(T const& src, fml32& dest, int mode) // mode = FUPDATE, FJOIN, FOJOIN, FCONCAT 
{
    if(!dest)
    {
        dest.reserve(0);
    }
    int rc = Fvstof32(dest.as_fbfr(),
                    reinterpret_cast<char*>(const_cast<T*>(&src)),
                    mode,
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1 && Ferror32 == FNOSPACE)
    {
        dest.reserve(sizeof(T) * 4);
        rc = Fvstof32(dest.as_fbfr(),
                    reinterpret_cast<char*>(const_cast<T*>(&src)),
                    mode,
                    const_cast<char*>(type_name<T>::value()));
    }
    if(rc == -1)
    {
        throw fml32::last_error("Fvstof32");
    }   
}

template <typename T>
fml16 to_fml16(T const& x) 
{
    fml16 result;
    convert(x, result, FUPDATE);
    return result;
}

template <typename T>
fml32 to_fml32(T const& x) 
{
    fml32 result;
    convert(x, result, FUPDATE);
    return result;
}

#if TUXEDO_VERSION >= 1222
//----------------STRUCT TO RECORD --------------------------------
template<typename T>
record to_record(T const& x, std::string const& record_type, std::string const& field_name)
{
    record result{record_type};
    int rc = Fvstor32(result.as_record(),
                      field_name.empty() ? nullptr : const_cast<char*>(field_name.c_str()),
                      reinterpret_cast<char*>(const_cast<T*>(&x)),
                      const_cast<char*>(type_name<T>::value()),
                      TPNOFLAGS);

    if(rc == -1)
    {
        throw fml32::last_error("Fvstor32");
    }
    return result;
}

//----------------RECORD TO STRUCT --------------------------------
template<typename T>
T to_struct(record const& x, std::string const& field_name)
{
    T result;
    int rc = Fvrtos32(const_cast<RECORD*>(x.as_record()),
                  field_name.empty() ? nullptr : const_cast<char*>(field_name.c_str()),
                  reinterpret_cast<char*>(&result),
                  const_cast<char*>(type_name<T>::value()),
                  TPNOFLAGS);
    if(rc == -1)
    {
        throw fml32::last_error("Fvrtos32");
    }
    return result;
}

// 
/*template<typename T> void view16_to_record(T const& src, char* dest, long destlen)
{
    int rc = Fvstot(reinterpret_cast<char*>(const_cast<T*>(&src)),
                    dest,
                    destlen,
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvstot");
    }
}
template<typename T> void view32_to_record(T const& src, char* dest, long destlen)
{
    int rc = Fvstot32(reinterpret_cast<char*>(const_cast<T*>(&src)),
                      dest,
                      destlen,
                      const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvstot32");
    }
}

template<typename T> void record_to_view16(const char* src, T& dest)
{
    int rc = Fvttos(reinterpret_cast<char*>(&dest),
                    const_cast<char*>(src),
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml16::last_error("Fvttos");
    }
}
template<typename T> void record_to_view32(const char* src, T& dest)
{
    int rc = Fvttos32(reinterpret_cast<char*>(&dest),
                    const_cast<char*>(src),
                    const_cast<char*>(type_name<T>::value()));
    if(rc == -1)
    {
        throw fml32::last_error("Fvttos32");
    }
}

void set_record_codeset_16(unsigned char* translation_table)
{
    int rc = Fcodeset(translation_table);
    if(rc == -1)
    {
        throw fml16::last_error("Fcodeset");
    }  
}

void set_record_codeset_32(unsigned char* translation_table)
{
    int rc = Fcodeset32(translation_table);
    if(rc == -1)
    {
        throw fml32::last_error("Fcodeset32");
    }  
}
*/
#endif

}
