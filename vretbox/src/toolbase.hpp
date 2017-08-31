#ifndef _TOOLBASE_HPP_
#define  _TOOLBASE_HPP_

#include <boost/program_options.hpp>

namespace vretbox {

	/**
	* \brief
	*/
	class ToolBase
	{
		
	protected:

		boost::program_options::variables_map mArgs;
		
	public:

		/**
		 * \brief 
		 */
		virtual ~ToolBase() = 0;

		/**
		 * \brief 
		 * \param _args 
		 * \return 
		 */
		virtual bool init(boost::program_options::variables_map _args) = 0;

		/**
		 * \brief 
		 */
		virtual void run() = 0;


	};

	inline ToolBase::~ToolBase() {}
}

#endif //_TOOLBASE_HPP_