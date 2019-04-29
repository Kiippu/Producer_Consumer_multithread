#include "TrafficDataBase.h"
#include "SimpleEvent.h"

TrafficDataBaseRaw::TrafficDataBaseRaw()
{
	m_bufferLimit = 0;
}

std::unique_ptr<TrifficLightData> TrafficDataBaseRaw::get()
{
	{
		std::unique_lock<std::mutex> scopedLock(m_waitMutex);
		if (m_buffer.empty()) {
			return nullptr;
		}
		else {
			m_bufferLimit--;
			auto result = std::move(m_buffer[0]);
			if (!m_buffer.empty())
				m_buffer.erase(m_buffer.begin());
			return std::move(result);
		}
	}
}

bool TrafficDataBaseRaw::empty()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	return (m_buffer.size() == 0);
}

bool TrafficDataBaseRaw::full()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	m_bufferLimit;
	if (m_bufferLimit == m_maxBufferLimit) {
		printf("Buffer Full - Waiting on Consumers\n");
	}
	return (m_bufferLimit == m_maxBufferLimit);
}

bool TrafficDataBaseRaw::add(std::unique_ptr<TrifficLightData> data)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	unsigned count = m_buffer.size();
	m_buffer.push_back(std::move(data));
	if (count < m_buffer.size()) {
		m_bufferLimit++;
		return true;
	}
	return false;
}

unsigned TrafficDataBaseRaw::size()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	return m_buffer.size();
}

unsigned& TrafficDataBaseRaw::getLimit()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	return m_maxBufferLimit;
}


unsigned& TrafficDataBaseRaw::getAddedCount()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	return m_bufferLimit;
}