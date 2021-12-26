#include "luadbg.h"
#include "lua_net.h"
#include "net_thread_queue.h"

#include <functional>
#include <thread>
 
#include "cxezio/buffer.h" 

#include "luadbg.inl"
#include "lua_bind.h"
#include "cxlua.h"

#include "asio.hpp"

using namespace cxezio;
using asio::ip::tcp;
std::string LUADBG_LINES_ENDING = "";
int debugger_send_message(lua_State* L);
void luadbg_set_line_ending_in_c(const char* le)
{
	LUADBG_LINES_ENDING = le;
}
const char* luadbg_get_line_ending_in_c()
{
	return LUADBG_LINES_ENDING.c_str();
}

std::thread* debuggee_thread;
NetThreadQueue g_DebugAdapterQueue;

#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C"  int luaopen_cjson(lua_State * L);

void luadbg_register_common_lua_functions(lua_State* L) {
	luaL_requirelib(L, "cjson", luaopen_cjson);
	luaopen_netlib(L);
	luaopen_net_thread_queue(L);
	script_system_register_function(L, luadbg_set_line_ending_in_c);
	script_system_register_function(L, luadbg_get_line_ending_in_c);
}

class DebugSession
{
public:
	DebugSession(asio::io_context& io_context, lua_State* L, tcp::socket socket)
		:m_IOContext(io_context),
		m_L(L),
		m_Socket(std::move(socket))
	{
		m_Buffer.resize(1);
		read_msg();
	};

	void read_msg() {
		asio::async_read(m_Socket,
			asio::buffer(m_Buffer.data(), m_Buffer.size()),
			[this](std::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					ezBuf.Write(m_Buffer.data(), m_Buffer.size());
					lua_getglobal(m_L, "debuggee_on_message");
					lua_push_cxezio_buffer(m_L, ezBuf);
					lua_push_net_thread_queue(m_L, &g_DebugAdapterQueue);
					int res = lua_pcall(m_L, 2, 0, 0);
					check_lua_error(m_L, res);

					read_msg();
				}
				else
				{

				}
			});
	}
	void Send(const char* data, size_t len)
	{
		Buffer sendBuff;
		sendBuff.Write(data, len);
		asio::write(m_Socket,
			asio::buffer(sendBuff.Peek(),
				sendBuff.readable_size()));
	}

private:
	asio::io_context& m_IOContext;
	Buffer ezBuf;
	tcp::socket m_Socket;
	std::vector<char> m_Buffer;
	lua_State* m_L;
};

DebugSession* _DbgSession;
void do_accept(tcp::acceptor& acceptor, lua_State* L, asio::io_context& iocontext) {
	acceptor.async_accept(
		[&acceptor, L, &iocontext](std::error_code ec, tcp::socket socket)
		{
			if (!ec)
			{
				_DbgSession = new DebugSession(iocontext, L, std::move(socket));
				do_accept(acceptor, L, iocontext);
			}
		});
}
void DebuggeeThreadFunc(int port)
{
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luadbg_register_common_lua_functions(L);

	int res = luaL_loadbuffer(L, debuggee_code, strlen(debuggee_code), "__debuggee__");
	check_lua_error(L, res);
	lua_pushstring(L, "debuggee");
	res = lua_pcall(L, 1, LUA_MULTRET, 0);
	check_lua_error(L, res);
	asio::io_context iocontext;
	tcp::endpoint endpoint(tcp::v4(), port);
	tcp::acceptor acceptor(iocontext, endpoint);
	do_accept(acceptor, L, iocontext);

	iocontext.run();
	lua_close(L);
}

int luadbg_listen(lua_State* L)
{
	int port = (int)lua_tointeger(L, 1);
	if (port > 0)
	{
		debuggee_thread = new std::thread(DebuggeeThreadFunc, port);
		int res = luaL_loadbuffer(L, debugger_code, strlen(debugger_code), "@__debugger__");
		check_lua_error(L, res);
		lua_pushstring(L, "debugger");
		res = lua_pcall(L, 1, LUA_MULTRET, 0);
		check_lua_error(L, res);
	}
	printf("luadbg_listen %d\n", port);
	return 0;
}

void luadbg_stop()
{
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
		static std::string msgstr;
		auto& msg = g_DebugAdapterQueue.Front(NetThreadQueue::Read);
		msgstr = std::string(msg.Peek(), msg.readable_size());
		g_DebugAdapterQueue.PopFront(NetThreadQueue::Read);
		return msgstr.c_str();
	}
	return "";
}

int debugger_send_message(lua_State* L)
{
	size_t len = 0;
	const char* data = luaL_tolstring(L, 1, &len);
	_DbgSession->Send(data, len);
	return 0;
}

bool debugger_is_connected()
{
	return _DbgSession != nullptr;
}

LuaProxy* (*__proxy__)();
LuaProxy* __lua_proxy__()
{
	return __proxy__();
}

#ifdef __cplusplus
extern "C" {
#endif	 
	LUADBGAPI int _luaopen_luadbg(LuaProxy* (*proxy)(), lua_State* L)
	{
		__proxy__ = proxy;
		luaL_requirelib(L, "cjson", luaopen_cjson);

		script_system_register_luac_function(L, luadbg_listen);
		script_system_register_function(L, luadbg_stop);

		script_system_register_function(L, debugger_sleep);

		script_system_register_function(L, debugger_fetch_message);
		script_system_register_function(L, debugger_is_connected);
		script_system_register_luac_function(L, debugger_send_message);

		script_system_register_function(L, luadbg_get_line_ending_in_c);
		return 1;
	}
#ifdef __cplusplus
}
#endif
