#include "featuresignatures.hpp"

defuse::FeatureSignatures::FeatureSignatures()
{
	mID = "";
	mVectors = cv::Mat();
}

defuse::FeatureSignatures::FeatureSignatures(std::string _id)
	: FeaturesBase(_id)
{
	mVectors = cv::Mat();
}

defuse::FeatureSignatures::FeatureSignatures(std::string _id, cv::Mat _features) 
	: FeaturesBase(_id, _features)
{

}

void defuse::FeatureSignatures::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Features" << mVectors
		<< "}";
}

void defuse::FeatureSignatures::read(const cv::FileNode& node)
{
	node["Features"] >> mVectors;
}
