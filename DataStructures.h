#pragma once
#include <chrono>
#include <map>

enum class eMeasermentSet {
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
	MS_INVALID = UINT32_MAX
};

struct TrifficLightData
{
	TrifficLightData(eMeasermentSet stamp, unsigned id, unsigned carCount)
	{
		ID = id;
		dataPair = std::make_pair(stamp, carCount);
	}
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