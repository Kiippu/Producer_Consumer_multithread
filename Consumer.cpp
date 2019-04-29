#include "Consumer.h"
#include "SimpleEvent.h"
#include <Windows.h>



Consumer::Consumer(TrafficDataBaseRaw& globalbuffer_raw, TrafficDataProcessed& globalBuffe_processsed)
	: m_globalbuffer_raw(globalbuffer_raw), 
	m_globalbuffer_processed(globalBuffe_processsed), 
	m_globalEvents(SimpleEvent::getInstance())
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	s_activeObjects++;
	m_globalTimer = &Timer::getInstance();
	m_eventID = m_globalEvents.registerEvent("NEW_DATASET_CONSUME", Event([&]() {
		if(!m_exit)
			m_wait.notify_all();
	}));
	printf("Consumer of ID:%d created\n", s_activeObjects);
}


Consumer::~Consumer()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	s_activeObjects--;
	m_globalEvents.removeEvent("NEW_DATASET_CONSUME", m_eventID);
	printf("----- Finised consuming with ID:%d -----\n", s_activeObjects);
	if (s_activeObjects <= 1) {
		m_globalbuffer_processed.setFull(true);
	}
}


bool Consumer::get()
{
	return false;
}

bool Consumer::process()
{
	// infinate loop to call and run tasks
	while (true)
	{
		{
			//// lock this scope
			//if (m_globalbuffer_raw.empty())
			//	Sleep(100);
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer_raw.empty(); });
			// double check exit bool
			if (m_exit)
				return false;
			m_mostRecentData = m_globalbuffer_raw.get();
			//m_globalbuffer.add(std::move(m_mostRecentData));
			if (m_mostRecentData != nullptr) {
				consume();
				consumCount++;
				//printf("ConsumCount: %d\n", consumCount);
			}
			if (m_globalbuffer_raw.empty()) {
				return false;
			}
		}
		UpdateDelta();
		///repeat
	}
}

bool Consumer::busy()
{
	return false;
}

bool Consumer::consume() 
{
	if (!m_mostRecentData)
		return false;
	auto result = m_globalbuffer_processed.add(std::move(m_mostRecentData));

	return result;
}
