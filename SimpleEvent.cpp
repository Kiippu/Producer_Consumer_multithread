#include "SimpleEvent.h"



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
	auto it = m_events.find(name);
	if (it != m_events.end()) 
	{
		m_waitMutex.lock();
		it->second.insert(std::make_pair(it->second.size(),event));
		unsigned k = it->second.size()-1;
		m_waitMutex.unlock();
		return k;
	}
	else
	{
		auto p = m_events.find(name);
		m_waitMutex.lock();
		m_events.insert(std::make_pair(name,std::map<unsigned,Event>()));
		m_events.find(name)->second.insert(std::make_pair(m_events.find(name)->second.size(), event));
		unsigned k = m_events.find(name)->second.size() - 1;
		m_waitMutex.unlock();
		return k;
	}
	return false;
}

void SimpleEvent::removeEvent(const char * name, unsigned id)
{
	auto it = m_events.find(name);
	if (it != m_events.end())
	{
		m_waitMutex.lock();
		it->second.erase(id);
		m_waitMutex.unlock();
	}
}

bool SimpleEvent::postEvent(const char * name)
{
	m_waitMutex.lock();
	m_eventsInWait.push_back(name);
	m_waitMutex.unlock();
	m_wait.notify_all();
	return true;
}
