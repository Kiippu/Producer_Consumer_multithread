#pragma once
#include "iBoundBufferObjects.h"
#include "DataStructures.h"
#include "TrafficDataProcessed.h"
#include "TrafficDataBase.h"
#include <vector>
static unsigned consumCount = 0;
class SimpleEvent;

class Consumer :
	public iBoundBufferObjects
{
public:
	Consumer(TrafficDataBaseRaw& globalbuffer_raw, TrafficDataProcessed& globalBuffe_processsed);
	~Consumer();

	/// Pure Virtual methods
	bool get();
	bool process();
	bool busy();

	///class specific
	bool consume();

private:
	Timer * m_globalTimer;
	SimpleEvent&						m_globalEvents;
	TrafficDataBaseRaw&					m_globalbuffer_raw;
	TrafficDataProcessed&				m_globalbuffer_processed;
	std::unique_ptr<TrifficLightData>	m_mostRecentData;
	unsigned							m_eventID;


};

