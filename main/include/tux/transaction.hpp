/** @defgroup xa xa
Transactions */

/** @file transaction.hpp
@c transaction class, @c transaction_branch_association class, and related functions.
@ingroup xa */
#pragma once
#include <string>
#include <chrono>
#include "atmi.h"
#include "tux/util.hpp"

namespace tux
{

/** Check whether a transaction is currently in progress [@c tpgetlev].
@ingroup xa */
bool transaction_in_progress();

/** Abort the current transaction [@c tpabort].
@ingroup xa */
void abort_transaction();
/** Begin a transaction [@c tpbegin].
@param timeout optionally specify a timeout in seconds
@ingroup xa */
void begin_transaction(optional<std::chrono::seconds> timeout = optional<std::chrono::seconds>());
/** Commit a transaction [@ tpcommit].
@ingroup xa */
void commit_transaction();
/** Suspend the current transaction [@c tpsuspend].
@returns an id that can be used to resume the transaction later
@ingroup xa */
TPTRANID suspend_current_transaction();
/** Resume a transaction [@c tpresume].
@param tranid the id returned from suspend_current_transaction()
@ingroup xa */
void resume(TPTRANID const& tranid);

/** RAII wrapper to begin and either commit or abort (rollback) a transaction.
Also supports suspending and resuming the transaction.
@ingroup xa */
class transaction
{
public:
    /** Start a transaction with an optional timeout [@c tpbegin]. @sa begin_transaction()*/
    transaction(optional<std::chrono::seconds> timeout = optional<std::chrono::seconds>()); 
    transaction(transaction const&) = delete; /**< Non-copyable. */
    transaction& operator=(transaction const&) = delete; /**< Non-copyable. */
    transaction(transaction&&) = delete; /**< Non-moveable. */
    transaction& operator=(transaction&&) = delete; /**< Non-moveable. */
    ~transaction() noexcept; /**< Destruct aborts if not committed [@c tpabort]. @sa abort_transaction() */ 
    
    void commit(); /**< Commit the transaction [@c tpcommit]. @sa commit_transaction()*/
    void abort(); /**< Explicitly abort the transaction [@c tpabort]. @sa abort_transaction()*/
    
    /** Suspend the transaction [@c tpsuspend]. @sa suspend_current_transaction() */
    void suspend();
    /** Returns the id if the transaction is currently suspended. @sa  suspend() */
    optional<TPTRANID> suspended_tranid() const noexcept;
    /** Resume this transaction if it is currently suspended. @sa suspend(), tux::resume(TPTRANID const&) */
    void resume();
private:
    bool completed_ = false;
    optional<TPTRANID> id_;
};

/** Set the commit mode [@c tpscmt].
@param mode
@arg TP_CMT_LOGGED
@arg TP_CMT_COMPLETE
@ingroup xa */
int set_commit_return_mode(int mode);

// below is for working with multiple RMs in a single server group
//void open_linked_resource_manager();
//void close_linked_resource_manager();
/** Open a resource manager [@c tpopen or @c tprmopen].
@ingroup xa */
void open_resource_manager(std::string const& resource_manager_name = "");
/** Close a resource manager [@c tpclose or @c tprmclose].
@ingroup xa */
void close_resource_manager(std::string const& resource_manager_name = "");
/** Start a unit of work associated to the specified resource manager [@c tprmstart].
@sa transaction_branch_association
@ingroup xa */
void start_work(std::string const& resource_manager_name);
/** End the current unit of work associated to the specified resource manager [@c tprmend].
@sa transaction_branch_association
@ingroup xa */
void end_work(std::string const& resource_manager_name);

/*
// this may not be too useful, since I assume
// most of the time the rm is opened in tpsvrinit and closed in tpsvrdone
class linked_resource_manager
{
    linked_resource_manager();
    linked_resource_manager(linked_resource_manager const&) = delete;
    linked_resource_manager& operator=(linked_resource_manager const&) = delete;
    linked_resource_manager(linked_resource_manager&&) = delete;
    linked_resource_manager& operator=(linked_resource_manager&&) = delete;
    ~linked_resource_manager() noexcept;
};

// this may not be too useful, since I assume
// most of the time the rm is opened in tpsvrinit and closed in tpsvrdone
class resource_manager
{
public:
    resource_manager(std::string const& name); // open
    resource_manager(resource_manager const&) = delete;
    resource_manager& operator=(resource_manager const&) = delete;
    resource_manager(resource_manager&&) = delete;
    resource_manager& operator=(resource_manager&&) = delete;
    ~resource_manager() noexcept; // close
    
    std::string name();
private:
    std::string name_;
};*/

/** RAII wrapper to begin and end a transaction branch association. @ingroup xa */
class transaction_branch_association
{
public:
    //transaction_branch_association(resource_manager const& rm);
    /** Start a unit of work linked to the specified resource manager [@c tprmstart]. @sa start_work */
    transaction_branch_association(std::string const& rm_name);
    transaction_branch_association(transaction_branch_association const&) = delete; /**< Non-copyable. */
    transaction_branch_association& operator=(transaction_branch_association const&) = delete; /**< Non-copyable. */
    transaction_branch_association(transaction_branch_association&&) = delete; /**< Non-moveable. */
    transaction_branch_association& operator=(transaction_branch_association&&) = delete; /**< Non-moveable. */
    ~transaction_branch_association() noexcept; /**< Destruct ends the unit of work [@c tprmend]. @sa end(), end_work()*/
    
    void end(); /**< Explicitly end the unit of work [@ tprmend].  @sa end_work() */
private:
    std::string name_;
};

}
