#include "Producer.h"
#include "SimpleEvent.h"
#include "Timer.h"




Producer::Producer(TrafficDataBaseRaw& Globalbuffer, SimpleEvent& GlobalEventSystem)
	: m_globalbuffer(Globalbuffer), m_globalEvents(GlobalEventSystem)
{
	m_globalTimer = &Timer::getInstance();
	m_eventID = m_globalEvents.registerEvent("NEW_DATASET",Event([&]() {
		printf("EVENT: producer of ID: %d\n", getID());
		if (!m_exit)
			if (s_lightIDs == s_lightIDsMax)
				reset();
	}));
	printf("Producer of ID:%d created\n", getID());
}


Producer::~Producer()
{
	m_globalEvents.removeEvent("NEW_DATASET", m_eventID);
}

bool Producer::get()
{
	return false;
}

bool Producer::process()
{
	// infinate loop to call and run tasks
	while (true)
	{
		{
			// lock this scope
			if(s_lightIDs == 0)
				printf("+++ Starting Produce loop\n");
			if (s_lightIDs == s_lightIDsMax) {
				printf("+++ Finised production, TrafficDataBaseRaw::list.() == %d \n", m_globalbuffer.size());
				return true;
			}
			produce();

			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			// check  exist and exit flag
			m_wait.wait(scopedLock, [&]() { return m_exit || (!m_globalbuffer.full() && s_lightIDs < s_lightIDsMax+1); });
			// double check exit bool
			if (m_exit)
				return false;
			//printf("PRODUCER:%d -- ID:%d -- carCount:%d \n",getID(), m_mostRecentData->ID, m_mostRecentData->dataPair.second);
			m_globalbuffer.add(std::move(m_mostRecentData));
			if (m_globalbuffer.size() == 1)
				m_globalEvents.postEvent("NEW_DATASET_CONSUME");
			printf("Produced extra data\n");
		}
		///repeat
	}
}

bool Producer::busy()
{
	return false;
}

bool Producer::reset()
{
	s_lightIDs = 0;
	m_wait.notify_all();
	return false;
}

bool Producer::setThreadAtomicBool(bool exit)
{
	return false;
}

bool Producer::produce()
{
	{
		std::unique_lock<std::mutex> scopedLock(m_waitMutex);
		m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer.full(); });
		auto ran = ((time(0) + s_lightIDs)* Timer::getInstance().getElapsed());
		srand(ran);
		unsigned carCount = rand() % 1000 + 1;
		eTimeHour timeID = m_globalTimer->getHour();
		unsigned lightID = s_lightIDs++;
		eMeasermentSet setTime = m_globalTimer->getProduceSet();
		m_mostRecentData.reset();
		m_mostRecentData = std::make_unique<TrifficLightData>(timeID, setTime, lightID, carCount);

	}
	return true;
}
