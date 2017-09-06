#ifndef _DEFUSE_HAMMING_HPP_
#define _DEFUSE_HAMMING_HPP_

#include <opencv2/core/mat.hpp>
#include "distancebase.hpp"
#include "opencv2/features2d/features2d.hpp"

namespace defuse {

	class Hamming : public DistanceBase
	{
	public:

    cv::Ptr<cv::BFMatcher> mMatcher;

    /**
     * \brief Hamming Distance
     */
    Hamming();

    float compute(FeaturesBase& _f1, FeaturesBase& _f2) override;

    /**
     * \brief  Calculates the distance between two features
     * \param _f1 features
     * \param _f2 features
     * \return Distance between _f1 and _f2
     */
    float compute(cv::Mat& _f1, cv::Mat& _f2) const;

    float computeNorm(cv::Mat& _f1, cv::Mat& _f2) const;

    /**
    * \brief Prints settings
    * \return Settings of Hamming distance
    */
    std::string toString() const override;

    std::string getDistanceID() const override;
  };
}

#endif //_DEFUSE_HAMMING_HPP_
