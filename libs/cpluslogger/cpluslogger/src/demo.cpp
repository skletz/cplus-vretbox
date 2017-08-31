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

#include "../include/cpluslogger.hpp"

using namespace cpluslogger;

int main(int argc, char** argv)
{
	LOG_INFO("CPLUSlogger Demo started ... ");

	Logger::get()->reportinglevel(ReportingLevel::ALL);
	LOG_WARNING("CPLUSlogger warning ... ");
	LOG_ERROR("CPLUSlogger error ... ");
	LOG_FATAL("CPLUSlogger fatal ... ");
	LOG_DEBUG("CPLUSlogger simple debugging ... ");

	Logger::get()->reportinglevel(ReportingLevel::WARNING);
	LOG_WARNING("CPLUSlogger warning ... ");
	LOG_ERROR("CPLUSlogger not visible ... ");
	LOG_FATAL("CPLUSlogger not visible ... ");

	Logger::get()->logging(false);
	LOG_INFO("CPLUSlogger not visible ... ");

	Logger::get()->logging(true);
	Logger::get()->filelogging(true);
	LOG_INFO("CPLUSlogger writes now also to file.");
	
	Logger::get()->logfile("mylogfile.log");
	LOG_INFO("CPLUSlogger change logfile.");

	LOG_LDEBUG(DINFO, "Enhanced debugging info...");
	LOG_LDEBUG(DWARNING, "Enhanced debugging warning...");
	LOG_LDEBUG(DERROR, "Enhanced debugging error...");
	LOG_LDEBUG(DFATAL, "Enhanced debugging fatal...");

	Logger::get()->debuggingLevel(DebugLevel::debugWARNING);
	LOG_LDEBUG(DINFO, "Enhanced debugging info...");
	LOG_LDEBUG(DWARNING, "Enhanced debugging warning...");
	LOG_LDEBUG(DERROR, "Enhanced debugging error...");
	LOG_LDEBUG(DFATAL, "Enhanced debugging fatal...");

	Logger::get()->debuggingLevel(DebugLevel::debugALL);

	Logger::get()->perfmonitoring(true);

	LOG_PERFMON(PRESULT, "calculation results ...");
	LOG_PERFMON(PINTERIM, "calculation interim results ...");
	LOG_PERFMON(PTIME, "execution time ...");

	LOG_INFO("CPLUSlogger Demo finished.");
	return EXIT_SUCCESS;
}
