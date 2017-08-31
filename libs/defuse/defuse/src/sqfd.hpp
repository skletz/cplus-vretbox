#ifndef _DEFUSE_SQFD_HPP_
#define _DEFUSE_SQFD_HPP_

#include <cplusutil.hpp>
#include "minkowski.hpp"
#include "distancebase.hpp"

/**
* \brief Signature Quadratic Form Distance (SQFD)
*
* @author skletz
* @version 2.0 12/08/17
*/
namespace defuse {

	class SQFD : public DistanceBase
	{

	public:

		Minkowski* mGDDistance;
		int mSimilarity;
		float mAlpha;
		
		SQFD(Minkowski* _GDDistance, int _similarity, float _alpha);

		float compute(FeaturesBase& _f1, FeaturesBase& _f2) override;
		
		float compute(FeaturesBase& _f1, int _idx1, FeaturesBase& _f2, int _idx2) const;

		double computePartialSQFD(FeaturesBase& _f1, FeaturesBase& _f2) const;

		std::string toString() const override;

		std::string getDistanceID() const override;
	};
}

#endif