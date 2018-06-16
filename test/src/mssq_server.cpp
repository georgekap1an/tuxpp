#include <unistd.h>
#include "tux/all.hpp"

using namespace std;
using namespace tux;

extern "C" void NOTIFY(TPSVCINFO* info);

int tpsvrinit(int argc, char** argv)
{
	try
	{
		string svc_name = "NOTIFY" + to_string(getpid());
#if TUXEDO_VERSION >= 1222
		advertisex("NOTIFY" + to_string(getpid()), NOTIFY, TPSECONDARYRQ);
#endif
		subscribe("CONFIG_UPDATE", "", svc_name);
		return 0;
	}
	catch(exception const& e)
	{
		log("WARN: %s [tpsvrinit]", e.what());
		return -1;
	}
}

extern "C" void NOTIFY(TPSVCINFO* info)
{
	service svc(info);
	broadcast("", "", "", cstring(svc.invocation_name()).buffer());
	svc.reply(TPSUCCESS);
}
