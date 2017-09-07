#ifndef _DEFUSE_FLOWDYSIGXTRACTOR_HPP_
#define _DEFUSE_FLOWDYSIGXTRACTOR_HPP_

#include "sigxtractor.hpp"

/**
 * Feature Signatures Extractor
 * This class implements the exraction of flow-based dynamic feature signatures.
 */
namespace defuse {


	class FlowDySIGXtractor : public SIGXtractor
	{
	public:

		std::string mName = "Dynamic Feature Signatures 1 Xtractor";
		
		int mMaxFrames;

		bool mResetTracking;

		FlowDySIGXtractor(
			std::vector<cv::Point2f> _samplepoints,
			int _maxFrames = 5,
			int _initSeeds = 100,
			int _initialCentroids = 10, 
			int _iterations = 5, 
			int _minClusterSize = 2, 
			float _minDistance = 0.01,
			float _dropThreshold = 0.0, 
			int _grayscaleBits = 5, 
			int _windowRadius = 4,
			bool _resetTracking = true);

		FeaturesBase* xtract(VideoBase* _videobase) override;

		std::string toString() override;

		std::string getXtractorID() const override;

		bool computeDynamicSignatures(cv::VideoCapture& _video, std::string filename, cv::OutputArray _signatures);

		void getMotionSamples(
			std::vector<uchar>& statusVector, std::vector<float>& errorVector, 
			std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& currPoints, 
			int height, int width, cv::Mat& out);

		void drawMotionDirection(const cv::Mat _source, cv::Mat _samples, cv::Mat& _result);

		void drawResidualImage(const cv::Mat _current, const cv::Mat _previous, cv::Mat& _result);

		void drawDynamicSamples(const cv::Mat _source, const cv::Mat _samples, cv::Mat& _result);

		float angleBetween(const cv::Point &v1, const cv::Point &v2);

		double square(int a) const;

	};
}

#endif //_DEFUSE_FLOWDYSIGXTRACTOR_HPP_