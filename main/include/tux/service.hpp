/** @file service.hpp
@c service class and related functions.
@ingroup comm */
#pragma once
#include "atmi.h"
#include "tux/buffer.hpp"
#include "tux/conversation.hpp"

namespace tux
{

/** Abstraction for writing service routines.
It's intended to ease development of service
routines, particularly in a C++ codebase, where
the @c longjmp in @c tpreturn interacts poorly with
C++ destructors.   It's intended to be used like:
@code
using namespace tux;

extern "C" void MY_SERVICE_ROUTINE(TPSVCINFO* info)
{
      service svc(info);
      try
      {
          // access data from info
          fml32 data = svc.move_request();
          CLIENTID client_id = svc.client_id();
          bool reply_expected = svc.flag_is_set(TPNOREPLY);
          // ... do work
          // set reply
          svc.reply(TPSUCCESS, data.move_buffer());
          
      }
      catch(exception& e)
      {
          // handle error
          log("ERROR: %s", e.what());
          svc.reply(TPFAIL);
      }
}
@endcode
@note The instance of tux::service should be declared prior
to any stack variables which depend on their destructors being run on scope
exit.  A simple guideline is to make it the first variable declared.
@c tpreturn (or @c tpforward) is only called when the tux::service
destructor is run, not at the time reply() or forward() is called. The
try/catch blocks aren't strictly necessary but recommended.
@ingroup comm */
class service
{
public:
    service() = delete; /**< No default constructor. */
    service(service const& x) = delete; /**< Non-copyable. */
    service& operator=(service const& x) = delete; /**< Non-copyable. */
    service(service&& x) = delete; /**< Non-moveable. */
    service& operator=(service&& x) = delete; /**< Non-moveable. */
    ~service() noexcept; /**< Destructor [@c tpreturn or @c tpforward]. */
    
    explicit service(TPSVCINFO* svcinfo) noexcept; /**< Construct a service from a @c TPSVCINFO*. */
    
    buffer& request() noexcept; /**< Returns a reference to the request buffer [@c TPSVCINFO::data, @c TPSVCINFO::len]. */
    buffer const& request() const noexcept; /**< Returns a reference to the request buffer [@c TPSVCINFO::data, @c TPSVCINFO::len]. */
    buffer move_request() noexcept; /**< Moves the request buffer [@c TPSVCINFO::data, @c TPSVCINFO::len]. */
    
    /** Returns the service name used by the client to invoke this service [@c TPSVCINFO::name]. */
    const char* invocation_name() const noexcept; 
    /** Returns the flags associated with this invocation [@c TPSVCINFO::flags].
    @arg TPCONV
    @arg TPTRAN
    @arg TPNOREPLY
    @arg TPSENDONLY
    @arg TPRECVONLY
    @sa flag_is_set() */
    long flags() const noexcept;
    /** Check for a particular flag [@c TPSVCINFO::flags].
    @sa flags() */
    bool flag_is_set(long flag) const noexcept;
    long app_key() const noexcept; /**< Returns the app key [@c TPSVCINFO::appkey]. */
    int connection_descriptor() const noexcept; /**< Returns the connection descriptor (for conversations) [@c TPSVCINFO::cd]. */
    CLIENTID& client_id() noexcept; /**< Returns the client id [@c TPSVCINFO::cltid]. */
    
    
    /** Mark that the service should send a reply back to the caller.
    This does not actually call @c tpreturn, but @c tpreturn will be called
    when the destructor for @c this is called.
    @param rval TPSUCCESS, TPFAIL, or TPEXIT
    @param output optional output data
    @param rcode optional application return code (accessible via tpurcode on the client side) */
    void reply(int rval, buffer&& output = buffer(), long rcode = 0) noexcept;
    /** Mark that the service should forward the request to another service.
    This does not actually call @c tpforward, but @c tpforward will be called
    when the destructor for @c this is called.
    @param service_name the name of the service to which to forward the request
    @param output optional output data to forward */
    void forward(std::string const& service_name, buffer&& output = buffer()) noexcept;

    bool conversation_open() const noexcept; /**< Test if the service invocation was in conversation mode [@c TPCONV]. */
    bool conversation_in_send_mode() const noexcept; /**< Test if service has control of the conversation [@c TPSENDONLY]. */
    bool conversation_in_receive_mode() const noexcept; /**< Test if client has control of the conversation [@c TPRECVONLY]. */
    void send(buffer const& data = buffer(), long flags = TPNOFLAGS); /**< Send a message to the caller [@c tpsend]. */
    buffer receive(long flags = TPNOFLAGS, buffer&& output = buffer()); /**< Receive a message from the caller [@c tprecv]. */
    
private:
    TPSVCINFO* svcinfo_ = nullptr;
    buffer request_;
    int rval_ = TPFAIL;
    long rcode_ = 0;
    char forward_service_name_[XATMI_SERVICE_NAME_LENGTH];
    buffer output_;
    conversation conversation_;
};

/** @sa advertise(), advertisex() */
using service_function = void(TPSVCINFO* msg);

/** Advertise a service [@c tpadvertise].
@param service_name name to advertise
@param f function pointer to service_routine
@ingroup comm */
void advertise(std::string const& service_name, service_function* f);
#if TUXEDO_VERSION >= 1222
/** Advertise a service unique to this process (in an MSSQ set) [@c tpadvertisex].
@since Tuxedo 12.2.2
@param service_name name to advertise as
@param f function pointer to service_routine
@param flags
@arg TPSINGLETON advertise the singleton service
@arg TPSECONDARYRQ advertise the service on the secondary queue for the MSSQ server
@ingroup comm */
void advertisex(std::string const& service_name, service_function* f, long flags = TPSINGLETON); 
#endif

/** Unadvertise a service [@c tpunadvertise]. @ingroup comm */
void unadvertise(std::string const& service_name);
    
}
