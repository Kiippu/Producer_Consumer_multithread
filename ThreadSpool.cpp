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

ThreadSpool::ThreadSpool()
{
	// get core count and create threadScheduler in placing them in member dictionary
	m_maxThreadsSupported = std::thread::hardware_concurrency();
	for (unsigned i = 0; i < m_maxThreadsSupported; i++)
	{
		if (i == THREAD_ID::ID_MAX)// exit if max is reached by through error
			break;
		m_threadPool.insert(std::make_pair(THREAD_ID(i),std::unique_ptr<ThreadScheduler>(new ThreadScheduler)));
	}
}

