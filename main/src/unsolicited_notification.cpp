#include "tux/unsolicited_notification.hpp"
#include "tux/util.hpp"

using namespace std;

namespace tux
{

void notify(CLIENTID const& clientid,
            buffer const& data,
            long flags)
{
    int rc = tpnotify(const_cast<CLIENTID*>(&clientid),
                      const_cast<char*>(data.data()),
                      data.size(),
                      flags);
    if(rc == -1)
    {
        throw last_error("tpnotify");
    }
}

void broadcast(std::string const& lmid,
                std::string const& username,
                std::string const& clientname,
                buffer const& data,
                long flags)
{
    int rc = tpbroadcast(lmid.empty() ? nullptr : const_cast<char*>(lmid.c_str()),
                         username.empty() ? nullptr : const_cast<char*>(username.c_str()),
                         clientname.empty() ? nullptr : const_cast<char*>(clientname.c_str()),
                         const_cast<char*>(data.data()),
                         data.size(),
                         flags);
    if(rc == -1)
    {
        throw last_error("tpbroadcast");
    }
}

unsolicited_notification_function* set_notification_handler(unsolicited_notification_function* f)
{
    unsolicited_notification_function* result = tpsetunsol(f);
    if(result == TPUNSOLERR)
    {
        throw last_error("tpsetunsol");
    }
    return result;
}

int check_unsolicited()
{
    int result = tpchkunsol();
    if(result == -1)
    {
        throw last_error("tpchkunsol");
    }
    return result;
}


}
