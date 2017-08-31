#ifndef _DEFUSE_RANKEDRESULT_HPP_
#define _DEFUSE_RANKEDRESULT_HPP_

#include "resultbase.hpp"

namespace defuse {

	class RankedResult : public ResultBase 
	{

	public:

		int mMatchingCriteria;

		double mDistance;

		double mSearchTime;

		cv::Mat mFeatures;

		RankedResult(std::string _id, cv::Mat _features);

		/**
		 * \brief 
		 * \param _id 
		 */
		RankedResult(std::string _id);

		/**
		 * \brief 
		 */
		~RankedResult();

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


	static void write(cv::FileStorage& fs, const std::string&, const RankedResult& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, ResultBase& x, const RankedResult& default_value)
	{
		if (node.empty())
			LOG_ERROR("AbstractFeature OpenCV read; default Value!");

		x.read(node);
	}
}

#endif //_DEFUSE_RANKEDRESULT_HPP_