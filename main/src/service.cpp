#include <cstring>
#include "tux/service.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{

service::~service() noexcept
{
    static const long flags = 0;
    if(forward_service_name_[0] != '\0')
    {
        tpforward(forward_service_name_, output_.data(), output_.data_size(), flags);
    }
    else
    {
        tpreturn(rval_, rcode_, output_.data(), output_.data_size(), flags);
    }
}
   
service::service(TPSVCINFO* svcinfo) noexcept :
    svcinfo_(svcinfo),
    request_(svcinfo->data, svcinfo->len),
    conversation_(svcinfo)
{
    forward_service_name_[0] = '\0';
}



buffer& service::request() noexcept
{
    return request_;
}

buffer const& service::request() const noexcept
{
    return request_;
}

buffer service::move_request() noexcept
{
    return move(request_);
}

const char* service::invocation_name() const noexcept
{
   return svcinfo_->name; 
}

long service::flags() const noexcept
{
    return svcinfo_->flags;
}

bool service::flag_is_set(long flag) const noexcept
{
    return (flags() & flag) == flag;
}

long service::app_key() const noexcept
{
    return svcinfo_->appkey;
}

int service::connection_descriptor() const noexcept
{
    return svcinfo_->cd;
}

CLIENTID& service::client_id() noexcept
{
    return svcinfo_->cltid;
}

void service::reply(int rval, buffer&& output, long rcode) noexcept
{
    rval_ = rval;
    rcode_ = rcode;
    output_ = move(output);
}

void service::forward(std::string const& service_name, buffer&& output) noexcept
{
    strncpy(forward_service_name_, service_name.c_str(), sizeof(forward_service_name_));
    output_ = move(output);
}

/*void service::forward(const char* service_name, buffer&& output) noexcept
{
    strncpy(forward_service_name_, service_name, sizeof(forward_service_name_));
    output_ = move(output);
}*/

bool service::conversation_open() const noexcept
{
    return conversation_.open();
}

bool service::conversation_in_send_mode() const noexcept
{
    return conversation_.in_send_mode();
}

bool service::conversation_in_receive_mode() const noexcept
{
    return conversation_.in_receive_mode();
}

void service::send(buffer const& data, long flags)
{
    conversation_.send(data, flags);
}

buffer service::receive(long flags, buffer&& output)
{
    return conversation_.receive(flags, move(output));
}

void advertise(string const& service_name, service_function* f)
{
    int rc = tpadvertise(const_cast<char*>(service_name.c_str()), f);
    if(rc == -1)
    {
        throw last_error("tpadvertise '" + service_name + "'");
    }
}

#if TUXEDO_VERSION >= 1222
void advertisex(string const& service_name, service_function* f, long flags)
{
    int rc = tpadvertisex(const_cast<char*>(service_name.c_str()), f, flags);
    if(rc == -1)
    {
        throw last_error("tpadvertisex '" + service_name + "'");
    }
}
#endif

void unadvertise(string const& service_name)
{
    int rc = tpunadvertise(const_cast<char*>(service_name.c_str()));
    if(rc == -1)
    {
        throw last_error("tpunadvertise '" + service_name + "'");
    }
}
    
}
