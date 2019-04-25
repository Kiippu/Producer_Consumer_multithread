#include "Producer.h"



Producer::Producer(TrafficDataBaseRaw& Globalbuffer)
	: m_globalbuffer(Globalbuffer)
{
	m_globalTimer = &Timer::getInstance();
	m_waitMutex.try_lock();
	m_localMaxLightID = s_MAX_LIGHT_COUNT;
	m_nextLightID = s_lightCount;
	s_lightCount += s_MAX_LIGHT_COUNT;
	m_waitMutex.unlock();

	for (size_t i = 0; i < m_localMaxLightID; i++)
	{
		m_lightID.push_back(m_nextLightID + i);
	}
}


Producer::~Producer()
{
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
			produce();
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			// check  exist and exit flag
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer.empty(); });
			// double check exit bool
			if (m_exit)
				return false;
			m_globalbuffer.add(std::move(m_mostRecentData));
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
	m_nextLightID = 0;
	return false;
}

bool Producer::produce()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	m_wait.wait(scopedLock, [&]() { return m_exit || m_nextLightID != m_localMaxLightID; });

	srand(time(0));
	unsigned carCount = rand() % 1000 + 1;
	eTimeHour timeID = m_globalTimer->getHour();
	unsigned lightID = m_nextLightID;
	eMeasermentSet setTime = m_globalTimer->getProduceSet();
	m_mostRecentData = std::make_unique<TrifficLightData>(timeID, setTime, lightID, carCount);
	
	m_nextLightID++;
	return true;
}
