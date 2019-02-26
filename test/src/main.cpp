#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

#include "lua_proxy.h"

//using LuadbgOpenFunc = bool(*)(void* proxy, lua_State* L);
//typedef bool (*luadbg_open_func)(LuaProxy* proxy, lua_State* L);
typedef bool(*luadbg_open_func)(LuaProxy* (*proxy)(), lua_State* L);

int main(int argc ,char** argv)
{
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	//luaopen_debugger(L);
	 
	//luaL_dostring(L, R"-(package.loadlib("D:\\Github\\luadebugger\\test\\luadbg.dll", "luaopen_dbg"))-");

	//luaopen_debugger(L);
	//luaL_dofile(L, argv[1]);
	HMODULE hmod = LoadLibraryA("D:\\Github\\luadebugger\\test\\luadbg.dll");

	if (hmod) {
		/*luadbg_open_func creator = (luadbg_open_func)GetProcAddress(hmod, "luadbg_open");
		creator(__lua_proxy_impl__, L);*/
		luadbg_open_func creator = (luadbg_open_func)GetProcAddress(hmod, "luadbg_openfn");
		creator(__lua_proxy_impl__, L);
	}

	luaL_dofile(L, "d:/Github/luadebugger/test/main.lua");
	
	printf("%s\n", lua_tostring(L, -1));

	return 0;
}