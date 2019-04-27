#define _CRT_SECURE_NO_WARNINGS
#include "FileSystemHelper.h"
#include <iostream>
#include <sys\stat.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace fs = std::experimental::filesystem;
using namespace std;


void FileSystemHelper::createFolder(std::string pathName)
{
	fs::path tempPath{ pathName };
	fs::create_directories(tempPath);
}

void FileSystemHelper::removeFolders(std::string pathName)
{
	fs::path tempPath{ pathName };
	fs::remove_all(tempPath);
}

vector<string> FileSystemHelper::getDirectoryFiles(const string & dir, const vector<string> & extensions)
{
	//temp vector
	vector<string> files;
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
			out << "\n" << stream;
		}
	}
	// if there is not file of that name
	else {
		cout << "[\'ERROR\'] { " << file << " } was not opened or found." << endl;
	}
	// close file
	out.close();
	cout << msgType.size() << endl;
}

void FileSystemHelper::replaceDataInFile(std::string path, std::string filename, std::string extension, std::string member, std::string data)
{
	// make path
	fs::path dir = path;
	fs::path file_name = filename + "." + extension;
	fs::path file = dir / file_name;

	//open in stream
	ifstream stream;
	stream.open(file);

	/****************************
	TODO:
	add in JSON file searcher

	may need to change arg's

	*****************************/
}