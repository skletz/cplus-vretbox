#include "resultbase.hpp"

defuse::ResultBase::ResultBase(){}

defuse::ResultBase::~ResultBase(){}

void defuse::ResultBase::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "mID" << mID
		<< "}";
}

void defuse::ResultBase::read(const cv::FileNode& node)
{
	mID = static_cast<std::string>(node["mID"]);
}
