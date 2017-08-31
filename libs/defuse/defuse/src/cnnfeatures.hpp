#ifndef _DEFUSE_CNNFEATURES_HPP_
#define _DEFUSE_CNNFEATURES_HPP_

#include <cplusutil.hpp>
#include <opencv2/opencv.hpp>
#include "featuresbase.hpp"

namespace defuse {

	class CNNFeatures : public FeaturesBase
	{

	public:

		CNNFeatures();

		CNNFeatures(std::string _id);

		CNNFeatures(std::string _id, cv::Mat _features);;

		~CNNFeatures() {};

		void write(cv::FileStorage& fs) const;

		void read(const cv::FileNode& node);

	};

	static void write(cv::FileStorage& fs, const std::string&, const std::vector<CNNFeatures>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const CNNFeatures& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, CNNFeatures& x, const CNNFeatures& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}

}
#endif