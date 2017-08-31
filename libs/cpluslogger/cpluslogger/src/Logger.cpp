#include "Logger.hpp"
#include <iostream>

cpluslogger::Logger* cpluslogger::Logger::mLoggerInstance = nullptr;

cpluslogger::Logger::Logger() {}

cpluslogger::Logger* cpluslogger::Logger::get()
{
	if (mLoggerInstance == nullptr)
	{
		mLoggerInstance = new Logger();
	}

	return mLoggerInstance;
}

void cpluslogger::Logger::write(ReportingLevel _level)
{
	if (mLogging && mReportingLevel >= _level)
	{
		if (!mDebugging && (_level != ReportingLevel::DEBUG))
		{
			return;
		}
		
		write();
	}
	
}

void cpluslogger::Logger::write(DebugLevel _level)
{
	if (mDebugging && mDebugLevel >= _level)
		write();
}

void cpluslogger::Logger::write(std::string _what)
{
	if (mPerfmonitoring)
		write();
}

void cpluslogger::Logger::write()
{
	if (mFilelogging)
	{
		if (!mFilestream.is_open())
		{
			std::string logfile = mLogfile;
			openLogfile(logfile.c_str());
		}

		mFilestream << mStream.str() << std::endl;
	}
	
	std::cout << mStream.str() << std::endl;

	try
	{
		mStream.str("");
		mStream.clear();
	}catch(const std::exception& e)
	{
		std::cout << "An exception occurred during Logging. Exception Nr. " << e.what() << '\n';
	}

}

void cpluslogger::Logger::logging(bool _enable)
{
	mLogging = _enable;
}

void cpluslogger::Logger::filelogging(bool _enable)
{
	if (mFilelogging && mFilestream.is_open())
	{
		LOG_WARNING("Filelogging is already enabled.");
		return;
	}

	mFilelogging = _enable;

}

void cpluslogger::Logger::openLogfile(const char* _fname)
{
	mFilestream.open(_fname, std::fstream::app);
	if (!mFilestream.is_open())
	{
		LOG_WARNING("Logfile could not open: " << _fname;);
	}
}

void cpluslogger::Logger::reportinglevel(ReportingLevel _level)
{
	mReportingLevel = _level;
}

void cpluslogger::Logger::debugging(bool _debugging)
{
	mDebugging = _debugging;
}

void cpluslogger::Logger::debuggingLevel(DebugLevel _level)
{
	mDebugLevel = _level;
}

void cpluslogger::Logger::logfile(std::string _file)
{
	if (mFilestream.is_open())
	{
		mFilestream.flush();
		mFilestream.close();
	}

	mLogfile = _file;

	openLogfile(mLogfile.c_str());
}

void cpluslogger::Logger::perfmonitoring(bool _perfmonitoring)
{
	mPerfmonitoring = _perfmonitoring;
}


void cpluslogger::Logger::log(ReportingLevel _level, std::ostringstream& msg)
{
	std::cout << log(_level).str() << msg.str() << std::endl;
}

std::ostringstream& cpluslogger::Logger::log(DebugLevel _level)
{
	mStream.str("");
	mStream << kNowTime();
	mStream << " D" << debugLevelToString(_level) << ":\t";
	mStream << std::string(_level > debugALL ? _level - debugALL : 0, '\t');
	return mStream;
}

std::ostringstream& cpluslogger::Logger::log(std::string _what)
{
	mStream.clear();
	mStream << kNowTime();
	try
	{
		mStream << " P" << _what << ":\t";
	}
	catch (const std::exception& e)
	{
		std::cout << "An exception occurred during Logging. Exception Nr. " << e.what() << '\n';
	}
	return mStream;
}

std::ostringstream& cpluslogger::Logger::log(ReportingLevel _level)
{
	mStream.str("");
	mStream << kNowTime();
	std::string report = reportingLevelToString(_level);
	mStream << " " << report << ":\t";
	//int rl = (_level > ALL) ? (_level - ALL) : 0;
	std::string l = std::string(_level > ALL ? (_level - ALL) : 0, '\t');
	mStream << l;
	return mStream;
}

std::string cpluslogger::Logger::reportingLevelToString(ReportingLevel _level)
{
	return cpluslogger::ReportingNames[_level];
}

std::string cpluslogger::Logger::debugLevelToString(DebugLevel _level)
{
	return cpluslogger::DebugNames[_level];
}

cpluslogger::Logger::~Logger(){}

