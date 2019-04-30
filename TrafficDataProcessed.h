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
	/// add data to final data set
	bool add(std::unique_ptr<TrifficLightData> data);
	/// get the data set
	std::map<eTimeHour, std::unique_ptr<TrafficLightResults>>& get();
	/// get a data set
	TrafficLightResults* getResults(eTimeHour);
	/// print current results in buffer
	bool printResults(uint64_t);
	/// current processed data sets count
	unsigned & count() { return m_inventoryCount; };

	
private:
	std::mutex													m_waitMutex;			/// wrapper's mutual exclusion
	std::map<eTimeHour,std::unique_ptr<TrafficLightResults>>	m_buffer;				/// master buffer
	std::atomic<bool>											m_print;				/// force print
	bool														m_full;					/// max data taken
	unsigned													m_inventoryCount = 1;	/// current setcounter
};

