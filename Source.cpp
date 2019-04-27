#include "Producer.h"
#include "Consumer.h"
#include "ThreadSpool.h"
#include "SimpleEvent.h"
#include "FileSystemHelper.h"

#include <Windows.h>


int main()
{
	TrafficDataBaseRaw * buffer_raw = new TrafficDataBaseRaw;
	TrafficDataProcessed* buffer_processed = new TrafficDataProcessed;
	Timer* globalTimer = &Timer::getInstance();
	SimpleEvent * globalEvents = &SimpleEvent::getInstance();
	FileSystemHelper * globalFileHelper = &FileSystemHelper::getInstance();


	//std::shared_ptr<Producer> producerTest = std::make_shared<Producer>(*buffer_raw, *globalEvents);
	//std::shared_ptr<Consumer>  consumerTest = std::make_shared<Consumer>(*buffer_raw.get(), *buffer_processed.get());

	auto thread0 = ThreadSpool::getInstance().getAvaliable();
	thread0->addFiber(Fiber([&buffer_raw,&globalEvents]() {Producer p(*buffer_raw, *globalEvents); p.process(); }));
	//Sleep(1000);
	auto thread1 = ThreadSpool::getInstance().getAvaliable();
	thread1->addFiber(Fiber([&buffer_raw,&buffer_processed,&globalEvents]() {Consumer c(*buffer_raw, *buffer_processed); c.process(); }));



	auto thread_events = ThreadSpool::getInstance().getAvaliable();
	thread_events->setOcupation(THREAD_TYPE::EVENTS);
	thread_events->addFiber(Fiber([&globalEvents]() {globalEvents->process(); }));
	globalEvents->registerEvent("BEGIN_PRODUCE", Event([&]() {
		printf("EVENT: Begin Production!");
		if (auto thread0 = ThreadSpool::getInstance().getAvaliable())
			thread0->addFiber(Fiber([&buffer_raw, &globalEvents,&thread0]() {Producer p(*buffer_raw, *globalEvents); p.process(); }));
	}));
	globalEvents->postEvent("BEGIN_PRODUCE");



	while (globalTimer->update())
	{
		if(ThreadSpool::getInstance().count() > 3)
		{
			/*if (buffer_raw->full())
			{
				if (auto thread0 = ThreadSpool::getInstance().getAvaliable())
					thread0->addFiber(Fiber([&buffer_raw, &globalEvents,&thread0]() {Producer p(*buffer_raw, *globalEvents); p.process(); }));
			}*/
			if (!buffer_raw->empty())
			{
				if (auto thread1 = ThreadSpool::getInstance().getAvaliable())
					thread1->addFiber(Fiber([&buffer_raw, &buffer_processed, &thread1]() {Consumer c(*buffer_raw, *buffer_processed); c.process(); }));
				
			}
		}
		


		//if (!buffer_raw->empty())
		//{
		//	if(auto thread0 = ThreadSpool::getInstance().getAvaliable())
		//		thread0->addFiber(Fiber([&buffer_raw]() {Producer p(*buffer_raw); p.process(); }));
		//}
		//Sleep(3000);

		//globalEvents->postEvent("NEW_DATASET");
		//globalEvents->postEvent("NEW_DATASET");
		//printf("TIMER::[ delta: %d, elapsed: %d, hour: %d, set: %d]\n", globalTimer->getDelta(), globalTimer->getElapsed());
	}


	//producerTest.process();
	//consumerTest.process();

	delete buffer_raw;
	delete buffer_processed;
	delete globalEvents;

	return 0;
}