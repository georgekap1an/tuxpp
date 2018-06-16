#include <utility>
#include <algorithm>
#include <vector>
#include <thread>
#include <unistd.h>
#include "tux/all.hpp"
#include "views32.h"

using namespace std;
using namespace tux;

extern "C" void NOTIFY(TPSVCINFO* info);
extern "C" void TOUPPER(TPSVCINFO* info);
extern "C" void REVERSE(TPSVCINFO* info);

int tpsvrinit(int argc, char** argv)
{
	try
	{
		//open_resource_manager();
		//string svc_name = "NOTIFY" + to_string(getpid());
		//advertisex("NOTIFY" + to_string(getpid()), NOTIFY);
		//subscribe("CONFIG_UPDATE", "", svc_name);
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
		//close_resource_manager();
		//unsubscribe();
	}
	catch(exception const& e)
	{
		log("WARN: %s [tpsvrdone]", e.what());
	}	
}

extern "C" void CALC(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		cstring request = svc.move_request();
		view32<string_info> reply;
		set(reply->original_string, request.data());
		array<int, 256> counts;
		fill(begin(counts), end(counts), 0);
		for(char c : request)
		{
			reply->byte_count += 1;
			reply->ascii_sum += (int)c;
			counts[c] += 1;
		}
		int offset = max_element(begin(counts), end(counts)) - begin(counts);
		log("DEBUG: offset=%i, val=%i", offset, counts[offset]);
		reply->most_frequent_char = offset;
		svc.reply(TPSUCCESS, reply.move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void NOTIFY(TPSVCINFO* info)
{
	service svc(info);
	log("INFO: %s was invoked in pid %i", svc.invocation_name(), getpid());
	svc.reply(TPSUCCESS);
}

extern "C" void TOUPPER(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		if(svc.conversation_open())
		{
			if(svc.flags() == TPNOFLAGS)
			{
				throw runtime_error("unexpected flags value");
			}
			if(!svc.flag_is_set(TPCONV))
			{
				throw runtime_error("unexpected flags value");
			}
			if(svc.connection_descriptor() == -1)
			{
				throw runtime_error("unexpected connection descriptor");
			}
			vector<cstring> msgs;
			while(svc.conversation_in_receive_mode())
			{
				msgs.emplace_back(svc.receive());
			}
			if(!svc.conversation_in_send_mode())
			{
				throw runtime_error("conversation wasn't in send mode as expected");
			}
			unsigned i = 0;
			for(auto&& msg : msgs)
			{
				++i;
				transform(begin(msg), end(msg), begin(msg), ::toupper);
				if(i == msgs.size())
				{
					svc.reply(TPSUCCESS, msg.move_buffer(), 5);
				}
				else
				{
					svc.send(msg.buffer());
				}
				
			}
			
		}
		else
		{
			if(svc.request().type() == "XML")
			{
				xml reply = svc.move_request();
				cout << reply << endl;
				cout << reply.size() << endl;
				svc.reply(TPSUCCESS, reply.move_buffer(), 1);
			}
			else
			{
				cstring reply = svc.move_request();
				transform(begin(reply), end(reply), begin(reply), ::toupper);
				notify(svc.client_id(), reply.buffer());
				svc.reply(TPSUCCESS, reply.move_buffer(), 1);
			}
		}
	}
	catch(exception const& e)
	{
		svc.reply(TPFAIL, cstring(e.what()).move_buffer());
	}
}

extern "C" void SLOW_TOUPPER(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		this_thread::sleep_for(chrono::milliseconds(50));
		cstring reply = svc.move_request();
		transform(begin(reply), end(reply), begin(reply), ::toupper);
		svc.reply(TPSUCCESS, reply.move_buffer());
	}
	catch(exception const& e)
	{
		svc.reply(TPFAIL, cstring(e.what()).move_buffer());
	}
}

extern "C" void REVERSE(TPSVCINFO* info)
{
	service svc(info);
	try
	{

		cstring reply = svc.move_request();
		reverse(begin(reply), end(reply));
		svc.reply(TPSUCCESS, reply.move_buffer(), 2);
	}
	catch(exception const& e)
	{
		log("ERROR: %s [REVERSE]", e.what());
		svc.reply(TPFAIL);
	}

}

extern "C" void ECHO_XML(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		buffer& buf_ref = svc.request();
		if(buf_ref.type() != "STRING")
		{
			throw runtime_error("unexpected buffer type");
		}
		if(svc.invocation_name() != string("ECHO_XML"))
		{
			throw runtime_error("unexpected invocation name");
		}
		log("INFO: clientid='%s'", to_string(svc.client_id()).c_str());
		log("INFO: prio=%i", get_priority());
		log("INFO: app_key=%i", svc.app_key());
		
		cstring request = svc.move_request();
		xml reply;
		reply += "<request>";
		reply += request.data();
		reply += "</request>";
		svc.reply(TPSUCCESS, reply.move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [ECHO_XML]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void NO_REPLY_SVC(TPSVCINFO* info)
{
	// this service simulates a service
	// which only needs to reply back
	// with success or failure, thus
	// not requiring a reply buffer
	service svc(info);
	try
	{
		// do something important with request
		svc.reply(TPSUCCESS);
	}
	catch(exception const& e)
	{
		log("ERROR: %s [NO_REPLY_SVC]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void BAD_SVC(TPSVCINFO* info)
{
	// this service always fails
	service svc(info);
	svc.reply(TPFAIL);
}

extern "C" void VERY_SLOW_SVC(TPSVCINFO* info)
{
	service svc(info);
	this_thread::sleep_for(chrono::seconds(5));
	svc.reply(TPSUCCESS);
}

extern "C" void TRIGGER_NOTIFY(TPSVCINFO* info)
{
	service svc(info);
	notify(svc.client_id(), cstring("notification message").buffer());
	svc.reply(TPSUCCESS);
}

extern "C" void TRIGGER_NOTIFY_TWICE(TPSVCINFO* info)
{
	service svc(info);
	notify(svc.client_id(), cstring("notification message").buffer());
	notify(svc.client_id(), cstring("notification message").buffer());
	svc.reply(TPSUCCESS);
}

extern "C" void TRIGGER_BROADCAST(TPSVCINFO* info)
{
	service svc(info);
	broadcast("", "", "", cstring("broadcast message").buffer());
	svc.reply(TPSUCCESS);
}

extern "C" void ECHO_CLIENTID(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		CLIENTID clientid = svc.client_id();
		string clientid_str = to_string(clientid);
		if(clientid_str.empty())
		{
			throw runtime_error("empty clientid string");
		}
		CLIENTID generated_clientid = to_clientid(clientid_str);
		if(memcmp(&generated_clientid, &clientid, sizeof(clientid)) != 0)
		{
			throw runtime_error("client id's not equivalent");
		}
		
		svc.reply(TPSUCCESS, cstring(clientid_str).move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [ECHO_CLIENTID]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void FORWARDING_SVC(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		cstring request = svc.move_request();
		request += "->FORWARDING";
		svc.forward("FORWARD_TARGET", request.move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [FORWARDING_SVC]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void FORWARD_TARGET(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		cstring request = svc.move_request();
		request += "->FORWARDED";
		svc.reply(TPSUCCESS, request.move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [FORWARD_TARGET]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void SECRET_SVC(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		svc.reply(TPSUCCESS, cstring("drink more ovaltine").move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [SECRET_SVC]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void REVEAL_SECRET(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		advertise("dECODER_rING", SECRET_SVC);
		svc.reply(TPSUCCESS, cstring("dECODER_rING").move_buffer());
	}
	catch(exception const& e)
	{
		log("ERROR: %s [REVEAL_SECRET]", e.what());
		svc.reply(TPFAIL);
	}
}

extern "C" void HIDE_SECRET(TPSVCINFO* info)
{
	service svc(info);
	try
	{
		unadvertise("dECODER_rING");
		svc.reply(TPSUCCESS);
	}
	catch(exception const& e)
	{
		log("ERROR: %s [HIDE_SECRET]", e.what());
		svc.reply(TPFAIL);
	}
}


