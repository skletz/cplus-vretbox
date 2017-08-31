#ifndef _DEFUSE_SAMPLEPOINTS_HPP_
#define _DEFUSE_SAMPLEPOINTS_HPP_

#include <cplusutil.hpp>

#include <boost/unordered_map.hpp>
#include <opencv2/core.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>

namespace defuse {

	class SamplePoints
	{

	public:
		enum Distribution { RANDOM, REGULAR, GAUSSIAN};

	private:

		Distribution mDistribution;

		int mSampleCnt;

		std::vector<cv::Point2f> mPoints;

		float mMean;

		float mStddev;

	public:

		/**
		 * \brief Constructor
		 * \param _samplesCnt = 10, number of samples
		 * \param _distribution = Distribution::RANDOM, distribution of sample points
		 * \param _initializeSamplepoints true, if the sample points should also be created 
		 */
		SamplePoints(int _samplesCnt = 10, Distribution _distribution = RANDOM, float mean = 0.0, float stddev = 1.0, bool _initializeSamplepoints = false);

		/**
		 * \brief Constructor
		 * \param _samplepoints, where points ranges between 0.0 and 1.0
		 * \param _distribution of sample points
		 */
		SamplePoints(std::vector<cv::Point2f> _samplepoints, Distribution _distribution, float mean = 0.0, float stddev = 1.0);

		void initSamplePoints(Distribution distribution, int samplesCnt);

		int getSampleCnt() const;

		std::vector<cv::Point2f> getPoints() const;

		Distribution getDistribution() const;

		std::string getSamplePointFileName() const;

		void restoreDistribution(std::string _filename);

		bool serialize(std::string _destinationfile);
		bool deserialize(std::string _sourcefile);
		
		bool writeBinary(std::string _file);
		bool readBinary(std::string _file);

		bool writeTextfile(std::string _file);
		bool readTextfile(std::string _file);

		bool writeOpenCVData(std::string _file) const;
		bool readOpenCVData(std::string _file);

		//OpenCV YAML (De)-Serialization
		void write(cv::FileStorage& fs) const;
		void read(const cv::FileNode& node);

	};

	const boost::unordered_map<SamplePoints::Distribution, const char*> distributionToString = boost::assign::map_list_of
		(SamplePoints::RANDOM, "random")
		(SamplePoints::REGULAR, "regular")
		(SamplePoints::GAUSSIAN, "gaussian");

	static void write(cv::FileStorage& fs, const std::string&, const SamplePoints& x)
	{
		x.write(fs);
	}

	static void read(const cv::FileNode& node, SamplePoints& x, const SamplePoints& default_value = SamplePoints())
	{
		if (node.empty())
			x = default_value;
		else
			x.read(node);
	}
}

#endif