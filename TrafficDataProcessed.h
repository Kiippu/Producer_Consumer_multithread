#pragma once
#include <vector>
#include "DataStructures.h"
#include <memory>
#include <atomic>

class TrafficDataProcessed
{
public:
	TrafficDataProcessed();
	~TrafficDataProcessed();

	bool add(std::unique_ptr<TrifficLightData> data);
	std::map<eTimeHour, std::unique_ptr<TrafficLightResults>>& get();
	TrafficLightResults* getResults(eTimeHour);

	bool printResults();

private:
	std::map<eTimeHour,std::unique_ptr<TrafficLightResults>> m_buffer;
	std::atomic<bool> m_print;
};

