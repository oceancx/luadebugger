#include "lua_net.h"
#include "ezio/tcp_connection.h"
#include <iostream>
using namespace ezio;

static const char* skey_mt_tcp_connection = "key_mt_tcp_connection";
static const char* skey_mt_buffer = "key_mt_buffer";

using LuaTcpConnection = TCPConnection*;
using LuaEzioBuffer = Buffer* ;

TCPConnection* lua_check_tcpconnection(lua_State* L,int index)
{
	LuaTcpConnection* conn = (LuaTcpConnection*)lua_touserdata(L, index);
	return *conn;
}
int tcp_connection_to_host_port(lua_State* L)
{
	TCPConnection* conn = (TCPConnection*)lua_check_tcpconnection(L, 1);
	lua_pushstring(L, conn->peer_addr().ToHostPort().c_str());
	return 1;
}

int tcp_connection_connected(lua_State* L)
{
	TCPConnection* conn = (TCPConnection*)lua_check_tcpconnection(L, 1);
	lua_pushboolean(L, conn->connected());
	return 1;
}

int tcp_connection_cleanup(lua_State* L)
{
	LuaTcpConnection* conn = (LuaTcpConnection*)lua_touserdata(L, 1);
	free(conn);
	conn = nullptr;
	printf("TCPConnection cleanup\n");
	return 0;
}

int tcp_connection_send(lua_State* L)
{
	TCPConnection* conn = (TCPConnection*)lua_check_tcpconnection(L, 1);
	size_t len = 0;
	const char* data = lua_tolstring(L, 2, &len);
	conn->Send(kbase::StringView(data, len));
	return 0;
}

luaL_Reg mt_tcp_connection_reg[] = {
	//{"__gc",tcp_connection_cleanup},
	{ "tohostport",tcp_connection_to_host_port},
	{ "connected",tcp_connection_connected},
	{ "send",tcp_connection_send },
	{ NULL, NULL }
};

void lua_push_tcp_connection(lua_State*L, const TCPConnectionPtr& conn)
{
	LuaTcpConnection* ptr = (LuaTcpConnection*)lua_newuserdata(L, sizeof(LuaTcpConnection));
	*ptr = conn.get();
	luaL_setmetatable(L, skey_mt_tcp_connection);
}

Buffer* lua_check_buffer(lua_State*L , int index)
{
	LuaEzioBuffer* buffer = (LuaEzioBuffer*)lua_touserdata(L, index);
	return *buffer;
}

int buffer_readablesize(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	lua_pushinteger(L, buffer->readable_size());
	return 1;
}

int buffer_preview(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	luaL_Buffer lbuf;
	luaL_buffinitsize(L, &lbuf, len);
	luaL_addlstring(&lbuf, buffer->Peek(), len);
	luaL_pushresult(&lbuf);
	return 1;
}

int buffer_consume(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	buffer->Consume(len);
	return 0;
}

int buffer_readstring(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int len = (int)lua_tointeger(L, 2);
	lua_pushlstring(L,buffer->Peek(),len);
	buffer->Consume(len);
	return 1;
}


int buffer_readfloat(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	float v = buffer->ReadAsFloat();
	lua_pushnumber(L, v);
	return 1;
}

int buffer_readint(lua_State* L)
{
	Buffer* buffer = (Buffer*)lua_check_buffer(L, 1);
	int v = buffer->ReadAsInt32();
	lua_pushinteger(L, v);
	return 1;
}

int buffer_gc(lua_State* L)
{
	LuaEzioBuffer* buf = (LuaEzioBuffer*)lua_touserdata(L, 1);
	free(buf);
	buf = nullptr;
	printf("TCPConnection :gc\n");
	return 0;
}

luaL_Reg mt_buffer_reg[] = {
	{ "readablesize",buffer_readablesize },
	{ "preview",buffer_preview },
	{ "consume",buffer_consume },
	{ "readstring",buffer_readstring },
	{ "readfloat",buffer_readfloat },
	{ "readint",buffer_readint },
	//{ "__gc",buffer_gc },
	{ NULL, NULL }
};

void lua_push_ezio_buffer(lua_State*L, Buffer& buf)
{
	LuaEzioBuffer* ptr =(LuaEzioBuffer*)lua_newuserdata(L, sizeof(LuaEzioBuffer));
	*ptr = &buf;
	luaL_setmetatable(L, skey_mt_buffer);
}

void luaopen_netlib(lua_State* L)
{
	if (luaL_newmetatable(L, skey_mt_tcp_connection)) {
		luaL_setfuncs(L,mt_tcp_connection_reg, 0);
		lua_setfield(L, -1, "__index");	
	}
	else {
		std::cout << "associate mt_tcp_connection error!" << std::endl;
	}

	if (luaL_newmetatable(L, skey_mt_buffer)) {
		luaL_setfuncs(L, mt_buffer_reg, 0);
		lua_setfield(L, -1, "__index");	
	}
	else {
		std::cout << "associate mt_buffer error!" << std::endl;
	}
}
