#include "tux/service_error.hpp"
#include "atmi.h"

using namespace std;

namespace tux
{

#if __GNUC__ && GCC_VERSION < 40900
// these must be defaulted outside of class def
// workaround for bug in gcc < 4.9 (SUSE)
service_error::service_error(service_error const& x) = default;
service_error& service_error::operator=(service_error const& x) = default;
#endif

service_error::service_error(service_error&& x)
: runtime_error(x.what())
{
    *this = x;
}

service_error& service_error::operator=(service_error&& x) noexcept
{
    *this = x;
    return *this;
}
    
service_error::service_error(string const& service_name, long user_code) :
  runtime_error("\"" + service_name + "\"." + tpstrerror(TPESVCFAIL)),
  user_code_(user_code),
  buffer_(make_shared<class buffer>())
{
}
   
service_error::service_error(string const& service_name, long user_code, class buffer&& data) :
   runtime_error("\"" + service_name + "\"." + tpstrerror(TPESVCFAIL)),
   user_code_(user_code),
   buffer_(make_shared<class buffer>(move(data)))
{
}

class buffer& service_error::buffer() noexcept { return *buffer_; }
class buffer const& service_error::buffer() const noexcept { return *buffer_; }
class buffer&& service_error::move_buffer() noexcept { return move(*buffer_); }
long service_error::user_code() const noexcept { return user_code_; }
    
}
