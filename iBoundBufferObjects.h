#pragma once
#include <atomic>
#include <thread>
#include <condition_variable>

class iBoundBufferObjects
{
public:
	iBoundBufferObjects();
	~iBoundBufferObjects();
	/// do what needs to be done
	virtual bool process() = 0;
	/// checlk if busy
	virtual bool busy() = 0;
	/// get needed data 
	virtual bool get() = 0;


private:

	std::condition_variable         m_wait;				/// wait conditional
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::atomic<bool>               m_exit;				/// flag exit and clean up
};

