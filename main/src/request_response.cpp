#include <mutex>
#include <map>
#include "tux/request_response.hpp"
#include "tux/context.hpp"
#include "tux/transaction.hpp"
#include "tux/service_error.hpp"


#include <iostream>

using namespace std;

namespace tux
{

class pending_async_call_table
{
public:
    void add(int cd, async_call* call)
    {
        auto ctx = get_context();
        auto key = make_pair(ctx, cd);
        lock_guard<mutex> lock(mtx_);
        table_[key] = call;
        ++(sizes_[ctx]);
    }
    
    async_call* erase(int cd)
    {
        auto ctx = get_context();
        auto key = make_pair(ctx, cd);
        lock_guard<mutex> lock(mtx_);
        auto i = table_.find(key);
        if(i == table_.end())
        {
            return nullptr;
        }
        auto result = i->second;
        table_.erase(i);
        --(sizes_[ctx]);
        return result;
    }
    
    int size()
    {
        auto ctx = get_context();
        lock_guard<mutex> lock(mtx_);
        return sizes_[ctx];
    }
    
    /*void cancel_all()
    {
        auto ctx = get_context();
        lock_guard<mutex> lock(mtx_);
        auto i = table_.begin();
        auto end = table_.end();
        while(i != end)
        {
            if(get<0>(i->first) == ctx)
            {
               if(i->second)
               {
                   i->second->cancel();
               }
               table_.erase(i++);
            }
            else
            {
               ++i;
            }
        }
    }*/
private:
    mutex mtx_;
    map<tuple<TPCONTEXT_T,int>, async_call*> table_;
    map<TPCONTEXT_T, size_t> sizes_;
};

pending_async_call_table pending_async_calls;

/*
service_reply::service_reply(buffer&& data, long urcode) :
  exists_(true),  
  data_(move(data)),
  urcode_(urcode)
{
}

bool service_reply::exists() const noexcept
{
    return exists_;
}

class buffer service_reply::get_data()
{
    if(!exists_)
    {
        throw runtime_error("service_reply null");
    }
    return move(data_);
}

long service_reply::get_urcode() const
{
    if(!exists_)
    {
        throw runtime_error("service_reply null");
    }
    return urcode_;
}

service_reply::operator bool () const noexcept
{
    return exists_;
}





expected<service_reply, error> try_call(std::string const& service,
                                        buffer const& input,
                                        long flags,
                                        buffer&& output)
{
// prep output placeholder
    if(!output)
    {
        output.alloc_default();   
    }
    
    // tpcall
    const char* i = input.data();
    long ilen = input.data_size();
    long olen = output.data_size();
    char* o = output.release();
    long reply_data_size = 0;
    int rc = tpcall(const_cast<char*>(service.c_str()),
                    const_cast<char*>(i),
                    ilen,
                    &o,
                    &reply_data_size,
                    flags);
    bool received_reply_data = reply_data_size > 0;
    output.acquire(o, received_reply_data ? reply_data_size : olen);
    
    // handle any errors
    if(rc == -1 && tperrno != TPESVCFAIL)
    {
        return expected<service_reply, error>(last_error("tpcall(\"" + service + "\")")); 
    }
    
    
    service_reply reply;
    reply.failed = rc == -1;
    reply.message.urcode = tpurcode;
    if(received_reply_data)
    {
        // only pilfer the supplied output buffer's
        // memory if we actually got a reply
        reply.message.buffer = move(output);
    }
    return expected<service_reply, error>(move(reply));   
}*/
/*
message call(string const& service,
                    buffer const& input,
                    long flags,
                    buffer&& output)
{
    auto reply = move(try_call(service, input, flags, move(output)).value());
    if(reply.failed)
    {
        throw service_error(service, reply.message.urcode, move(reply.message.buffer));
    }
    return move(try_call(service, input, flags, move(output)).value().message);
}*/


buffer call(string const& service,
            buffer const& input,
            long flags,
            buffer&& output)
{
    
    // prep output placeholder
    if(!output)
    {
        output.alloc_default();   
    }
    
    // tpcall
    const char* i = input.data();
    long ilen = input.data_size();
    long olen = output.data_size();
    char* o = output.release();
    long reply_data_size = 0;
    int rc = tpcall(const_cast<char*>(service.c_str()),
                    const_cast<char*>(i),
                    ilen,
                    &o,
                    &reply_data_size,
                    flags);
    bool received_reply_data = reply_data_size > 0;
    output.acquire(o, received_reply_data ? reply_data_size : olen);
    
    // handle any errors
    if(rc == -1)
    {
        if(tperrno == TPESVCFAIL)
        {
            throw received_reply_data ?
                      service_error(service, tpurcode, move(output)) :
                      service_error(service, tpurcode);   
        }
        throw last_error("tpcall(\"" + service + "\")");
    }
    
    // only pilfer the supplied output buffer's memory if we actually got a reply
    if(received_reply_data)
    {
        return move(output);
    }
    else
    {
        return buffer();    
    }
}

async_call::async_call(async_call&& x)
{
   state_ = x.state_;
   error_ = move(x.error_);
   service_name_ = move(x.service_name_);
   call_descriptor_ = x.call_descriptor_;
   reply_stored_ = x.reply_stored_;
   reply_ = move(x.reply_);
   urcode_ = x.urcode_;
   process_reply_ = move(x.process_reply_);
   process_error_ = move(x.process_error_);
   x.reset_all_but_handlers();
}

async_call& async_call::operator=(async_call&& x)
{
    if(&x != this)
    {
        state_ = x.state_;
        error_ = move(x.error_);
        service_name_ = move(x.service_name_);
        call_descriptor_ = x.call_descriptor_;
        reply_stored_ = x.reply_stored_;
        reply_ = move(x.reply_);
        urcode_ = x.urcode_;
        process_reply_ = move(x.process_reply_);
        process_error_ = move(x.process_error_);
        x.reset_all_but_handlers();
    }
    return *this;
}

async_call::~async_call() noexcept
{
    cancel(); 
}

bool async_call::failed() const noexcept
{
    return state_ == state::failed;
}

bool async_call::pending() const noexcept
{
    return state_ == state::pending;
}

bool async_call::succeeded() const noexcept
{
    return state_ == state::succeeded;
}

void async_call::start(string const& service,
            buffer const& input,
            long flags) noexcept
{
    try
    {
        cancel();
        service_name_ = service;
        int rc = tpacall(const_cast<char*>(service_name_.c_str()),
               const_cast<char*>(input.data()),
               input.data_size(),
               flags);
        if(rc == -1 && tperrno == TPELIMIT)
        {
            log("WARN: tpacall limit reached.  attempting to process outstanding calls");
            process_pending_async_calls();
            rc = tpacall(const_cast<char*>(service_name_.c_str()),
                        const_cast<char*>(input.data()),
                        input.data_size(),
                        flags);
        }
        if(rc == -1)
        {
            state_ = state::failed;
            throw last_error("tpacall(\"" + service + "\")");       
        }
        if((flags & TPNOREPLY) == TPNOREPLY)
        {
            reset_all_but_handlers();
        }
        else
        {
            state_ = state::pending;
            call_descriptor_ = rc;
            pending_async_calls.add(call_descriptor_, this);
        }

    }
    catch(...)
    {  
        process_error(current_exception());
    }
}
   
void async_call::cancel() noexcept
{
    if(state_ == state::pending)
    {
        int rc = tpcancel(call_descriptor_);
        if(rc == -1)
        {
            auto e = last_error("tpcancel(" + service_name_ + ")");
            log("WARN: %s [cd=%i]", e.what(), call_descriptor_);
        }
        else
        {
            pending_async_calls.erase(call_descriptor_);
        }
    }
    reset_all_but_handlers();
}

void async_call::clear() noexcept
{
    cancel();
    process_reply_ = nullptr;
    process_error_ = nullptr;
}

optional<buffer> async_call::get_reply_nonblocking(long flags, buffer&& output)
{
    flags |= TPNOBLOCK;
    return private_get_reply(false, flags, move(output));
}

buffer async_call::get_reply(long flags, buffer&& output)
{
    return move(*private_get_reply(true, flags, move(output)));
}
   
optional<buffer> async_call::private_get_reply(bool throw_on_block, long flags, buffer&& output)
{   
    if(error_)
    {
        exception_ptr e = error_;
        error_ = nullptr;
        rethrow_exception(e);
    }

    if(reply_stored_)
    {
        reply_stored_ = false;
        return move(reply_);
    }

    if(state_ != state::pending)
    {
        throw runtime_error("invalid context");
    }
    
    // make sure flags does not include TPGETANY
    flags &= ~TPGETANY;
    
    // prep args
    if(!output)
    {
        output.alloc_default();
    }
    long olen = output.data_size();
    char* o = output.release();
    long reply_data_size = 0;
    
    // tpgetrply
    int rc = tpgetrply(&call_descriptor_, &o, &reply_data_size, flags);
    
    // handle buffers
    bool received_reply_data = reply_data_size > 0;
    output.acquire(o, received_reply_data ? reply_data_size : olen);
    
    // handle state
    bool call_descriptor_valid = false;
    if(rc == -1)
    {
        if(tperrno == TPEINVAL || tperrno == TPEBLOCK ||
           (tperrno == TPETIME && !transaction_in_progress()))
        {
            call_descriptor_valid = true;
        }
    }
    if(!call_descriptor_valid)
    {
        pending_async_calls.erase(call_descriptor_);
        call_descriptor_ = 0;
        state_ = rc == -1 ? state::failed : state::succeeded;
    }
    
    // capture tpurcode
    if(rc != -1 || tperrno == TPESVCFAIL)
    {
        urcode_ = tpurcode;
    }
    
    // handle any errors
    if(rc == -1)
    {
        if(tperrno == TPEBLOCK && !throw_on_block)
        {
            return optional<buffer>();
        }
        if(tperrno == TPESVCFAIL)
        {
            throw received_reply_data ?
                          service_error(service_name_, tpurcode, move(output)) :
                          service_error(service_name_, tpurcode);   
        }
        throw last_error("tpgetrply(" + service_name_ + ")");
    }

    // only pilfer the supplied output buffer's memory if we actually got a reply
    if(received_reply_data)
    {
        return move(output);
    }
    else
    {
        return buffer();    
    }
}

async_call::async_call(string const& service,
         buffer const& input,
         long flags)
{
    start(service, input, flags);
}
   
void async_call::then(function<void(buffer& reply, int urcode)> f)
{
    process_reply_ = f;
    if(reply_stored_ && process_reply_)
    {
        process_reply_(reply_, urcode_);
    }
}

void async_call::on_error(function<void(exception_ptr e)> f)
{
    process_error_ = f;
    if(error_ && process_error_)
    {
        process_error(error_);
    }
}

int async_call::call_descriptor() const noexcept
{
    return call_descriptor_;
}

const string& async_call::service_name() const noexcept
{
    return service_name_;
}

int async_call::urcode() const noexcept
{
    return urcode_;
}
  
void async_call::reset_all_but_handlers() noexcept
{ 
   state_ = state::init;
   error_ = nullptr;
   service_name_.clear();
   call_descriptor_ = 0;
   reply_stored_ = false;
   reply_.free();
   urcode_ = 0;
}

void async_call::process_error(exception_ptr e) noexcept
{
    if(!e)
    {
        return;
    }
    if(process_error_)
    {
        try
        {
            process_error_(e);
        }
        catch(exception const& e)
        {
            log("ERROR: %s [async_call::process_error/%s]", e.what(), service_name_.c_str());
        }
        catch(...)
        {
            log("ERROR: ? [async_call::process_error/%s]", service_name_.c_str());
        }
    }
    else
    {
        error_ = e;
    }
}

void async_call::process_reply(buffer& b, int urcode)
{
    if(process_reply_)
    {
        process_reply_(b, urcode);
    }
    else
    {
        reply_stored_ = true;
        reply_ = move(b);
        urcode_ = urcode;
    }
}

bool async_calls_pending()
{
    return pending_async_calls.size() > 0;
}

async_call* get_any_reply(long flags, buffer&& output)
{
    // make sure TPGETANY is set
    flags |= TPGETANY;
    
    // make sure there are pending calls
    if(!async_calls_pending())
    {
        return nullptr;
    }
    
    async_call* acall_ptr = nullptr;
    try
    {
        // prep args
        int cd = 0;
        if(!output)
        {
            output.alloc_default();
        }
        long olen = output.data_size();
        char* o = output.release();
        long reply_data_size = 0;
        
        // tpgetrply
        int rc = tpgetrply(&cd, &o, &reply_data_size, flags);
        
        // handle buffers
        bool received_reply_data = reply_data_size > 0;
        output.acquire(o, received_reply_data ? reply_data_size : olen);
        
        // handle state
        if(cd > 0)
        {
            // is the call descriptor now invalid?
            bool call_descriptor_valid = false;
            if(rc == -1)
            {
                if(tperrno == TPEINVAL || tperrno == TPEBLOCK)
                {
                    // not sure if this is even possible
                    call_descriptor_valid = true;
                }
            }
            if(!call_descriptor_valid)
            {
                acall_ptr = pending_async_calls.erase(cd);
                if(acall_ptr)
                {
                    acall_ptr->call_descriptor_ = 0;
                    acall_ptr->state_ = rc == -1 ? async_call::state::failed : async_call::state::succeeded;
                }
            }
        }
        
        // capture tpurcode
        if((rc != -1 || tperrno == TPESVCFAIL) && acall_ptr)
        {
            acall_ptr->urcode_ = tpurcode;
        }
        
        // handle any errors
        if(rc == -1)
        {
            if(tperrno == TPEBLOCK)
            {
                return nullptr;
            }
            string service_name = acall_ptr ? acall_ptr->service_name() : "?";
            if(tperrno == TPESVCFAIL)
            {                    
                throw received_reply_data ?
                        service_error(service_name, tpurcode, move(output)) :
                        service_error(service_name, tpurcode);    
            }
            else
            {
                throw last_error("tpgetrply(" + service_name + ",TPGETANY)");    
            }
    
        }
        
        // process reply
        if(acall_ptr)
        {
            buffer empty;
            acall_ptr->process_reply(received_reply_data ? output : empty, tpurcode);                
        }
    }
    catch(...)
    {
        if(acall_ptr)
        {
            acall_ptr->process_error(current_exception());
        }
        try
        {
            throw;
        }
        catch(exception const& e)
        {
            log("ERROR: %s [process_pending_async_calls]", e.what());
        }
        catch(...)
        {
            log("ERROR: unknown error [process_pending_async_calls]");
        }
    
    }
    
    return acall_ptr;
}

void process_pending_async_calls(long flags, buffer&& output)
{
    while(async_calls_pending())
    {
        get_any_reply(flags, move(output));
    }
}

bool process_pending_async_calls(std::chrono::seconds timeout,
                                 long flags,
                                 buffer&& output)
{
    using namespace std::chrono;
    auto deadline = system_clock::now() + timeout;
    while(async_calls_pending())
    {
        auto block_duration = deadline - system_clock::now();
        if(block_duration < seconds{1})
        {
            block_duration = seconds{1};
        }
        set_block_time(block_time_scope::next, duration_cast<seconds>(block_duration));
        get_any_reply(flags, move(output));
        if(system_clock::now() > deadline)
        {
            //pending_async_calls.cancel_all();
            return false;
        }
    }
    return true;
}

bool process_pending_async_calls(std::chrono::milliseconds timeout,
                                long flags,
                                buffer&& output)
{
    using namespace std::chrono;
    auto deadline = system_clock::now() + timeout;
    while(async_calls_pending())
    {
        auto block_duration = deadline - chrono::system_clock::now();
        if(block_duration < milliseconds{1})
        {
            block_duration = milliseconds{1};
        }
        set_block_time(block_time_scope::next, duration_cast<milliseconds>(block_duration));
        get_any_reply(flags, move(output));
        if(system_clock::now() > deadline)
        {
            //pending_async_calls.cancel_all();
            return false;
        }
    }
    return true;
}



    
}
