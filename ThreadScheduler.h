#pragma once
#include <thread>
#include <memory>
#include <functional>
#include "Fiber.h"
#include <condition_variable>
#include <mutex>
#include <list>
#include <atomic>


/*
	Class:			ThreadScheduler
	Description:	wrapper for Thread and its tasks
*/


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

private:
	/// loop to check and run tasks
	void process();

private:
	std::unique_ptr<std::thread>    m_masterThread;		/// wrappers thread
	std::condition_variable         m_wait;				/// wait conditional
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::list<Fiber>                m_taskBoard;		/// list of fibers which hold tasks to run
	std::atomic<bool>               m_exit;				/// flag exit and clean up
};

