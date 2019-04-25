#pragma once
#include <vector>
#include "DataStructures.h"
#include <memory>

class TrafficDataBaseRaw
{
public:
	TrafficDataBaseRaw();
	~TrafficDataBaseRaw();

	std::unique_ptr<TrifficLightData> get();

	bool empty();
	bool max();
	bool add(std::unique_ptr<TrifficLightData> data);

private:
	unsigned m_bufferLimit;
	unsigned m_maxBufferLimit = 1000;
	std::vector<std::unique_ptr<TrifficLightData>> m_buffer;
};

