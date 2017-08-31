#ifndef __DebugLevel_hpp__
#define __DebugLevel_hpp__

namespace cpluslogger
{
	enum DebugLevel
	{
		debugINFO,
		debugWARNING,
		debugERROR,
		debugFATAL,
		debugALL
	};

	static const char* DebugNames[] = { "INFO", "WARNING", "ERROR", "FATAL" };
	// statically check that the size of ReportingNames fits the number of ReportingLevel
	static_assert(sizeof(cpluslogger::DebugNames) / sizeof(char*) == cpluslogger::debugALL, "DebugLevel: Sizes do not match!");
}

#endif //__DebugLevel_hpp__
