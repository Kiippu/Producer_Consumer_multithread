#include "Consumer.h"
#include "SimpleEvent.h"
#include <Windows.h>



Consumer::Consumer(TrafficDataBaseRaw& globalbuffer_raw, TrafficDataProcessed& globalBuffe_processsed)
	: m_globalbuffer_raw(globalbuffer_raw), 
	m_globalbuffer_processed(globalBuffe_processsed), 
	m_globalEvents(SimpleEvent::getInstance())
{
	m_globalTimer = &Timer::getInstance();
	m_eventID = m_globalEvents.registerEvent("NEW_DATASET_CONSUME", Event([&]() {
		printf("EVENT: Comsumer of ID: %d\n", getID());
		if(!m_exit)
			m_wait.notify_all();
	}));
}


Consumer::~Consumer()
{
	m_globalEvents.removeEvent("NEW_DATASET_CONSUME", m_eventID);
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
			// lock this scope
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer_raw.empty(); });
			// double check exit bool
			if (m_exit)
				return false;
			m_mostRecentData = m_globalbuffer_raw.get();
			//m_globalbuffer.add(std::move(m_mostRecentData));
			if(m_mostRecentData != nullptr)
				consume();
		}
		if (m_globalbuffer_raw.empty())
			m_globalEvents.postEvent("PRINT_DATA");
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
	printf("CONSUMER:%d data{ id:%d, count:%d}\n", getID(), m_mostRecentData->ID, m_mostRecentData->dataPair.second);
	return m_globalbuffer_processed.add(std::move(m_mostRecentData));
}
