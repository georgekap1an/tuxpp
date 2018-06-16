#include "tux/message_queuing.hpp"

using namespace std;

namespace tux
{

const char* diagnostic_str(long diagnostic_code)
{
    switch(diagnostic_code)
    {
    case QMEINVAL: return "QMEINVAL";
    case QMEBADRMID: return "QMEBADRMID";
    case QMENOTOPEN: return "QMENOTOPEN";
    case QMETRAN: return "QMETRAN";
    case QMEBADMSGID: return "QMEBADMSGID";
    case QMESYSTEM: return "QMESYSTEM";
    case QMEOS: return "QMEOS";
    case QMEABORTED: return "QMEABORTED|QMENOTA";
    case QMEPROTO: return "QMEPROTO";
    case QMEBADQUEUE: return "QMEBADQUEUE";
    case QMENOMSG: return "QMENOMSG";
    case QMEINUSE: return "QMEINUSE";
    case QMENOSPACE: return "QMENOSPACE";
    case QMERELEASE: return "QMERELEASE";
    case QMEINVHANDLE: return "QMEINVHANDLE";
    case QMESHARE: return "QMESHARE";
    default: return "";
    }
}

void enqueue(string const& queue_space,
             string const& queue_name,
             TPQCTL& ctl,
             buffer const& input,
             long flags)
{
    int rc = tpenqueue(const_cast<char*>(queue_space.c_str()),
                       const_cast<char*>(queue_name.c_str()),
                       &ctl,
                       const_cast<char*>(input.data()),
                       input.size(),
                       flags);
    if(rc == -1)
    {
        if(tperrno == TPEDIAGNOSTIC)
        {
            string what = "tpenqueue.TPEDIAGNOSTIC.";
            what += diagnostic_str(ctl.diagnostic);
            throw error(tperrno, ctl.diagnostic, what);
        }
        throw last_error("tpenqueue");
    }
}

optional<buffer> private_dequeue(bool throw_on_block,
                         string const& queue_space,
                         string const& queue_name,
                         TPQCTL& ctl,           
                         long flags,
                         buffer&& output)
{
   // prep args
    if(!output)
    {
        output.alloc_default();
    }
    long olen = output.data_size();
    char* o = output.release();
    long reply_data_size = 0;
    
    int rc = tpdequeue(const_cast<char*>(queue_space.c_str()),
                       const_cast<char*>(queue_name.c_str()),
                       &ctl,
                       &o,
                       &reply_data_size,
                       flags);
    
    // handle buffers
    bool received_reply_data = reply_data_size > 0;
    output.acquire(o, received_reply_data ? reply_data_size : olen);
    
    if(rc == -1)
    {
        if(tperrno == TPEDIAGNOSTIC)
        {
            if(ctl.diagnostic == QMENOMSG && !throw_on_block)
            {
                return optional<buffer>();
            }
            string what = "tpdequeue.TPEDIAGNOSTIC.";
            what += diagnostic_str(ctl.diagnostic);
            throw error(tperrno, ctl.diagnostic, what);
        }
        throw last_error("tpdequeue");
    }
    
    return received_reply_data ? move(output) : buffer();
}

optional<buffer> dequeue_nonblocking(string const& queue_space,
                                        string const& queue_name,
                                        TPQCTL& ctl,           
                                        long flags,
                                        buffer&& output)
{
    ctl.flags &= ~TPQWAIT;
    return private_dequeue(false, queue_space, queue_name, ctl, flags, move(output));
}

buffer dequeue(string const& queue_space,
                string const& queue_name,
                TPQCTL& ctl,           
                long flags,
                buffer&& output)
{
    ctl.flags |= TPQWAIT;
    return move(*private_dequeue(true, queue_space, queue_name, ctl, flags, move(output)));
}

    
}
