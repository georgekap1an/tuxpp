/** @file service_error.hpp
@c service_error class.
@ingroup comm */
#pragma once
#include <string>
#include <memory>
#include "tux/buffer.hpp"

#define GCC_VERSION (__GNUC__ * 10000 \
+ __GNUC_MINOR__ * 100 \
+ __GNUC_PATCHLEVEL__)

namespace tux
{
  
    
/** Models an error returned by a service routine [@c TPESVCFAIL].
Contains a return code and optionally a buffer.
@ingroup comm */
class service_error : public std::runtime_error
{
public:
  service_error() noexcept = delete; /** No default constructor. */
  // had to default in .cpp for SUSE,
  // that appears to cause problems for SUN
#if __GNUC__ && GCC_VERSION < 40900
  service_error(service_error const& x);
  service_error& operator=(service_error const& x); 
#else
  service_error(service_error const& x) = default; /**< Copy construct. */
  service_error& operator=(service_error const& x) = default; /**< Copy assign. */
#endif  
  service_error(service_error&& x); /**< Move construct. */
  service_error& operator=(service_error&& x) noexcept; /**< Move assign. */
  ~service_error() noexcept = default; /**< Destruct. */
  
  /** Construct from a service_name and a user_code [@c tpurcode] */
  service_error(std::string const& service_name, long user_code);
  /** Construct from a service_name, user_code [@c tpurcode], and a buffer. */
  service_error(std::string const& service_name, long user_code, buffer&& data);
  
  class buffer& buffer() noexcept; /**< Access the stored buffer. */
  class buffer const& buffer() const noexcept; /**< Access the stored buffer. */
  class buffer&& move_buffer() noexcept; /**< Move the stored buffer. */
  long user_code() const noexcept; /**< Returns the user_code [@c tpurcode]. */
      
private:
   long user_code_ = 0;
   // need a shared_ptr because
   // exception classes need to be
   // copyable
   std::shared_ptr<class buffer> buffer_;
};
    
}
