#include "iBoundBufferObjects.h"



iBoundBufferObjects::iBoundBufferObjects()
{
	m_ID = s_ID_iBoundBufferObjects++;
}


iBoundBufferObjects::~iBoundBufferObjects()
{
}

unsigned & iBoundBufferObjects::getID()
{
	return m_ID;
}
