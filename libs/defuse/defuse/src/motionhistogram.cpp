#include "motionhistogram.hpp"


defuse::MotionHistogram::MotionHistogram()
{
	mID = "";
	mVectors = cv::Mat();
}

defuse::MotionHistogram::MotionHistogram(std::string _id)
	: FeaturesBase(_id)
{
	mVectors = cv::Mat();
}

defuse::MotionHistogram::MotionHistogram(std::string _id, cv::Mat _features)
	: FeaturesBase(_id, _features)
{
}

void defuse::MotionHistogram::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Features" << mVectors
		<< "}";
}

void defuse::MotionHistogram::read(const cv::FileNode& node)
{
	node["Features"] >> mVectors;
}
