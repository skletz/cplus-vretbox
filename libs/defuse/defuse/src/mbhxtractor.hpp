#ifndef _DEFUSE_MBHXTRACTOR_HPP_
#define _DEFUSE_MBHXTRACTOR_HPP_

#include "xtractorbase.hpp"

/**
 * Feature Signatures Extractor
 * This class implements the exraction of flow-based dynamic feature signatures.
 */
namespace defuse {


	class MBHXtractor : public XtractorBase
	{
	public:

		std::string mName = "Motion Boundary Histogram Xtractor";
		
		MBHXtractor();

		FeaturesBase* xtract(VideoBase* _videobase) override;

		std::string toString() override;

		std::string getXtractorID() const override;

	};
}

#endif //_DEFUSE_MBHXTRACTOR_HPP_