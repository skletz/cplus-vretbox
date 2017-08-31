#ifndef _DEFUSE_DISTANCEBASE_HPP_
#define _DEFUSE_DISTANCEBASE_HPP_

#include "featuresbase.hpp"

namespace defuse {

	class DistanceBase
	{

	public:
		virtual ~DistanceBase() = default;

		virtual float compute(FeaturesBase& _f1, FeaturesBase& _f2) = 0;

		virtual std::string toString() const = 0;

		virtual std::string getDistanceID() const = 0;
	};
}

#endif //_DEFUSE_DISTANCEBASE_HPP_