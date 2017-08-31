#include "rankedresult.hpp"

defuse::RankedResult::RankedResult(std::string _id, cv::Mat _features)
	: mMatchingCriteria(0), mDistance(0.0), mSearchTime(0.0)
{
	mID = _id;
	mFeatures = _features;
}

defuse::RankedResult::RankedResult(std::string _id)
	: mMatchingCriteria(0), mDistance(0.0), mSearchTime(0.0)
{
	mID = _id;
}

defuse::RankedResult::~RankedResult(){}

void defuse::RankedResult::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "mID" << mID
		<< "Features" << mFeatures
		<< "}";
}

void defuse::RankedResult::read(const cv::FileNode& node)
{
	mID = static_cast<std::string>(node["mID"]);
	node["Features"] >> mFeatures;
}