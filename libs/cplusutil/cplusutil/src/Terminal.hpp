#ifndef __CplusutilTerminal_hpp__
#define __CplusutilTerminal_hpp__

#include <cstdio>
#include <string>

namespace cplusutil
{
	/**
	* \brief Terminal
	*
	* @author skletz
	* @version 2.0 06/05/17
	*/
	class Terminal
	{
	public:
		
		/**
		 * \brief Writes a progressbar to the terminal and show how manx steps of maximum steps has already been reached.
		 * \param _label displayed name in front of the progress bar
		 * \param _step current iteration
		 * \param _total maximum number of iterations
		 */
		static void showProgress(std::string _label, int _step, int _total);
	};
}

#endif //__CplusutilTerminal_hpp__