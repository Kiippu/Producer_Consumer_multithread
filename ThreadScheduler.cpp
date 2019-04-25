#include "ThreadScheduler.h"

ThreadScheduler::ThreadScheduler()
{
	// create unique thread and pass class method as argument
	m_exit = false;
	m_masterThread = std::unique_ptr<std::thread>(new std::thread(std::bind(&ThreadScheduler::process, this)));
}

ThreadScheduler::~ThreadScheduler()
{
	// on class destruction join thread and clean up vars
	{
		std::lock_guard<std::mutex> lock(m_waitMutex);
		m_exit = true;
		m_wait.notify_one();
	}
	m_masterThread->join();
}

void ThreadScheduler::addFiber(Fiber task)
{
	// lock and add task to list
	std::lock_guard<std::mutex> lock(m_waitMutex);
	m_taskBoard.push_back(task);
	m_wait.notify_one();
}

bool ThreadScheduler::busy()
{
	// return if list is empty
	bool busy = !m_taskBoard.empty();
	return busy;
}

void ThreadScheduler::process()
{
	// infinate loop to call and run tasks
	while (true)
	{
		{
			// lock this scope
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			// check if tasks exist of exist
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_taskBoard.empty(); });
			// double check exit bool
			if (m_exit)
				return;
			// run the task and remove it from the list
			m_taskBoard.front().run();
			m_taskBoard.pop_front();
		}
		///repeat
	}
}
