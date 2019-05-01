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
	/// event to trigger consume if waiting
	m_eventID = m_globalEvents.registerEvent("NEW_DATASET_CONSUME", Event([&]() {
		if(!m_exit)
			m_wait.notify_one();
	}));
	printf("Consumer of ID:%d created\n", s_activeObjects);
}


Consumer::~Consumer()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);					// lock
	s_activeObjects--;														// static decrement obj counter
	m_globalEvents.removeEvent("NEW_DATASET_CONSUME", m_eventID);			// remove attached events
	printf("----- Finised consuming with ID:%d -----\n", s_activeObjects);	
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
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer_raw.empty(); });
			// double check exit bool
			if (m_exit)
				return false;
			m_mostRecentData = m_globalbuffer_raw.get();	// get data to consume

			if (m_mostRecentData != nullptr) {				// consume or not
				consume();
				consumCount++;								// increment total consumed
			}
			if (m_globalbuffer_raw.empty()) {				// leave if no more data is left
				return false;
			}
		}
		UpdateDelta();										// update elapsed timer
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
	auto result = m_globalbuffer_processed.add(std::move(m_mostRecentData));	// get data

	return result;
}
