/** @defgroup buffers buffers
Various data structures backed by a tpalloc'ed buffer */

/** @file buffer.hpp
@c buffer class and related functions.
@ingroup buffers */
#pragma once
#include <string>
#include "atmi.h"


namespace tux
{

/** Models a typed buffer allocated via @c tpalloc.
A buffer has a "null" state by default, and is
moveable but not copyable.
@ingroup buffers */
class buffer
{
public:
  static const char* default_type; /**< Used by buffer::alloc_default; initialized to "FML32". */
  static const char* default_subtype; /**< Used by buffer::alloc_default; initialized to nullptr. */
  static long default_size; /**< Used by buffer::alloc_default; initialized to 0. */
  
  buffer() noexcept = default; /**< Default construct (no allocation). */
  buffer(buffer const& x) = delete; /**< Non-copyable. */
  buffer& operator=(buffer const& x) = delete; /**< Non-copyable. */
  buffer(buffer&& x) noexcept; /**< Move construct (x is left in default constructed state). */
  buffer& operator=(buffer&& x) noexcept; /**< Move assign (x is left in default constructed state). */
  ~buffer() noexcept; /**< Frees resources as needed [@c tpfree]. */
  
  /** Construct an allocated buffer [@c tpalloc].
  @param type e.g. "FML32", "STRING", "VIEW32"
  @param subtype optional subtype like a viewname
  @param  size optional initial capacity
  @sa buffer::alloc
  */
  explicit buffer(const char* type,
         const char* subtype = nullptr,
         long size = 0); 
  

  /** Construct by acquiring previously @c tpalloc 'ed buffer.
  @param data previously @c tpalloc 'ed buffer
  @param data_size optionally specifies used portion of buffer (not needed for self-describing buffers)
  @sa buffer::acquire*/
  explicit buffer(char* data, long data_size = 0) noexcept; 
  
  /** Allocate a buffer [@c tpalloc].
  @param type e.g. "FML32", "STRING", "VIEW32"
  @param subtype optional subtype like a viewname
  @param  size optional initial capacity
  */
  void alloc(const char* type,
         const char* subtype = nullptr,
         long size = 0); /**< Manually @c tpalloc. */
  
  /** Allocates a buffer using buffer::default_type,
  buffer::default_subtype, and buffer::default_size.
  This is used internally in other parts of the library
  which need to provide a pre-allocated buffer to an ATMI
  function.
  */
  void alloc_default();
  
  void realloc(long size); /**< Changes the buffer's capacity [@c tprealloc]. */
  
  void free() noexcept; /**< Frees the buffer [@c tpfree]. */
  
  /** Takes ownership of a previously tpalloc'ed buffer.
  @param data previously tpalloc'ed buffer
  @param data_size optionally specifies used portion of buffer (not needed for self-describing buffers) */
  void acquire(char* data, long data_size = 0) noexcept;
  
  /** Releases ownership of buffer.
  @returns the just released buffer. */
  char* release() noexcept;
  
  explicit operator bool() const noexcept; /**< Test for "null"/default state. */
  std::string type() const; /**< Returns buffer type [@c tptypes]. */
  std::string subtype() const; /**< Returns buffer subtype [@c tptypes]. */
  long size() const; /**< Returns buffer size (capacity) [@c tptypes]. */
  /** Returns used portion of buffer, as set by user.
  @sa buffer::data_size(long) */
  long data_size() const noexcept; 
  
  /** Sets the length of data contained in the buffer.
  This is unnecessary for buffer types that are self-
  describing (many are).  However, if you are using a buffer type
  which is not, such as XML or CARRAY, you must explicitly
  set the data_size buffer passing the buffer to ATMI routines
  which require buffer length (e.g. @c tpcall). */
  void data_size(long size) noexcept;
  
  char* data() noexcept { return data_; } /**< Returns pointer to raw buffer. */
  const char* data() const noexcept { return data_; } /**< Returns pointer to raw buffer. */
  
  
  
private:
  char* data_ = nullptr;
  long data_size_ = 0;
};

/** Controls serialization of buffers.
@sa export_buffer, import_buffer
@ingroup buffers */
enum class export_mode { binary, base64 };

/** Serializes a buffer [@c tpexport].
@relates buffer
@param x buffer to be serialized
@param mode serialization mode
@param output optional pre-allocated string
@returns string containing serialized buffer
@sa import_buffer
*/
std::string export_buffer(buffer const& x,
                          export_mode mode = export_mode::binary,
                          std::string&& output = std::string());


/** Deserializes a buffer [@c tpimport].
@relates buffer
@param x string to be parsed
@param mode serialization mode
@param output optional pre-allocated buffer
@returns deserialized buffer
@sa export_buffer
*/
buffer import_buffer(std::string const& x,
                     export_mode mode = export_mode::binary,
                     buffer&& output = buffer());

                          


}
