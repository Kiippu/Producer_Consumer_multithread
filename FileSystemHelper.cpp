#define _CRT_SECURE_NO_WARNINGS
#include "FileSystemHelper.h"
#include <iostream>
#include <sys\stat.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

namespace fs = std::experimental::filesystem;


void FileSystemHelper::createFolder(std::string pathName)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	fs::path tempPath{ pathName };
	fs::create_directories(tempPath);
}

void FileSystemHelper::removeFolders(std::string pathName)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	fs::path tempPath{ pathName };
	fs::remove_all(tempPath);
}

std::vector<std::string> FileSystemHelper::getDirectoryFiles(const std::string & dir, const std::vector<std::string> & extensions)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	//temp vector
	std::vector<std::string> files;
	//look thorugh all directories for files with extension
	for (auto & p : fs::recursive_directory_iterator(dir))
	{
		//if the file is a file not folder
		if (fs::is_regular_file(p))
		{
			//if file has extension
			if (extensions.empty() || find(extensions.begin(), extensions.end(), p.path().extension().string()) != extensions.end())
			{
				//add DIR to temp vector
				files.push_back(p.path().string());
			}
		}
	}
	return files;
}

void FileSystemHelper::createFile(std::string path, std::string fileName, std::string extension)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	// make path
	fs::path dir = path;
	fs::path file_name = fileName;
	
	// create DIR
	fs::path fileCreate = dir / file_name;
	fileCreate.replace_extension(extension);
	std::ofstream(fileCreate) << "";
}

void FileSystemHelper::writeToFileAppend(std::string path, std::string filename, std::string extension, std::string stream,bool timeStamp, std::string msgType)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	using namespace std::chrono;
	// make path
	fs::path dir = path;
	fs::path file_name = filename;
	file_name.replace_extension(extension);
	fs::path file = dir / file_name;

	// open out stream
	std::ofstream out;
	out.open(file,std::ios::app);
	if (out.is_open()) {
		//if there is a msg and time stamp
		if (msgType.size() > 4 && timeStamp == true) {
			auto time_point = system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(time_point);
			auto time = std::ctime(&now_c);
			auto str_time = std::string(time).substr(0, std::string(time).size() - 2);
			out << "\n[\'" << msgType << "\'][ " << str_time << " ] \"" << stream << "\"";
		}
		//if ther is msg but not timestamp
		else if(msgType.size() > 0 && timeStamp == false){
			out << "\n[\'" << msgType << "\']" << stream << "\"";
		}
		//if not msg and yes to timestamp
		else if (msgType.size() == 0 && timeStamp == true) {
			auto time_point = system_clock::now();
			std::time_t now_c = std::chrono::system_clock::to_time_t(time_point);
			auto time = std::ctime(&now_c);
			auto str_time = std::string(time).substr(0, std::string(time).size() - 2);
			out << "\n[ " << str_time << " ] \"" << stream << "\"";
		}
		//if there is no msg and no want timestamp
		else if(msgType.size() == 0 && timeStamp == false) {
			out << stream;
		}
	}
	// if there is not file of that name
	else {
		std::cout << "[\'ERROR\'] { " << file << " } was not opened or found." << std::endl;
	}
	// close file
	out.close();
	//std::cout << msgType.size() << std::endl;
}

std::string FileSystemHelper::getLineInFile(std::string path, std::string filename, std::string extension, unsigned line = UINT32_MAX)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	// make path
	fs::path dir = path;
	fs::path file_name = filename;
	file_name.replace_extension(extension);
	fs::path fileDir = dir / file_name;

	std::ifstream file;
	file.open(fileDir);
	std::string str;
	std::string file_contents;

	if (file.is_open()) {
		int line_no = 0;
		if (line != UINT32_MAX)
		{
			while (std::getline(file, str)) {
				if (line_no == line) {
					file.close();
					return str;
				}
				++line_no;
			}
		}
		else
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			str = buffer.str();
		}
	}
	else
		printf("File not open");

	file.close();
	return str;
}


std::vector<std::string> FileSystemHelper::getMapOfLines(std::string path, std::string filename, std::string extension)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	// make path
	fs::path dir = path;
	fs::path file_name = filename;
	file_name.replace_extension(extension);
	fs::path fileDir = dir / file_name;

	std::ifstream file;
	file.open(fileDir);
	std::string str;
	std::string file_contents;
	std::vector<std::string> list;
	if (file.is_open()) {
		int line_no = 0;
		while (std::getline(file, str)) {
			list.push_back(str);
			++line_no;
		}
	}
	else
		printf("File not open");

	file.close();
	return list;
}

void FileSystemHelper::replaceDataInFile(std::string path, std::string filename, std::string extension, std::string member, std::string data)
{
	std::unique_lock<std::mutex> scopedLock(m_waitMutex);
	// make path
	fs::path dir = path;
	fs::path file_name = filename + "." + extension;
	fs::path file = dir / file_name;

	//open in stream
	std::ifstream stream;
	stream.open(file);

	/****************************
	TODO:
	add in JSON file searcher

	may need to change arg's

	*****************************/
}