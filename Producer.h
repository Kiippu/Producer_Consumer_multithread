#pragma once
#include "iBoundBufferObjects.h"
#include "DataStructures.h"
#include "TrafficDataBase.h"
#include <vector>

class Timer;
class SimpleEvent;

static unsigned s_lightIDs = 0;
static unsigned s_lightIDsMax = 20;

class Producer :
	public iBoundBufferObjects
{
public:
	Producer(TrafficDataBaseRaw& Globalbuffer, SimpleEvent& GlobalEventSystem);
	~Producer();

	/// Pure Virtual methods
	bool get();
	bool process();
	bool busy();
	bool reset();

	bool setThreadAtomicBool(bool exit);

	///class specific
	bool produce();


private:
	Timer*								m_globalTimer;
	TrafficDataBaseRaw&					m_globalbuffer;
	SimpleEvent&						m_globalEvents;
	std::unique_ptr<TrifficLightData>	m_mostRecentData;
	std::vector<unsigned>				m_lightID;
	unsigned							m_nextLightID;
	unsigned							m_localMaxLightID;
	unsigned							m_eventID;

};

