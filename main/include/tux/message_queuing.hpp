/** @file message_queuing.hpp
Routines for communicating via message queuing.
@ingroup comm */
#pragma once
#include <memory>
#include "atmi.h"
#include "tux/util.hpp"
#include "tux/buffer.hpp"


namespace tux
{

/** Enqueue a message [@c tpenqueue].
@param queue_space the name of the queue space
@param queue_name the name of the queue
@param ctl a struct controlling the enqueue operation
on input, and containing some additional information
on output.  There are many options, of which a few
notable ones are mentioned below:
@arg @c replyqueue - the reply queue to which the reply should be put
@arg @c corrid - a correlation id which can be used later to retrieve
a corresponding reply
@arg flags - set various flags, including indicating to Tuxedo
which other members should be inspected. For instance, if we
want the @c replyqueue and @c corrid fields to be used, we'd need to
set the @c TPQCORRID and @c TPQREPLYQ flags.
@param input optional data to send
@param flags additional flags (beyond those in @c ctl) that control
the enqueue operation
@sa dequeue(), dequeue_nonblocking()
@ingroup comm */
void enqueue(std::string const& queue_space,
             std::string const& queue_name,
             TPQCTL& ctl,
             buffer const& input = buffer(),
             long flags = TPNOFLAGS);

/** Dequeue a message if one is available [@c tpdequeue].
@param queue_space the name of the queue space
@param queue_name the name of the queue
@param ctl a struct controlling the dequeue operation
on input, and containing some additional information
on output.  For instance, if we want to retrieve
a particular message by correlation id, we need to
set the @c TPQGETBYCORRID flag on @c TPQCTL::flags.  (If
the @c TPQWAIT bit is set, it is cleared prior to calling @c tpdequeue.)
@param flags additional flags (beyond those in @c ctl) that control
the enqueue operation
@param output optional (pre-allocated) buffer used for return value
@returns buffer if one was retrieved, or else a "null" optional
@sa dequeue(), enqueue()
@ingroup comm */
optional<buffer> dequeue_nonblocking(std::string const& queue_space,
             std::string const& queue_name,
             TPQCTL& ctl,
             long flags = TPNOFLAGS,
             buffer&& output = buffer());

/** Dequeue a message [@c tpdequeue].
@param queue_space the name of the queue space
@param queue_name the name of the queue
@param ctl a struct controlling the dequeue operation
on input, and containing some additional information
on output.  For instance, if we want to retrieve
a particular message by correlation id, we need to
set the @c TPQGETBYCORRID flag on @c TPQCTL::flags.  (The
@c TPQWAIT bit is set automatically prior to calling @c tpdequeue.)
@param flags additional flags (beyond those in @c ctl) that control
the enqueue operation
@param output optional (pre-allocated) buffer used for return value
@returns buffer if one was retrieved, or else a "null" optional
@sa dequeue_nonblocking(), enqueue()
@ingroup comm */
buffer dequeue(std::string const& queue_space,
             std::string const& queue_name,
             TPQCTL& ctl,
             long flags = TPNOFLAGS,
             buffer&& output = buffer());
    
}
