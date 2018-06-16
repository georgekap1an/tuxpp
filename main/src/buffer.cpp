#include "tux/buffer.hpp"
#include "tux/util.hpp"

#include <iostream>

using namespace std;

namespace tux
{
   
const char* buffer::default_type = "FML32";
const char* buffer::default_subtype = nullptr;
long buffer::default_size = 0;

buffer::buffer(buffer&& x) noexcept
{
  acquire(x.data_, x.data_size_);
  x.data_ = nullptr;
  x.data_size_ = 0;
}

buffer& buffer::operator=(buffer&& x) noexcept
{
  if(&x != this)
  {
    acquire(x.data_, x.data_size_);
    x.data_ = nullptr;
    x.data_size_ = 0;
  }
  return *this;
}

buffer::~buffer() noexcept
{
  free();
}

buffer::buffer(const char* type,
       const char* subtype,
       long size)
{
  //stopwatch s;
  //s.start();
  data_ = tpalloc(const_cast<char*>(type),
                  const_cast<char*>(subtype),
                  size);
  //s.stop();
  //cout << "tpalloc: " << s.elapsed().count() << "us" << endl;
  if(!data_)
  {
    throw last_error("tpalloc");
  }
  else
  {
    //cout << "tpalloc @" << (void*)data_  << endl;
  }
}

buffer::buffer(char* data, long data_size) noexcept
{
  acquire(data, data_size);
}
  
void buffer::alloc(const char* type,
       const char* subtype,
       long size)
{
  buffer tmp(type, subtype, size);
  *this = move(tmp);
}

void buffer::alloc_default()
{
  alloc(buffer::default_type, buffer::default_subtype, buffer::default_size);  
}

void buffer::realloc(long size)
{
  data_ = tprealloc(data_, size);
  if(!data_)
  {
    throw last_error("tprealloc");
  }
  //cout << "tprealloc @" << (void*)data_ << endl;
}

void buffer::free() noexcept
{
  if(data_)
  {
      //cout << "tpfree @" << (void*)data_ << endl;
      tpfree(data_);
  }
  data_ = nullptr;
  data_size_ = 0;
}

void buffer::acquire(char* data, long data_size) noexcept
{
  free();
  data_ = data;
  data_size_ = data_size;
}

char* buffer::release() noexcept
{
  //cout << "releasing " << (void*)data_ << endl;
  char* result = data_;
  data_ = nullptr;
  data_size_ = 0;
  return result;
}

buffer::operator bool() const noexcept
{
  return data_;
}

string buffer::type() const
{
  if(!data_)
  {
    return "";
  }
  string type(8, ' ');
  long rc = tptypes(const_cast<char*>(data_),
                    const_cast<char*>(type.data()),
                    nullptr);
  if(rc == -1)
  {
    throw last_error("tptypes");
  }
  trim_to_null_terminator(type);
  return type;
}

string buffer::subtype() const
{
  if(!data_)
  {
    return "";
  }
  string subtype(16, ' ');
  long rc = tptypes(const_cast<char*>(data_),
                    nullptr,
                    const_cast<char*>(subtype.data()));
  if(rc == -1)
  {
    throw last_error("tptypes");
  }
  trim_to_null_terminator(subtype);
  return subtype;
}

long buffer::size() const
{
  if(!data_)
  {
    return 0;
  }
  long size = tptypes(const_cast<char*>(data_),
                    nullptr,
                    nullptr);
  if(size == -1)
  {
    throw last_error("tptypes");
  }
  return size;
}

long buffer::data_size() const noexcept
{
  if(!data_)
  {
    return 0;
  }
  return data_size_;
}

void buffer::data_size(long size) noexcept
{
  if(data_)
  {
     data_size_ = size;
  }
}

/*
char* buffer::data() noexcept
{
  return data_;
}

const char* buffer::data() const noexcept
{
   return data_;
}
*/
std::string export_buffer(buffer const& x, export_mode mode, string&& output)
{
  if(!x)
  {
    return string();
  }

  // guess at serialized size
  static const long buffer_overhead = 48;
  long result_size = (x.data_size() == 0 ? x.size() : x.data_size()) + buffer_overhead;
  if(mode == export_mode::base64)
  {
    result_size = (result_size * 4 / 3) + 3;
  }
  output.resize(result_size);
  
  // try export
  long flags = mode == export_mode::base64 ? TPEX_STRING : TPNOFLAGS;
  int rc = tpexport(const_cast<char*>(x.data()),
                    x.data_size(),
                    const_cast<char*>(output.data()),
                    &result_size,
                    flags);
  
  if(rc == -1 && tperrno == TPELIMIT)
  {
    // string was too small, but now we know the exact size;
    // try again
    output.resize(result_size);
    rc = tpexport(const_cast<char*>(x.data()),
                    x.data_size(),
                    const_cast<char*>(output.data()),
                    &result_size,
                    flags);
  }
  if(rc == -1)
  {
    throw last_error("tpexport");
  }
  output.resize(result_size);
  return move(output);
}

buffer import_buffer(string const& x, export_mode mode, buffer&& output)
{
  if(x.size() == 0)
  {
    return buffer();
  }
  if(!output)
  {
    output.alloc_default();
  }
  long olen = output.data_size();
  char* o = output.release();
  long flags = mode == export_mode::base64 ? TPEX_STRING : TPNOFLAGS;
  int rc = tpimport(const_cast<char*>(x.data()),
                    x.size(),
                    &o,
                    &olen,
                    flags);
  output.acquire(o, olen);
  if(rc == -1)
  {
    throw last_error("tpimport");
  }
  return move(output);
}

}
