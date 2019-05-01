#pragma once
#include <map>
#include <string>
#include <functional>
#include "Fiber.h"
#include <vector>
#include <condition_variable>
#include <mutex>
#include <atomic>


// member functor
using TASK = std::function<void()>;
class Event
{
public:
	Event(std::function<void()> task)
		: m_task(task) {};

	~Event() {};
	/// run the lambda
	void run() {
		m_task();
	}

private:
	TASK m_task;
};


class SimpleEvent
{
public:
	static SimpleEvent&getInstance()
	{
		static SimpleEvent instance;
		return instance;
	}
	/// no copy
	SimpleEvent(SimpleEvent&temp) = delete;
	void operator=(SimpleEvent const&temp) = delete;
	~SimpleEvent() {};

	/// infinate loop for checking events list
	void process();
	/// add event to be called - return ID of event
	// NOTE ID IS USED TO REMOVE THE EVENT. KEEP THIS!! 
	unsigned registerEvent(const char*, Event);
	/// remove event via ID returned by registerEvent()
	void removeEvent(const char* name, unsigned id);
	/// call event
	bool postEvent(const char*);

	void exit();


private:
	SimpleEvent();

	std::condition_variable							m_wait;				/// wait conditional
	std::mutex										m_waitMutex;		/// wrapper's mutual exclusion
	std::map<const char*, std::map<unsigned,Event>>	m_events;			/// all registered events
	std::vector<const char*>						m_eventsInWait;		/// events to be called
	const char*										m_runEvent;			/// next event to call
	std::atomic<bool>								m_exit;				/// flag exit and clean up
};

