#pragma once
#include "iBoundBufferObjects.h"
#include "DataStructures.h"
#include "TrafficDataBase.h"
#include <vector>

class Timer;

static unsigned s_lightCount = 0;
static const unsigned s_MAX_LIGHT_COUNT = 50;

class Producer :
	public iBoundBufferObjects
{
public:
	Producer(TrafficDataBaseRaw& Globalbuffer);
	~Producer();

	/// Pure Virtual methods
	bool get();
	bool process();
	bool busy();
	bool reset();

	///class specific
	bool produce();

private:
	Timer * m_globalTimer;
	TrafficDataBaseRaw& m_globalbuffer;
	std::unique_ptr<TrifficLightData> m_mostRecentData;
	std::vector<unsigned> m_lightID;
	unsigned m_nextLightID;
	unsigned m_localMaxLightID;


};

