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

#include <luadbg.h>


int main(int argc ,char** argv)
{
	lua_State * L = luaL_newstate();
	luaL_openlibs(L); 
	luaopen_luadbg(L);
	luaL_dofile(L, "main.lua");
	
	printf("%s\\n", lua_tostring(L, -1));

	return 0;
}