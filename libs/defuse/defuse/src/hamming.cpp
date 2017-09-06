#include "hamming.hpp"
#include <opencv2/core.hpp>
#include <sstream>
#include <iostream>

defuse::Hamming::Hamming()
{

}

float defuse::Hamming::compute(FeaturesBase& _f1, FeaturesBase& _f2)
{
	float dist = compute(_f1.mVectors, _f2.mVectors);
	return dist;
}

float defuse::Hamming::compute(cv::Mat& _f1, cv::Mat& _f2) const
{

  // Declare one type off matcher
  cv::BFMatcher matcher(cv::NORM_HAMMING, true);
  std::vector<cv::DMatch> matches;
  matcher.match(_f1, _f2, matches);
  // sort matches
  float result = float(matches.size());

  //-- Quick calculation of max and min distances between keypoints
  double max_dist = 0; double min_dist = 100;
  for( int i = 0; i < _f1.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }
  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  // OLD: not working, f1 & f2 must be same size
  //float result_bits = float(cv::norm(_f1, _f2, cv::NORM_HAMMING));

  return result;
}


std::string defuse::Hamming::toString() const
{
	return "Hamming";
}

std::string defuse::Hamming::getDistanceID() const
{
	return "Hamming";
}
