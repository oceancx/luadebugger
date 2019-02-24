#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
extern  void luaopen_debugger(lua_State* L);

int main(int argc ,char** argv)
{
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	
	luaopen_debugger(L);
	luaL_dofile(L, argv[1]);
	printf("%s\n", lua_tostring(L, -1));

	return 0;
}