#include "hamming.hpp"
#include <opencv2/core.hpp>
#include <sstream>
#include <iostream>

defuse::Hamming::Hamming()
{
  //mMatcher = new cv::BFMatcher(cv::NORM_HAMMING, true); // with crossCheck
  mMatcher = new cv::BFMatcher(cv::NORM_HAMMING, false); // without crossCheck
}

float defuse::Hamming::compute(FeaturesBase& _f1, FeaturesBase& _f2)
{
	float dist = compute(_f1.mVectors, _f2.mVectors);
	return dist;
}


float defuse::Hamming::compute(cv::Mat& _f1, cv::Mat& _f2) const
{
	float result = 0;
	// cv::BFMatcher matcher(cv::NORM_HAMMING, true); // with crossCheck
	// cv::BFMatcher matcher(cv::NORM_HAMMING, false); // without crossCheck
	// matcher.match(_f1, _f2, matches);

	// use BF matcher (hamming) to detect closes keypoint matches
	/*******int rows = std::min(_f1.rows, _f2.rows);
	std::vector<cv::DMatch> matches = std::vector<cv::DMatch>(rows);
	mMatcher->match(_f1, _f2, matches);

	// sort matches
	float result = 0;

	for( int i = 0; i < _f1.rows; i++ )
	{ 
		double dist = matches[i].distance;
		result += float(dist);
	}

	// LOG_INFO("# BF " << matches.size())
	result /= float(matches.size());
	*******/
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

	int size = std::min(_f2.rows, _f1.rows);

	std::vector<std::vector<cv::DMatch>> matches = std::vector<std::vector<cv::DMatch>>(size);

	mMatcher->knnMatch(_f1, _f2, matches, 1);

	int counter = 0;
	float dist = 0.0;
	for (int i = 0; i < matches.size(); i++)
	{
		
		if (matches.at(i).size() > 0)
		{
			dist = matches.at(i).at(0).distance;
		}
	}
	result = dist / float(matches.size());

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

	// LOG_INFO("# Norm " << _f1.rows)

  result /= float(_f1.rows);

  return result;
}


std::string defuse::Hamming::toString() const
{
	return "cvHamming, Matcher: cvBFwC";
}

std::string defuse::Hamming::getDistanceID() const
{
	return "CVHAMBF_WC";
}
