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


LuaProxy* __lua_proxy__();
#define lua_newstate( f,  ud)  __lua_proxy__()->vir_lua_newstate( f,  ud)
#define lua_close( L)  __lua_proxy__()->vir_lua_close( L)
#define lua_newthread( L)  __lua_proxy__()->vir_lua_newthread( L)
#define lua_atpanic( L,  panicf)  __lua_proxy__()->vir_lua_atpanic( L,  panicf)
#define lua_version( L)  __lua_proxy__()->vir_lua_version( L)
#define lua_absindex( L,  idx)  __lua_proxy__()->vir_lua_absindex( L,  idx)
#define lua_gettop( L)  __lua_proxy__()->vir_lua_gettop( L)
#define lua_settop( L,  idx)  __lua_proxy__()->vir_lua_settop( L,  idx)
#define lua_pushvalue( L,  idx)  __lua_proxy__()->vir_lua_pushvalue( L,  idx)
#define lua_rotate( L,  idx,  n)  __lua_proxy__()->vir_lua_rotate( L,  idx,  n)
#define lua_copy( L,  fromidx,  toidx)  __lua_proxy__()->vir_lua_copy( L,  fromidx,  toidx)
#define lua_checkstack( L,  n)  __lua_proxy__()->vir_lua_checkstack( L,  n)
#define lua_xmove( from,  to,  n)  __lua_proxy__()->vir_lua_xmove( from,  to,  n)
#define lua_isnumber( L,  idx)  __lua_proxy__()->vir_lua_isnumber( L,  idx)
#define lua_isstring( L,  idx)  __lua_proxy__()->vir_lua_isstring( L,  idx)
#define lua_iscfunction( L,  idx)  __lua_proxy__()->vir_lua_iscfunction( L,  idx)
#define lua_isinteger( L,  idx)  __lua_proxy__()->vir_lua_isinteger( L,  idx)
#define lua_isuserdata( L,  idx)  __lua_proxy__()->vir_lua_isuserdata( L,  idx)
#define lua_type( L,  idx)  __lua_proxy__()->vir_lua_type( L,  idx)
#define lua_typename( L,  tp)  __lua_proxy__()->vir_lua_typename( L,  tp)
#define lua_tonumberx( L,  idx,  isnum)  __lua_proxy__()->vir_lua_tonumberx( L,  idx,  isnum)
#define lua_tointegerx( L,  idx,  isnum)  __lua_proxy__()->vir_lua_tointegerx( L,  idx,  isnum)
#define lua_toboolean( L,  idx)  __lua_proxy__()->vir_lua_toboolean( L,  idx)
#define lua_tolstring( L,  idx, len)  __lua_proxy__()->vir_lua_tolstring( L,  idx, len)
#define lua_rawlen( L,  idx)  __lua_proxy__()->vir_lua_rawlen( L,  idx)
#define lua_tocfunction( L,  idx)  __lua_proxy__()->vir_lua_tocfunction( L,  idx)
#define lua_touserdata( L,  idx)  __lua_proxy__()->vir_lua_touserdata( L,  idx)
#define lua_tothread( L,  idx)  __lua_proxy__()->vir_lua_tothread( L,  idx)
#define lua_topointer( L,  idx)  __lua_proxy__()->vir_lua_topointer( L,  idx)
#define lua_arith( L,  op)  __lua_proxy__()->vir_lua_arith( L,  op)
#define lua_rawequal( L,  idx1,  idx2)  __lua_proxy__()->vir_lua_rawequal( L,  idx1,  idx2)
#define lua_compare( L,  idx1,  idx2,  op)  __lua_proxy__()->vir_lua_compare( L,  idx1,  idx2,  op)
#define lua_pushnil( L)  __lua_proxy__()->vir_lua_pushnil( L)
#define lua_pushnumber( L, n)  __lua_proxy__()->vir_lua_pushnumber( L, n)
#define lua_pushinteger( L, n)  __lua_proxy__()->vir_lua_pushinteger( L, n)
#define lua_pushlstring( L,  s, len)  __lua_proxy__()->vir_lua_pushlstring( L,  s, len)
#define lua_pushstring( L,  s)  __lua_proxy__()->vir_lua_pushstring( L,  s)
#define lua_pushvfstring( L,  fmt, argp)  __lua_proxy__()->vir_lua_pushvfstring( L,  fmt, argp)
#define lua_pushfstring( L,  fmt, ...)  __lua_proxy__()->vir_lua_pushfstring( L,  fmt, ...)
#define lua_pushcclosure( L,  fn,  n)  __lua_proxy__()->vir_lua_pushcclosure( L,  fn,  n)
#define lua_pushboolean( L,  b)  __lua_proxy__()->vir_lua_pushboolean( L,  b)
#define lua_pushlightuserdata( L,  p)  __lua_proxy__()->vir_lua_pushlightuserdata( L,  p)
#define lua_pushthread( L)  __lua_proxy__()->vir_lua_pushthread( L)
#define lua_getglobal( L,  name)  __lua_proxy__()->vir_lua_getglobal( L,  name)
#define lua_gettable( L,  idx)  __lua_proxy__()->vir_lua_gettable( L,  idx)
#define lua_getfield( L,  idx,  k)  __lua_proxy__()->vir_lua_getfield( L,  idx,  k)
#define lua_geti( L,  idx, n)  __lua_proxy__()->vir_lua_geti( L,  idx, n)
#define lua_rawget( L,  idx)  __lua_proxy__()->vir_lua_rawget( L,  idx)
#define lua_rawgeti( L,  idx, n)  __lua_proxy__()->vir_lua_rawgeti( L,  idx, n)
#define lua_rawgetp( L,  idx,   p)  __lua_proxy__()->vir_lua_rawgetp( L,  idx,   p)
#define lua_createtable( L,  narr,  nrec)  __lua_proxy__()->vir_lua_createtable( L,  narr,  nrec)
#define lua_newuserdata( L, sz)  __lua_proxy__()->vir_lua_newuserdata( L, sz)
#define lua_getmetatable( L,  objindex)  __lua_proxy__()->vir_lua_getmetatable( L,  objindex)
#define lua_getuservalue( L,  idx)  __lua_proxy__()->vir_lua_getuservalue( L,  idx)
#define lua_setglobal( L,  name)  __lua_proxy__()->vir_lua_setglobal( L,  name)
#define lua_settable( L,  idx)  __lua_proxy__()->vir_lua_settable( L,  idx)
#define lua_setfield( L,  idx,  k)  __lua_proxy__()->vir_lua_setfield( L,  idx,  k)
#define lua_seti( L,  idx, n)  __lua_proxy__()->vir_lua_seti( L,  idx, n)
#define lua_rawset( L,  idx)  __lua_proxy__()->vir_lua_rawset( L,  idx)
#define lua_rawseti( L,  idx, n)  __lua_proxy__()->vir_lua_rawseti( L,  idx, n)
#define lua_rawsetp( L,  idx,   p)  __lua_proxy__()->vir_lua_rawsetp( L,  idx,   p)
#define lua_setmetatable( L,  objindex)  __lua_proxy__()->vir_lua_setmetatable( L,  objindex)
#define lua_setuservalue( L,  idx)  __lua_proxy__()->vir_lua_setuservalue( L,  idx)
#define lua_callk( L,  nargs,  nresults, ctx, k)  __lua_proxy__()->vir_lua_callk( L,  nargs,  nresults, ctx, k)
#define lua_pcallk( L,  nargs,  nresults,  errfunc, ctx, k)  __lua_proxy__()->vir_lua_pcallk( L,  nargs,  nresults,  errfunc, ctx, k)
#define lua_load( L, reader,  data,  chunkname,  mode)  __lua_proxy__()->vir_lua_load( L, reader,  data,  chunkname,  mode)
#define lua_dump( L, writer,  data,  strip)  __lua_proxy__()->vir_lua_dump( L, writer,  data,  strip)
#define lua_yieldk( L,  nresults, ctx, k)  __lua_proxy__()->vir_lua_yieldk( L,  nresults, ctx, k)
#define lua_resume( L,  from,  narg)  __lua_proxy__()->vir_lua_resume( L,  from,  narg)
#define lua_status( L)  __lua_proxy__()->vir_lua_status( L)
#define lua_isyieldable( L)  __lua_proxy__()->vir_lua_isyieldable( L)
#define lua_gc( L,  what,  data)  __lua_proxy__()->vir_lua_gc( L,  what,  data)
#define lua_error( L)  __lua_proxy__()->vir_lua_error( L)
#define lua_next( L,  idx)  __lua_proxy__()->vir_lua_next( L,  idx)
#define lua_concat( L,  n)  __lua_proxy__()->vir_lua_concat( L,  n)
#define lua_len( L,  idx)  __lua_proxy__()->vir_lua_len( L,  idx)
#define lua_stringtonumber( L,  s)  __lua_proxy__()->vir_lua_stringtonumber( L,  s)
#define lua_getallocf( L,  ud)  __lua_proxy__()->vir_lua_getallocf( L,  ud)
#define lua_setallocf( L,  f,  ud)  __lua_proxy__()->vir_lua_setallocf( L,  f,  ud)
#define lua_getstack( L,  level, ar)  __lua_proxy__()->vir_lua_getstack( L,  level, ar)
#define lua_getinfo( L,  what, ar)  __lua_proxy__()->vir_lua_getinfo( L,  what, ar)
#define lua_getlocal( L,  ar,  n)  __lua_proxy__()->vir_lua_getlocal( L,  ar,  n)
#define lua_setlocal( L,  ar,  n)  __lua_proxy__()->vir_lua_setlocal( L,  ar,  n)
#define lua_getupvalue( L,  funcindex,  n)  __lua_proxy__()->vir_lua_getupvalue( L,  funcindex,  n)
#define lua_setupvalue( L,  funcindex,  n)  __lua_proxy__()->vir_lua_setupvalue( L,  funcindex,  n)
#define lua_upvalueid( L,  fidx,  n)  __lua_proxy__()->vir_lua_upvalueid( L,  fidx,  n)
#define lua_upvaluejoin( L,  fidx1,  n1,  fidx2,  n2)  __lua_proxy__()->vir_lua_upvaluejoin( L,  fidx1,  n1,  fidx2,  n2)
#define lua_sethook( L, func,  mask,  count)  __lua_proxy__()->vir_lua_sethook( L, func,  mask,  count)
#define lua_gethook( L)  __lua_proxy__()->vir_lua_gethook( L)
#define lua_gethookmask( L)  __lua_proxy__()->vir_lua_gethookmask( L)
#define lua_gethookcount( L)  __lua_proxy__()->vir_lua_gethookcount( L)
#define luaopen_base( L)  __lua_proxy__()->vir_luaopen_base( L)
#define luaopen_coroutine( L)  __lua_proxy__()->vir_luaopen_coroutine( L)
#define luaopen_table( L)  __lua_proxy__()->vir_luaopen_table( L)
#define luaopen_io( L)  __lua_proxy__()->vir_luaopen_io( L)
#define luaopen_os( L)  __lua_proxy__()->vir_luaopen_os( L)
#define luaopen_string( L)  __lua_proxy__()->vir_luaopen_string( L)
#define luaopen_utf8( L)  __lua_proxy__()->vir_luaopen_utf8( L)
#define luaopen_bit32( L)  __lua_proxy__()->vir_luaopen_bit32( L)
#define luaopen_math( L)  __lua_proxy__()->vir_luaopen_math( L)
#define luaopen_debug( L)  __lua_proxy__()->vir_luaopen_debug( L)
#define luaopen_package( L)  __lua_proxy__()->vir_luaopen_package( L)
#define luaL_checkversion_( L, ver, sz)  __lua_proxy__()->vir_luaL_checkversion_( L, ver, sz)
#define luaL_getmetafield( L,  obj,  e)  __lua_proxy__()->vir_luaL_getmetafield( L,  obj,  e)
#define luaL_callmeta( L,  obj,  e)  __lua_proxy__()->vir_luaL_callmeta( L,  obj,  e)
#define luaL_tolstring( L,  idx, len)  __lua_proxy__()->vir_luaL_tolstring( L,  idx, len)
#define luaL_argerror( L,  arg,  extramsg)  __lua_proxy__()->vir_luaL_argerror( L,  arg,  extramsg)
#define luaL_checklstring( L,  arg, l)  __lua_proxy__()->vir_luaL_checklstring( L,  arg, l)
#define luaL_optlstring( L,  arg,  def, l)  __lua_proxy__()->vir_luaL_optlstring( L,  arg,  def, l)
#define luaL_checknumber( L,  arg)  __lua_proxy__()->vir_luaL_checknumber( L,  arg)
#define luaL_optnumber( L,  arg, def)  __lua_proxy__()->vir_luaL_optnumber( L,  arg, def)
#define luaL_checkinteger( L,  arg)  __lua_proxy__()->vir_luaL_checkinteger( L,  arg)
#define luaL_optinteger( L,  arg, def)  __lua_proxy__()->vir_luaL_optinteger( L,  arg, def)
#define luaL_checkstack( L,  sz,  msg)  __lua_proxy__()->vir_luaL_checkstack( L,  sz,  msg)
#define luaL_checktype( L,  arg,  t)  __lua_proxy__()->vir_luaL_checktype( L,  arg,  t)
#define luaL_checkany( L,  arg)  __lua_proxy__()->vir_luaL_checkany( L,  arg)
#define luaL_newmetatable( L,  tname)  __lua_proxy__()->vir_luaL_newmetatable( L,  tname)
#define luaL_setmetatable( L,  tname)  __lua_proxy__()->vir_luaL_setmetatable( L,  tname)
#define luaL_testudata( L,  ud,  tname)  __lua_proxy__()->vir_luaL_testudata( L,  ud,  tname)
#define luaL_checkudata( L,  ud,  tname)  __lua_proxy__()->vir_luaL_checkudata( L,  ud,  tname)
#define luaL_where( L,  lvl)  __lua_proxy__()->vir_luaL_where( L,  lvl)
#define luaL_error( L,  fmt )  __lua_proxy__()->vir_luaL_error( L,  fmt)
#define luaL_checkoption( L,  arg,  def,   lst)  __lua_proxy__()->vir_luaL_checkoption( L,  arg,  def,   lst)
#define luaL_fileresult( L,  stat,  fname)  __lua_proxy__()->vir_luaL_fileresult( L,  stat,  fname)
#define luaL_execresult( L,  stat)  __lua_proxy__()->vir_luaL_execresult( L,  stat)
#define luaL_ref( L,  t)  __lua_proxy__()->vir_luaL_ref( L,  t)
#define luaL_unref( L,  t,  ref)  __lua_proxy__()->vir_luaL_unref( L,  t,  ref)
#define luaL_loadfilex( L,  filename,  mode)  __lua_proxy__()->vir_luaL_loadfilex( L,  filename,  mode)
#define luaL_loadbufferx( L,  buff, sz,  name,  mode)  __lua_proxy__()->vir_luaL_loadbufferx( L,  buff, sz,  name,  mode)
#define luaL_loadstring( L,  s)  __lua_proxy__()->vir_luaL_loadstring( L,  s)
#define luaL_newstate()  __lua_proxy__()->vir_luaL_newstate()
#define luaL_len( L,  idx)  __lua_proxy__()->vir_luaL_len( L,  idx)
#define luaL_gsub( L,  s,  p,  r)  __lua_proxy__()->vir_luaL_gsub( L,  s,  p,  r)
#define luaL_setfuncs( L,  l,  nup)  __lua_proxy__()->vir_luaL_setfuncs( L,  l,  nup)
#define luaL_getsubtable( L,  idx,  fname)  __lua_proxy__()->vir_luaL_getsubtable( L,  idx,  fname)
#define luaL_traceback( L,  L1,  msg,  level)  __lua_proxy__()->vir_luaL_traceback( L,  L1,  msg,  level)
#define luaL_requiref( L,  modname,  openf,  glb)  __lua_proxy__()->vir_luaL_requiref( L,  modname,  openf,  glb)
#define luaL_buffinit( L, B)  __lua_proxy__()->vir_luaL_buffinit( L, B)
#define luaL_prepbuffsize(B, sz)  __lua_proxy__()->vir_luaL_prepbuffsize(B, sz)
#define luaL_addlstring(B,  s, l)  __lua_proxy__()->vir_luaL_addlstring(B,  s, l)
#define luaL_addstring(B,  s)  __lua_proxy__()->vir_luaL_addstring(B,  s)
#define luaL_addvalue(B)  __lua_proxy__()->vir_luaL_addvalue(B)
#define luaL_pushresult(B)  __lua_proxy__()->vir_luaL_pushresult(B)
#define luaL_pushresultsize(B, sz)  __lua_proxy__()->vir_luaL_pushresultsize(B, sz)
#define luaL_buffinitsize( L, B, sz)  __lua_proxy__()->vir_luaL_buffinitsize( L, B, sz)
#define luaL_pushmodule( L,  modname,  sizeh)  __lua_proxy__()->vir_luaL_pushmodule( L,  modname,  sizeh)
#define luaL_openlib( L,  libname,  l,  nup)  __lua_proxy__()->vir_luaL_openlib( L,  libname,  l,  nup)
#define luaL_openlibs( L)  __lua_proxy__()->vir_luaL_openlibs( L)

