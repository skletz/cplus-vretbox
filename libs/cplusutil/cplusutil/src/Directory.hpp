#ifndef __UTIL_DIRECTORY_H__
#define __UTIL_DIRECTORY_H__

#include <string>
#include <vector>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
class Directory
{
	//String containing the part of filename interpreted as a path 
	fs::path mPath;
	
public:
	std::string mDirName;
	std::vector<std::string> directories;

	Directory(std::string path);

	std::string getPath();
	void addDirectory(std::string directory);
};

#endif //__UTIL_DIRECTORY_H__
