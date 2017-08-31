#include "cnnfeatures.hpp"

defuse::CNNFeatures::CNNFeatures()
{
	mID = "";
	mVectors = cv::Mat();
}

defuse::CNNFeatures::CNNFeatures(std::string _id)
	: FeaturesBase(_id)
{
	mVectors = cv::Mat();
}

defuse::CNNFeatures::CNNFeatures(std::string _id, cv::Mat _features)
	: FeaturesBase(_id, _features)
{
}

void defuse::CNNFeatures::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Features" << mVectors
		<< "}";
}

void defuse::CNNFeatures::read(const cv::FileNode& node)
{
	node["Features"] >> mVectors;
}
