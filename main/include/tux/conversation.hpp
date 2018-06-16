/** @file conversation.hpp
@c conversation class (abstraction for communicating in a conversational style).
@ingroup comm */
#pragma once
#include <string>
#include "atmi.h"
#include "tux/buffer.hpp"
#include "tux/util.hpp"

namespace tux
{
/** A conversation is a state-preserving
connection between client and server.
@ingroup comm */
class conversation
{
public:
    conversation() noexcept = default; /**< Default construct. */
    conversation(conversation const& x) = delete; /**< Non-copyable. */
    conversation& operator=(conversation const& x) = delete; /**< Non-copyable. */
    conversation(conversation&& x) noexcept; /**< Move construct. */
    conversation& operator=(conversation&& x) noexcept; /**< Move assign. */
    ~conversation() = default; /**< Destruct. */
    
    /** Construct from TPSVCINFO* in a service routine [@c TPSVCINFO::cd, @c TPSVCINFO::flags].
    The following information is captured:
    @arg is a conversation actually open? if so, ...
    @arg what is the connection descriptor?
    @arg who currently has permission to send messages: client or service? */
    explicit conversation(TPSVCINFO* svcinfo) noexcept;
    /** Construct an active conversation [@c tpconnect]
    @param service_name name of service to which to connect
    @param data optional buffer to send
    @flags various options, two of which are:
    @arg TPSENDONLY: client controls the conversation and can send messages
    @arg TPRECVONLY: service controls the conversation and can send messages
    @sa connect() */
    explicit conversation(std::string const& service_name,
                 buffer const& data = buffer(),
                 long flags = TPSENDONLY);
    /** Connect to a service [@c tpconnect].
    @param service_name name of service to which to connect
    @param data optional buffer to send
    @flags various options, two of which are:
    @arg TPSENDONLY: client controls the conversation and can send messages
    @arg TPRECVONLY: service controls the conversation and can send messages
    @sa connect() */
    void connect(std::string const& service_name,
                 buffer const& data = buffer(),
                 long flags = TPSENDONLY);
    /** Send a message [@c tpsend].
    If the caller wishes to transfer control to the service (so
    it can send messages) after this message is delivered, flags
    should include TPRECVONLY.
    @pre The conversation must be open and caller must be in send mode.
    @sa receive() */
    void send(buffer const& data = buffer(),
              long flags = TPNOFLAGS);
    /** Receive a message [@c tprecv].
    @pre The conversation must be open and caller must be in receive mode.
    @param output optionally supply a (pre-allocated) buffer to use for the output
    @sa send(), receive_nonblocking() */
    buffer receive(long flags = TPNOFLAGS, buffer&& output = buffer());
    /** Receive a message if one is already available [@c tprecv].
    @pre The conversation must be open and caller must be in receive mode.
    @param output optionally supply a (pre-allocated) buffer to use for the output
    @returns a buffer if a message was ready, or else a "null" optional
    @sa send(), receive_nonblocking() */
    optional<buffer> receive_nonblocking(long flags = TPNOBLOCK, buffer&& output = buffer());
    
    /** Immediately tear down the connection [@c tpdiscon].
    To close a conversation gracefully, the service routine
    should call @c tpreturn.  This method allows a client
    to abruptly end the conversation.
    @pre The caller must be the initiator of the
    conversation (must be the client, not the service routine). */
    void disconnect();
    
    int connection_descriptor() const noexcept; /**< Returns the connection descriptor, or -1 if conversation not open. */
    bool open() const noexcept; /**< Returns true if the conversation is open. */
    explicit operator bool() const noexcept; /**< Returns true if the conversation is open. */
    /** Returns true if the conversation was closed gracefully.
    @note The purpose of this method is to test whether it's correct to
    access the tpurcode global.   However, in my testing Tuxedo didn't
    seem to be propagating this as advertised in the docs. */
    bool closed_gracefully() const noexcept; 
    bool in_send_mode() const noexcept; /**< Returns true if the caller can send messages. */
    bool in_receive_mode() const noexcept; /**< Returns true if the caller can receive messages. */
              
private:
    int cd_ = -1;
    bool controls_ = false;
    bool closed_gracefully_ = false;
    
    optional<buffer> private_receive(bool throw_on_block, long flags, buffer&& output);
};
    
}
