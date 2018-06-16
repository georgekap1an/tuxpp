#include "tux/pub_sub.hpp"
#include "tux/util.hpp"

#include <iostream>

using namespace std;

namespace tux
{

void post(string const& event_name,
          buffer const& data,
          long flags)
{
    int rc = tppost(const_cast<char*>(event_name.c_str()),
                    const_cast<char*>(data.data()),
                    data.size(),
                    flags);
    if(rc == -1)
    {
        throw last_error("tppost '" + event_name + "'");
    }
}


// core subscribe (private)
long subscribe(string const& event_expression,
                string const& filter_string,
                TPEVCTL* ctl,
                long flags)
{
    long subscription = tpsubscribe(const_cast<char*>(event_expression.c_str()),
                                    filter_string.empty() ? nullptr : const_cast<char*>(filter_string.c_str()),
                                    ctl,
                                    flags);
    
    if(subscription == -1)
    {
        throw last_error("tpsubscribe." + event_expression);
    }
    return subscription;
}

// subscribe with notification via
// unsolicited notification
long subscribe(string const& event_expression,
               string const& filter_string,
               long flags)
{
    return subscribe(event_expression,
                     filter_string,
                     nullptr,
                     flags);
}
// subscribe with notification via
// service
long subscribe(string const& event_expression,
               string const& filter_string,
               string const& service_name,       
               long event_flags,
               long flags)
{
    TPEVCTL ctl;
    init(ctl);
    event_flags &= ~TPEVQUEUE;
    event_flags |= TPEVSERVICE;
    ctl.flags = event_flags;
    set(ctl.name1, service_name);
    return subscribe(event_expression,
                     filter_string,
                     &ctl,
                     flags);
}
// subscribe with notification via
// stable storage queue
long subscribe(string const& event_expression,
               string const& filter_string,
               string const& queue_space,
               string const& queue_name,
               TPQCTL* queue_control,               
               long event_flags,
               long flags)
{
    TPEVCTL ctl;
    init(ctl);
    event_flags &= ~TPEVSERVICE;
    event_flags |= TPEVQUEUE;
    ctl.flags = event_flags;
    set(ctl.name1, queue_space);
    set(ctl.name2, queue_name);
    if(queue_control)
    {
        ctl.qctl = *queue_control;
    }
    return subscribe(event_expression,
                     filter_string,
                     &ctl,
                     flags);    
}



long unsubscribe(long subscription_handle, long flags)
{
    int rc = tpunsubscribe(subscription_handle, flags);
    if(rc == -1)
    {
        throw last_error("tpunsubscribe");
    }
    return subscription_handle == -1 ? tpurcode : 1; // returns number of subscriptions successfully removed
}

subscription::subscription(subscription&& x) noexcept
{
    handle_ = x.handle_;
    unsubscribe_flags_ = x.unsubscribe_flags_;
    x.handle_ = -1;
    x.unsubscribe_flags_ = TPNOFLAGS;
}

subscription& subscription::operator=(subscription&& x) noexcept
{
    if(this != &x)
    {
        handle_ = x.handle_;
        unsubscribe_flags_ = x.unsubscribe_flags_;
        x.handle_ = -1;
        x.unsubscribe_flags_ = TPNOFLAGS;
    }
    return *this;
}

subscription::~subscription() noexcept
{
    try
    {
        unsubscribe(unsubscribe_flags_);
    }
    catch(exception const& e)
    {
        log("WARN: %s", e.what());
    }
}

void subscription::subscribe(string const& event_expression,
               string const& filter_string,
               long flags)
{
    handle_ = tux::subscribe(event_expression, filter_string, flags);
}

void subscription::subscribe(string const& event_expression,
               string const& filter_string,
               string const& service_name,               
               long event_flags,
               long flags)
{
    handle_ = tux::subscribe(event_expression, filter_string, service_name, event_flags, flags);
}

void subscription::subscribe(string const& event_expression,
               string const& filter_string,
               string const& queue_space,
               string const& queue_name,
               TPQCTL* queue_control,
               long event_flags,
               long flags)
{
    handle_ = tux::subscribe(event_expression, filter_string, queue_space, queue_name, queue_control, event_flags, flags);
}

void subscription::unsubscribe(long flags)
{
    if(handle_ != -1)
    {
        tux::unsubscribe(handle_, flags);
    }
}

// subscribe with notification via
// unsolicited notification
subscription::subscription(string const& event_expression,
                           string const& filter_string,
                           long flags)
{
    subscribe(event_expression, filter_string, flags);
}

// subscribe with notification via
// service
subscription::subscription(string const& event_expression,
                           string const& filter_string,
                           string const& service_name,               
                           long event_flags,
                           long flags)
{
    subscribe(event_expression, filter_string, service_name, event_flags, flags);
}

// subscribe with notification via
// stable storage queue
subscription::subscription(string const& event_expression,
                           string const& filter_string,
                           string const& queue_space,
                           string const& queue_name,
                           TPQCTL* queue_control,
                           long event_flags,
                           long flags)
{
    subscribe(event_expression, filter_string, queue_space, queue_name, queue_control, event_flags, flags);
}



void subscription::set_unsubscribe_flags(long flags)
{
    unsubscribe_flags_ = flags;
}

subscription::operator bool() const noexcept
{
    return handle_ != -1;
}

}
