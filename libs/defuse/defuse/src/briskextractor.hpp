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
    cv::Ptr<cv::BRISK> mKeypointDetector;
    cv::Ptr<cv::BRISK> mDescriptorExtactor;
    enum KeyFrameSelection { FirstFrame, MiddleFrame, LastFrame };
    //Default Setting of flow-based sampling
    KeyFrameSelection mKeyFrameSelection = MiddleFrame;

    BRISKXtractor();
    FeaturesBase* xtract(VideoBase* _videobase) override;
    std::string toString() const override;
    std::string getXtractorID() const override;
    void showProgress(int _step, int _total) const override;
    bool computeBriskKeypoints(cv::Mat& _grayimage, std::vector<cv::KeyPoint>& _keypoints) const;
    bool computeBriskDescriptors(cv::Mat& _grayimage, std::vector<cv::KeyPoint>& _keypoints, cv::OutputArray& _signatures) const;
    void testBrisk(VideoBase* _videobase, int _framedistance);
  };
}

#endif //_DEFUSE_BRISKXTRACTOR_HPP_
