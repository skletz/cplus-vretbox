#ifndef _DEFUSE_BRISKXTRACTOR_HPP_
#define _DEFUSE_BRISKXTRACTOR_HPP_

#include "xtractorbase.hpp"

#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"

namespace defuse
{
  class HOGXtractor : public XtractorBase
  {

  public:

	std::string mName = "Static HOG Xtractor";

	HOGXtractor();

    FeaturesBase* xtract(VideoBase* _videobase) override;

    std::string toString() override;

    std::string getXtractorID() const override;

  };
}

#endif //_DEFUSE_BRISKXTRACTOR_HPP_
