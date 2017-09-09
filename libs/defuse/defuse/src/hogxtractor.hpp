#ifndef _DEFUSE_HOGXTRACTOR_HPP_
#define _DEFUSE_HOGXTRACTOR_HPP_

#include "xtractorbase.hpp"

#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"

namespace defuse
{
  class HOGXtractor : public XtractorBase
  {

	  static int const CELLSIZE = 8;
	  cv::Size mWinSize;
	  cv::Size mBlockSize;
	  cv::Size mBlockStride;
	  cv::Size mCellSize;

  public:

	std::string mName = "Static HOG Xtractor";

	HOGXtractor();

    FeaturesBase* xtract(VideoBase* _videobase) override;

    std::string toString() override;

    std::string getXtractorID() const override;

	float computecvHOGDescriptor(cv::VideoCapture& _videobase, std::string filename, cv::OutputArray& _descriptor);

	float computeHogFeatures(cv::Mat& image, cv::Mat& descriptor, std::string filename);

	  /**
	 * \brief http://www.juergenbrauer.org/old_wiki/doku.php?id=public:hog_descriptor_computation_and_visualization
	 * \param origImg 
	 * \param descriptorValues 
	 * \param winSize 
	 * \param cellSize 
	 * \param scaleFactor 
	 * \param viz_factor 
	 * \param image 
	 */
	void visualizeHOGDescriptor(
		cv::Mat& origImg, std::vector<float>& descriptorValues,
		cv::Size winSize, cv::Size cellSize, int scaleFactor, double viz_factor, cv::Mat& image);


  };
}

#endif //_DEFUSE_HOGXTRACTOR_HPP_
