#pragma once
#include "lua_bind.h"
#include <string>
#include <mutex>
#include <deque>
class NetThreadQueue
{
public:
	enum EQueue
	{
		Read = 0,
		Write
	};

	NetThreadQueue() = default;
	~NetThreadQueue() = default;
	void PushBack(int q, std::string msg);
	std::string Front(int q);
	void PopFront(int q);
	int Size(int q);
	bool Empty(int q);
	void Clear(int q);

private:
	bool checkq(int q) { return q >= Read && q <= Write; }
	std::mutex m_Locks[2];
	std::deque<std::string> m_Queues[2];
};

void lua_push_net_thread_queue(lua_State*L,NetThreadQueue* q);

void luaopen_net_thread_queue(lua_State* L);