#include "TrafficDataBase.h"
#include "SimpleEvent.h"

TrafficDataBaseRaw::TrafficDataBaseRaw()
{
	m_bufferLimit = 0;
}

std::unique_ptr<TrifficLightData> TrafficDataBaseRaw::get()
{
	if (m_buffer.empty())
		return std::make_unique<TrifficLightData>(eTimeHour::TH_INVALID,eMeasermentSet::MS_INVALID, UINT32_MAX, UINT32_MAX);
	else {
		m_bufferLimit--;
		auto result = std::move(m_buffer.at(0));
		m_waitMutex.lock();
		if(!m_buffer.empty())
			m_buffer.erase(m_buffer.begin());
		m_waitMutex.unlock();
		if (empty())
			SimpleEvent::getInstance().postEvent("PRINT_DATA");
		return std::move(result);
	}
}

bool TrafficDataBaseRaw::empty()
{
	return m_buffer.empty();
}

bool TrafficDataBaseRaw::full()
{
	m_bufferLimit;
	if (m_bufferLimit == m_maxBufferLimit)
		printf("Buffer Full - Waiting on Consumers\n");
	return (m_bufferLimit == m_maxBufferLimit);
}

bool TrafficDataBaseRaw::add(std::unique_ptr<TrifficLightData> data)
{
	unsigned count = m_buffer.size();
	//printf("PRODUCE: data{ id:%d, count:%d}", data->ID, data->dataPair.second);
	m_waitMutex.lock();
	m_buffer.push_back(std::move(data));
	m_waitMutex.unlock();
	if (count < m_buffer.size()) {
		m_bufferLimit++;
		return true;
	}
	return false;
}

unsigned TrafficDataBaseRaw::size()
{
	return m_buffer.size();
}

unsigned& TrafficDataBaseRaw::getLimit()
{
	return m_bufferLimit;
}
