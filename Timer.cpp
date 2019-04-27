#define _CRT_SECURE_NO_WARNINGS
#include "Timer.h"
#include "SimpleEvent.h"
#include <ctime>

Timer::Timer()
{
	m_globalEvents = &SimpleEvent::getInstance();
	m_beginTimerList = std::make_shared<TIME_REGISTER>();
	m_finishTimerList = std::make_shared<TIME_REGISTER>();
	m_finalTimerSheetMs = std::make_shared<TIME_VECTOR>();
	m_displayNameList = std::make_shared<TIME_DISPLAY_VECTOR_PAIR>();
	m_elapsedTime = 0;

	addStartTime(eTimeLogType::TT_BEGIN, "Total program runtime");
	addStartTime(eTimeLogType::TT_DELTA, "Time since last update was competed");
}

// log start timer
void Timer::addStartTime(eTimeLogType eDisplayName, std::string displayName)
{
	m_displayNameList->insert(std::make_pair(eDisplayName, displayName));
	m_beginTimerList->insert(std::make_pair(eDisplayName, std::chrono::high_resolution_clock::now()));
	if(eDisplayName > eTimeLogType::TT_LIMIT)
		m_finalTimerSheetMs->push_back(TIME_VECTOR_PAIR(eDisplayName, 0));
};
// lof finish timer
void Timer::addFinishTime(eTimeLogType id)
{
	//log finish timer
	auto finishTimer = std::chrono::high_resolution_clock::now();
	m_finishTimerList->insert(std::make_pair(id, finishTimer));
};
// print time sheet of finihsed timers
void Timer::printFinalTimeSheet()
{
	// iterate through all values in timesheet and print them.
	for (auto & obj : *m_finalTimerSheetMs)
	{
		auto differenceInTime = m_finishTimerList->at(obj.first) - m_beginTimerList->at(obj.first);
		obj.second = std::chrono::duration_cast<std::chrono::milliseconds>(differenceInTime).count();
		std::cout << m_displayNameList->at(obj.first) << " : " << std::to_string(obj.second) << "ms / " << std::to_string(float(float(obj.second) / 1000.f)) << "sec to execute\n" << std::endl;
	}
}

int64_t & Timer::getDelta()
{
	return m_delta;
}

int64_t & Timer::getElapsed()
{
	return m_elapsedTime;
}

eTimeHour & Timer::getHour()
{
	return m_currentHour;
}

eMeasermentSet & Timer::getProduceSet()
{
	return m_currentProduceSet;
}

bool Timer::update()
{
	auto now = std::chrono::high_resolution_clock::now();
	// set elapsed time
	auto differenceInTime = now - m_beginTimerList->at(eTimeLogType::TT_BEGIN);
	m_elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(differenceInTime).count();
	//m_elapsedTime = m_elapsedTime + ;

	//set delta
	differenceInTime = now - m_beginTimerList->at(eTimeLogType::TT_DELTA);
	m_delta = std::chrono::duration_cast<std::chrono::milliseconds>(differenceInTime).count();
	m_beginTimerList->at(eTimeLogType::TT_DELTA) = now;

	// set new time enum types every 5 and 60 mins
	unsigned min = std::floor((m_elapsedTime / 1000) / 60) * s_timeMultiplyer; 
	if (min % 2 == 0) { // normally == 5
		if (m_currentProduceSet != eMeasermentSet(unsigned(std::floor(min / 2))))
		{
			m_currentProduceSet = eMeasermentSet(unsigned(std::floor(min / 2)));
			if (min % 60 == 0) // normally == 60
				m_currentHour = eTimeHour(unsigned(std::floor(min / 60)));
			m_globalEvents->postEvent("NEW_DATASET");
		}
	}

	return true;
}
