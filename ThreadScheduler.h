#pragma once
#include <thread>
#include <memory>
#include <functional>
#include "Fiber.h"
#include <condition_variable>
#include <mutex>
#include <list>
#include <atomic>

static unsigned s_ID = 0;

/*
	Class:			ThreadScheduler
	Description:	wrapper for Thread and its tasks
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

enum THREAD_TYPE {
	GENERAL,
	PRODUCER,
	CONSUMER,
	EVENTS,
	MAX = UINT32_MAX
};


class ThreadScheduler
{
public:
	ThreadScheduler();
	~ThreadScheduler();
public:
	/// add task to thread
	void addFiber(Fiber task);
	/// check if thread is busy
	bool busy();
	/// get thread ID
	unsigned& getID();
	///get/set thread type
	THREAD_TYPE& getOcupation();
	bool setOcupation(THREAD_TYPE type);

	void exit();


private:
	/// loop to check and run tasks
	void process();

private:
	THREAD_TYPE						m_threadType;		/// what the thread is used for
	unsigned						m_threadID;			/// thread ID 
	std::unique_ptr<std::thread>    m_masterThread;		/// wrappers thread
	std::condition_variable         m_wait;				/// wait conditional
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::list<Fiber>                m_taskBoard;		/// list of fibers which hold tasks to run
	std::atomic<bool>               m_exit;				/// flag exit and clean up
};

