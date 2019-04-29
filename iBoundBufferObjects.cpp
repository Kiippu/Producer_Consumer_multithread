#include "iBoundBufferObjects.h"
#include <chrono>
#include <ctime>

iBoundBufferObjects::iBoundBufferObjects()
{
	m_ID = s_ID_iBoundBufferObjects++;
	m_delta = 0;
	m_exit = false;
	m_exitOnWayOut = false;
	m_timeOld = std::chrono::high_resolution_clock::time_point::max();
}


iBoundBufferObjects::~iBoundBufferObjects()
{
	m_exitOnWayOut = true;
}

unsigned iBoundBufferObjects::getID()
{
	unsigned k = m_ID;
	return k;
}

long long & iBoundBufferObjects::getDelta()
{
	{
		return m_delta;
	}
}

void iBoundBufferObjects::exit() {
	{
		m_exit = true;
	}
}

std::atomic<bool>& iBoundBufferObjects::hasExited()
{
	return m_exitOnWayOut;
}
void iBoundBufferObjects::exitedThread()
{
	m_exitOnWayOut = true;
}

void iBoundBufferObjects::UpdateDelta()
{
	if (m_timeOld == std::chrono::high_resolution_clock::time_point::max())
	{
		m_timeOld = std::chrono::high_resolution_clock::now();
	}
	else
	{
		std::unique_lock<std::mutex> scopedLock(m_waitMutex);
		auto now = std::chrono::high_resolution_clock::now();
		auto differenceInTime = now - m_timeOld;
		m_delta = std::chrono::duration_cast<std::chrono::milliseconds>(differenceInTime).count();
		m_timeOld = now;
		//printf("m_delta: %d\n", m_delta);
	}
}
