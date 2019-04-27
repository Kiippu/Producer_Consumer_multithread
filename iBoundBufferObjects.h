#pragma once
#include <atomic>
#include <thread>
#include <condition_variable>

static unsigned s_ID_iBoundBufferObjects = 0;

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

	unsigned& getID();


protected:

	unsigned						m_ID;
	std::condition_variable         m_wait;				/// wait conditional
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::atomic<bool>               m_exit;				/// flag exit and clean up
};

