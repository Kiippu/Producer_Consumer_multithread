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
	/// Buffer objects
	std::shared_ptr<TrafficDataBaseRaw> buffer_raw = std::make_shared<TrafficDataBaseRaw>();
	std::shared_ptr <TrafficDataProcessed> buffer_processed = std::make_shared < TrafficDataProcessed>();
	///utility classes
	Timer* globalTimer = &Timer::getInstance();									// Timer class
	SimpleEvent * globalEvents = &SimpleEvent::getInstance();					// events system
	FileSystemHelper * globalFileHelper = &FileSystemHelper::getInstance();		// file system
	ThreadSpool * globalThreadPool = &ThreadSpool::getInstance();				// thread pool

	/// creating unique data set
	unsigned dataCount = lights * 12;
	std::string dataset;
	for (size_t i = 0; i < lights; i++)				// number of lights to check 
	{
		unsigned timestamp = 0;
		for (size_t j = 0; j < 12; j++)				// number of measurments per hour
		{
			timestamp = j;
			auto ran = (time(0) *((i+2)*j+2));		// random number 
			srand(ran);
			unsigned carCount = rand() % 1000 + 1;
			dataset.append("ID#" + std::to_string(i) + "-TimeStamp:" + std::to_string(timestamp) + "[" + std::to_string(carCount) + "]\n");
		}
	}

	/// utility class file system helper
	globalFileHelper->createFolder("./LightData");											// create folder if doesnt exist
	globalFileHelper->createFile("./LightData", "originalDataSet", "txt");					// create file if doesnt exist
	globalFileHelper->writeToFileAppend("./LightData", "originalDataSet", "txt", dataset);	// save string to file

	/// Simple events system if i need events thrown in paralell
	auto thread_events = ThreadSpool::getInstance().getAvaliable();
	thread_events->setOcupation(THREAD_TYPE::EVENTS);
	thread_events->addFiber(Fiber([&globalEvents]() {globalEvents->process(); }));			// add object to a fiber to run on a thread

	/// consumer and producer thread maps
	std::map< unsigned,std::shared_ptr<Consumer>> consumerList;
	std::map< unsigned, std::shared_ptr<Producer>> producerList;

	/// timers to regulate creation and deletion of threads, aims to keep 1-1 ratio
	double averageProducerTime = 1;
	double averageConsumerTime = 1;
	/// threads left after main threads are created - disposible thread scehuler objs
	unsigned startingThreadCount = globalThreadPool->isAvaliable();

	/// create first producer and set up
	if (auto threadProducer = ThreadSpool::getInstance().getAvaliable()) {
		std::shared_ptr<Producer> pro = std::make_shared<Producer>(*buffer_raw, *globalEvents);
		unsigned id = pro->getID();
		producerList.insert(std::make_pair(id, pro));
		threadProducer->addFiber(Fiber([&producerList, id]() { while (producerList[id]->process()) {}; producerList.erase(id); }));
		threadProducer->setOcupation(THREAD_TYPE::PRODUCER);
	}

	/// variables check in main thread loop
	unsigned size;
	auto limit = buffer_raw->getLimit();
	auto currentState = 0;
	unsigned count = 0;
	/// main thread loop
	while (globalTimer->update() && (count < dataCount - 1))
	{
		/// reset variables
		size  = buffer_raw->size();
		count = buffer_processed->count();
		currentState = buffer_raw->getAddedCount();
		/// blocks when data set == expected
		if (count < dataCount-1)
		{
			/// get current variable thread states
			unsigned consumerCount = globalThreadPool->getTypeCount(THREAD_TYPE::CONSUMER);
			unsigned producerCount = globalThreadPool->getTypeCount(THREAD_TYPE::PRODUCER);
			unsigned threadMax = globalThreadPool->getMaxCount();

			/// find average elapsed time for each stage
			for (auto& prod : producerList)						// current producer list
			{
				if (prod.second->getDelta() > 0)
					averageProducerTime = ((averageProducerTime + prod.second->getDelta()) / 2);
			}
			for (auto& cons : consumerList )					// current consumer list
			{
				if (cons.second->getDelta() > 0)
					averageConsumerTime = ((averageConsumerTime + cons.second->getDelta()) / 2);
			}

			if (currentState >= dataCount - 1)
				int k = 0;
			/// create new producer or not?
			if ((float(averageConsumerTime / averageProducerTime) <= 1.) || (currentState < dataCount-1))
			{
				if (size < limit)									// produced vs required to produce
				{
					if (producerCount < (startingThreadCount - 2))	// producer count vs max thread count
						if (auto threadProducer = ThreadSpool::getInstance().getAvaliable()) {
							std::shared_ptr<Producer> pro = std::make_shared<Producer>(*buffer_raw, *globalEvents);
							unsigned id = pro->getID();
							producerList.insert(std::make_pair(id, pro));
							/// looped producer method and if it finish it signs it has finished and safe to destro/ erase
							threadProducer->addFiber(Fiber([&producerList, id]() { while (producerList[id]->process()) {}; producerList[id]->exitedThread(); }));
							threadProducer->setOcupation(THREAD_TYPE::PRODUCER);
						}
				}
				else		// if max then make sure consumersare only needed
					averageProducerTime = 0.0001;
			}
			/// remove producer with elapsed timer || end buffer set || no consumers currently created 
			else if (float(averageConsumerTime / averageProducerTime  > 1.2 || (currentState >= dataCount - 1)) || consumerList.size() == 0)
			{
				std::unique_lock<std::mutex> scopedLock(waitMutex);
				bool removal = false;
				for (auto &obj : producerList)				// remove signaled finishe threads over any others
				{
					if (obj.second->hasExited()) {
						producerList.erase(obj.first);
						removal = true;
						break;
					}
				}
				if (!removal) {								// remove any if none have signalled finished
					auto it = producerList.begin();
					if (it != producerList.end())
						if (it->second != nullptr)
							it->second->exit();
				}
			}
			/// create consumers  - average timer in favor || buffer size hits a limit || no consumers exist 
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
		}
		/// required light count has been created 
		else
		{
			buffer_processed->printResults(globalTimer->getElapsed());	// print results to file
			break;
		}

	}
	printf("MAIN THREAD FINISHED!\n");
	/// time to clean up; before main thread exits
	globalThreadPool->cleanUpThreads();
	Sleep(2000);
	return 0;
}