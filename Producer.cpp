#include "Producer.h"
#include "SimpleEvent.h"
#include "Timer.h"
#include "FileSystemHelper.h"




Producer::Producer(TrafficDataBaseRaw& Globalbuffer, SimpleEvent& GlobalEventSystem)
	: m_globalbuffer(Globalbuffer), m_globalEvents(GlobalEventSystem)
{
	//s_lightIDsMax = Globalbuffer.getLimit();
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	s_activeObjects++;
	m_globalTimer = &Timer::getInstance();
	m_eventID = m_globalEvents.registerEvent("NEW_DATASET",Event([&]() {
		reset();
	}));
	printf("Producer of ID:%d created\n", getID());
}


Producer::~Producer()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	s_activeObjects--;
	m_globalEvents.removeEvent("NEW_DATASET", m_eventID);
	printf("----- Finised production with ID:%d -----\n", getID());
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
			// check  exist and exit flag
			if (m_globalbuffer.full())
				return false;
			// double check exit bool
			if (m_exit) {
				return false;
			}
			//printf("PRODUCER:%d -- ID:%d -- carCount:%d \n",getID(), m_mostRecentData->ID, m_mostRecentData->dataPair.second);
			produce();
			if (m_mostRecentData != nullptr) {
				m_globalbuffer.add(std::move(m_mostRecentData));
				//m_globalEvents.postEvent("NEW_DATASET_CONSUME");
			}
			m_mostRecentData = nullptr;
			/*if (m_globalbuffer.size() == 1)
				m_globalEvents.postEvent("NEW_DATASET_CONSUME");*/
		}
		UpdateDelta();
		///repeat
	}
}

bool Producer::busy()
{
	return false;
}

bool Producer::reset()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
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
		std::string line;
		{
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer.full(); });

			if (s_lightIDs < s_lines.size())
				line = s_lines[s_lightIDs++];

			if (s_lightIDs == s_lines.size())
				int k = 0;

		}
		if (line.empty()) 
		{
			m_exit = true;
			return false;
		}

		auto foundText = line.find('#');
		std::string text;
		if (++foundText != std::string::npos)
			while (line[foundText] != '-')
			{
				text += line[foundText];
				foundText++;
			}
		unsigned lightID = std::stoi(text);

		text.clear();
		foundText = line.find(':');
		if (++foundText != std::string::npos)
			while (line[foundText] != '[')
			{
				text += line[foundText];
				foundText++;
			}
		eMeasermentSet setTime = eMeasermentSet(std::stoi(text));

		text.clear();
		foundText = line.find('[');
		if (++foundText != std::string::npos)
			while (line[foundText] != ']')
			{
				text += line[foundText];
				foundText++;
			}
		unsigned carCount = std::stoi(text);


		//auto ran = ((time(0) + s_lightIDs) * Timer::getInstance().getElapsed());
		//srand(ran);
		//unsigned carCount = rand() % 1000 + 1;
		eTimeHour timeID = m_globalTimer->getHour();
		//m_waitMutex.lock();
		//unsigned lightID = s_lightIDs++;
		//m_waitMutex.unlock();
		//eMeasermentSet setTime = m_globalTimer->getProduceSet();
		{
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_mostRecentData.reset();
			m_mostRecentData = std::make_unique<TrifficLightData>(timeID, setTime, lightID, carCount);
		}

	}
	return true;
}
