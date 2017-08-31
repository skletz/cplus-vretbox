#include "sqfd.hpp"

defuse::SQFD::SQFD(Minkowski* _GDDistance, int _similarity, float _alpha)
{
	mSimilarity =_similarity;
	mAlpha = _alpha;
	mGDDistance = _GDDistance;
}

float defuse::SQFD::compute(FeaturesBase& _f1, FeaturesBase& _f2)
{
	double result = 0;

	result += computePartialSQFD(_f1, _f1);
	result += computePartialSQFD(_f2, _f2);
	result -= computePartialSQFD(_f1, _f2) * 2;
	result = float(sqrt(result));
	return float(result);
}

float defuse::SQFD::compute(FeaturesBase& _f1, int _idx1, FeaturesBase& _f2, int _idx2) const
{
	return mGDDistance->compute(_f1.mVectors, _idx1, _f2.mVectors, _idx2);
}

double defuse::SQFD::computePartialSQFD(FeaturesBase& _f1, FeaturesBase& _f2) const
{
	float result = 0;
	for (int i = 0; i < _f1.mVectors.rows; i++)
	{
		for (int j = 0; j < _f2.mVectors.rows; j++)
		{
			float dist = compute(_f1, i, _f2, j);

			result +=
				_f1.mVectors.at<float>(i, _f1.mVectors.cols - 1) *
				_f2.mVectors.at<float>(j, _f2.mVectors.cols - 2) *
				(1 / (mAlpha + dist));
		}
	}
	return result;
}

std::string defuse::SQFD::toString() const
{
	std::stringstream st;
	st << mGDDistance->toString() << ";";
	st << "Similarity: S" << mSimilarity << ";";
	st << "Alpha: A" << mAlpha;
	return st.str();
}

std::string defuse::SQFD::getDistanceID() const
{
	std::stringstream st;
	st << "SMD_";
	st << mGDDistance->toString();
	st << "_S" << mSimilarity;
	st << "_A" << mAlpha;
	return st.str();
}
