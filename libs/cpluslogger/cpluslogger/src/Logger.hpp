#ifndef __Logger_hpp__
#define __Logger_hpp__

#include <sstream>
#include <string>

#include "cplusLoggerMacros.hpp"
#include "cplusLoggerGlobal.hpp"

#include "ReportingLevel.hpp"
#include "DebugLevel.hpp"
#include "cplusLoggerDefaults.hpp"
#include <fstream>

namespace cpluslogger
{
	class Logger
	{
		public:
			
			/*!
				
			*/
			static Logger* get();

			void write();

			void write(ReportingLevel _level);

			void write(DebugLevel _level);

			void write(std::string _what);

			~Logger();

			void logging(bool _enable);

			void filelogging(bool _enable);

			void openLogfile(const char *_fname);

			void reportinglevel(ReportingLevel _level);

			void logdir(std::string _dir);

			void logfile(std::string _file);

			void debugging(bool _debugging);

			void debuggingLevel(DebugLevel _level);

			void perfmonitoring(bool _perfmonitoring);

			std::ostringstream& log(ReportingLevel _level);

			void log(ReportingLevel _level, std::ostringstream& msg);

			std::ostringstream& log(DebugLevel _level);

			std::ostringstream& log(std::string _what);

		protected:
			std::ostringstream mStream;
			std::ofstream mFilestream;

		private:

			static cpluslogger::Logger* mLoggerInstance;

			bool mLogging = DEFAULT_LOGGING;

			bool mFilelogging = DEFAULT_FILELOGGING;

			ReportingLevel mReportingLevel = DEFAULT_REPORTINGLEVEL;

			bool mDebugging = DEFAULT_DEBUGGING;

			DebugLevel mDebugLevel = DEFAULT_DEBUGGINGLEVEL;

			std::string mLogfile = DEFAULT_LOGFILE;

			bool mPerfmonitoring = DEFUALT_PERFMON;

			std::string reportingLevelToString(ReportingLevel _level);

			std::string debugLevelToString(DebugLevel _level);

			Logger();
	};
}

#endif //__Logger_hpp__

