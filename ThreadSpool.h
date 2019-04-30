#pragma once
#include <memory>
#include <map>
#include "ThreadScheduler.h"
#include <vector>

/*
	Class:			ThreadSpool
	Description:	container and organisation of thread wrappers
*/

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
	/// get coiunt of all type
	unsigned getTypeCount(THREAD_TYPE type);
	/// get max thread count
	unsigned getMaxCount();
	/// clean up threads
	void cleanUpThreads();

private:
	ThreadSpool();														/// private constructor for singlton
	unsigned m_maxThreadsSupported;										/// systems core count
	std::map<THREAD_ID,std::unique_ptr<ThreadScheduler>> m_threadPool;	/// dictionary of threads with ID keys
};

