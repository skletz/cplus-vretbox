#ifndef _DEFUSE_VALUATOR_HPP_
#define _DEFUSE_VALUATOR_HPP_

#include "featuresbase.hpp"
#include "rankedresult.hpp"
#include "distancebase.hpp"
#include "cplusutil.hpp"
#include <map>
#include <mutex>

namespace defuse {

	class Valuator
	{

	public:

		std::string mModelname;

		File* mCompDistanceTimeFile;

		std::vector<std::pair<int, double>> mMaps;

		double mAAMap;

		std::vector<std::pair<int, double>> mCompDistanceTimes;

		std::vector<std::map<std::tuple<int, double>, double>> mPrecisionRecallCurveValues;

		std::vector<std::tuple<int, double, double, double>> mPrecisionRecallAtK;

		std::vector<std::pair<int, double>> mPrecisionAtK;

		std::vector<std::pair<int, double>> mRecallAtK;

		//The same as mMaps
		std::vector<std::pair<int, double>> mAvgPrecisionAtK;

		bool mStoreValues;

		std::mutex mLock;

		Valuator();

		double computeRandomMAPs(int _id, std::vector<FeaturesBase*> _queries,
			DistanceBase* _distance, std::vector<FeaturesBase*> _model);

		double sortModelToQuery(
			FeaturesBase* _query, std::vector<FeaturesBase*> _model, 
			DistanceBase* _distance, std::vector<RankedResult*>& _results);

		double sortModelRandomToQuery(
			FeaturesBase* _query, std::vector<FeaturesBase*> _model,
			DistanceBase* _distance, std::vector<RankedResult*>& _results);

		double computeMAPAtK(
			int k, int _id, std::vector<FeaturesBase*> _queries,
			DistanceBase* _distance, std::vector<FeaturesBase*> _model);

		double computeAP(
			FeaturesBase* _query, std::vector<RankedResult*> _results,
			int _maxMatchings, std::map<std::tuple<int, double>, double>& _precisionRecallProgress,
			std::tuple<int, double, double, double>& _precAtKValues);

		double computeAPAtK(
			int k, FeaturesBase* _query, std::vector<RankedResult*> _results,
			int _maxMatchings, std::map<std::tuple<int, double>, double>& _precisionRecallProgress,
			std::tuple<int, double, double, double>& _precAtKValues);

		void addMapValues(int id, double map);
		
		void addCompTimeValues(int id, double map);

		void addPrecisionRecallCurveValues(std::map<std::tuple<int, double>, double> _precisionRecall, int _values);

		void addPrecisionRecallAtKValues(int _id, std::vector<std::tuple<int, double, double, double>> _precisionRecallAtK, int _values);

		bool writeValuesToCSVTemplate(File* _template, std::string _type, std::vector<std::pair<int, double>> _values);

		bool writePrecesionRecallValues(File* _file, std::vector<std::map<std::tuple<int, double>, double>> _precisionRecallValues);

		void interpolateRecallTo11Steps(
			std::vector<std::map<std::tuple<int, double>, double>> _precisionRecall, 
			std::vector<std::map<std::tuple<int, double>, double>>& _interpolatedPrecisionRecall);

		~Valuator(){};

		std::unique_lock<std::mutex> locking();

		void showProgress(std::string label, int _step, int _total) const;
	};
}

#endif //_DEFUSE_VALUATOR_HPP_
