#pragma once
#include <vector>
#include "DataStructures.h"
#include <memory>
#include <mutex>

class TrafficDataBaseRaw
{
public:
	TrafficDataBaseRaw();
	~TrafficDataBaseRaw() {};

	std::unique_ptr<TrifficLightData> get();

	bool empty();
	bool full();
	bool add(std::unique_ptr<TrifficLightData> data);
	unsigned size();
	unsigned& getLimit();
	unsigned& getAddedCount();
	std::vector<std::unique_ptr<TrifficLightData>>& getList() { return m_buffer; };

private:
	std::mutex										m_waitMutex;		/// wrapper's mutual exclusion
	unsigned										m_bufferLimit;
	unsigned										m_maxBufferLimit = 5000;
	std::vector<std::unique_ptr<TrifficLightData>>	m_buffer;
};

