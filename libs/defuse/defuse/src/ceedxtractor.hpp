#ifndef _DEFUSE_CEEDXTRACTOR_HPP_
#define _DEFUSE_CEEDXTRACTOR_HPP_

#include <opencv2/opencv.hpp>
#include "xtractorbase.hpp"
#include "fuzzificator.hpp"

/**
* CEED Extractor (Color and Edge Directivity Descriptor)
* This class implements the extraction of color and edge directivities.
* Parts of the source code are based on the JAVA implemenation of CEED using LIRE.
*
* Original implementation of CEED:
* @title: CEED source code
* @author Savvas Chatzichristofis
* @date 2017
* @version 1.0
* @availability http://bit.ly/2xcj1rr
*
* Adapted and modified:
* @author skletz
* @version 1.0 09/09/2017
*/
namespace defuse
{
  class CEEDXtractor : public XtractorBase
  {

	  struct Neighborhood
	  {
		  double area1;
		  double area2;
		  double area3;
		  double area4;
	  };

	  struct Mask
	  {
		  double mask1;
		  double mask2;
		  double mask3;
		  double mask4;
		  double mask5;
	  };

	  static int const BLOCKSIZE_MAX = 40;

	  static int const BLOCKSIZE_MIN = 20;

	  Fuzzificator* mFuzzificator;

	  double T0;
	  double T1;
	  double T2;
	  double T3;


  public:

	std::string mName = "Static HOG Xtractor";

	CEEDXtractor();

    FeaturesBase* xtract(VideoBase* _videobase) override;

    std::string toString() override;

    std::string getXtractorID() const override;

	double computeCEEDDescriptor(
		cv::VideoCapture& _video, std::string filename, 
		cv::OutputArray& _descriptor);

	bool describe(cv::Mat& image, cv::Mat& descriptor);

	bool extractFromBlock(cv::Mat& imageBlock, cv::Mat& grayBlock, cv::Mat& features);
  };
}

#endif //_DEFUSE_CEEDXTRACTOR_HPP_
