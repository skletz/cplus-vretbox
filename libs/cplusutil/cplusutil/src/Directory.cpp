#include "Directory.hpp"
#include "FileIO.hpp"

Directory::Directory(std::string path)
{
	std::string p = cplusutil::FileIO::expandEnvironemntVariable(path);
	mPath = cplusutil::FileIO::getAbsolutePath(p);
	cplusutil::FileIO::createDirectoriesIfNotExist(mPath.string());
	mDirName = cplusutil::FileIO::getName(mPath.string());
	for (auto& part : mPath)
	{
		directories.push_back(part.string());
	}
}

void Directory::addDirectory(std::string directory)
{
	std::string d = cplusutil::FileIO::expandEnvironemntVariable(directory);

	boost::filesystem::path tmp{ mPath };
	tmp /= d;
	mPath = tmp.string();
	directories.push_back(d);
	cplusutil::FileIO::createDirectoriesIfNotExist(mPath.string());
	mDirName = d;
}

std::string Directory::getPath()
{
	return mPath.string();
}
