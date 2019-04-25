#define _CRT_SECURE_NO_WARNINGS
#include "Timer.h"
#include <ctime>

Timer::Timer()
{
	m_beginTimerList = std::make_shared<TIME_REGISTER>();
	m_finishTimerList = std::make_shared<TIME_REGISTER>();
	m_finalTimerSheetMs = std::make_shared<TIME_VECTOR>();
	m_displayNameList = std::make_shared<TIME_DISPLAY_VECTOR_PAIR>();

	addStartTime(eTimeLogType::TT_BEGIN, "Total program runtime");
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
	return m_delta;
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
	m_elapsedTime = m_elapsedTime / 1000;

	//set delta
	differenceInTime = now - m_beginTimerList->at(eTimeLogType::TT_DELTA);
	m_delta = std::chrono::duration_cast<std::chrono::milliseconds>(differenceInTime).count();
	m_beginTimerList->at(eTimeLogType::TT_DELTA) = now;

	// realtime VS simulated time
	if(!DEBUG){
		auto end = std::chrono::system_clock::now();
		std::time_t end_time = std::chrono::system_clock::to_time_t(end);
		auto time = std::ctime(&end_time);
		std::cout << time[7] << time[8] << std::endl;
	}
	else
	{
		unsigned min = std::floor(m_elapsedTime) / 60;
		if (min % 5 == 0)
		{
			m_currentHour = eTimeHour(min);
		}
	}

	return true;
}
