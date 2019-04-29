#pragma once
#include <atomic>
#include <thread>
#include <condition_variable>
#include "FileSystemHelper.h"
#include <string>

static unsigned s_ID_iBoundBufferObjects = 0;
static unsigned s_activeObjects = 0;

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

	unsigned getID();

	long long & getDelta();

	void exit();
	std::atomic<bool>& hasExited();
	void exitedThread();

	void UpdateDelta();

protected:

	unsigned						m_ID;
	std::condition_variable         m_wait;				/// wait conditional
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::atomic<bool>               m_exit;				/// flag exit and clean up
	std::atomic<bool>               m_exitOnWayOut;				/// flag exit and clean up
	long long m_delta;
	std::chrono::time_point<std::chrono::steady_clock > m_timeOld;
};

