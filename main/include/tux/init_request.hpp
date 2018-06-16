/** @file init_request.hpp
@c init_request class and related functions.
@ingroup buffers*/
#pragma once
#include <string>
#include <utility>
#include <memory>
#include "atmi.h"
#include "tux/buffer.hpp"


namespace tux
{
/** Models a "TPINIT" typed buffer; used to connect to a domain.
@ingroup buffers*/
class init_request
{
public:
    init_request() noexcept = default; /**< Default construct; no allocation. */
    init_request(init_request const& x); /**< Copy construct. */
    init_request& operator=(init_request const& x); /**< Copy assign. */
    init_request(init_request&& x) noexcept = default; /**< Move construct. */
    init_request& operator=(init_request&& x) noexcept = default; /**< Move assign. */
    ~init_request() noexcept = default; /**< Destruct. */
       
    void username(std::string const& x); /**< Sets the username [@c TPINIT::usrname]. */
    void client_name(std::string const& x); /**< Sets the client name [@c TPINIT::cltname]. */
    void app_password(std::string const& x); /**< Sets the application password [@c TPINIT::passwd]. */
    void groupname(std::string const& x); /**< Sets the group name [@c TPINIT::grpname]. */
    /** Sets one or more flags [@c TPINIT::flags].
    Valid flags include:
    @arg TPU_SIG
    @arg TPU_DIP
    @arg TPU_THREAD
    @arg TPU_IGN
    @arg TPSA_FASTPATH
    @arg TPSA_PROTECTED
    @arg TPMULTICONTEXTS */
    void flags(long flags); 
    void auth_data(std::string const& x); /**< Sets authentication data (e.g. user password) [@c TPINIT::data, @c TPINIT::datalen]. */
    
    std::string username() const;  /**< Returns the username [@c TPINIT::usrname]. */
    std::string client_name() const; /**< Returns the client name [@c TPINIT::cltname]. */
    std::string app_password() const; /**< Returns the application password [@c TPINIT::passwd]. */
    std::string groupname() const; /**< Returns the group name [@c TPINIT::grpname]. */
    long flags() const; /**< Return current flags [@c TPINIT::flags]. */
    std::string auth_data() const; /**< Returns authentication data (e.g. user password) [@c TPINIT::data, @c TPINIT::datalen]. */
    
    explicit operator bool () const noexcept; /**< Test for null state. */
    
    TPINIT* as_tpinit() noexcept; /**< Returns pointer to underlying TPINIT struct. */
    const TPINIT* as_tpinit() const noexcept; /**< Returns pointer to underlying TPINIT struct. */
       
private:
    void alloc(long auth_data_len = 16);
    class buffer buffer_;
    
    friend bool operator==(init_request const& a, init_request const& b);
};

bool operator==(init_request const& a, init_request const& b); 
/** @relates init_request */ bool operator!=(init_request const& a, init_request const& b);

}
