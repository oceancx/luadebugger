#include "debugger.h"
#include "cxlua.h"

#include <functional>
#include <thread>

#include "kbase/at_exit_manager.h"
#include "ezio/io_context.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/socket_address.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_connection.h"
#include "ezio/buffer.h"
#include "ezio/acceptor.h"
#include "ezio/connector.h"

#include "debugger.inl"

using namespace ezio;

std::string LINES_ENDING = "";
void set_line_ending_in_c(const char* le)
{
	LINES_ENDING = le;
}
const char* get_line_ending_in_c()
{
	return LINES_ENDING.c_str();
}


std::thread* debuggee_thread;
NetThreadQueue g_DebugAdapterQueue;
TCPConnectionPtr g_DebugAdapterHandler;
TimerID g_RuntimeSendTimerID ;
EventLoop* g_RuntimeServerLoop = nullptr;

void check_lua_error(lua_State* L, int res)
{
	if (res != LUA_OK) { printf("%s\n", lua_tostring(L, -1)); }
}

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);
void DebuggeeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);

	script_system_register_function(L, set_line_ending_in_c);

	int res = luaL_loadbuffer(L, debuggee_code,strlen(debuggee_code),"__debuggee__");
	check_lua_error(L, res);
	lua_pushstring(L, "debuggee");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	EventLoop loop;
	g_RuntimeServerLoop = &loop;
	SocketAddress addr(port);
	TCPServer server(&loop, addr, "RuntimeServer");

	server.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_DebugAdapterHandler = conn->connected() ? conn : nullptr;
		if (g_DebugAdapterHandler) 
		{
			g_RuntimeSendTimerID = g_RuntimeServerLoop->RunTaskEvery([]() {
				if (g_DebugAdapterHandler && g_DebugAdapterHandler->connected())
				{
					while (!g_DebugAdapterQueue.Empty(NetThreadQueue::Write))
					{
						auto msg = g_DebugAdapterQueue.Front(NetThreadQueue::Write);
						g_DebugAdapterQueue.PopFront(NetThreadQueue::Write);
						g_DebugAdapterHandler->Send(msg);
					}
				}
			}, TimeDuration(1));
		}
		else
		{
			if (g_RuntimeSendTimerID)
			{
				g_RuntimeServerLoop->CancelTimedTask(g_RuntimeSendTimerID);
			}
		}

		lua_getglobal(L, "debuggee_on_connection");
		lua_push_tcp_connection(L, conn);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 2, 0, 0);
		check_lua_error(L, res);
	});
	server.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "debuggee_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_DebugAdapterQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	server.Start();

	
	loop.Run();

	lua_close(L);
}

kbase::AtExitManager exit_manager;
int debugger_start_session(lua_State* L)
{
	ezio::IOServiceContext::Init();

	int port = (int)lua_tointeger(L, 1);
	if (port > 0)
	{
		int res = luaL_loadbuffer(L, debugger_code, strlen(debugger_code), "__debugger__");
		check_lua_error(L, res);
		lua_pushstring(L, "debugger");
		res = lua_pcall(L, 1, LUA_MULTRET, 0);
		check_lua_error(L, res);

		debuggee_thread = new std::thread(DebuggeeThreadFunc, port);
	}
	return 0;
}

void debugger_stop_session()
{
	if(g_RuntimeServerLoop)
		g_RuntimeServerLoop->Quit();

	if (debuggee_thread)
	{
		debuggee_thread->join();
		delete debuggee_thread;
		debuggee_thread = nullptr;
	}
}


void debugger_sleep(int s)
{
#if defined(_WIN32)
	Sleep(s);
#else
    sleep(s);
#endif
}

const char* debugger_fetch_message()
{
	if (!g_DebugAdapterQueue.Empty(NetThreadQueue::Read))
	{
		static std::string msg;
		msg = g_DebugAdapterQueue.Front(NetThreadQueue::Read);
		g_DebugAdapterQueue.PopFront(NetThreadQueue::Read);
		return msg.c_str();
	}
	return "";
}

void debugger_send_message(const char* msg)
{
	g_DebugAdapterQueue.PushBack(NetThreadQueue::Write,msg);
}

bool debugger_is_connected()
{
	return g_DebugAdapterHandler != nullptr && g_DebugAdapterHandler->connected();
}


void luaopen_debugger(lua_State* L)
{
	luaL_requirelib(L, "cjson", luaopen_cjson);
	
	script_system_register_luac_function(L, debugger_start_session);
	script_system_register_function(L, debugger_stop_session);
	script_system_register_function(L, debugger_sleep);

	script_system_register_function(L, debugger_fetch_message);
	script_system_register_function(L, debugger_send_message);
	script_system_register_function(L, debugger_is_connected);
	script_system_register_function(L, get_line_ending_in_c);
	
}

