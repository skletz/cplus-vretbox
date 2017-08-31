#ifndef _DEFUSE_SIGXTRACTOR_HPP_
#define _DEFUSE_SIGXTRACTOR_HPP_

#include "featuresbase.hpp"
#include "videobase.hpp"
#include "xtractorbase.hpp"
#include "samplepoints.hpp"

#include <opencv2/opencv.hpp>

/**
 * Feature Signatures Extractor
 * This class implements the extraction of static feature signatures. 
 * Parts of the source code are based on the implemenation of 
 * PCT (position-color-texture) signatures, which were planned 
 * as a contribution for OpenCV's extra modules.
 * 
 * Original implementation of position-color-texture signatures:
 * @title: PCTSignatures source code
 * @author Gregor Kovalcik, Martin Krulis, Jakub Lokoc
 * @date 19/10/15 
 * @version 1.0 
 * @availability https://github.com/GregorKovalcik/opencv_contrib
 * 
 * Adapted and modified:
 * @author skletz
 * @version 2.0 07/08/2017 
 */
namespace defuse {

	typedef int idx;
	typedef struct {
		idx x;
		idx y;
		idx l;
		idx a;
		idx b;
		idx c;
		idx e;
		idx w;
		idx dims;
		//dynamic extension
		idx t;
		idx x2;
		idx y2;
		idx md;
	} FSIndex;

	typedef float range;
	typedef struct {
		range l;
		range a;
		range b;
	} LABranges;

	typedef float normalize;
	typedef struct {
		normalize c;
		normalize e;
	} CENormalize;

	class SIGXtractor : public XtractorBase
	{
	public:

		std::string mName = "Static Feature Signatures Xtractor";
		
		enum KeyFrameSelection { FirstFrame, MiddleFrame, LastFrame };

		FSIndex indices;
		LABranges ranges;
		CENormalize normalizer;

		//Default Setting of flow-based sampling
		KeyFrameSelection mKeyFrameSelection = SIGXtractor::KeyFrameSelection::MiddleFrame;

		//Default Setting of the k-means clustering
		int mInitSeeds;

		int mMaxClusters;

		int mIterations;

		int mMinimalClusterSize;

		float mMinimalDistance;

		float mClusterDropThreshold;

		//Stored samplepoints, used as random distribution
		std::vector<cv::Point2f> mSamplepoints;

		//Contrast, Entropy - Grayscale Bitmap
		int mGrayscaleBits;

		int mWindowRadius;

		std::vector<double> mWeights;

		std::vector<double> mTranslations;

	private:
		float mP;

	public:
		float (SIGXtractor::*mDistance)(cv::Mat&, int, cv::Mat&, int) const;

		
		/**
		 * \brief Default constructor to create feature signatures
		 * \param _samplepoints: std::vector<cv::Point2f>
		 * \param _initSeeds = 100
		 * \param _initialCentroids = 10
		 * \param _iterations = 5
		 * \param _minClusterSize = 2
		 * \param _minDistance = 0.01
		 * \param _dropThreshold = 0
		 * \param _samplepoints = nullptr
		 * \param _grayscaleBits = 5
		 * \param _windowRadius = 4
		 */
		SIGXtractor(
			std::vector<cv::Point2f> _samplepoints,
			int _initSeeds = 100,
			int _initialCentroids = 10, 
			float lpdistance = 1.0,
			int _iterations = 5, 
			int _minClusterSize = 2, 
			float _minDistance = 0.01,
			float _dropThreshold = 0.0, 
			int _grayscaleBits = 4, 
			int _windowRadius = 5);


		/**
		 * \brief 
		 * \param _videobase 
		 * \return 
		 */
		FeaturesBase* xtract(VideoBase* _videobase) override;


		void setLpDistance(float lpdistance);

		float getLpDistance();
		/**
		 * \brief 
		 * \return 
		 */
		std::string toString() const override;

		/**
		 * \brief 
		 * \return 
		 */
		std::string getXtractorID() const override;
	
		void deNormalizePoints(std::vector<cv::Point2f> samplepoints, int width, int height, std::vector<cv::Point2f>& output) const;

		void showProgress(int _step, int _total) const override;

	
		bool computeStaticSignatures(cv::VideoCapture& _video, std::string filename, cv::OutputArray _signatures) const;
		void getSamples(cv::Mat& frame, std::vector<cv::Point2f> initPoints, cv::Mat& samples) const;
		void getClusters(cv::Mat& samples, cv::Mat& signatures) const;
	private:
		void initGrayscaleBitmap(cv::Mat image, std::vector<std::uint32_t>& data, std::vector<std::uint32_t>& histogram) const;
		void getContrastEntropy(std::size_t x, std::size_t y, int width, int height, std::vector<std::uint32_t>& data, std::vector<std::uint32_t>& histogram, double& contrast, double& entropy, std::size_t windowRadius) const;

		std::uint32_t getPixel(std::size_t x, std::size_t y, int width, std::vector<std::uint32_t>& data) const;
		void setPixel(std::size_t x, std::size_t y, int width, std::vector<std::uint32_t>& data, std::uint32_t val) const;
		void updateHistogram(std::uint32_t a, std::uint32_t b, std::vector<std::uint32_t>& histogram) const;

		void joinCloseClusters(cv::Mat clusters) const;
		void dropLightPoints(cv::Mat& clusters) const;
		int findClosestCluster(cv::Mat &clusters, cv::Mat &points, int pointIdx) const;
		void normalizeWeights(cv::Mat &clusters) const;
		void singleClusterFallback(const cv::Mat &points, cv::Mat &clusters) const;
		void cropClusters(cv::Mat &clusters) const;

		float computeL1(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const;
		float computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const;
		float computeLp(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const;
	public:
		void drawSamples(const cv::Mat _source, const cv::Mat _samples, cv::Mat& _result) const;
		void drawSignatures(const cv::Mat _source, const cv::Mat _signature, cv::Mat& _result) const;
		void showImage(const cv::Mat _image, cv::Size _size, int x, int y, std::string name) const;
		void saveImage(std::string file, std::string fileextension, std::string name, cv::Mat& image) const;
	};
}

#endif //_DEFUSE_SIGXTRACTOR_HPP_