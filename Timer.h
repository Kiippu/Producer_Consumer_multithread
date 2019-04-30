#pragma once
#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <mutex>

const bool s_DEBUG = false;
const float s_timeMultiplyer = 2.;

/// forward declaration
class SimpleEvent;

/// timer logging types/ID's
enum eTimeLogType {
	TT_BEGIN,
	TT_NOW,
	TT_DELTA,
	TT_CONSUMER_0,
	TT_CONSUMER_1,
	TT_CONSUMER_2,
	TT_CONSUMER_3,
	TT_CONSUMER_4,
	TT_CONSUMER_5,
	TT_CONSUMER_6,
	TT_CONSUMER_7,
	TT_PRODUCER_0,
	TT_PRODUCER_1,
	TT_PRODUCER_2,
	TT_PRODUCER_3,
	TT_PRODUCER_4,
	TT_PRODUCER_5,
	TT_PRODUCER_6,
	TT_PRODUCER_7,
	TT_LIMIT = 1000,
	TT_MAX = UINT32_MAX
};
/// posible times i a day 24hr
enum eTimeHour
{
	TH_00,
	TH_01,
	TH_02,
	TH_03,
	TH_04,
	TH_05,
	TH_06,
	TH_07,
	TH_08,
	TH_09,
	TH_10,
	TH_11,
	TH_12,
	TH_13,
	TH_14,
	TH_15,
	TH_16,
	TH_17,
	TH_18,
	TH_19,
	TH_20,
	TH_21,
	TH_22,
	TH_23,
	TH_24,
	TH_INVALID
};
/// measurement data sets
enum eMeasermentSet {
	MS_05,
	MS_10,
	MS_15,
	MS_20,
	MS_25,
	MS_30,
	MS_35,
	MS_40,
	MS_45,
	MS_50,
	MS_55,
	MS_60,
	MS_LIMIT,
	MS_INVALID = UINT32_MAX
};

/// typedefs for easy type creation
using TIME_REGISTER = std::map <eTimeLogType, std::chrono::time_point<std::chrono::steady_clock>>;
using TIME_VECTOR_PAIR = std::pair<eTimeLogType, long long>;
using TIME_DISPLAY_VECTOR_PAIR = std::map <eTimeLogType, std::string>;
using TIME_VECTOR = std::vector<TIME_VECTOR_PAIR>;

/*
	Class:			Timer
	Description:	Timer to count elapsed time of tested algorithms
*/

class Timer
{
public:
	/// singleton implementation
	static Timer&getInstance()
	{
		static Timer instance;
		return instance;
	}
	/// no copy
	Timer(Timer&temp) = delete;
	void operator=(Timer const&temp) = delete;
	~Timer() {};

	/// register start timer
	void addStartTime(eTimeLogType eDisplayName, std::string displayName);
	/// registe finish of timer
	void addFinishTime(eTimeLogType id);
	/// print all timers that have finished so far
	void printFinalTimeSheet();
	/// get master delta
	int64_t getDelta();
	/// get master elapsed time 
	int64_t getElapsed();
	/// get hour - used in prototype for frealtime data seat generator @update()
	eTimeHour getHour();
	/// get measurment - used in prototype for frealtime data seat generator @update()
	eMeasermentSet getProduceSet();
	/// master update for timers
	bool update();

private:
	Timer();

	SimpleEvent*								m_globalEvents;			/// global events
	eTimeHour									m_currentHour;			/// hour
	eMeasermentSet								m_currentProduceSet;	/// meaurment type
	int64_t										m_elapsedTime;			/// master time elapsed
	int64_t										m_delta;				/// master delta time
	std::mutex									m_waitMutex;			/// wrapper's mutual exclusion

	std::shared_ptr<TIME_REGISTER>				m_beginTimerList;		/// time lists/maps
	std::shared_ptr<TIME_REGISTER>				m_finishTimerList;
	std::shared_ptr<TIME_VECTOR>				m_finalTimerSheetMs;
	std::shared_ptr<TIME_DISPLAY_VECTOR_PAIR>	m_displayNameList;


};

