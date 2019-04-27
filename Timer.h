#pragma once
#include <chrono>
#include <string>
#include <map>
#include <vector>
#include <iostream>

const bool s_DEBUG = false;
const float s_timeMultiplyer = 1.;

class SimpleEvent;

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

	void addStartTime(eTimeLogType eDisplayName, std::string displayName);
	void addFinishTime(eTimeLogType id);
	void printFinalTimeSheet();

	int64_t& getDelta();
	int64_t& getElapsed();
	eTimeHour& getHour();
	eMeasermentSet& getProduceSet();

	bool update();

private:
	Timer();

	SimpleEvent*								m_globalEvents;
	eTimeHour									m_currentHour;
	eMeasermentSet								m_currentProduceSet;
	int64_t										m_elapsedTime;
	int64_t										m_delta;

	std::shared_ptr<TIME_REGISTER>				m_beginTimerList;
	std::shared_ptr<TIME_REGISTER>				m_finishTimerList;
	std::shared_ptr<TIME_VECTOR>				m_finalTimerSheetMs;
	std::shared_ptr<TIME_DISPLAY_VECTOR_PAIR>	m_displayNameList;


};

