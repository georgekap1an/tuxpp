/** @file pub_sub.hpp
Routines for communicating in a publish/subscribe style.
@ingroup comm */
#pragma once
#include "tux/buffer.hpp"

namespace tux
{

/** Post (publish) an event [@c tppost].
@ingroup comm */
void post(std::string const& event_name,
            buffer const& data = buffer(),
            long flags = TPNOFLAGS);

/** Subscribe to events via unsolicited notification [@c tpsubscribe].
@param event_expression regular expression used to match event names
@param filter_string optional string to match event data.   For "STRING"
(cstring) buffers, this is a regular expression.  For "FML"/"FML32"
(fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
boolean expression.
@param flags optional flags
@returns subscription handle
@sa subscription::subscription(std::string const&, std::string const&, long)
@ingroup comm */
long subscribe(std::string const& event_expression,
               std::string const& filter_string,
               long flags = TPNOFLAGS);

/** Subscribe to events via a service [@c tpsubscribe].
@param event_expression regular expression used to match event names
@param filter_string optional string to match event data.   For "STRING"
(cstring) buffers, this is a regular expression.  For "FML"/"FML32"
(fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
boolean expression.
@param service_name service that should be invoked for a matching event
@param event_flags will be set on @c TPEVCTL::flags (TPEVSERVICE is set
automatically)
@param flags optional flags
@returns subscription handle
@sa subscription::subscription(std::string const&, std::string const&, std::string const&, long, long)
@ingroup comm */
long subscribe(std::string const& event_expression,
               std::string const& filter_string,
               std::string const& service_name,               
               long event_flags = TPNOFLAGS,
               long flags = TPNOFLAGS);

/** Subscribe to events via a stable storage queue [@c tpsubscribe].
@param event_expression regular expression used to match event names
@param filter_string optional string to match event data.   For "STRING"
(cstring) buffers, this is a regular expression.  For "FML"/"FML32"
(fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
boolean expression.
@param queue_space the queue space name
@param queue_name the queue name
@param queue_control optional struct to further control how the broker
enqueues the event message
@param event_flags will be set on @c TPEVCTL::flags (TPEVQUEUE is set
automatically)
@param flags optional flags
@returns subscription handle
@sa subscription::subscription(std::string const&,std::string const&,std::string const&,std::string const&,TPQCTL*,long,long), enqueue()
@ingroup comm */
long subscribe(std::string const& event_expression,
               std::string const& filter_string,
               std::string const& queue_space,
               std::string const& queue_name,
               TPQCTL* queue_control = nullptr,
               long event_flags = TPNOFLAGS,
               long flags = TPNOFLAGS);


/** Cancel one or more subscriptions [@c tpunsubscribe].
@param subscription_handle the handle returned when the subscription was
created.  -1 is a wildcard value, instructing Tuxedo to cancel all non-
persistent subscriptions previously created by the process.
@param flags optional flags
@returns the number of subscriptions successfully removed
@sa subscription::~subscription()
@ingroup comm */
long unsubscribe(long subscription_handle = -1, long flags = TPNOFLAGS);

/** RAII wrapper around creating and canceling subscriptions. @ingroup comm */
class subscription
{
public:
    
    subscription() noexcept = default; /**< Default construct. */
    subscription(subscription const& x) = delete; /**< Non-copyable.*/
    subscription& operator=(subscription const& x) = delete; /**< Non-copyable.*/
    subscription(subscription&& x) noexcept; /**< Move construct.*/
    subscription& operator=(subscription&& x) noexcept; /**< Move assign.*/
    ~subscription() noexcept; /**< Destruct [@c tpunsubscribe]. */
    
    /** Subscribe to events via unsolicited notification [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param flags optional flags
    @sa tux::subscribe(std::string const&, std::string const&, long) */
    void subscribe(std::string const& event_expression,
                   std::string const& filter_string,
                   long flags = TPNOFLAGS);
    
    /** Subscribe to events via a service [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param service_name service that should be invoked for a matching event
    @param event_flags will be set on @c TPEVCTL::flags (TPEVSERVICE is set
    automatically)
    @param flags optional flags
    @sa tux::subscribe(std::string const&, std::string const&, std::string const&, long, long) */
    void subscribe(std::string const& event_expression,
                   std::string const& filter_string,
                   std::string const& service_name,               
                   long event_flags = TPNOFLAGS,
                   long flags = TPNOFLAGS);
    
    /** Subscribe to events via a stable storage queue [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param queue_space the queue space name
    @param queue_name the queue name
    @param queue_control optional struct to further control how the broker
    enqueues the event message
    @param event_flags will be set on @c TPEVCTL::flags (TPEVQUEUE is set
    automatically)
    @param flags optional flags
    @sa tux::subscribe(std::string const&,std::string const&,std::string const&,std::string const&,TPQCTL*,long,long), enqueue() */
    void subscribe(std::string const& event_expression,
                   std::string const& filter_string,
                   std::string const& queue_space,
                   std::string const& queue_name,
                   TPQCTL* queue_control = nullptr,
                   long event_flags = TPNOFLAGS,
                   long flags = TPNOFLAGS);
    
    /** Cancel the subscription [@c tpunsubscribe].
    @param flags optional flags
    @sa tux::::unsubscribe() */
    void unsubscribe(long flags = TPNOFLAGS);
    

    /** Subscribe to events via unsolicited notification [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param flags optional flags
    @sa tux::subscribe(std::string const&, std::string const&, long) */
    subscription(std::string const& event_expression,
                   std::string const& filter_string,
                   long flags = TPNOFLAGS);

    /** Subscribe to events via a service [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param service_name service that should be invoked for a matching event
    @param event_flags will be set on @c TPEVCTL::flags (TPEVSERVICE is set
    automatically)
    @param flags optional flags
    @sa tux::subscribe(std::string const&, std::string const&, std::string const&, long, long) */
    subscription(std::string const& event_expression,
                   std::string const& filter_string,
                   std::string const& service_name,               
                   long event_flags = TPNOFLAGS,
                   long flags = TPNOFLAGS);

    /** Subscribe to events via a stable storage queue [@c tpsubscribe].
    @param event_expression regular expression used to match event names
    @param filter_string optional string to match event data.   For "STRING"
    (cstring) buffers, this is a regular expression.  For "FML"/"FML32"
    (fml16 / fml32) and "VIEW"/"VIEW32" (view16 / view32) buffers, this is
    boolean expression.
    @param queue_space the queue space name
    @param queue_name the queue name
    @param queue_control optional struct to further control how the broker
    enqueues the event message
    @param event_flags will be set on @c TPEVCTL::flags (TPEVQUEUE is set
    automatically)
    @param flags optional flags
    @sa tux::subscribe(std::string const&,std::string const&,std::string const&,std::string const&,TPQCTL*,long,long), enqueue() */
    subscription(std::string const& event_expression,
                   std::string const& filter_string,
                   std::string const& queue_space,
                   std::string const& queue_name,
                   TPQCTL* queue_control = nullptr,
                   long event_flags = TPNOFLAGS,
                   long flags = TPNOFLAGS);
    
    /** Optionally set flags to be used in ~subscription [@c tpunsubscribe]. */
    void set_unsubscribe_flags(long flags);
    
    /** Test whether this represents an active subscription. */
    explicit operator bool() const noexcept;

private:
    long handle_ = -1;
    long unsubscribe_flags_ = TPNOFLAGS;
};
    
}
