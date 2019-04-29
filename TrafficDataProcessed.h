#pragma once
#include <vector>
#include "DataStructures.h"
#include <memory>
#include <atomic>
#include <mutex>


class TrafficDataProcessed
{
public:
	TrafficDataProcessed();
	~TrafficDataProcessed();

	bool add(std::unique_ptr<TrifficLightData> data);
	std::map<eTimeHour, std::unique_ptr<TrafficLightResults>>& get();
	TrafficLightResults* getResults(eTimeHour);
	bool printResults();
	void setFull(bool c) { m_full = c; };
	bool& full() { return m_full; };
	unsigned & count() { return m_inventoryCount; };

	
private:
	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion
	std::map<eTimeHour,std::unique_ptr<TrafficLightResults>> m_buffer;
	std::atomic<bool> m_print;
	bool m_full;
	unsigned m_inventoryCount = 1;
};

