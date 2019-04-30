#pragma once
#include "iBoundBufferObjects.h"
#include "DataStructures.h"
#include "TrafficDataBase.h"
#include <vector>

class Timer;
class SimpleEvent;

static unsigned s_lightIDs = 0; // lights created counter

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

	///class specific
	bool produce();


private:
	Timer*								m_globalTimer;		/// global timer
	TrafficDataBaseRaw&					m_globalbuffer;		/// glboal data set buffer
	SimpleEvent&						m_globalEvents;		/// global events
	std::unique_ptr<TrifficLightData>	m_mostRecentData;	/// most recet data to be placed in buffer
	unsigned							m_eventID;			/// objects ID to be used when removaling attached events

};

