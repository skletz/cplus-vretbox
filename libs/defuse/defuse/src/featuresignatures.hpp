#ifndef _DEFUSE_FEATURESIGNATURES_HPP_
#define _DEFUSE_FEATURESIGNATURES_HPP_

#include <cplusutil.hpp>
#include <cpluslogger.hpp>
#include <opencv2/opencv.hpp>
#include "featuresbase.hpp"

namespace defuse {

	class FeatureSignatures : public FeaturesBase
	{

	public:

		/**
		 * \brief Default Constructor
		 */
		FeatureSignatures();


		/**
		 * \brief 
		 * \param _id 
		 */
		FeatureSignatures(std::string _id);

		/**
		 * \brief 
		 * \param _id 
		 * \param _features 
		 */
		FeatureSignatures(std::string _id, cv::Mat _features);

		/**
		 * \brief 
		 */
		~FeatureSignatures() {};

		/**
		 * \brief 
		 * \param fs 
		 */
		void write(cv::FileStorage& fs) const;

		/**
		 * \brief 
		 * \param node 
		 */
		void read(const cv::FileNode& node);
	};


	static void write(cv::FileStorage& fs, const std::string&, const std::vector<FeatureSignatures>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const FeatureSignatures& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, FeatureSignatures& x, const FeatureSignatures& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}
}
#endif