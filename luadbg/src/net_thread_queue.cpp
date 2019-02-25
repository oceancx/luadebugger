#include "net_thread_queue.h"
#include <assert.h>
#include <iostream>
void NetThreadQueue::PushBack(int q, std::string msg)
{
	assert(checkq(q));
	m_Locks[q].lock();
	m_Queues[q].push_back(msg);
	m_Locks[q].unlock();
}

std::string NetThreadQueue::Front(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	auto f = m_Queues[q].front();
	m_Locks[q].unlock();
	return f;
}

void NetThreadQueue::PopFront(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	m_Queues[q].pop_front();
	m_Locks[q].unlock();
}

int NetThreadQueue::Size(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	int size = (int)m_Queues[q].size();
	m_Locks[q].unlock();
	return size;
}

bool NetThreadQueue::Empty(int q)
{
	assert(checkq(q));
	m_Locks[q].lock();
	auto res = m_Queues[q].empty();
	m_Locks[q].unlock();
	return res;
}

void NetThreadQueue::Clear(int q)
{
	m_Locks[q].lock();
	m_Queues[q].clear();
	m_Locks[q].unlock();
}

void lua_push_net_thread_queue(lua_State*L, NetThreadQueue* q)
{
	NetThreadQueue** ptr = (NetThreadQueue**)lua_newuserdata(L, sizeof(NetThreadQueue*));
	*ptr = q;
	luaL_setmetatable(L, "mt_net_thread_queue");
}

NetThreadQueue* lua_check_net_thread_queue(lua_State*L, int index)
{
	NetThreadQueue** ptr = (NetThreadQueue**)lua_touserdata(L, index);
	return *ptr;
}

int net_thread_queue_push_back(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	const char* msg = lua_tostring(L, 3);
	ptr->PushBack(q, msg);
	return 0;
}

int net_thread_queue_pop_front(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	ptr->PopFront(q);
	return 0;
}


int net_thread_queue_front(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	auto msg = ptr->Front(q);
	lua_pushstring(L, msg.c_str());
	return 1;
}

int net_thread_queue_size(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	int size = ptr->Size(q);
	lua_pushinteger(L, size);
	return 1;
}

int net_thread_queue_empty(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	bool empty = ptr->Empty(q);
	lua_pushboolean(L, empty);
	return 1;
}

int net_thread_queue_clear(lua_State* L)
{
	auto* ptr = lua_check_net_thread_queue(L, 1);
	int q = (int)lua_tointeger(L, 2);
	ptr->Clear(q);
	return 0;
}
luaL_Reg mt_net_thread_queue[] = {
	{ "push_back",net_thread_queue_push_back },
	{ "pop_front",net_thread_queue_pop_front },
	{ "front",net_thread_queue_front },
	{ "size",net_thread_queue_size },
	{ "empty",net_thread_queue_empty },
	{ "clear",net_thread_queue_clear },
	{ NULL, NULL }
};


void luaopen_net_thread_queue(lua_State* L)
{
	if (luaL_newmetatable(L, "mt_net_thread_queue")) {
		luaL_setfuncs(L, mt_net_thread_queue, 0);
		lua_setfield(L, -1, "__index");
	}
	else {
		std::cout << "associate mt_net_thread_queue error!" << std::endl;
	}
}
