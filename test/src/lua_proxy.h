#pragma once
#include "lua.hpp"

struct LuaProxy
{
	virtual lua_State *(vir_lua_newstate)(lua_Alloc f, void *ud) = 0;
	virtual void       (vir_lua_close)(lua_State *L) = 0;
	virtual lua_State *(vir_lua_newthread)(lua_State *L) = 0;
	virtual lua_CFunction(vir_lua_atpanic)(lua_State *L, lua_CFunction panicf) = 0;
	virtual const lua_Number *(vir_lua_version)(lua_State *L) = 0;
	virtual int   (vir_lua_absindex)(lua_State *L, int idx) = 0;
	virtual int   (vir_lua_gettop)(lua_State *L) = 0;
	virtual void  (vir_lua_settop)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_pushvalue)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_rotate)(lua_State *L, int idx, int n) = 0;
	virtual void  (vir_lua_copy)(lua_State *L, int fromidx, int toidx) = 0;
	virtual int   (vir_lua_checkstack)(lua_State *L, int n) = 0;
	virtual void  (vir_lua_xmove)(lua_State *from, lua_State *to, int n) = 0;
	virtual int             (vir_lua_isnumber)(lua_State *L, int idx) = 0;
	virtual int             (vir_lua_isstring)(lua_State *L, int idx) = 0;
	virtual int             (vir_lua_iscfunction)(lua_State *L, int idx) = 0;
	virtual int             (vir_lua_isinteger)(lua_State *L, int idx) = 0;
	virtual int             (vir_lua_isuserdata)(lua_State *L, int idx) = 0;
	virtual int             (vir_lua_type)(lua_State *L, int idx) = 0;
	virtual const char     *(vir_lua_typename)(lua_State *L, int tp) = 0;
	virtual lua_Number(vir_lua_tonumberx)(lua_State *L, int idx, int *isnum) = 0;
	virtual lua_Integer(vir_lua_tointegerx)(lua_State *L, int idx, int *isnum) = 0;
	virtual int             (vir_lua_toboolean)(lua_State *L, int idx) = 0;
	virtual const char     *(vir_lua_tolstring)(lua_State *L, int idx, size_t *len) = 0;
	virtual size_t(vir_lua_rawlen)(lua_State *L, int idx) = 0;
	virtual lua_CFunction(vir_lua_tocfunction)(lua_State *L, int idx) = 0;
	virtual void	       *(vir_lua_touserdata)(lua_State *L, int idx) = 0;
	virtual lua_State      *(vir_lua_tothread)(lua_State *L, int idx) = 0;
	virtual const void     *(vir_lua_topointer)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_arith)(lua_State *L, int op) = 0;
	virtual int   (vir_lua_rawequal)(lua_State *L, int idx1, int idx2) = 0;
	virtual int   (vir_lua_compare)(lua_State *L, int idx1, int idx2, int op) = 0;
	virtual void        (vir_lua_pushnil)(lua_State *L) = 0;
	virtual void        (vir_lua_pushnumber)(lua_State *L, lua_Number n) = 0;
	virtual void        (vir_lua_pushinteger)(lua_State *L, lua_Integer n) = 0;
	virtual const char *(vir_lua_pushlstring)(lua_State *L, const char *s, size_t len) = 0;
	virtual const char *(vir_lua_pushstring)(lua_State *L, const char *s) = 0;
	virtual const char *(vir_lua_pushvfstring)(lua_State *L, const char *fmt, va_list argp) = 0;
	virtual const char *(vir_lua_pushfstring)(lua_State *L, const char *fmt, ...) = 0;
	virtual void  (vir_lua_pushcclosure)(lua_State *L, lua_CFunction fn, int n) = 0;
	virtual void  (vir_lua_pushboolean)(lua_State *L, int b) = 0;
	virtual void  (vir_lua_pushlightuserdata)(lua_State *L, void *p) = 0;
	virtual int   (vir_lua_pushthread)(lua_State *L) = 0;
	virtual int (vir_lua_getglobal)(lua_State *L, const char *name) = 0;
	virtual int (vir_lua_gettable)(lua_State *L, int idx) = 0;
	virtual int (vir_lua_getfield)(lua_State *L, int idx, const char *k) = 0;
	virtual int (vir_lua_geti)(lua_State *L, int idx, lua_Integer n) = 0;
	virtual int (vir_lua_rawget)(lua_State *L, int idx) = 0;
	virtual int (vir_lua_rawgeti)(lua_State *L, int idx, lua_Integer n) = 0;
	virtual int (vir_lua_rawgetp)(lua_State *L, int idx, const void *p) = 0;
	virtual void  (vir_lua_createtable)(lua_State *L, int narr, int nrec) = 0;
	virtual void *(vir_lua_newuserdata)(lua_State *L, size_t sz) = 0;
	virtual int   (vir_lua_getmetatable)(lua_State *L, int objindex) = 0;
	virtual int  (vir_lua_getuservalue)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_setglobal)(lua_State *L, const char *name) = 0;
	virtual void  (vir_lua_settable)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_setfield)(lua_State *L, int idx, const char *k) = 0;
	virtual void  (vir_lua_seti)(lua_State *L, int idx, lua_Integer n) = 0;
	virtual void  (vir_lua_rawset)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_rawseti)(lua_State *L, int idx, lua_Integer n) = 0;
	virtual void  (vir_lua_rawsetp)(lua_State *L, int idx, const void *p) = 0;
	virtual int   (vir_lua_setmetatable)(lua_State *L, int objindex) = 0;
	virtual void  (vir_lua_setuservalue)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_callk)(lua_State *L, int nargs, int nresults, lua_KContext ctx, lua_KFunction k) = 0;
	virtual int (vir_lua_pcallk)(lua_State *L, int nargs, int nresults, int errfunc, lua_KContext ctx, lua_KFunction k) = 0;
	virtual int (vir_lua_load)(lua_State *L, lua_Reader reader, void *data, const char *chunkname, const char *mode) = 0;
	virtual int (vir_lua_dump)(lua_State *L, lua_Writer writer, void *data, int strip) = 0;
	virtual int (vir_lua_yieldk)(lua_State *L, int nresults, lua_KContext ctx, lua_KFunction k) = 0;
	virtual int  (vir_lua_resume)(lua_State *L, lua_State *from, int narg) = 0;
	virtual int  (vir_lua_status)(lua_State *L) = 0;
	virtual int (vir_lua_isyieldable)(lua_State *L) = 0;
	virtual int (vir_lua_gc)(lua_State *L, int what, int data) = 0;
	virtual int   (vir_lua_error)(lua_State *L) = 0;
	virtual int   (vir_lua_next)(lua_State *L, int idx) = 0;
	virtual void  (vir_lua_concat)(lua_State *L, int n) = 0;
	virtual void  (vir_lua_len)(lua_State *L, int idx) = 0;
	virtual size_t(vir_lua_stringtonumber)(lua_State *L, const char *s) = 0;
	virtual lua_Alloc(vir_lua_getallocf)(lua_State *L, void **ud) = 0;
	virtual void      (vir_lua_setallocf)(lua_State *L, lua_Alloc f, void *ud) = 0;
	virtual int (vir_lua_getstack)(lua_State *L, int level, lua_Debug *ar) = 0;
	virtual int (vir_lua_getinfo)(lua_State *L, const char *what, lua_Debug *ar) = 0;
	virtual const char *(vir_lua_getlocal)(lua_State *L, const lua_Debug *ar, int n) = 0;
	virtual const char *(vir_lua_setlocal)(lua_State *L, const lua_Debug *ar, int n) = 0;
	virtual const char *(vir_lua_getupvalue)(lua_State *L, int funcindex, int n) = 0;
	virtual const char *(vir_lua_setupvalue)(lua_State *L, int funcindex, int n) = 0;
	virtual void *(vir_lua_upvalueid)(lua_State *L, int fidx, int n) = 0;
	virtual void (vir_lua_upvaluejoin)(lua_State *L, int fidx1, int n1, int fidx2, int n2) = 0;
	virtual void (vir_lua_sethook)(lua_State *L, lua_Hook func, int mask, int count) = 0;
	virtual lua_Hook(vir_lua_gethook)(lua_State *L) = 0;
	virtual int (vir_lua_gethookmask)(lua_State *L) = 0;
	virtual int (vir_lua_gethookcount)(lua_State *L) = 0;
	virtual int (vir_luaopen_base)(lua_State *L) = 0;
	virtual int (vir_luaopen_coroutine)(lua_State *L) = 0;
	virtual int (vir_luaopen_table)(lua_State *L) = 0;
	virtual int (vir_luaopen_io)(lua_State *L) = 0;
	virtual int (vir_luaopen_os)(lua_State *L) = 0;
	virtual int (vir_luaopen_string)(lua_State *L) = 0;
	virtual int (vir_luaopen_utf8)(lua_State *L) = 0;
	virtual int (vir_luaopen_bit32)(lua_State *L) = 0;
	virtual int (vir_luaopen_math)(lua_State *L) = 0;
	virtual int (vir_luaopen_debug)(lua_State *L) = 0;
	virtual int (vir_luaopen_package)(lua_State *L) = 0;
	virtual void (vir_luaL_checkversion_)(lua_State *L, lua_Number ver, size_t sz) = 0;
	virtual int (vir_luaL_getmetafield)(lua_State *L, int obj, const char *e) = 0;
	virtual int (vir_luaL_callmeta)(lua_State *L, int obj, const char *e) = 0;
	virtual const char *(vir_luaL_tolstring)(lua_State *L, int idx, size_t *len) = 0;
	virtual int (vir_luaL_argerror)(lua_State *L, int arg, const char *extramsg) = 0;
	virtual const char *(vir_luaL_checklstring)(lua_State *L, int arg, size_t *l) = 0;
	virtual const char *(vir_luaL_optlstring)(lua_State *L, int arg, const char *def, size_t *l) = 0;
	virtual lua_Number(vir_luaL_checknumber)(lua_State *L, int arg) = 0;
	virtual lua_Number(vir_luaL_optnumber)(lua_State *L, int arg, lua_Number def) = 0;
	virtual lua_Integer(vir_luaL_checkinteger)(lua_State *L, int arg) = 0;
	virtual lua_Integer(vir_luaL_optinteger)(lua_State *L, int arg, lua_Integer def) = 0;
	virtual void (vir_luaL_checkstack)(lua_State *L, int sz, const char *msg) = 0;
	virtual void (vir_luaL_checktype)(lua_State *L, int arg, int t) = 0;
	virtual void (vir_luaL_checkany)(lua_State *L, int arg) = 0;
	virtual int   (vir_luaL_newmetatable)(lua_State *L, const char *tname) = 0;
	virtual void  (vir_luaL_setmetatable)(lua_State *L, const char *tname) = 0;
	virtual void *(vir_luaL_testudata)(lua_State *L, int ud, const char *tname) = 0;
	virtual void *(vir_luaL_checkudata)(lua_State *L, int ud, const char *tname) = 0;
	virtual void (vir_luaL_where)(lua_State *L, int lvl) = 0;
	virtual int (vir_luaL_error)(lua_State *L, const char *fmt, ...) = 0;
	virtual int (vir_luaL_checkoption)(lua_State *L, int arg, const char *def, const char *const lst[]) = 0;
	virtual int (vir_luaL_fileresult)(lua_State *L, int stat, const char *fname) = 0;
	virtual int (vir_luaL_execresult)(lua_State *L, int stat) = 0;
	virtual int (vir_luaL_ref)(lua_State *L, int t) = 0;
	virtual void (vir_luaL_unref)(lua_State *L, int t, int ref) = 0;
	virtual int (vir_luaL_loadfilex)(lua_State *L, const char *filename, const char *mode) = 0;
	virtual int (vir_luaL_loadbufferx)(lua_State *L, const char *buff, size_t sz, const char *name, const char *mode) = 0;
	virtual int (vir_luaL_loadstring)(lua_State *L, const char *s) = 0;
	virtual lua_State *(vir_luaL_newstate)(void) = 0;
	virtual lua_Integer(vir_luaL_len)(lua_State *L, int idx) = 0;
	virtual const char *(vir_luaL_gsub)(lua_State *L, const char *s, const char *p, const char *r) = 0;
	virtual void (vir_luaL_setfuncs)(lua_State *L, const luaL_Reg *l, int nup) = 0;
	virtual int (vir_luaL_getsubtable)(lua_State *L, int idx, const char *fname) = 0;
	virtual void (vir_luaL_traceback)(lua_State *L, lua_State *L1, const char *msg, int level) = 0;
	virtual void (vir_luaL_requiref)(lua_State *L, const char *modname, lua_CFunction openf, int glb) = 0;
	virtual void (vir_luaL_buffinit)(lua_State *L, luaL_Buffer *B) = 0;
	virtual char *(vir_luaL_prepbuffsize)(luaL_Buffer *B, size_t sz) = 0;
	virtual void (vir_luaL_addlstring)(luaL_Buffer *B, const char *s, size_t l) = 0;
	virtual void (vir_luaL_addstring)(luaL_Buffer *B, const char *s) = 0;
	virtual void (vir_luaL_addvalue)(luaL_Buffer *B) = 0;
	virtual void (vir_luaL_pushresult)(luaL_Buffer *B) = 0;
	virtual void (vir_luaL_pushresultsize)(luaL_Buffer *B, size_t sz) = 0;
	virtual char *(vir_luaL_buffinitsize)(lua_State *L, luaL_Buffer *B, size_t sz) = 0;
	virtual void (vir_luaL_pushmodule)(lua_State *L, const char *modname, int sizehint) = 0;
	virtual void (vir_luaL_openlib)(lua_State *L, const char *libname, const luaL_Reg *l, int nup) = 0;
	virtual void (vir_luaL_openlibs)(lua_State *L) = 0;
};
