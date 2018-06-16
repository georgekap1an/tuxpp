#include "tux/conversation.hpp"
#include "tux/service_error.hpp"

#include <iostream>

using namespace std;

namespace tux
{

conversation::conversation(conversation&& x) noexcept
{
    cd_ = x.cd_;
    controls_ = x.controls_;
    closed_gracefully_ = x.closed_gracefully_;
    x.cd_ = -1;
    x.controls_ = false;
    x.closed_gracefully_ = false;
}

conversation& conversation::operator=(conversation&& x) noexcept
{
    if(this != &x)
    {
        cd_ = x.cd_;
        controls_ = x.controls_;
        closed_gracefully_ = x.closed_gracefully_;
        x.cd_ = -1;
        x.controls_ = false;
        x.closed_gracefully_ = false;
    }
    return *this;
}

conversation::conversation(TPSVCINFO* svcinfo) noexcept
{
    if((svcinfo->flags & TPCONV) == TPCONV)
    {
        cd_ = svcinfo->cd;
    }
    if((svcinfo->flags & TPSENDONLY) == TPSENDONLY)
    {
        controls_ = true;
    }
}


conversation::conversation(string const& service_name,
                 buffer const& data ,
                 long flags)
{
    connect(service_name, data, flags);
}

void conversation::connect(string const& service_name,
             buffer const& data,
             long flags)
{
    closed_gracefully_ = false;
    int rc = tpconnect(const_cast<char*>(service_name.c_str()),
                       const_cast<char*>(data.data()),
                       data.data_size(),
                       flags);
    if(rc == -1)
    {
        throw last_error("tpconnect");
    }
    cd_ = rc;
    if((flags & TPRECVONLY) == TPRECVONLY)
    {
        controls_ = false;
    }
    if((flags & TPSENDONLY) == TPSENDONLY)
    {
        controls_ = true;
    }
}

const char* conv_event_str(long revent)
{
    switch(revent)
    {
        case TPEV_DISCONIMM: return "TPEV_DISCONIMM";
        case TPEV_SENDONLY: return "TPEV_SENDONLY";
        case TPEV_SVCERR: return "TPEV_SVCERR";
        case TPEV_SVCFAIL: return "TPEV_SVCFAIL";
        case TPEV_SVCSUCC: return "TPEV_SVCSUCC";
        default: return "";
    }
}

void conversation::send(buffer const& data,
          long flags)
{
    long revent = 0; // TODO figure this out
    int rc = tpsend(cd_,
                    const_cast<char*>(data.data()),
                    data.data_size(),
                    flags,
                    &revent);
    if(rc == -1)
    {
        cd_ = -1;
        controls_ = false;
        if(tperrno == TPEEVENT)
        {
            throw runtime_error(conv_event_str(revent));
        }
        throw last_error("tpsend");
    }
    if((flags & TPRECVONLY) == TPRECVONLY)
    {
        controls_ = false;
    }
}

buffer conversation::receive(long flags, buffer&& output)
{
    return move(*private_receive(true, flags, move(output)));            
}

optional<buffer> conversation::receive_nonblocking(long flags, buffer&& output)
{
    flags |= TPNOBLOCK;
    return private_receive(false, flags, move(output));
}

void conversation::disconnect()
{
    if(open())
    {
        int rc = tpdiscon(cd_);
        if(rc == -1)
        {
            throw last_error("tpdiscon");
        }
    }
}


int conversation::connection_descriptor() const noexcept
{
    return cd_;
}

bool conversation::open() const noexcept
{
    return cd_ != -1;
}

conversation::operator bool() const noexcept
{
    return open();
}

bool conversation::closed_gracefully() const noexcept
{
    return closed_gracefully_;
}

bool conversation::in_send_mode() const noexcept
{
    return open() && controls_;
}

bool conversation::in_receive_mode() const noexcept
{
    return open() && !controls_;
}

optional<buffer> conversation::private_receive(bool throw_on_block, long flags, buffer&& output)
{
    if(!output)
    {
        output.alloc_default();
    }
    
    // prep input
    long revent = 0; // TODO figure this out
    long olen = output.data_size();
    char* o = output.release();
    long reply_data_size = 0;
    
    int rc = tprecv(cd_,
                    &o,
                    &reply_data_size,
                    flags,
                    &revent);
    
    // handle buffer
    bool received_reply_data = reply_data_size > 0;
    output.acquire(o, received_reply_data ? reply_data_size : olen);
    
    
    if(rc == -1)
    {
        if(tperrno == TPEBLOCK && !throw_on_block)
        {
            return optional<buffer>();
        }
        if(tperrno == TPEEVENT && revent == TPEV_SVCSUCC)
        {
            // not an error, but the conversation is over
            cd_ = -1;
            controls_= false;
            closed_gracefully_ = true;
        }
        else if(tperrno == TPEEVENT && revent == TPEV_SENDONLY)
        {
            // control has been granted
            controls_ = true;
        }
        else if(tperrno == TPEEVENT)
        {
            cd_ = -1;
            controls_ = false;            
            if(revent == TPEV_SVCFAIL)
            {
                closed_gracefully_ = true;
               throw service_error("", tpurcode, received_reply_data ? move(output) : buffer()); 
            }
            else
            {
                throw runtime_error(conv_event_str(revent));
            }
        }
        else
        {
            // handle error
            throw last_error("tprecv");
        }
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
    
}
