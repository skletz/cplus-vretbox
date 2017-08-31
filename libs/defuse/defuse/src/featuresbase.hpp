#ifndef _DEFUSE_FEATURESBASE_HPP_
#define _DEFUSE_FEATURESBASE_HPP_
#include <cpluslogger.hpp>
#include <vector>
#include <opencv2/core/core.hpp>

namespace defuse {

	class FeaturesBase
	{
	public:

		/**
		 * \brief 
		 */
		FeaturesBase();

		/**
		 * \brief 
		 * \param _id 
		 */
		FeaturesBase(std::string _id);

		/**
		 * \brief 
		 * \param _id 
		 * \param _features 
		 */
		FeaturesBase(std::string _id, cv::Mat _features);

		/**
		 * \brief 
		 * \param _features 
		 */
		FeaturesBase(cv::Mat _features);

		/**
		 * \brief 
		 */
		~FeaturesBase();

		std::string mID;

		cv::Mat mVectors;

		int mMatchingCriteria;

		float mExtractionTime;

		std::string getMatType(cv::Mat inputMat) const;
		int getMatType(std::string type) const;
		int getMatDepth(std::string type) const;

		bool serialize(std::string _destinationfile);
		bool deserialize(std::string _sourcefile);

		bool writeBinary(std::string _file);
		bool readBinary(std::string _file);

		bool writeTextfile(std::string _file);
		bool readTextfile(std::string _file);

		bool writeCSVFile(std::string _file);
		bool readCSVFile(std::string _file);

		bool writeOpenCVData(std::string _file) const;
		bool readOpenCVData(std::string _file);

		//OpenCV YAML (De)-Serialization
		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);

	};

	static void write(cv::FileStorage& fs, const std::string&, const std::vector<FeaturesBase>& x)
	{
		fs << "[";
		for (unsigned int i = 0; i < x.size(); i++)
		{
			x.at(i).write(fs);
		}
		fs << "]";
	}

	static void write(cv::FileStorage& fs, const std::string&, const FeaturesBase& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, FeaturesBase& x, const FeaturesBase& default_value)
	{
		if (node.empty())
			LOG_ERROR("Error: Features OpenCV read; default Value!");

		x.read(node);
	}

	
}

#endif //_DEFUSE_FEATURESBASE_HPP_
