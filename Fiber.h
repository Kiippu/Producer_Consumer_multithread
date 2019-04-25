#pragma once
#include <functional>

/*
	Class:			Fiber
	Description:	wrapper for ThreadScheduler lamda task
*/

// member functor
using TASK = std::function<void()>;
class Fiber
{
public:
	Fiber(std::function<void()> task) 
		: m_task(task){};

	~Fiber() {};
	/// run the lambda
	void run() {
		m_task();
	}

private:
	TASK m_task;
};

