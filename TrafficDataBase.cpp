#include "TrafficDataBase.h"

TrifficLightData TrafficDataBaseRaw::get()
{
	if (M_buffer.empty())
		return TrifficLightData(eMeasermentSet::MS_INVALID, UINT32_MAX, UINT32_MAX);
	else
	{
		TrifficLightData temp = M_buffer[0];
		M_buffer.erase(M_buffer.begin());
		return temp;
	}
}
