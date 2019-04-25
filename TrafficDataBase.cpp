#include "TrafficDataBase.h"

TrafficDataBaseRaw::TrafficDataBaseRaw()
{
	m_bufferLimit = m_maxBufferLimit;
}

std::unique_ptr<TrifficLightData> TrafficDataBaseRaw::get()
{
	if (m_buffer.empty())
		return std::make_unique<TrifficLightData>(eTimeHour::TH_INVALID,eMeasermentSet::MS_INVALID, UINT32_MAX, UINT32_MAX);
	else {
		m_bufferLimit--;
		return std::move(m_buffer.at(0));
	}
}

bool TrafficDataBaseRaw::empty()
{
	return m_buffer.empty();
}

bool TrafficDataBaseRaw::max()
{
	m_bufferLimit;
	return (m_bufferLimit < 1);
}

bool TrafficDataBaseRaw::add(std::unique_ptr<TrifficLightData> data)
{
	unsigned count = m_buffer.size();
	m_buffer.push_back(std::move(data));
	if (count < m_buffer.size()) {
		m_bufferLimit++;
		return true;
	}
	return false;
}
