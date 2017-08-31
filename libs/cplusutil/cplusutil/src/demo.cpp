/** CPLUSUtil (Version 2.0) ****************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* This library provides several help classes and functions, 
* such as file input/output or string operations as well as 
* managing files and directories.
* ******************************************************
* 
* Purpose: Demo Usage
*
* @author skletz
* @version 2.0 06/05/17
*
**/

#include "../include/cplusutil.hpp"
#include <iostream>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#include <windows.h>
std::string getexepath()
{
	char result[MAX_PATH];
	return std::string(result, GetModuleFileName(NULL, result, MAX_PATH));
}
#else
#include <limits.h>
#include <unistd.h>
std::string getexepath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}
#endif

void demoFileIO(std::string path)
{
	//Ist the current Path validate
	bool isValid = cplusutil::FileIO::isValidPathToFile(path);
	if (isValid)
	{
		std::cout << "The path " << path << " is valid!" << std::endl;
	}
	else
	{
		std::cout << "The path " << path << " is not valid!" << std::endl;
	}

	std::cout << "The current working directory is " << cplusutil::FileIO::getCurrentWorkingDirectory() << std::endl;
}

void demoTerminal(std::string label, int _max)
{
	for (int iCounter = 0; iCounter <= _max; iCounter++)
	{
		cplusutil::Terminal::showProgress(label, iCounter, _max);
	}
}

int main(int argc, char** argv)
{
	//FileIO
	demoFileIO(getexepath());

	//Terminal
	demoTerminal("Demonstration", 1000);

	return cplusutil::PROCESS_SUCCESS;
}