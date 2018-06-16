#include "tux/convert.hpp"


using namespace std;

namespace tux
{
    
fml32 to_fml32(fml16 const& a)
{
    fml32 b;
    if(a)
    {
        long len = a.size();
        b.reserve(len);
        int rc = F16to32(b.as_fbfr(), const_cast<FBFR*>(a.as_fbfr()));
        if(rc == -1 && Ferror == FNOSPACE)
        {
            long field_count = a.field_count();
            b.reserve(field_count, len * 2);
            rc = F16to32(b.as_fbfr(), const_cast<FBFR*>(a.as_fbfr()));     
        }
        if(rc == -1)
        {
            throw fml16::last_error("F16to32");
        }
    }
    return b;
}

fml16 to_fml16(fml32 const& a)
{
    fml16 b;
    if(a)
    {
        long len = a.size();
        b.reserve(len);
        int rc = F32to16(b.as_fbfr(), const_cast<FBFR32*>(a.as_fbfr()));
        if(rc == -1 && Ferror == FNOSPACE)
        {
            long field_count = a.field_count();
            b.reserve(field_count, len * 2);
            rc = F32to16(b.as_fbfr(), const_cast<FBFR32*>(a.as_fbfr()));     
        }
        if(rc == -1)
        {
            throw fml16::last_error("F32to16");
        }
    }
    return b;
}

#if TUXEDO_VERSION >= 1222
void convert(record const& src, fml32& dest, int mode, std::string const& field_name)
{
    if(src)
    {
        if(!dest)
        {
            dest.reserve(0);
        }
        int rc = Fvrtof32(dest.as_fbfr(),
                         const_cast<RECORD*>(src.as_record()),
                         field_name.empty() ? nullptr : const_cast<char*>(field_name.c_str()),
                         mode,
                         TPNOFLAGS);
        if(rc == -1 && Ferror32 == FNOSPACE)
        {
            dest.reserve(dest.size() + src.buffer().size() * 4);
            rc = Fvrtof32(dest.as_fbfr(),
                            const_cast<RECORD*>(src.as_record()),
                            field_name.empty() ? nullptr : const_cast<char*>(field_name.c_str()),
                            mode,
                            TPNOFLAGS);
        }
        if(rc == -1)
        {
            throw fml32::last_error("Fvrtof32");
        }
    }
    else
    {
        if(mode == FJOIN)
        {
            dest.clear();
        }
    }



}

fml32 to_fml32(record const& x, std::string const& field_name)
{
    fml32 result;
    convert(x, result, FUPDATE, field_name);
    return result;
}

void convert(fml32 const& src, record& dest, std::string const& field_name)
{
    if(src)
    {
        int rc = Fvftor32(const_cast<FBFR32*>(src.as_fbfr()),
                          dest.as_record(),
                          field_name.empty() ? nullptr : const_cast<char*>(field_name.c_str()),
                          TPNOFLAGS);
        if(rc == -1)
        {
            throw fml32::last_error("Fvftor32");
        }
    }
}

record to_record(fml32 const& x, std::string const& record_type, std::string const& field_name)
{
    record result{record_type};
    convert(x, result, field_name);
    return result;
}
#endif

xml to_xml(fml16 const& x, std::string const& root_name)
{
    xml result;
    if(x)
    {
        result.reserve(512);
        char* xmlbufp = result.buffer().release();
        int rc = tpfmltoxml(const_cast<FBFR*>(x.as_fbfr()),
                            nullptr,
                            root_name.empty() ? nullptr : const_cast<char*>(root_name.c_str()),
                            &xmlbufp,
                            TPNOFLAGS);
        result.buffer().acquire(xmlbufp, strlen(xmlbufp) + 1);
        if(rc == -1)
        {
            throw last_error("tpfmltoxml");
        }
    }
    return result;
}

xml to_xml(fml32 const& x, std::string const& root_name)
{
    xml result;
    if(x)
    {
        result.reserve(512);
        char* xmlbufp = result.buffer().release();
        int rc = tpfml32toxml(const_cast<FBFR32*>(x.as_fbfr()),
                              nullptr,
                              root_name.empty() ? nullptr : const_cast<char*>(root_name.c_str()),
                              &xmlbufp,
                              TPNOFLAGS);
        result.buffer().acquire(xmlbufp, strlen(xmlbufp) + 1);
        if(rc == -1)
        {
            throw last_error("tpfml32toxml");
        }
    }
    return result;
}

std::pair<fml16, std::string> to_fml16(xml const& x, long flags)
{
    fml16 result;
    std::string root;
    if(x)
    {
        result.reserve(0);
        FBFR* fmlbufp = reinterpret_cast<FBFR*>(result.buffer().release());
        char* rootp = nullptr;
        int rc = tpxmltofml(const_cast<char*>(x.data()),
                            nullptr,
                            &fmlbufp,
                            &rootp,
                            flags);
        result.buffer().acquire(reinterpret_cast<char*>(fmlbufp));
        if(rootp)
        {
            root = rootp;
            delete [] rootp;
        }
        if(rc == -1)
        {
            throw last_error("tpxmltofml");
        }
    }
    return {result, root};
}

std::pair<fml32, std::string> to_fml32(xml const& x, long flags)
{
    fml32 result;
    std::string root;
    if(x)
    {
        result.reserve(0);
        FBFR32* fmlbufp = reinterpret_cast<FBFR32*>(result.buffer().release());
        char* rootp = nullptr;
        int rc = tpxmltofml32(const_cast<char*>(x.data()),
                              nullptr,
                              &fmlbufp,
                              &rootp,
                              flags);
        result.buffer().acquire(reinterpret_cast<char*>(fmlbufp));
        if(rootp)
        {
            root = rootp;
            delete [] rootp;
        }
        if(rc == -1)
        {
            throw last_error("tpxmltofml32");
        }
    }
    return {result, root};
}


}
