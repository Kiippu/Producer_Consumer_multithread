#pragma once
#include <chrono>
#include <map>
#include "Timer.h"
#include <algorithm>


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
public:
	TrafficLightResults() {};
	~TrafficLightResults() {};

	std::map<unsigned, std::vector<std::pair<eMeasermentSet, unsigned>>> m_dataSet;
	bool m_isDirty;

	bool addData(unsigned id, std::pair<eMeasermentSet, unsigned> data)
	{
		auto pairType = data.first;
		auto pairCount = data.second;
		auto it = m_dataSet.find(id);
		if (it != m_dataSet.end())
		{
			it->second.push_back(data);
			std::sort(it->second.begin(), it->second.end(), [](auto &left, auto &right) {
				return left.second < right.second;
			});
		}
		else {
			m_dataSet.insert(std::make_pair(id, std::vector<std::pair<eMeasermentSet,unsigned>>()));
			m_dataSet[id].push_back(data);
		}
		//std::map<eMeasermentSet, unsigned> idSet = m_dataSet[id];
		//idSet[data.first] = data.second;
		//m_dataSet[id].insert(data);
		return true;
	}
};