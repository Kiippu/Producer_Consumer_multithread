#pragma once
#include <atomic>
#include <thread>
#include <condition_variable>
#include "FileSystemHelper.h"
#include <string>

static unsigned s_ID_iBoundBufferObjects = 0;
static unsigned s_activeObjects = 0;

/// data set created in main()
static std::vector<std::string> s_lines = FileSystemHelper::getInstance().getMapOfLines("./LightData", "originalDataSet", "txt");

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
	/// get ID
	unsigned getID();
	/// get elapsed time
	long long & getDelta();
	/// exit atomic bool = true
	void exit();
	/// has exited?
	std::atomic<bool>& hasExited();
	/// needs to exit 
	void exitedThread();
	/// update personal delta elapsed time
	void UpdateDelta();

protected:

	unsigned						m_ID;							/// class ID
	std::condition_variable         m_wait;							/// wait conditional
	std::mutex                      m_waitMutex;					/// wrapper's mutual exclusion
	std::atomic<bool>               m_exit;							/// flag exit and clean up
	std::atomic<bool>               m_exitOnWayOut;					/// flag exit and clean up
	long long m_delta;												/// delat time
	std::chrono::time_point<std::chrono::steady_clock > m_timeOld;	/// old time to compare with now time
};

