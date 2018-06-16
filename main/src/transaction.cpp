#include "atmi.h"
#include "tux/transaction.hpp"

using namespace std;

namespace tux
{
    
bool transaction_in_progress()
{
    return tpgetlev() > 0;
}

void abort_transaction()
{
    int rc = tpabort(TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpabort");
    }
}

void begin_transaction(optional<chrono::seconds> timeout)
{
    int rc = tpbegin(timeout ? timeout->count() : 0, TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpbegin");
    }
}

void commit_transaction()
{
    int rc = tpcommit(TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpcommit");
    }
}

TPTRANID suspend_current_transaction()
{
    TPTRANID result;
    int rc = tpsuspend(&result, TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tpsuspend");
    }
    return result;
}

void resume(TPTRANID const& tranid)
{
   int rc = tpresume(const_cast<TPTRANID*>(&tranid), TPNOFLAGS);
   if(rc == -1)
   {
       throw last_error("tpresume");
   }
}



int set_commit_return_mode(int mode) // TP_CMT_LOGGED or TP_CMT_COMPLETE
{
    int result = tpscmt(mode);
    if(result == -1)
    {
        throw last_error("tpscmt");
    }
    return result;
}

void open_resource_manager(std::string const& resource_manager)
{
    if(resource_manager.empty())
    {
        int rc = tpopen();
        if(rc == -1)
        {
            throw last_error("tpopen");
        }
    }
    else
    {
        int rc = tprmopen(const_cast<char*>(resource_manager.c_str()), TPNOFLAGS);
        if(rc == -1)
        {
            throw last_error("tprmopen(" + resource_manager + ")");
        }
    }
}

void close_resource_manager(std::string const& resource_manager)
{
    if(resource_manager.empty())
    {
        int rc = tpclose();
        if(rc == -1)
        {
            throw last_error("tpclose");
        }
    }
    else
    {
        int rc = tprmclose(const_cast<char*>(resource_manager.c_str()), TPNOFLAGS);
        if(rc == -1)
        {
            throw last_error("tprmclose(" + resource_manager + ")");
        }
    }
}

void start_work(std::string const& resource_manager)
{
    int rc = tprmstart(const_cast<char*>(resource_manager.c_str()), TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tprmstart(" + resource_manager + ")");
    }
}

void end_work(std::string const& resource_manager)
{
    int rc = tprmend(const_cast<char*>(resource_manager.c_str()), TPNOFLAGS);
    if(rc == -1)
    {
        throw last_error("tprmend(" + resource_manager + ")");
    }
}


transaction::transaction(optional<std::chrono::seconds> timeout)
{
    begin_transaction(timeout);
}

transaction::~transaction() noexcept
{
    if(!completed_)
    {
        try
        {
            abort();
        }
        catch(exception const& e)
        {
            log("WARN: %s", e.what());
        }
    }
}

void transaction::commit()
{
    commit_transaction();
    completed_ = true;
}

void transaction::abort()
{
    abort_transaction();
    completed_ = true;
}

void transaction::suspend()
{
    if(!id_)
    {
        id_ = suspend_current_transaction();
    }
}

optional<TPTRANID> transaction::suspended_tranid() const noexcept
{
    return id_;
}

void transaction::resume()
{
    if(id_)
    {
        tux::resume(*id_);
    }
    id_.reset();
}
/*
linked_resource_manager::linked_resource_manager()
{
    open_linked_resource_manager();
}

linked_resource_manager::~linked_resource_manager() noexcept
{
    try
    {
        close_linked_resource_manager();
    }
    catch(exception const& e)
    {
        log("WARN: %s", e.what());
    }
}

resource_manager::resource_manager(string const& name) :
    name_(name)
{
    open_resource_manager(name_);
}

resource_manager::~resource_manager() noexcept
{
    try
    {
        close_resource_manager(name_);
    }
    catch(exception const& e)
    {
        log("WARN: %s", e.what());
    }
}*/


transaction_branch_association::transaction_branch_association(string const& rm_name)
{
    start_work(rm_name);
}

transaction_branch_association::~transaction_branch_association() noexcept
{
    try
    {
        end();
    }
    catch(exception const& e)
    {
        log("WARN: %s", e.what());
    }
}

void transaction_branch_association::end()
{
    end_work(name_);
}


}
