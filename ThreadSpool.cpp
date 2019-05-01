#include "ThreadSpool.h"

unsigned ThreadSpool::isAvaliable()
{
	// count id threads are not busy
	unsigned count = 0;
	for (auto & thread : m_threadPool)
	{
		if (!thread.second->busy())
			count++;
	}
	return count;
}

ThreadScheduler * ThreadSpool::getAvaliable()
{
	// return an empty thread
	for (auto & thread : m_threadPool)
	{
		if (!thread.second->busy())
			return thread.second.get();
	}
	return nullptr;
}

ThreadScheduler * ThreadSpool::get(THREAD_ID id)
{
	//get thread with ID or return null
	auto iter = m_threadPool.end();
	iter = m_threadPool.find(id);
	if (iter == m_threadPool.end())
	{
		return iter->second.get();
	}
	return nullptr;
}

unsigned ThreadSpool::getTypeCount(THREAD_TYPE type)
{
	unsigned count = 0;
	for (auto& obj : m_threadPool)
	{
		if (obj.second->getOcupation() == type)
			count++;
	}
	return count;
}

unsigned ThreadSpool::getMaxCount()
{
	return m_maxThreadsSupported;
}

void ThreadSpool::cleanUpThreads()
{
	//ThreadSpool::getInstance().get(THREAD_ID::ID0)->exit();
	for (size_t i = 0; i < m_threadPool.size(); i++)
	{
		auto thread = std::move(m_threadPool[THREAD_ID(i)]);
		//m_threadPool.erase(THREAD_ID(i));
		//thread->exit();
	}
}

ThreadSpool::ThreadSpool()
{
	// get core count and create threadScheduler in placing them in member dictionary
	m_maxThreadsSupported = std::thread::hardware_concurrency();
	for (unsigned i = 0; i < m_maxThreadsSupported; i++)
	{
		if (i == THREAD_ID::ID_MAX)// exit if max is reached by through error
			break;
		m_threadPool.insert(std::make_pair(THREAD_ID(i),std::make_unique<ThreadScheduler>()));
	}
}

