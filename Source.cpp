#include "Producer.h"
#include "Consumer.h"
#include "ThreadSpool.h"
#include "SimpleEvent.h"
#include "FileSystemHelper.h"

#include <Windows.h>
#include <map>

const unsigned lights = 1000;
std::mutex    waitMutex;

int main()
{
	std::shared_ptr<TrafficDataBaseRaw> buffer_raw = std::make_shared<TrafficDataBaseRaw>();
	std::shared_ptr <TrafficDataProcessed> buffer_processed = std::make_shared < TrafficDataProcessed>();
	Timer* globalTimer = &Timer::getInstance();
	SimpleEvent * globalEvents = &SimpleEvent::getInstance();
	FileSystemHelper * globalFileHelper = &FileSystemHelper::getInstance();


	//std::shared_ptr<Producer> producerTest = std::make_shared<Producer>(*buffer_raw, *globalEvents);
	//std::shared_ptr<Consumer>  consumerTest = std::make_shared<Consumer>(*buffer_raw.get(), *buffer_processed.get());

	unsigned dataCount = lights * 12;
	std::string dataset;
	for (size_t i = 0; i < lights; i++)
	{
		unsigned timestamp = 0;
		for (size_t j = 0; j < 12; j++)
		{
			timestamp = j;
			auto ran = (time(0) *((i+2)*j+2));
			srand(ran);
			unsigned carCount = rand() % 1000 + 1;
			dataset.append("ID#" + std::to_string(i) + "-TimeStamp:" + std::to_string(timestamp) + "[" + std::to_string(carCount) + "]\n");
			/*if (i == (lights - 1) && j == 11) {
				dataset.pop_back();
			}*/
		}
	}

	globalFileHelper->createFolder("./LightData");
	globalFileHelper->createFile("./LightData", "originalDataSet", "txt");
	globalFileHelper->writeToFileAppend("./LightData", "originalDataSet", "txt", dataset);


	bool run = true;
	bool full = true;

	auto thread_events = ThreadSpool::getInstance().getAvaliable();
	thread_events->setOcupation(THREAD_TYPE::EVENTS);
	thread_events->addFiber(Fiber([&globalEvents]() {globalEvents->process(); }));
	//globalEvents->registerEvent("BEGIN_PRODUCE", Event([&]() {
	//	//if (auto thread0 = ThreadSpool::getInstance().getAvaliable())
	//	//	thread0->addFiber(Fiber([&buffer_raw, &globalEvents,&thread0]() {Producer p(*buffer_raw, *globalEvents); p.process(); }));
	//	buffer_processed->setFull(false);
	//	run = true;
	//}));
	//globalEvents->postEvent("BEGIN_PRODUCE");

	//auto thread0 = ThreadSpool::getInstance().getAvaliable();
	//thread0->addFiber(Fiber([&buffer_raw, &globalEvents]() {Producer p(*buffer_raw, *globalEvents); p.process(); }));
	//thread0->setOcupation(THREAD_TYPE::PRODUCER);
	//Sleep(1000);
	//auto thread1 = ThreadSpool::getInstance().getAvaliable();
	//thread1->addFiber(Fiber([&buffer_raw, &buffer_processed, &globalEvents]() {Consumer c(*buffer_raw, *buffer_processed); c.process(); }));
	ThreadSpool * globalThreadPool = &ThreadSpool::getInstance();
	std::map< unsigned,std::shared_ptr<Consumer>> consumerList;
	std::map< unsigned, std::shared_ptr<Producer>> producerList;

	double averageProducerTime = 1;
	double averageConsumerTime = 1;

	unsigned startingThreadCount = globalThreadPool->isAvaliable();

	if (auto threadProducer = ThreadSpool::getInstance().getAvaliable()) {
		std::shared_ptr<Producer> pro = std::make_shared<Producer>(*buffer_raw, *globalEvents);
		unsigned id = pro->getID();
		producerList.insert(std::make_pair(id, pro));
		threadProducer->addFiber(Fiber([&producerList, id]() { while (producerList[id]->process()) {}; printf("thread ended"); producerList.erase(id); }));
		threadProducer->setOcupation(THREAD_TYPE::PRODUCER);
	}

	unsigned size;
	auto limit = buffer_raw->getLimit();
	auto currentState = 0;
	unsigned count = 0;
	while (globalTimer->update() && (count < dataCount - 1))
	{
		size  = buffer_raw->size();
		count = buffer_processed->count();
		currentState = buffer_raw->getAddedCount();
		//printf("size: %d , light: %d\n", count, dataCount);

		//printf("count: %d", count);
/*
		if (count == (dataCount - 1))
			break;*/

		if (count < dataCount-1)
		{
			unsigned consumerCount = globalThreadPool->getTypeCount(THREAD_TYPE::CONSUMER);
			unsigned producerCount = globalThreadPool->getTypeCount(THREAD_TYPE::PRODUCER);
			unsigned threadMax = globalThreadPool->getMaxCount();

			for (auto& prod : producerList)
			{
				//long long elapsed = prod.second->getDelta();
				if (prod.second->getDelta() > 0)
					averageProducerTime = ((averageProducerTime + prod.second->getDelta()) / 2);
			}

			for (auto& cons : consumerList )
			{
				//long long elapsed = cons.second->getDelta();
				if (cons.second->getDelta() > 0)
					averageConsumerTime = ((averageConsumerTime + cons.second->getDelta()) / 2);
				//printf("elapsed: %d\n", elapsed);
			}

			if (currentState == limit)
				int k = 0;

			if (float(averageConsumerTime / averageProducerTime) <= 1. && currentState != limit)
			{
				if (size < limit)
				{
					if (producerCount < (startingThreadCount - 1))
						if (auto threadProducer = ThreadSpool::getInstance().getAvaliable()) {
							std::shared_ptr<Producer> pro = std::make_shared<Producer>(*buffer_raw, *globalEvents);
							unsigned id = pro->getID();
							producerList.insert(std::make_pair(id, pro));
							threadProducer->addFiber(Fiber([&producerList, id]() { while (producerList[id]->process()) {}; producerList[id]->exitedThread(); }));
							threadProducer->setOcupation(THREAD_TYPE::PRODUCER);
						}
				}
				else
					averageProducerTime = 0.0001;
			}
			else if (float(averageConsumerTime / averageProducerTime  > 1.2 || (currentState == dataCount - 1)) || consumerList.size() == 0)
			{
				std::unique_lock<std::mutex> scopedLock(waitMutex);
				bool removal = false;
				for (auto &obj : producerList)
				{
					if (obj.second->hasExited()) {
						consumerList.erase(obj.first);
						removal = true;
						break;
					}
				}
				if (!removal) {
					auto it = producerList.begin();
					if (it != producerList.end())
						if (it->second != nullptr)
							it->second->exit();
				}
			}

			if (float(averageProducerTime / averageConsumerTime) < 1. || size > dataCount/3 || size == dataCount || consumerList.size() == 0)
			{
				if (size > 0)
				{
					if (consumerCount < producerCount)
						if (auto threadConsumer = ThreadSpool::getInstance().getAvaliable()) {
							std::shared_ptr<Consumer> con = std::make_shared<Consumer>(*buffer_raw, *buffer_processed);
							unsigned id = con->getID();
							consumerList.insert(std::make_pair(id, con));

							threadConsumer->addFiber(Fiber([&consumerList, id]() { while (consumerList[id]->process()) {}; }));
							threadConsumer->setOcupation(THREAD_TYPE::CONSUMER);
						}
				}
			}
			/*else if (float(averageProducerTime / averageConsumerTime) > 1.5 && buffer_raw.size() == 0)
			{
				std::unique_lock<std::mutex> scopedLock(waitMutex);
				auto it = consumerList.begin();
				if (it != consumerList.end()) {
					if(it->second != nullptr)
						it->second->exit();
				}
			}*/
		}
		else 
		{
			buffer_processed->printResults();
			break;
		}

	}
	printf("MAIN THREAD FINISHED!\n");
	Sleep(2000);
	return 0;
}