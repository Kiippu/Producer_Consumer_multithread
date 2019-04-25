#pragma once
#include <chrono>
#include <map>
#include "Timer.h"


struct TrifficLightData
{
	TrifficLightData(eTimeHour timeID,eMeasermentSet stamp, unsigned id, unsigned carCount)
	{
		ID = id;
		TimeID = timeID;
		dataPair = std::make_pair(stamp, carCount);
	}
	eTimeHour TimeID;
	unsigned ID;
	std::pair<eMeasermentSet,unsigned> dataPair;
};

class TrafficLightResults
{
	TrafficLightResults() {};
	~TrafficLightResults() {};
	bool addData(unsigned hour, eMeasermentSet timeStamp, unsigned carCount)
	{
		auto set = m_dataSet[hour];
		auto iter = set.find(timeStamp);
		if (iter == set.end()) {
			set.insert(std::make_pair(timeStamp, carCount));
			return true;
		}
		return false;
	}
	std::map<unsigned, std::map<eMeasermentSet, unsigned>> m_dataSet;
	bool isDirty;
};