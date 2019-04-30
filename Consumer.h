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
	Timer *								m_globalTimer;				/// global timer 
	SimpleEvent&						m_globalEvents;				/// global events
	TrafficDataBaseRaw&					m_globalbuffer_raw;			/// global buffer to consume from
	TrafficDataProcessed&				m_globalbuffer_processed;	/// global buffer to place consumed
	std::unique_ptr<TrifficLightData>	m_mostRecentData;			/// most recent picked data
	unsigned							m_eventID;					/// this objects event ID - used when removing attached events


};

