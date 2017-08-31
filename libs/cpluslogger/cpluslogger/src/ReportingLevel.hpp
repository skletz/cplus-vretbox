#ifndef __ReportingLevel_hpp__
#define __ReportingLevel_hpp__

namespace cpluslogger
{
	enum ReportingLevel
	{
		INFO,
		WARNING,
		ERR, //ERROR already defined with 0
		FATAL,
		DEBUG,
		ALL
	};

	static const char* ReportingNames[] = { "INFO", "WARNING", "ERROR", "FATAL", "DEBUG"};
	// statically check that the size of ReportingNames fits the number of ReportingLevel
	static_assert(sizeof(cpluslogger::ReportingNames) / sizeof(char*) == cpluslogger::ALL, "ReportingLevel: Sizes do not match!");
}

#endif //__ReportingLevel_hpp__
