#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}
//extern  void luaopen_debugger(lua_State* L);
#define luaL_requirelib(L,name,fn) (luaL_requiref(L, name, fn, 1),lua_pop(L, 1))
extern "C" int luaopen_cjson(lua_State *L);
int main(int argc ,char** argv)
{
	lua_State * L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requirelib(L, "cjson", luaopen_cjson);

	//luaL_dostring(L, R"-(package.loadlib("D:\\Github\\luadebugger\\test\\luadbg.dll", "luaopen_dbg"))-");

	//luaopen_debugger(L);
	//luaL_dofile(L, argv[1]);
	luaL_dofile(L, "d:/Github/luadebugger/test/main.lua");
	
	printf("%s\n", lua_tostring(L, -1));

	return 0;
}