#pragma once
#include <vector>
#include "DataStructures.h"

class TrafficDataBaseRaw
{
public:
	TrafficDataBaseRaw();
	~TrafficDataBaseRaw();

	TrifficLightData get();

	bool add(TrifficLightData & data) { M_buffer.push_back(std::move(data)); };

private:
	std::vector<TrifficLightData> M_buffer;
};

