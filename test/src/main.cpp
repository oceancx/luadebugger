#include <iostream>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

}


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