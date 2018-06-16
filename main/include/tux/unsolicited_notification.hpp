/** @file unsolicited_notification.hpp
Routines for communicating via unsolicited notification.
@ingroup comm */
#pragma once
#include "atmi.h"
#include "tux/buffer.hpp"

namespace tux
{
    
/** Notify a client [@c tpnotify].
@param clientid id of intended recipient
@param data optional message to send
@param flags
@sa broadcast()
@ingroup comm */    
void notify(CLIENTID const& clientid,
            buffer const& data = buffer(),
            long flags = TPNOFLAGS);

/** Broadcast a message to multiple clients [@c tpbroadcast].
@param lmid optionally filter recipients by logical machine id
@param username optionally filter recipients by username
@param clientname optionally filter recipients by clientname
@param data optional message to send
@param flags
*@sa notify()
@ingroup comm */
void broadcast(std::string const& lmid,
               std::string const& username,
               std::string const& clientname,
               buffer const& data = buffer(),
               long flags = TPNOFLAGS);

/** @ingroup comm */
using unsolicited_notification_function = void(char* data, long len, long unused_flags);

/** Set the callback for handling unsolicited notifications [@c tpsetunsol].
@param f handler function
@returns a pointer to the callback function that was previously registered
@ingroup comm */
unsolicited_notification_function* set_notification_handler(unsolicited_notification_function* f);

/** Explicitly process any outstanding unsolicited notifications [@c tpchkunsol].
@returns the number of unsolicited messages dispatched
@ingroup comm */
int check_unsolicited();


}
