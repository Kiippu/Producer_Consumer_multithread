#include "TrafficDataProcessed.h"
#include "FileSystemHelper.h"
#include "SimpleEvent.h"
#include <Windows.h>


TrafficDataProcessed::~TrafficDataProcessed()
{
}

TrafficDataProcessed::TrafficDataProcessed()
{
	SimpleEvent * globalEvents = &SimpleEvent::getInstance();
	globalEvents->registerEvent("PRINT_DATA",Event([&]() {		// event to call printif need be
		printResults(10);
	}));
	m_print = false;
}


bool TrafficDataProcessed::add(std::unique_ptr<TrifficLightData> data)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	m_inventoryCount++;
	auto it = m_buffer.find(data->TimeID);
	if (it != m_buffer.end())							// if measuement exist use it
	{
		it->second->addData(data->ID, data->dataPair);
	}
	else {												// not measurment exist manke new one
		std::unique_ptr<TrafficLightResults> pair = std::make_unique<TrafficLightResults>();
		pair->addData(data->ID,data->dataPair);
		m_buffer.insert(std::make_pair(data->TimeID, std::move(pair)));
	}
	return true;
}

std::map<eTimeHour, std::unique_ptr<TrafficLightResults>>& TrafficDataProcessed::get()
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	return m_buffer;
}

TrafficLightResults * TrafficDataProcessed::getResults(eTimeHour hour)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	auto it = m_buffer.find(hour);
	if (it == m_buffer.end())
		return nullptr;
	return it->second.get();
}

bool TrafficDataProcessed::printResults(uint64_t elapsed)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	printf("PRINTING RESULTS");
	m_print = false;
	FileSystemHelper * globalFileHelper = &FileSystemHelper::getInstance();

	globalFileHelper->createFolder("./LightData");

	std::vector<std::string> dataArray;
	unsigned idSize = m_inventoryCount / 12;
	std::string data;
	std::string filename("sortedLightData_");
	data.append("\n\n--------------- Data Set Results ---------------\n");
	data.append("--------------- " + std::to_string(idSize) + " light ID's -------------\n");
	data.append("--------------- " + std::to_string(idSize*12) + " Measurments ---------------\n");
	data.append("--------------- " + std::to_string(elapsed) + " Ms Taken ---------------\n\n");
	for (auto& sets_hourly : m_buffer)													// build string
	{
		filename.append(std::to_string(sets_hourly.first));
		globalFileHelper->createFile("./LightData", filename, "txt");
		data.append("Hourly set: " + std::to_string(sets_hourly.first) + "\n");			// add hour
		for (auto& subsets : sets_hourly.second->m_dataSet)
		{
			data.append("\n\t- Light ID: " + std::to_string(subsets.first) + "\n");		// add light ID
			for (auto& pair : subsets.second)											// ad measutremnt and car count
			{
				data.append("\t\t- Measurement: " + std::to_string(pair.first) + "\tCar Count: " + std::to_string(pair.second) + "\n");
			}
		}
	}
	data.append("\n\n---------------Data Set Results---------------\n\n");

	globalFileHelper->writeToFileAppend("./LightData", filename, "txt", data, true,"UPDATED");		// add to file
	return true;
}



