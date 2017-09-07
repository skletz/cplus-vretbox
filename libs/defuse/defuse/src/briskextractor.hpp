#ifndef _DEFUSE_BRISKXTRACTOR_HPP_
#define _DEFUSE_BRISKXTRACTOR_HPP_

#include "xtractorbase.hpp"

#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"

namespace defuse
{
  class BRISKXtractor : public XtractorBase
  {

  public:

	std::string mName = "Static BRISK Xtractor";

    cv::Ptr<cv::BRISK> mKeypointDetector;

    cv::Ptr<cv::BRISK> mDescriptorExtactor;

    enum KeyFrameSelection { FirstFrame, MiddleFrame, LastFrame };

    //Default Setting of flow-based sampling
    KeyFrameSelection mKeyFrameSelection = MiddleFrame;

    BRISKXtractor();

    FeaturesBase* xtract(VideoBase* _videobase) override;

    std::string toString() override;

    std::string getXtractorID() const override;

    bool computeBriskKeypoints(cv::Mat& _grayimage, std::vector<cv::KeyPoint>& _keypoints) const;

    bool computeBriskDescriptors(cv::Mat& _grayimage, std::vector<cv::KeyPoint>& _keypoints, cv::OutputArray& _signatures) const;

    void testBrisk(VideoBase* _videobase, int _framedistance) const;

  };
}

#endif //_DEFUSE_BRISKXTRACTOR_HPP_
