#include "String.hpp"
#include <iostream>
#include <stdarg.h>
#include <sstream>

std::string cplusutil::String::toStirng(int i)
{
	return std::to_string(i);
}

unsigned cplusutil::String::toUnsignedInt(std::string _number)
{
	std::istringstream reader(_number);
	unsigned int value;
	reader >> value;
	return value;
}

/*! Extract the integer part of a string
	
	Returns the integer containing the string. The _hasInteger out parameter
	indicates whether the string contains a integer or not.

	\param _string a string argument
	\param _hasInteger contains true if an integer is part of _string; otherwise, contains false.
	\return the integer part extracted from the string; -1 if there are no digits

*/
int cplusutil::String::extractIntegerPart(std::string _string, bool& _hasInteger)
{
	int integervalue = 0;
	std::string numbers = "0123456789";
	std::size_t integerpart = _string.find_first_of(numbers.c_str());

	try
	{
		integervalue = std::atoi(_string.c_str());
		_hasInteger = true;
	}
	catch (std::exception const &)
	{
		int counter = 0;
		while (integerpart != std::string::npos) {
			integerpart = _string.find_first_of(numbers.c_str(), integerpart + 1);
			counter++;
		}

		if (counter == 0)
		{
			_hasInteger = false;
			return -1;
		}

		_hasInteger = true;
		return int(integerpart);
	}

	if (integervalue == 0)
		_hasInteger = false;
	return int(integervalue);
}

int cplusutil::String::extractIntFromString(std::string fileName)
{
	int id = 0;
	std::string numbers = "0123456789";
	std::size_t found = fileName.find_first_of(numbers.c_str());
	try
	{
		id = std::atoi(fileName.c_str());
	}
	catch (std::exception const &)
	{

		while (found != std::string::npos) {
			std::cout << found << std::endl;
			found = fileName.find_first_of(numbers.c_str(), found + 1);
		}
		//LOG(WARNING) << "Video Filename contains no Integer to generate a unique ID; Video ID is set to 0";
		return int(found);

	}
	return id;
}

std::string cplusutil::String::concatIntegers(std::string delemniter, int args, ...)
{
	va_list list;
	va_start(list, args);

	std::stringstream ss;
	for (int i = 0; i < args; i++)
	{
		ss << va_arg(list, int);
		if ((i + 1) != args)
			ss << delemniter;
	}
	va_end(list);
	return ss.str();
}

std::string cplusutil::String::concatCStrings(std::string delemniter, int argc, ...)
{
	va_list argPtr;
	char * temp;

	va_start(argPtr, argc);

	std::stringstream ss;

	for (int i = 0; i < argc; i++)
	{
		temp = va_arg(argPtr, char*);
		std::string str(temp);
		ss << str;
		if ((i + 1) != argc)
			ss << delemniter;
	}

	va_end(argPtr);
	return ss.str();
}

std::vector<std::string>& cplusutil::String::split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> cplusutil::String::split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}