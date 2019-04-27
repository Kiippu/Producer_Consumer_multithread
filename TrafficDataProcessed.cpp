#include "TrafficDataProcessed.h"
#include "FileSystemHelper.h"
#include "SimpleEvent.h"


TrafficDataProcessed::~TrafficDataProcessed()
{
}

TrafficDataProcessed::TrafficDataProcessed()
{
	SimpleEvent * globalEvents = &SimpleEvent::getInstance();
	globalEvents->registerEvent("PRINT_DATA",Event([&]() {
		m_print = true;
	}));
	m_print = false;
}


bool TrafficDataProcessed::add(std::unique_ptr<TrifficLightData> data)
{
	auto it = m_buffer.find(data->TimeID);
	if (it != m_buffer.end())
	{
		it->second->addData(data->ID, data->dataPair);
		if(m_print)
			printResults();
	}
	else {
		std::unique_ptr<TrafficLightResults> pair = std::make_unique<TrafficLightResults>();
		pair->addData(data->ID,data->dataPair);
		m_buffer.insert(std::make_pair(data->TimeID, std::move(pair)));
	}
	return true;
}

std::map<eTimeHour, std::unique_ptr<TrafficLightResults>>& TrafficDataProcessed::get()
{
	return m_buffer;
}

TrafficLightResults * TrafficDataProcessed::getResults(eTimeHour hour)
{
	auto it = m_buffer.find(hour);
	if (it == m_buffer.end())
		return nullptr;
	return it->second.get();
}

bool TrafficDataProcessed::printResults()
{
	m_print = false;
	FileSystemHelper * globalFileHelper = &FileSystemHelper::getInstance();

	globalFileHelper->createFolder("./LightData");

	std::string data;
	std::string filename("sortedLightData_");
	data.append("\n\n---------------Data Set Results---------------\n\n");
	for (auto& sets_hourly : m_buffer)
	{
		filename.append(std::to_string(sets_hourly.first));
		globalFileHelper->createFile("./LightData", filename, "txt");
		data.append("Hourly set: " + std::to_string(sets_hourly.first) + "\n");
		for (auto& subsets : sets_hourly.second->m_dataSet)
		{
			data.append("\n\t- Light ID: " + std::to_string(subsets.first) + "\n");
			for (auto& pair : subsets.second)
			{
				data.append("\t\t- Measurement: " + std::to_string(pair.first) + "\tCar Count: " + std::to_string(pair.second) + "\n");
			}
		}
	}
	data.append("\n\n---------------Data Set Results---------------\n\n");

	globalFileHelper->writeToFileAppend("./LightData", filename, "txt", data, true,"UPDATED");
	return true;
}



