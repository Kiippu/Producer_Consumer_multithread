#include "SimpleEvent.h"

/// similar structure to my threadScheduler.h

SimpleEvent::SimpleEvent()
{
	m_runEvent = nullptr;
}

void SimpleEvent::process()
{
	while (true)
	{
		{
			// lock this scope
			std::unique_lock<std::mutex> scopedLock(m_waitMutex);
			// check if tasks exist of exist
			m_wait.wait(scopedLock, [&]() { return m_exit || !m_eventsInWait.empty(); });
			// double check exit bool
			if (m_exit)
				return;
			// get event to run
			m_runEvent = m_eventsInWait.front();
			m_eventsInWait.erase(m_eventsInWait.begin());
			// run all events registered with that ID.
			auto it = m_events.find(m_runEvent);
			if (it != m_events.end()) {
				for (auto eventObj : it->second)
				{
					//m_waitMutex.lock();
					eventObj.second.run();
					//m_waitMutex.unlock();
				}
			}
			// remove event from queue and reset event to run
			m_runEvent = nullptr;
		}
		///repeat
	}
}

unsigned SimpleEvent::registerEvent(const char * name, Event event)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	auto it = m_events.find(name);
	if (it != m_events.end()) 
	{
		it->second.insert(std::make_pair(it->second.size(),event));
		unsigned k = it->second.size()-1;
		return k;
	}
	else
	{
		auto p = m_events.find(name);
		m_events.insert(std::make_pair(name,std::map<unsigned,Event>()));
		m_events.find(name)->second.insert(std::make_pair(m_events.find(name)->second.size(), event));
		unsigned k = m_events.find(name)->second.size() - 1;
		return k;
	}
	return false;
}

void SimpleEvent::removeEvent(const char * name, unsigned id)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	auto it = m_events.find(name);
	if (it != m_events.end())
	{
		it->second.erase(id);
	}
}

bool SimpleEvent::postEvent(const char * name)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	m_eventsInWait.push_back(name);
	m_wait.notify_one();
	return true;
}

void SimpleEvent::exit()
{
	{
	//std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	m_exit = true;
	m_wait.notify_one();
	}
}
