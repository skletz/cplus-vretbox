#include "FileIO.hpp"
#include <boost/foreach.hpp>
#include <iostream>


std::string cplusutil::FileIO::getAbsolutePath(std::string _relativPath)
{
	fs::path absolute(fs::initial_path());
	absolute = fs::system_complete(_relativPath);
	absolute.normalize();
	return absolute.string();
}

bool cplusutil::FileIO::createDirectoriesIfNotExist(std::string _path)
{
	fs::path absolutPath = getAbsolutePath(_path);
	return fs::create_directories(absolutPath);
}

bool cplusutil::FileIO::isValidPath(std::string _path)
{
	fs::path path{ _path };
	if (fs::exists(_path))
	{
		return true;
	}

	std::string absolute = fs::absolute(_path).string();
	if (fs::exists(absolute))
	{
		return true;
	}

	return false;
}

bool cplusutil::FileIO::isDirectory(std::string _path)
{
	fs::path path{ _path };
	if (fs::is_directory(path))
	{
		return true;
	}
	return false;
}

bool cplusutil::FileIO::isValidPathToDirectory(std::string _path)
{
	return (isValidPath(_path) && isDirectory(_path));
}

bool cplusutil::FileIO::isValidPathToFile(std::string _path)
{
	return (isValidPath(_path) && !isDirectory(_path));
}

std::string cplusutil::FileIO::getCurrentWorkingDirectory()
{
	return fs::current_path().string();
}

std::string cplusutil::FileIO::getFilename(std::string filePath)
{
	fs::path p = filePath;
	return p.filename().string();
}

std::string cplusutil::FileIO::getFilenameWithoutExtension(std::string filePath)
{
	fs::path p = filePath;

	size_t lastindex = p.filename().string().find_last_of(".");
	std::string rawname = p.filename().string().substr(0, lastindex);

	return rawname;
}

std::string cplusutil::FileIO::getParentDirPath(std::string filePath)
{
	fs::path p = filePath;
	return p.remove_filename().string();
}

std::string cplusutil::FileIO::getParentDirName(std::string filePath)
{
	fs::path p = filePath;
	return p.parent_path().string();
}

std::string cplusutil::FileIO::getName(std::string path)
{
	fs::path p = path;
	return p.filename().string();
}

std::string cplusutil::FileIO::getFileExtension(std::string file)
{
	int pos = file.find_last_of(".");
	std::string ext = "";
	if(pos != -1)
		ext = file.substr(pos);

	return ext;
}

std::vector<std::string> cplusutil::FileIO::getFileListFromDirectory(std::string directory, std::string fileExtension)
{
	std::vector<std::string> files;
	fs::path path_to_directory = directory;

	fs::directory_iterator v_it(path_to_directory), eovd;

	BOOST_FOREACH(fs::path const &p1, std::make_pair(v_it, eovd))
	{
		if (fs::is_regular_file(p1) && v_it->path().extension() == fileExtension)
		{
			files.push_back(p1.string());
		}
	}

	return files;
}

std::vector<std::string> cplusutil::FileIO::getFileListFromDirectory(std::string directory)
{
	std::vector<std::string> files;
	fs::path path_to_directory = directory;

	fs::directory_iterator v_it(path_to_directory), eovd;

	BOOST_FOREACH(fs::path const &p1, std::make_pair(v_it, eovd))
	{
		files.push_back(p1.string());
	}

	return files;
}


std::string cplusutil::FileIO::getFirstFileFromDirectory(std::string directory)
{
	std::string file;
	fs::path path_to_directory = directory;

	fs::directory_iterator v_it(path_to_directory), eovd;

	int counter = 0;

	BOOST_FOREACH(fs::path const &p1, std::make_pair(v_it, eovd))
	{
		file = p1.string();
		counter++;

		if (counter == 1)
			break;
	}

	return file;
}

std::string cplusutil::FileIO::expandEnvironemntVariable(std::string path)
{
	if (path.find("${") == std::string::npos)
		return path;

	std::string pre = path.substr(0, path.find("${"));
	std::string post = path.substr(path.find("${") + 2);

	if (post.find('}') == std::string::npos)
		return path;

	std::string variable = post.substr(0, post.find('}'));
	std::string value = "";

	post = post.substr(post.find('}') + 1);

	if (getenv(variable.c_str()) != nullptr)
	{
		value = std::string(getenv(variable.c_str()));

		if(std::isspace(value.at(value.size() - 1), std::locale(std::locale::classic())))
		{
			value = value.substr(0, value.size() - 2);
		}
	}
	else
		return path;

	return expandEnvironemntVariable(pre + value + post);
}

bool cplusutil::FileIO::copyFiles(std::string src, std::string dest, bool overWrite)
{

	std::string execute = src + " " + dest;
	std::string cmd = "copy";
	bool status = false;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WIN64)
	cmd = "copy";
	std::string toReplace = "/";
	src.replace(src.find(toReplace), toReplace.length(), "\\");
	dest.replace(dest.find(toReplace), toReplace.length(), "\\");
	execute = src + " " + dest;
	execute = cmd + " " + execute;
	status = system(execute.c_str());
#else
	cmd = "cp";
	execute = cmd + " " + execute;
	status = system(execute.c_str());
#endif
	std::cout << execute << std::endl;
	return status;
}
