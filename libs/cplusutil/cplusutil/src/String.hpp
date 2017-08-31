#ifndef __CplusutilString_h__
#define __CplusutilString_h__

#include <string>
#include <vector>

namespace cplusutil
{
		class String
		{
		public:
			static std::string toStirng(int i);

			static unsigned int toUnsignedInt(std::string _number);

			static int extractIntegerPart(std::string _string, bool& _hasInteger);

			static int extractIntFromString(std::string fileName);
			
			static std::string concatIntegers(std::string delemniter, int args ...);

			static std::string concatCStrings(std::string delemniter, int argc ...);

			static std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);

			static std::vector<std::string> split(const std::string &s, char delim);
		};
}


#endif //__CplusutilString_h__