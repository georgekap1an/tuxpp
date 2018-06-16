/** @file request_response.hpp
Routines for communicating in a request/response style.
@ingroup comm */
#pragma once
#include <functional>
#include <memory>
#include <chrono>
#include <utility>
#include "tux/buffer.hpp"
#include "tux/service_error.hpp"
#include "tux/util.hpp"

namespace tux
{
   
/** Call a service [@c tpcall].
@param service service name
@param input optional request data
@param flags optional flags
@param output optional (pre-allocated) buffer for the return value.
This can refer to same buffer that was used for the input; that is,
it's possible to "recycle" the input buffer.  If no reply is received --
either due to an error or because the service succeeded but simply didn't
return reply data -- then output is not modified.
@returns the reply buffer
@ingroup comm */
buffer call(std::string const& service,
            buffer const& input = buffer(),
            long flags = TPNOFLAGS,
            buffer&& output = buffer());



/** Call a service asynchronously. @ingroup comm */
class async_call
{
public:
   /** Models the state of the call.*/
   enum class state { init, failed, pending, succeeded };
   
   async_call() = default; /**< Default construct. */
   async_call(async_call const& x) = delete; /**< Non-copyable. */
   async_call& operator=(async_call const& x) = delete; /**< Non-copyable. */
   async_call(async_call&& x); /**< Move construct. */
   async_call& operator=(async_call&& x); /**< Move assign. */
   ~async_call() noexcept; /**< Destruct [possible @c tpcancel if call is outstanding]. */

   bool failed() const noexcept; /**< Test if the call failed. */
   bool pending() const noexcept; /**< Test if the call is pending. */ 
   bool succeeded() const noexcept; /**< Test if the call succeeded. */
   
   /** Start a call asynchronously [@c tpacall].
   @param service name of service to invoke
   @param input optional request data
   @param flags optional flags
   @note This does not throw.  If a failure occurs starting
   the invocation, the error is captured internally, and the state
   is set to failed.
   @note If the @c TPNOREPLY bit is set on flags, the tpcall
   returns successfully, then the state is reset to init.
   @note Internally, pending async calls are tracked in a
   process-wide thread safe table to support get_any_reply() and
   process_pending_async_calls(). */
   void start(std::string const& service,
            buffer const& input = buffer(),
            long flags = TPNOFLAGS) noexcept;
   
   /** Cancel the call if it is pending [@c tpcancel].
   Any errors encountered are logged to the userlog.
   @post state is reset. */
   void cancel() noexcept;
   /** Like cancel(), but also resets any handlers. */
   void clear() noexcept;

   /** Get the reply [@c tpgetrply].
   @param flags optional flags
   @param output optional pre-allocated buffer to be used for the return value.
   If no reply is received --
   either due to an error or because the service succeeded but simply didn't
   return reply data -- then output is not modified.
   @returns the reply buffer */
   buffer get_reply(long flags = TPNOFLAGS, buffer&& output = buffer());
   
   /** Get the reply if it is already available [@c tpgetrply].
   @param flags optional flags
   @param output optional pre-allocated buffer to be used for the return value.
   If no reply is received then output is not modified.
   @returns the reply buffer if available, or else a "null" buffer. */
   optional<buffer> get_reply_nonblocking(long flags = TPNOBLOCK, buffer&& output = buffer());
   
   /** Start a call asynchronously [@c tpacall].
   @param service name of service to invoke
   @param input optional request data
   @param flags optional flags
   @note This does not throw.  If a failure occurs starting
   the invocation, the error is captured internally, and the state
   is set to failed.
   @note If the @c TPNOREPLY bit is set on flags, the tpcall
   returns successfully, then the state is reset to init.
   @sa start() */
   async_call(std::string const& service,
            buffer const& input = buffer(),
            long flags = TPNOFLAGS);
   
   /** Attach a continuation.
   @param f function to call when reply is received
   @note The continuation can be attached before or after reply
   is received.  In the former case, f is called when the reply
   is received.   In the latter case, f is called when
   it is attached.
   @sa process_pending_async_calls() */
   void then(std::function<void(buffer& reply, int urcode)> f);
   
   /** Attach an error handler.
   @param f function to call when error is encountered
   @note The error handler can be attached before or after an
   error is encountered.  In the former case, f is called when the error
   is encountered.   In the latter case, f is called when
   it is attached.
   @sa process_pending_async_calls() */
   void on_error(std::function<void(std::exception_ptr e)> f);
   
   /** Returns the call descriptor if the call is pending, or else 0. */ 
   int call_descriptor() const noexcept;
   /** Returns the name of the service that was invoked. */
   const std::string& service_name() const noexcept;
   /** Returns the application return code sent in the service reply [@c tpurcode]. */
   int urcode() const noexcept;
     
private:
    void reset_all_but_handlers() noexcept;
    void process_error(std::exception_ptr e) noexcept;
    void process_reply(buffer& b, int urcode);
    optional<buffer> private_get_reply(bool throw_on_block, long flags, buffer&& output);
    
    state state_ = state::init;
    std::exception_ptr error_ = nullptr;
    std::string service_name_;
    int call_descriptor_ = 0;
    bool reply_stored_ = false;
    buffer reply_;
    int urcode_ = 0;
    std::function<void(buffer& reply, int urcode)> process_reply_;
    std::function<void(std::exception_ptr e)> process_error_;
    
    friend async_call* get_any_reply(long flags, buffer&& output);
    friend void process_pending_async_calls(long flags, buffer&& output);
};

/** Test if any async calls are pending for this process. @ingroup comm */
bool async_calls_pending();

/** Get the next reply from the reply queue [@c tpgetrply(TPGETANY)].
@param flags optional flags
@param output optional pre-allocated buffer to use for the return value
@returns a pointer to the async_call associated with the reply.
@ingroup comm */
async_call* get_any_reply(long flags = TPNOFLAGS, buffer&& output = buffer());

/** Process replies from the reply queue in the order they arrive [@c tpgetrply(TPGETANY)].
If continuations have been attached, they will be executed.  If not, the successful reply
(or exception) will be stored in the async_call for later processing.
@param flags optional flags
@param output optional pre-allocated buffer to use for the reply
@sa async_call::then(), async_call::on_error()
@ingroup comm */
void process_pending_async_calls(long flags = TPNOFLAGS,
                                 buffer&& output = buffer());

/** Process replies from the reply queue in the order they arrive [@c tpgetrply(TPGETANY)].
If continuations have been attached, they will be executed.  If not, the successful reply
(or exception) will be stored in the async_call for later processing.
@param timeout in seconds
@param flags optional flags
@param output optional pre-allocated buffer to use for the reply
@returns true if all outstanding replies were received prior to the timeout
@sa async_call::then(), async_call::on_error()
@ingroup comm */
bool process_pending_async_calls(std::chrono::seconds timeout,
                                 long flags = TPNOFLAGS,
                                 buffer&& output = buffer());
/** Process replies from the reply queue in the order they arrive [@c tpgetrply(TPGETANY)].
If continuations have been attached, they will be executed.  If not, the successful reply
(or exception) will be stored in the async_call for later processing.
@param timeout in milliseconds
@param flags optional flags
@param output optional pre-allocated buffer to use for the reply
@returns true if all outstanding replies were received prior to the timeout
@sa async_call::then(), async_call::on_error()
@ingroup comm */
bool process_pending_async_calls(std::chrono::milliseconds timeout,
                                long flags = TPNOFLAGS,
                                buffer&& output = buffer());
               
}
