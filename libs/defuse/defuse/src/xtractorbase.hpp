#ifndef _DEFUSE_XTRACTORBASE_HPP_
#define _DEFUSE_XTRACTORBASE_HPP_

#include <string>

#include "featuresbase.hpp"
#include "videobase.hpp"

namespace defuse {

	class XtractorBase
	{

	public:

		bool mDisaply = false;

		bool mSaveDisplay = false;

		Directory* mOutput;

		/**
		 *
		 */
		virtual ~XtractorBase() {	}

		/**
		 * \brief Extract features from video
		 */
		virtual FeaturesBase* xtract(VideoBase* _videobase) = 0;

		/**
		* \brief Get xtractor as string
		* \return xtractor as string
		*/
		virtual std::string toString() const = 0;

		/**
		* \brief Get xtractor as ID
		* \return xtractor ID as string
		*/
		virtual std::string getXtractorID() const = 0;

		/**
		* \brief Prints the current progress to standard command out
		* \param _step current step
		* \param _total total steps
		*/
		virtual void showProgress(int _step, int _total) const = 0;

	};
}

#endif //_DEFUSE_XTRACTORBASE_HPP_
