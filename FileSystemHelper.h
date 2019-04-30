/********************************************************************

HOWTO:
---------------------DIRECTORY HELP---------------------
for parent dir:			./folder_1/folder_2
for pre parent Dir:		././folder_1/folder_2
and etc:				./././blah/blah

---------------------EXTENSION HELP----------------------
for Extensions add a "." to the beginning
	eg.		.json 
		or	.txt 
		or	.mmd 
WILL NOT REGISTER WITH OUT PERIOD BEFORE EXTENSION

*********************************************************************/

#ifndef FILESYSTEM_HELPER_H
#define FILESYSTEM_HELPER_H
#pragma once


#include <string>
#include <vector>
#include <filesystem>
#include <mutex>

class FileSystemHelper
{
public:
	static FileSystemHelper & getInstance() {
		static FileSystemHelper instance;
		return instance;
	}

private:
	// private constructor to impl singleton
	FileSystemHelper() {};

	std::mutex                      m_waitMutex;		/// wrapper's mutual exclusion

public:
	// Safety to delete any copies or new instances
	FileSystemHelper(const FileSystemHelper&)	= delete;
	void operator=(FileSystemHelper const&)		= delete;
	
	// Creates folders 
	void createFolder(std::string path);
	// Removes folder and all contents
	void removeFolders(std::string pathName);
	// Gets all paths of file with extension in directory
	std::vector<std::string> getDirectoryFiles(const std::string & dir, const std::vector<std::string>& extensions);
	// Crates a file with extension
	void createFile(std::string path ,std::string fileName, std::string extension);
	// Write to a file, useful for logs or error msg's
	void writeToFileAppend(std::string path, std::string filename, std::string extension, std::string stream, bool timeStamp = false, std::string msgType = "");
	// get a line in the file
	std::string getLineInFile(std::string path, std::string filename, std::string extension, unsigned line);
	// get all lines in file in a vector
	std::vector<std::string> getMapOfLines(std::string path, std::string filename, std::string extension);

	// TODO: imple this!! depends on JSON or not etc
	void replaceDataInFile(std::string path, std::string filename, std::string extension, std::string member, std::string data);
};

#endif // !FILESYSTEM_HELPER_H