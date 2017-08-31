#ifndef __cplusLoggerMacros_h__
#define __cplusLoggerMacros_h__
//** CplusLoggerMacros *******************************************************

/*
	It provides a light weight simple logging. Default Logger logs to the console.
	Macros are: LOG_INFO, LOG_WARNING, LOG_ERROR, LOG_FATAL, LOG_DEBUG

	Special log infos for debugging mode uses macros like: LOG_DEBUG with a specific level.
	Based on the level it shows more details or not ;-).
	Levels are: DINFO, DWARNING, DERROR, DFATAL

	Log entries for performance analysis like the result of caluclations or its execution times
	uses macors like LOG_PERFORMANCE.
	Currently there are not more needs for performance logging, but it might be a requirement in
	special logging needs, so it could be extended.
*/

#include "ReportingLevel.hpp"
#include "DebugLevel.hpp"

//Genaral reporting modes
#define RINFO cpluslogger::ReportingLevel::INFO
#define RWARNING cpluslogger::ReportingLevel::WARNING
#define RERROR cpluslogger::ReportingLevel::ERR
#define RFATAL cpluslogger::ReportingLevel::FATAL
#define RDEBUG cpluslogger::ReportingLevel::DEBUG

//Simple Logging Levels
#define WRITE(level) cpluslogger::Logger::get()->write(level);

#define LOG_INFO(msg)		{cpluslogger::Logger::get()->log(RINFO)		<< msg; WRITE(RINFO)}
#define LOG_WARNING(msg)	{cpluslogger::Logger::get()->log(RWARNING)	<< msg; WRITE(RWARNING)}
#define LOG_ERROR(msg)		{cpluslogger::Logger::get()->log(RERROR)	<< msg; WRITE(RERROR)}
//Fatal = Application Error
#define LOG_FATAL(msg)		{cpluslogger::Logger::get()->log(RFATAL)	<< msg; WRITE(RFATAL)}

//For simple Debugging
#define LOG_DEBUG(msg)		{cpluslogger::Logger::get()->log(RDEBUG)	<< msg; WRITE(RDEBUG)}

//Enhanced Debugging levels
#define LOG_LDEBUG(level, msg)	{cpluslogger::Logger::get()->log(level)	<< msg; WRITE(level)}
#define DINFO cpluslogger::DebugLevel::debugINFO
#define DWARNING cpluslogger::DebugLevel::debugWARNING
#define DERROR cpluslogger::DebugLevel::debugERROR
#define DFATAL cpluslogger::DebugLevel::debugFATAL

//Enhanced Performance Logging for calcuation and execution times
#define LOG_PERFMON(what, msg) {cpluslogger::Logger::get()->log(what) << msg; WRITE(what)}
#define PRESULT "RESULT" 
#define PINTERIM "INTERIM" //Zwischenergebnisse
#define PTIME "TIME" //Ausführungszeite



//** CplusLoggerMacros *******************************************************
#endif //__cplusLoggerMacros_h__