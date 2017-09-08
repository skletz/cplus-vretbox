#include "hogxtractor.hpp"
#include <cpluslogger.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

defuse::HOGXtractor::HOGXtractor()
{

}

defuse::FeaturesBase* defuse::HOGXtractor::xtract(VideoBase* _videobase)
{

	return nullptr;
}

std::string defuse::HOGXtractor::toString()
{
	std::string result;
	result += "xtractor: HOG, ";
	result += getKeyframeSelectionAsString();
	return result;
}

std::string defuse::HOGXtractor::getXtractorID() const
{
	std::stringstream st;
	st << "HOG_";
	st << mKeyFrameSelection;
	return st.str();
}
