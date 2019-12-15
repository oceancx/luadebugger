#include "debugger_adapter.h"
#include <string>
#include "kbase/at_exit_manager.h"
#include "ezio/io_service_context.h"
#include "ezio/event_loop.h"
#include "ezio/tcp_server.h"
#include "ezio/tcp_client.h"
#include "kbase/string_util.h"
#include "lua_net.h"
#include "net_thread_queue.h"
#include <iostream>
#include <string.h>
#include <future>
#include "cxlua.h"
#include <luadbg.h>

using namespace ezio;

enum EDebugAdapterLaunchMode
{
	eLaunchMode_Launch = 0,
	eLaunchMode_Attach,
};

enum EDebugProtocolMode
{
	eMode_STDIO = 0,
	eMode_TCP,
};

void register_common_lua_functions(lua_State* L);

bool g_debugger_adapter_run = false;
void set_debugger_adapter_run(bool run) { g_debugger_adapter_run = run; }\
bool is_debugger_adapter_run() { return g_debugger_adapter_run; }
std::string CWD = "";
int port = 4711;
std::string EXTENSION_DIR(const char* dir)
{
	return CWD + dir;
}

EDebugAdapterLaunchMode g_LaunchMode;
EDebugProtocolMode g_Mode;

void debugger_adapter_init(int argc, char* argv[])
{
	port = command_arg_opt_int("port", 0);
	CWD = command_arg_opt_str("cwd", get_default_cwd().c_str());
	std::cerr << "workdir = " << CWD << "   port = " << port << std::endl;
}

std::string LINES_ENDING = "";
void set_line_ending_in_c(const char* le)
{
	if (LINES_ENDING == "") {
		LINES_ENDING = le;
	}
}
const char* get_line_ending_in_c()
{
	return LINES_ENDING.c_str();
}

void dbg_trace(const char* msg)
{
	std::cerr << msg << std::endl;
}

NetThreadQueue g_VscodeQueue;
NetThreadQueue g_RuntimeQueue;
TCPConnectionPtr g_VscodeHandler;
TCPConnectionPtr g_RuntimeHandler;
EventLoop* g_VsCodeLoop;
EventLoop* g_RuntimeLoop;

void _check_lua_error(lua_State* L,int res)
{
	if (res != LUA_OK) { 
		if (g_Mode == eMode_STDIO)
			std::cerr << lua_tostring(L, -1) << std::endl;
		else
			check_lua_error(L, res);
	}
} 

void VscodeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	register_common_lua_functions(L);	

	int res = luaL_loadfile(L, EXTENSION_DIR("threads.lua").c_str());
	_check_lua_error(L, res);
	lua_pushstring(L, "vscode");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	_check_lua_error(L, res);

	g_VsCodeLoop = new EventLoop();
	
	SocketAddress addr(port);
	TCPServer server(g_VsCodeLoop, addr, "VscodeServer");

	server.set_on_connection([L](const TCPConnectionPtr &conn) {
		g_VscodeHandler = conn->connected() ? conn : nullptr;
		if (g_VscodeHandler == nullptr)
		{
			g_VscodeQueue.Clear(NetThreadQueue::Read);
			g_VscodeQueue.Clear(NetThreadQueue::Write);
		}
		lua_getglobal(L, "vscode_on_connection");
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		_check_lua_error(L, res);
	});
	server.set_on_message([L](const TCPConnectionPtr &conn, Buffer &buf, TimePoint ts) {
		lua_getglobal(L, "vscode_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_VscodeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		_check_lua_error(L, res);
	});
	server.Start();
	

	g_VsCodeLoop->Run();
	g_VsCodeLoop = nullptr;
	lua_close(L);
}

void StdioVscodeThreadFunc(int port)
{
	g_debugger_adapter_run = true;
	while (g_debugger_adapter_run)
	{
		while (!g_RuntimeQueue.Empty(NetThreadQueue::Read))
		{
			ezio::Buffer& msg = g_RuntimeQueue.Front(NetThreadQueue::Read);
			std::string newmsg = "Content-Length: " + std::to_string(msg.readable_size()) + LINES_ENDING + "" + LINES_ENDING + std::string(msg.Peek(), msg.readable_size());
			g_RuntimeQueue.PopFront(NetThreadQueue::Read);
			g_VscodeQueue.PushBack(NetThreadQueue::Write, newmsg.data(), newmsg.size());
		}

		while (!g_VscodeQueue.Empty(NetThreadQueue::Write))
		{
			ezio::Buffer& msg = g_VscodeQueue.Front(NetThreadQueue::Write);
			//	g_VscodeHandler->Send(msg);
			dbg_trace("vscode respone: \n");
			dbg_trace(std::string(msg.Peek(),msg.readable_size()).c_str());
			std::cout.write(msg.Peek(), msg.readable_size());
			std::cout.flush();
			g_VscodeQueue.PopFront(NetThreadQueue::Write);
		}
	}
}

void RuntimeThreadFunc(const char* ip,int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	register_common_lua_functions(L);
	int res = luaL_loadfile(L, EXTENSION_DIR("threads.lua").c_str());
	
	_check_lua_error(L, res);
	lua_pushstring(L, "runtime");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	_check_lua_error(L, res);

	SocketAddress addr(ip, port);
	g_RuntimeLoop = new EventLoop();
	
	TCPClient client(g_RuntimeLoop, addr, "RuntimeClient");
	client.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_RuntimeHandler = conn->connected() ? conn : nullptr;
		if (g_RuntimeHandler == nullptr)
		{
			g_RuntimeQueue.Clear(NetThreadQueue::Read);
			g_RuntimeQueue.Clear(NetThreadQueue::Write);
		}else {
			while (!g_RuntimeQueue.Empty(NetThreadQueue::Write))
			{
				ezio::Buffer& msg = g_RuntimeQueue.Front(NetThreadQueue::Write);
				g_RuntimeHandler->Send(msg.ReadAllAsString());
				g_RuntimeQueue.PopFront(NetThreadQueue::Write);
			}
		}
		lua_getglobal(L, "runtime_on_connection");
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		_check_lua_error(L, res);
		if (g_RuntimeHandler == nullptr) {
			set_debugger_adapter_run(false);
			if (g_Mode == eMode_STDIO)
			{
				exit(0);
			}
		}
	});
	client.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "runtime_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_RuntimeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		_check_lua_error(L, res);
	});
	client.Connect();

	

	g_RuntimeLoop->Run();
	g_RuntimeLoop = nullptr;
	lua_close(L);
}

std::vector<std::thread*> thread_set;

void vscode_on_launch_cmd(const char*cmd,const char* ip, int port)
{
	system(cmd);
	thread_set[1] = new std::thread(RuntimeThreadFunc, ip, port);
}

void vscode_on_attach_cmd(const char* ip, int port)
{
	if (thread_set[1] == nullptr)
	{
		thread_set[1] = new std::thread(RuntimeThreadFunc, ip, port);
		while(!(g_RuntimeHandler && g_RuntimeHandler->connected())){
			Sleep(10);	
		}
	}
}

bool is_stdio_mode()
{
	return g_Mode == eMode_STDIO;
}


void register_common_lua_functions(lua_State* L)
{ 
	luaopen_cxlua(L);
	script_system_register_function(L, get_line_ending_in_c);
	script_system_register_function(L, set_line_ending_in_c);
	script_system_register_function(L, is_stdio_mode);
}
void vscode_send_message(const char* msg){
	if (g_VscodeHandler && g_VscodeHandler->connected()) {
		printf("\nDA => VS:\n%s\n", msg);
		g_VscodeHandler->Send(msg);
	}
}

void runtime_send_message(const char* msg){
	if (g_RuntimeHandler && g_RuntimeHandler->connected()) {
		printf("\nDA => RT:\n%s\n", msg);
		g_RuntimeHandler->Send(msg);
	}
}
int fetch_vscode_netq(lua_State*L){
	lua_push_net_thread_queue(L, &g_VscodeQueue);
	return 1;
}
int fetch_runtime_netq(lua_State*L){
	lua_push_net_thread_queue(L, &g_RuntimeQueue);
	return 1;
}
int debugger_adapter_run(int port)
{
	ezio::IOServiceContext::Init();
	thread_set.resize(2);
	if (port > 0)
	{
		g_Mode = eMode_TCP;
		thread_set[0] = new std::thread(VscodeThreadFunc, port);
		
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		script_system_register_function(L, vscode_on_launch_cmd);
		script_system_register_function(L, vscode_on_attach_cmd);
		script_system_register_function(L, set_debugger_adapter_run);
		script_system_register_function(L, is_debugger_adapter_run);
		
		script_system_register_function(L, dbg_trace);
		script_system_register_function(L, vscode_send_message);
		script_system_register_function(L, runtime_send_message);

		script_system_register_luac_function(L, fetch_vscode_netq);
		script_system_register_luac_function(L, fetch_runtime_netq);

			
		register_common_lua_functions(L);
		luaopen_luadbg(L);

		g_debugger_adapter_run = true;
		int res = luaL_dofile(L, EXTENSION_DIR("main.lua").c_str());
		_check_lua_error(L, res);


		
		lua_close(L);

		for (auto* t : thread_set)
		{
			if (t->joinable())
			{
				t->join();
			}
			delete t;
		}
		thread_set.clear();
	}
	else
	{
		g_Mode = eMode_STDIO;
		
		thread_set[0] = new std::thread(StdioVscodeThreadFunc, port);
		lua_State* L = luaL_newstate();
		luaL_openlibs(L);
		script_system_register_function(L, vscode_on_launch_cmd);
		script_system_register_function(L, vscode_on_attach_cmd);
		script_system_register_function(L, set_debugger_adapter_run);
		script_system_register_function(L, dbg_trace);
		register_common_lua_functions(L);
		int res = luaL_dofile(L, EXTENSION_DIR("main.lua").c_str());
		_check_lua_error(L, res);

		int c = 0;
		g_debugger_adapter_run = true;
		Buffer buf;
		while (std::cin.read(reinterpret_cast<char*>(&c), 1) && g_debugger_adapter_run)
		{
			buf.Write((int8_t)c);

			lua_getglobal(L, "stdio_on_message");
			lua_push_ezio_buffer(L, buf);
			lua_push_net_thread_queue(L, &g_VscodeQueue);
			lua_push_net_thread_queue(L, &g_RuntimeQueue);
			int res = lua_pcall(L, 3, 0, 0);
			_check_lua_error(L, res);
		}
		lua_close(L);

		for (auto* t : thread_set)
		{
			if (t->joinable())
			{
				t->join();
			}
			if (t)
			{
				delete t;
				t = nullptr;
			}
			
		}
		thread_set.clear();
	}

	return 0;
}

int main(int argc,char* argv[])
{
	handle_command_args(argc, argv);
	kbase::AtExitManager exit_manager;
	debugger_adapter_init(argc, argv);
	debugger_adapter_run(port);
	return 0;
}