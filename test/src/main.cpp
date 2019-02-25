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

//extern  void luaopen_debugger(lua_State* L);

//extern void luaopen_debugger(lua_State* L);
#include "lua_proxy.h"
struct  LuaProxyImpl : public LuaProxy
{
	lua_State *(vir_lua_newstate)(lua_Alloc f, void *ud) { return lua_newstate(f, ud); }
	void       (vir_lua_close)(lua_State *L) { return lua_close(L); }
	lua_State *(vir_lua_newthread)(lua_State *L) { return lua_newthread(L); }
	lua_CFunction(vir_lua_atpanic)(lua_State *L, lua_CFunction panicf) { return lua_atpanic(L, panicf); }
	const lua_Number *(vir_lua_version)(lua_State *L) { return lua_version(L); }
	int   (vir_lua_absindex)(lua_State *L, int idx) { return lua_absindex(L, idx); }
	int   (vir_lua_gettop)(lua_State *L) { return lua_gettop(L); }
	void  (vir_lua_settop)(lua_State *L, int idx) { return lua_settop(L, idx); }
	void  (vir_lua_pushvalue)(lua_State *L, int idx) { return lua_pushvalue(L, idx); }
	void  (vir_lua_rotate)(lua_State *L, int idx, int n) { return lua_rotate(L, idx, n); }
	void  (vir_lua_copy)(lua_State *L, int fromidx, int toidx) { return lua_copy(L, fromidx, toidx); }
	int   (vir_lua_checkstack)(lua_State *L, int n) { return lua_checkstack(L, n); }
	void  (vir_lua_xmove)(lua_State *from, lua_State *to, int n) { return lua_xmove(from, to, n); }
	int             (vir_lua_isnumber)(lua_State *L, int idx) { return lua_isnumber(L, idx); }
	int             (vir_lua_isstring)(lua_State *L, int idx) { return lua_isstring(L, idx); }
	int             (vir_lua_iscfunction)(lua_State *L, int idx) { return lua_iscfunction(L, idx); }
	int             (vir_lua_isinteger)(lua_State *L, int idx) { return lua_isinteger(L, idx); }
	int             (vir_lua_isuserdata)(lua_State *L, int idx) { return lua_isuserdata(L, idx); }
	int             (vir_lua_type)(lua_State *L, int idx) { return lua_type(L, idx); }
	const char     *(vir_lua_typename)(lua_State *L, int tp) { return lua_typename(L, tp); }
	lua_Number(vir_lua_tonumberx)(lua_State *L, int idx, int *isnum) { return lua_tonumberx(L, idx, isnum); }
	lua_Integer(vir_lua_tointegerx)(lua_State *L, int idx, int *isnum) { return lua_tointegerx(L, idx, isnum); }
	int             (vir_lua_toboolean)(lua_State *L, int idx) { return lua_toboolean(L, idx); }
	const char     *(vir_lua_tolstring)(lua_State *L, int idx, size_t *len) { return lua_tolstring(L, idx, len); }
	size_t(vir_lua_rawlen)(lua_State *L, int idx) { return lua_rawlen(L, idx); }
	lua_CFunction(vir_lua_tocfunction)(lua_State *L, int idx) { return lua_tocfunction(L, idx); }
	void	       *(vir_lua_touserdata)(lua_State *L, int idx) { return lua_touserdata(L, idx); }
	lua_State      *(vir_lua_tothread)(lua_State *L, int idx) { return lua_tothread(L, idx); }
	const void     *(vir_lua_topointer)(lua_State *L, int idx) { return lua_topointer(L, idx); }
	void  (vir_lua_arith)(lua_State *L, int op) { return lua_arith(L, op); }
	int   (vir_lua_rawequal)(lua_State *L, int idx1, int idx2) { return lua_rawequal(L, idx1, idx2); }
	int   (vir_lua_compare)(lua_State *L, int idx1, int idx2, int op) { return lua_compare(L, idx1, idx2, op); }
	void        (vir_lua_pushnil)(lua_State *L) { return lua_pushnil(L); }
	void        (vir_lua_pushnumber)(lua_State *L, lua_Number n) { return lua_pushnumber(L, n); }
	void        (vir_lua_pushinteger)(lua_State *L, lua_Integer n) { return lua_pushinteger(L, n); }
	const char *(vir_lua_pushlstring)(lua_State *L, const char *s, size_t len) { return lua_pushlstring(L, s, len); }
	const char *(vir_lua_pushstring)(lua_State *L, const char *s) { return lua_pushstring(L, s); }
	const char *(vir_lua_pushvfstring)(lua_State *L, const char *fmt, va_list argp) { return lua_pushvfstring(L, fmt, argp); }
	const char *(vir_lua_pushfstring)(lua_State *L, const char *fmt, ...) { return 0; }// return lua_pushfstring(L, fmt, ...);}
	void  (vir_lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n) { return lua_pushcclosure(L, fn, n); }
	void  (vir_lua_pushboolean)(lua_State *L, int b) { return lua_pushboolean(L, b); }
	void  (vir_lua_pushlightuserdata)(lua_State *L, void *p) { return lua_pushlightuserdata(L, p); }
	int   (vir_lua_pushthread)(lua_State *L) { return lua_pushthread(L); }
	int (vir_lua_getglobal)(lua_State *L, const char *name) { return lua_getglobal(L, name); }
	int (vir_lua_gettable)(lua_State *L, int idx) { return lua_gettable(L, idx); }
	int (vir_lua_getfield)(lua_State *L, int idx, const char *k) { return lua_getfield(L, idx, k); }
	int (vir_lua_geti)(lua_State *L, int idx, lua_Integer n) { return lua_geti(L, idx, n); }
	int (vir_lua_rawget)(lua_State *L, int idx) { return lua_rawget(L, idx); }
	int (vir_lua_rawgeti)(lua_State *L, int idx, lua_Integer n) { return lua_rawgeti(L, idx, n); }
	int (vir_lua_rawgetp)(lua_State *L, int idx, const void *p) { return lua_rawgetp(L, idx, p); }
	void  (vir_lua_createtable)(lua_State *L, int narr, int nrec) { return lua_createtable(L, narr, nrec); }
	void *(vir_lua_newuserdata)(lua_State *L, size_t sz) { return lua_newuserdata(L, sz); }
	int   (vir_lua_getmetatable)(lua_State *L, int objindex) { return lua_getmetatable(L, objindex); }
	int  (vir_lua_getuservalue)(lua_State *L, int idx) { return lua_getuservalue(L, idx); }
	void  (vir_lua_setglobal)(lua_State *L, const char *name) { return lua_setglobal(L, name); }
	void  (vir_lua_settable)(lua_State *L, int idx) { return lua_settable(L, idx); }
	void  (vir_lua_setfield)(lua_State *L, int idx, const char *k) { return lua_setfield(L, idx, k); }
	void  (vir_lua_seti)(lua_State *L, int idx, lua_Integer n) { return lua_seti(L, idx, n); }
	void  (vir_lua_rawset)(lua_State *L, int idx) { return lua_rawset(L, idx); }
	void  (vir_lua_rawseti)(lua_State *L, int idx, lua_Integer n) { return lua_rawseti(L, idx, n); }
	void  (vir_lua_rawsetp)(lua_State *L, int idx, const void *p) { return lua_rawsetp(L, idx, p); }
	int   (vir_lua_setmetatable)(lua_State *L, int objindex) { return lua_setmetatable(L, objindex); }
	void  (vir_lua_setuservalue)(lua_State *L, int idx) { return lua_setuservalue(L, idx); }
	void  (vir_lua_callk)(lua_State *L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k) { return lua_callk(L, nargs, nresults, ctx, k); }
	int (vir_lua_pcallk)(lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k) { return lua_pcallk(L, nargs, nresults, errfunc, ctx, k); }
	int (vir_lua_load)(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode) { return lua_load(L, reader, data, chunkname, mode); }
	int (vir_lua_dump)(lua_State *L, lua_Writer writer, void *data, int strip) { return lua_dump(L, writer, data, strip); }
	int (vir_lua_yieldk)(lua_State *L, int nresults, lua_KContext ctx, lua_KFunction k) { return lua_yieldk(L, nresults, ctx, k); }
	int  (vir_lua_resume)(lua_State *L, lua_State *from, int narg) { return lua_resume(L, from, narg); }
	int  (vir_lua_status)(lua_State *L) { return lua_status(L); }
	int (vir_lua_isyieldable)(lua_State *L) { return lua_isyieldable(L); }
	int (vir_lua_gc)(lua_State *L, int what, int data) { return lua_gc(L, what, data); }
	int   (vir_lua_error)(lua_State *L) { return lua_error(L); }
	int   (vir_lua_next)(lua_State *L, int idx) { return lua_next(L, idx); }
	void  (vir_lua_concat)(lua_State *L, int n) { return lua_concat(L, n); }
	void  (vir_lua_len)(lua_State *L, int idx) { return lua_len(L, idx); }
	size_t(vir_lua_stringtonumber)(lua_State *L, const char *s) { return lua_stringtonumber(L, s); }
	lua_Alloc(vir_lua_getallocf)(lua_State *L, void **ud) { return lua_getallocf(L, ud); }
	void      (vir_lua_setallocf)(lua_State *L, lua_Alloc f, void *ud) { return lua_setallocf(L, f, ud); }
	int (vir_lua_getstack)(lua_State *L, int level, lua_Debug *ar) { return lua_getstack(L, level, ar); }
	int (vir_lua_getinfo)(lua_State *L, const char *what, lua_Debug *ar) { return lua_getinfo(L, what, ar); }
	const char *(vir_lua_getlocal)(lua_State *L, const lua_Debug *ar, int n) { return lua_getlocal(L, ar, n); }
	const char *(vir_lua_setlocal)(lua_State *L, const lua_Debug *ar, int n) { return lua_setlocal(L, ar, n); }
	const char *(vir_lua_getupvalue)(lua_State *L, int funcindex, int n) { return lua_getupvalue(L, funcindex, n); }
	const char *(vir_lua_setupvalue)(lua_State *L, int funcindex, int n) { return lua_setupvalue(L, funcindex, n); }
	void *(vir_lua_upvalueid)(lua_State *L, int fidx, int n) { return lua_upvalueid(L, fidx, n); }
	void (vir_lua_upvaluejoin)(lua_State *L, int fidx1, int n1, int fidx2, int n2) { return lua_upvaluejoin(L, fidx1, n1, fidx2, n2); }
	void (vir_lua_sethook)(lua_State *L, lua_Hook func, int mask, int count) { return lua_sethook(L, func, mask, count); }
	lua_Hook(vir_lua_gethook)(lua_State *L) { return lua_gethook(L); }
	int (vir_lua_gethookmask)(lua_State *L) { return lua_gethookmask(L); }
	int (vir_lua_gethookcount)(lua_State *L) { return lua_gethookcount(L); }
	int (vir_luaopen_base)(lua_State *L) { return luaopen_base(L); }
	int (vir_luaopen_coroutine)(lua_State *L) { return luaopen_coroutine(L); }
	int (vir_luaopen_table)(lua_State *L) { return luaopen_table(L); }
	int (vir_luaopen_io)(lua_State *L) { return luaopen_io(L); }
	int (vir_luaopen_os)(lua_State *L) { return luaopen_os(L); }
	int (vir_luaopen_string)(lua_State *L) { return luaopen_string(L); }
	int (vir_luaopen_utf8)(lua_State *L) { return luaopen_utf8(L); }
	int (vir_luaopen_bit32)(lua_State *L) { return luaopen_bit32(L); }
	int (vir_luaopen_math)(lua_State *L) { return luaopen_math(L); }
	int (vir_luaopen_debug)(lua_State *L) { return luaopen_debug(L); }
	int (vir_luaopen_package)(lua_State *L) { return luaopen_package(L); }
	void (vir_luaL_checkversion_)(lua_State *L, lua_Number ver, size_t sz) { return luaL_checkversion_(L, ver, sz); }
	int (vir_luaL_getmetafield)(lua_State *L, int obj, const char *e) { return luaL_getmetafield(L, obj, e); }
	int (vir_luaL_callmeta)(lua_State *L, int obj, const char *e) { return luaL_callmeta(L, obj, e); }
	const char *(vir_luaL_tolstring)(lua_State *L, int idx, size_t *len) { return luaL_tolstring(L, idx, len); }
	int (vir_luaL_argerror)(lua_State *L, int arg, const char *extramsg) { return luaL_argerror(L, arg, extramsg); }
	const char *(vir_luaL_checklstring)(lua_State *L, int arg, size_t *l) { return luaL_checklstring(L, arg, l); }
	const char *(vir_luaL_optlstring)(lua_State *L, int arg, const char *def, size_t *l) { return luaL_optlstring(L, arg, def, l); }
	lua_Number(vir_luaL_checknumber)(lua_State *L, int arg) { return luaL_checknumber(L, arg); }
	lua_Number(vir_luaL_optnumber)(lua_State *L, int arg, lua_Number def) { return luaL_optnumber(L, arg, def); }
	lua_Integer(vir_luaL_checkinteger)(lua_State *L, int arg) { return luaL_checkinteger(L, arg); }
	lua_Integer(vir_luaL_optinteger)(lua_State *L, int arg, lua_Integer def) { return luaL_optinteger(L, arg, def); }
	void (vir_luaL_checkstack)(lua_State *L, int sz, const char *msg) { return luaL_checkstack(L, sz, msg); }
	void (vir_luaL_checktype)(lua_State *L, int arg, int t) { return luaL_checktype(L, arg, t); }
	void (vir_luaL_checkany)(lua_State *L, int arg) { return luaL_checkany(L, arg); }
	int   (vir_luaL_newmetatable)(lua_State *L, const char *tname) { return luaL_newmetatable(L, tname); }
	void  (vir_luaL_setmetatable)(lua_State *L, const char *tname) { return luaL_setmetatable(L, tname); }
	void *(vir_luaL_testudata)(lua_State *L, int ud, const char *tname) { return luaL_testudata(L, ud, tname); }
	void *(vir_luaL_checkudata)(lua_State *L, int ud, const char *tname) { return luaL_checkudata(L, ud, tname); }
	void (vir_luaL_where)(lua_State *L, int lvl) { return luaL_where(L, lvl); }
	int (vir_luaL_error)(lua_State *L, const char *fmt, ...) { return luaL_error(L, fmt, 0); }
	int (vir_luaL_checkoption)(lua_State *L, int arg, const char *def, const char *const lst[]) { return luaL_checkoption(L, arg, def, lst); }
	int (vir_luaL_fileresult)(lua_State *L, int stat, const char *fname) { return luaL_fileresult(L, stat, fname); }
	int (vir_luaL_execresult)(lua_State *L, int stat) { return luaL_execresult(L, stat); }
	int (vir_luaL_ref)(lua_State *L, int t) { return luaL_ref(L, t); }
	void (vir_luaL_unref)(lua_State *L, int t, int ref) { return luaL_unref(L, t, ref); }
	int (vir_luaL_loadfilex)(lua_State *L, const char *filename, const char *mode) { return luaL_loadfilex(L, filename, mode); }
	int (vir_luaL_loadbufferx)(lua_State *L, const char *buff, size_t sz, const char *name, const char *mode) { return luaL_loadbufferx(L, buff, sz, name, mode); }
	int (vir_luaL_loadstring)(lua_State *L, const char *s) { return luaL_loadstring(L, s); }
	lua_State *(vir_luaL_newstate)(void) { return luaL_newstate(); }
	lua_Integer(vir_luaL_len)(lua_State *L, int idx) { return luaL_len(L, idx); }
	const char *(vir_luaL_gsub)(lua_State *L, const char *s, const char *p, const char *r) { return luaL_gsub(L, s, p, r); }
	void (vir_luaL_setfuncs)(lua_State *L, const luaL_Reg *l, int nup) { return luaL_setfuncs(L, l, nup); }
	int (vir_luaL_getsubtable)(lua_State *L, int idx, const char *fname) { return luaL_getsubtable(L, idx, fname); }
	void (vir_luaL_traceback)(lua_State *L, lua_State *L1, const char *msg, int level) { return luaL_traceback(L, L1, msg, level); }
	void (vir_luaL_requiref)(lua_State *L, const char *modname, lua_CFunction openf, int glb) { return luaL_requiref(L, modname, openf, glb); }
	void (vir_luaL_buffinit)(lua_State *L, luaL_Buffer *B) { return luaL_buffinit(L, B); }
	char *(vir_luaL_prepbuffsize)(luaL_Buffer *B, size_t sz) { return luaL_prepbuffsize(B, sz); }
	void (vir_luaL_addlstring)(luaL_Buffer *B, const char *s, size_t l) { return luaL_addlstring(B, s, l); }
	void (vir_luaL_addstring)(luaL_Buffer *B, const char *s) { return luaL_addstring(B, s); }
	void (vir_luaL_addvalue)(luaL_Buffer *B) { return luaL_addvalue(B); }
	void (vir_luaL_pushresult)(luaL_Buffer *B) { return luaL_pushresult(B); }
	void (vir_luaL_pushresultsize)(luaL_Buffer *B, size_t sz) { return luaL_pushresultsize(B, sz); }
	char *(vir_luaL_buffinitsize)(lua_State *L, luaL_Buffer *B, size_t sz) { return luaL_buffinitsize(L, B, sz); }
	void (vir_luaL_pushmodule)(lua_State *L, const char *modname, int sizehint) { return luaL_pushmodule(L, modname, sizehint); }
	void (vir_luaL_openlib)(lua_State *L, const char *libname, const luaL_Reg *l, int nup) { return luaL_openlib(L, libname, l, nup); }
	void (vir_luaL_openlibs)(lua_State *L) { return luaL_openlibs(L); }

	
};

LuaProxyImpl lua_proxy;

using LuadbgOpenFunc = bool(*)(LuaProxy* proxy, lua_State* L);

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
		LuadbgOpenFunc	creator = (LuadbgOpenFunc)GetProcAddress(hmod, "luadbg_open");
		creator(&lua_proxy, L);
	}

	luaL_dofile(L, "d:/Github/luadebugger/test/main.lua");
	
	printf("%s\n", lua_tostring(L, -1));

	return 0;
}