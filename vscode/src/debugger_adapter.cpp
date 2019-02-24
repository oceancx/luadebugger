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

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))

extern "C" int luaopen_cjson(lua_State *L);

std::string PATH_SEP("");
std::string CWD = "";
int port = 4711;

const char* EXTENSION_DIR(const char* dir)
{
	static std::string s_dir;
	s_dir = CWD;
	s_dir.append(dir);
	return s_dir.c_str();
}
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

EDebugAdapterLaunchMode g_LaunchMode;
EDebugProtocolMode g_Mode;

bool g_debugger_adapter_run = false;



void debugger_adapter_init(int argc, char* argv[])
{
	for(int i=0;i<argc;i++)
	{
		std::string param = argv[i];
		
		if (param.find("--port=") != std::string::npos) {
			std::string str = param.substr(param.find_last_of("=")+1);
			port = std::stoi(str);
		}
		else if (i == 0) {
			if (param.find_last_of("\\")!= std::string::npos ) {
				PATH_SEP = "\\";
			}
			else if (param.find_last_of("/") != std::string::npos)
			{
				PATH_SEP = "/";
			}
			std::string str = param.substr(0, param.find_last_of(PATH_SEP));
			CWD = str + PATH_SEP;
		}
		std::cerr << "arg " << i <<  ":" << argv[i] << std::endl;
	}
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
void check_lua_error(lua_State* L,int res)
{
	if (res != LUA_OK) { 
		if (g_Mode == eMode_STDIO)
			std::cerr << lua_tostring(L, -1) << std::endl;
		else
			printf("%s\n", lua_tostring(L,-1)); 
	}
}

void VscodeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);
	script_system_register_function(L, set_line_ending_in_c);

	int res = luaL_loadfile(L, EXTENSION_DIR("threads.lua"));
	check_lua_error(L, res);
	lua_pushstring(L, "vscode");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	EventLoop loop;
	SocketAddress addr(port);
	TCPServer server(&loop, addr, "VscodeServer");

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
		check_lua_error(L, res);
	});
	server.set_on_message([L](const TCPConnectionPtr &conn, Buffer &buf, TimePoint ts) {
		lua_getglobal(L, "vscode_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_VscodeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);
	});
	server.Start();
	loop.RunTaskEvery([]() {
		if (g_VscodeHandler && g_VscodeHandler->connected())
		{
			while (!g_VscodeQueue.Empty(NetThreadQueue::Write))
			{
				auto msg = g_VscodeQueue.Front(NetThreadQueue::Write);
				g_VscodeQueue.PopFront(NetThreadQueue::Write);
				g_VscodeHandler->Send(msg);
			}
		}
	}, TimeDuration(1));

	loop.Run();
	lua_close(L);
}

void StdioVscodeThreadFunc(int port)
{
	g_debugger_adapter_run = true;
	while (g_debugger_adapter_run)
	{
		while (!g_VscodeQueue.Empty(NetThreadQueue::Write))
		{
			auto msg = g_VscodeQueue.Front(NetThreadQueue::Write);
			g_VscodeQueue.PopFront(NetThreadQueue::Write);
			//	g_VscodeHandler->Send(msg);
			dbg_trace("vscode respone: \n");
			dbg_trace(msg.c_str());
			std::cout.write(msg.data(), msg.size());
			std::cout.flush();
		}
	}
}

void RuntimeThreadFunc(const char* ip,int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);
	script_system_register_function(L, set_line_ending_in_c);
	int res = luaL_loadfile(L, EXTENSION_DIR("threads.lua"));
	
	check_lua_error(L, res);
	lua_pushstring(L, "runtime");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);

	SocketAddress addr(ip, port);
	EventLoop loop;
	TCPClient client(&loop, addr, "RuntimeClient");
	client.set_on_connection([L](const TCPConnectionPtr& conn) {
		g_RuntimeHandler = conn->connected() ? conn : nullptr;
		if (g_RuntimeHandler == nullptr)
		{
			g_RuntimeQueue.Clear(NetThreadQueue::Read);
			g_RuntimeQueue.Clear(NetThreadQueue::Write);
		}
		lua_getglobal(L, "runtime_on_connection");
		lua_push_tcp_connection(L, conn);
		int res = lua_pcall(L, 1, 0, 0);
		check_lua_error(L, res);
	});
	client.set_on_message([L](const TCPConnectionPtr& conn, Buffer& buf, TimePoint ts) {
		lua_getglobal(L, "runtime_on_message");
		lua_push_tcp_connection(L, conn);
		lua_push_ezio_buffer(L, buf);
		lua_push_net_thread_queue(L, &g_RuntimeQueue);
		int res = lua_pcall(L, 3, 0, 0);
		check_lua_error(L, res);

		
		if (g_Mode == eMode_STDIO)
		{
			while (!g_RuntimeQueue.Empty(NetThreadQueue::Read))
			{
				auto msg = g_RuntimeQueue.Front(NetThreadQueue::Read);
				g_RuntimeQueue.PopFront(NetThreadQueue::Read);
				msg = "Content-Length: " + std::to_string(msg.size()) + LINES_ENDING + "" + LINES_ENDING + msg;
				//std::cout.write(msg.data(), msg.size());
				g_VscodeQueue.PushBack(NetThreadQueue::Write, msg);
				/*lua_getglobal(L, "dispatch_runtime_message");
				lua_push_net_thread_queue(L, &g_RuntimeQueue);
				lua_pushstring(L, msg.c_str());
				lua_push_net_thread_queue(L, &g_VscodeQueue);

				res = lua_pcall(L, 3, 0, 0);
				check_lua_error(L, res);*/
			}
		}
	});
	client.Connect();

	loop.RunTaskEvery([]() {
		if (g_RuntimeHandler && g_RuntimeHandler->connected())
		{
			while (!g_RuntimeQueue.Empty(NetThreadQueue::Write))
			{
				auto msg = g_RuntimeQueue.Front(NetThreadQueue::Write);
				g_RuntimeQueue.PopFront(NetThreadQueue::Write);
				g_RuntimeHandler->Send(msg);
			}
		}
		
	}, TimeDuration(1));

	loop.Run();
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
	}
}

void set_debugger_adapter_run(bool run) { g_debugger_adapter_run = run; }

int fetch_runtime_handler(lua_State* L)
{
	if (g_RuntimeHandler)
	{
		lua_push_tcp_connection(L, g_RuntimeHandler);
		return 1;
	}
	return 0;
}

int fetch_vscode_handler(lua_State* L)
{
	if (g_VscodeHandler != nullptr)
	{
		lua_push_tcp_connection(L, g_VscodeHandler);
		return 1;
	}
	return 0;
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
		luaL_requirelib(L, "cjson", luaopen_cjson);
		luaopen_netlib(L);
		luaopen_net_thread_queue(L);

		script_system_register_function(L, vscode_on_launch_cmd);
		script_system_register_function(L, vscode_on_attach_cmd);
		script_system_register_function(L, set_debugger_adapter_run);
		script_system_register_luac_function(L, fetch_runtime_handler);
		script_system_register_luac_function(L, fetch_vscode_handler);
		script_system_register_function(L, get_line_ending_in_c);

		int res = luaL_dofile(L, EXTENSION_DIR("main.lua"));
		check_lua_error(L, res);

		g_debugger_adapter_run = true;
		while (g_debugger_adapter_run)
		{
			while (!g_VscodeQueue.Empty(NetThreadQueue::Read))
			{
				auto msg = g_VscodeQueue.Front(NetThreadQueue::Read);
				g_VscodeQueue.PopFront(NetThreadQueue::Read);
				
				lua_getglobal(L, "dispatch_vscode_message");
				lua_push_net_thread_queue(L, &g_VscodeQueue);
				lua_pushstring(L, msg.c_str());
				lua_push_net_thread_queue(L, &g_RuntimeQueue);

				res = lua_pcall(L, 3, 0, 0);
				check_lua_error(L, res);
			}

			while (!g_RuntimeQueue.Empty(NetThreadQueue::Read))
			{
				auto msg = g_RuntimeQueue.Front(NetThreadQueue::Read);
				g_RuntimeQueue.PopFront(NetThreadQueue::Read);

				lua_getglobal(L, "dispatch_runtime_message");
				lua_push_net_thread_queue(L, &g_RuntimeQueue);
				lua_pushstring(L, msg.c_str());
				lua_push_net_thread_queue(L, &g_VscodeQueue);

				res = lua_pcall(L, 3, 0, 0);
				check_lua_error(L, res);
			}
		}
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
		luaL_requirelib(L, "cjson", luaopen_cjson);
		luaopen_netlib(L);
		luaopen_net_thread_queue(L);
		script_system_register_function(L, set_line_ending_in_c);
		script_system_register_function(L, get_line_ending_in_c);
		script_system_register_function(L, vscode_on_launch_cmd);
		script_system_register_function(L, vscode_on_attach_cmd);
		script_system_register_function(L, set_debugger_adapter_run);
		script_system_register_luac_function(L, fetch_runtime_handler);
		script_system_register_luac_function(L, fetch_vscode_handler);
		script_system_register_function(L, dbg_trace);


		int res = luaL_dofile(L, EXTENSION_DIR("main.lua"));
		check_lua_error(L, res);

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
			check_lua_error(L, res);
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

int main(int argc, char* argv[])
{
	kbase::AtExitManager exit_manager;
	debugger_adapter_init(argc, argv);
	debugger_adapter_run(port);
	
	return 0;
}