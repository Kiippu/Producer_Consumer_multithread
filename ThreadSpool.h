#pragma once
#include <memory>
#include <map>
#include "ThreadScheduler.h"

/*
	Class:			ThreadSpool
	Description:	container and organisation of thread wrappers
*/

/// thread ID
enum THREAD_ID {
	ID0,
	ID1,
	ID2,
	ID3,
	ID4,
	ID5,
	ID6,
	ID7,
	ID_MAX
};

class ThreadSpool
{
public:
	/// singleton implementation
	static ThreadSpool&getInstance()
	{
		static ThreadSpool instance;
		return instance;
	}
	/// no copy
	ThreadSpool(ThreadSpool&temp) = delete;
	void operator=(ThreadSpool const&temp) = delete;

	/// get size of thread pool on current system
	size_t count() { return m_threadPool.size(); };
	/// access how many threads are avaliable
	unsigned isAvaliable();
	/// access a thread that is not busy
	ThreadScheduler * getAvaliable();
	/// access a thread by ID
	ThreadScheduler * get(THREAD_ID);

private:
	ThreadSpool();														/// private constructor for singlton
	unsigned m_maxThreadsSupported;										/// systems core count
	std::map<THREAD_ID,std::unique_ptr<ThreadScheduler>> m_threadPool;	/// dictionary of threads with ID keys
};

