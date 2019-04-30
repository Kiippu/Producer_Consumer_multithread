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
	// infinate loop to call and run producer
	while (true)
	{
		{
			// lock this scope
			if (m_globalbuffer.full())
				return false;
			// double check exit bool
			if (m_exit) {
				return false;
			}

			produce();
			if (m_mostRecentData != nullptr) {
				m_globalbuffer.add(std::move(m_mostRecentData));
			}
			m_mostRecentData = nullptr;
		}
		UpdateDelta();												// update elapsed timer
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

bool Producer::produce()
{
	{
		std::string line;
		{
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_globalbuffer.full(); });

			if (s_lightIDs < s_lines.size())			// get next line
				line = s_lines[s_lightIDs++];
		}
		if (line.empty())								// leave it empty buffer
		{
			m_exit = true;
			return false;
		}

		/// data parseing from string
		auto foundText = line.find('#');
		std::string text;
		if (++foundText != std::string::npos)
			while (line[foundText] != '-')				// get ID
			{
				text += line[foundText];
				foundText++;
			}
		unsigned lightID = std::stoi(text);

		text.clear();
		foundText = line.find(':');
		if (++foundText != std::string::npos)
			while (line[foundText] != '[')				// get measurment
			{
				text += line[foundText];
				foundText++;
			}
		eMeasermentSet setTime = eMeasermentSet(std::stoi(text));

		text.clear();
		foundText = line.find('[');
		if (++foundText != std::string::npos)
			while (line[foundText] != ']')				// get Car count
			{
				text += line[foundText];
				foundText++;
			}
		unsigned carCount = std::stoi(text);

		eTimeHour timeID = m_globalTimer->getHour();
		{
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			m_mostRecentData.reset();
			m_mostRecentData = std::make_unique<TrifficLightData>(timeID, setTime, lightID, carCount);	// create ptr to add later
		}
	}
	return true;
}
