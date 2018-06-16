#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "tux/all.hpp"

using namespace std;
using namespace tux;

bool shutting_down = false;
mutex m;
condition_variable cv;

void periodically_post()
{
    try
    {
       log("INFO: in app thread");
       context ctx(init_request(), context_mode::appthread);
       log("INFO: starting loop");
       unique_lock<mutex> lk(m);
       while(!shutting_down)
       {
           post("100 MS PASSED");
           cv.wait_for(lk, chrono::milliseconds(100));
       }
       log("INFO: finished [appthr]");
    }
    catch(exception const& e)
    {
        log("ERROR: %s [appthr]", e.what());
    }

}

int tpsvrinit(int argc, char** argv)
{
	try
	{

		thread(periodically_post).detach();
		return 0;
	}
	catch(exception const& e)
	{
		log("WARN: %s [tpsvrinit]", e.what());
		return -1;
	}
}

void tpsvrdone()
{
	try
	{
	    {
    	    lock_guard<mutex> lk(m);
    	    shutting_down = true;
    	}
	    cv.notify_all();
	}
	catch(exception const& e)
	{
		log("WARN: %s [tpsvrdone]", e.what());
	}	
}

