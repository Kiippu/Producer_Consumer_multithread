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

	/// get a data set
	std::unique_ptr<TrifficLightData> get();
	/// is empty?
	bool empty();
	/// is full?
	bool full();
	/// add data to buffer
	bool add(std::unique_ptr<TrifficLightData> data);
	/// size of buffer
	unsigned size();
	/// get maxLimit of buffer
	unsigned& getLimit();
	/// get current added count in total
	unsigned& getAddedCount();
	/// get full buffer 
	std::vector<std::unique_ptr<TrifficLightData>>& getList() { return m_buffer; };

private:
	std::mutex										m_waitMutex;				/// wrapper's mutual exclusion
	unsigned										m_bufferLimit;				/// buffer limit counter
	unsigned										m_maxBufferLimit = 5000;	/// limit max
	std::vector<std::unique_ptr<TrifficLightData>>	m_buffer;					/// buffer of data sets
};

