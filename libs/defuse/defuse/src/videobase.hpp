#ifndef _DEFUSE_VIDEOBASE_HPP_
#define _DEFUSE_VIDEOBASE_HPP_

#include <cplusutil.hpp>

namespace defuse {

	class VideoBase
	{
	public:

		/**
		 * \brief 
		 */
		File* mFile;


		/**
		* \brief
		*/
		VideoBase(File* _file);

		/**
		* \brief
		*/
		~VideoBase() {};

	};
}
#endif //_DEFUSE_VIDEOBASE_HPP_