#include "hamming.hpp"
#include <opencv2/core.hpp>
#include <sstream>
#include <iostream>

defuse::Hamming::Hamming()
{
  // mMatcher = cv::BFMatcher::create(cv::NORM_HAMMING, false); // without crossCheck
  // mMatcher = cv::BFMatcher(new cv::BFMatcher(cv::NORM_HAMMING, false);
}

float defuse::Hamming::compute(FeaturesBase& _f1, FeaturesBase& _f2)
{
	float dist = compute(_f1.mVectors, _f2.mVectors);
	return dist;
}


float defuse::Hamming::compute(cv::Mat& _f1, cv::Mat& _f2) const
{

  // Declare one type off matcher
  // cv::BFMatcher matcher(cv::NORM_HAMMING, true); // with crossCheck
  cv::BFMatcher matcher(cv::NORM_HAMMING, false); // without crossCheck
  std::vector<cv::DMatch> matches;
  // mMatcher.match(_f1, _f2, matches);
  matcher.match(_f1, _f2, matches);
  // sort matches
  float result = 0;

  for( int i = 0; i < _f1.rows; i++ )
  { double dist = matches[i].distance;
    result += float(dist);
  }

  result /= float(matches.size());

  //-- Quick calculation of max and min distances between keypoints
  // double max_dist = 0; double min_dist = 100;
  // for( int i = 0; i < _f1.rows; i++ )
  // { double dist = matches[i].distance;
  //   if( dist < min_dist ) min_dist = dist;
  //   if( dist > max_dist ) max_dist = dist;
  // }
  // printf("-- Max dist : %f \n", max_dist );
  // printf("-- Min dist : %f \n", min_dist );

  // OLD: not working, f1 & f2 must be same size
  //float result_bits = float(cv::norm(_f1, _f2, cv::NORM_HAMMING));

  return result;
}

float defuse::Hamming::computeNorm(cv::Mat& _f1, cv::Mat& _f2) const
{

  float result = 0;

  //for each representative in f1, find
  //the most similar in f2 (nearest neighbor matching)
  for (int i = 0; i < _f1.rows; i++)
  {
    float minDist = FLT_MAX;

    for (int j = 0; j < _f2.rows; j++)
    {
      float distance = float(cv::norm(_f1.row(i), _f2.row(j), cv::NORM_HAMMING));

      if (distance < minDist)
      {
        minDist = distance;
      }
    }
    result += minDist;
  }

  result /= float(_f1.rows);

  return result;
}


std::string defuse::Hamming::toString() const
{
	return "cvHamming, Matcher: cvBF";
}

std::string defuse::Hamming::getDistanceID() const
{
	return "cvHamming_cvBF";
}
