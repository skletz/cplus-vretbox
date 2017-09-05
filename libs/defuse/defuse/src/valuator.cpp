#include  "valuator.hpp"
#include "rankedresult.hpp"
#include <iostream>
#include <mutex>
#include <random>

defuse::Valuator::Valuator() { }

double defuse::Valuator::computeRandomMAPs(
	int _id, std::vector<FeaturesBase*> _queries,
	DistanceBase* _distance, std::vector<FeaturesBase*> _model)
{
	std::unique_lock<std::mutex> guard(locking());
	LOG_INFO("Group started: " << _id << " Radnom MAP@" << " ... ");
	guard.unlock();

	std::vector<std::tuple<int, double, double, double>> precisionRecallAtK;

	int querySize = int(_queries.size());

	std::map<std::tuple<int, double>, double> precAtValues;
	std::tuple<int, double, double, double> precAtKValues;

	double meanAveragePrecision = 0.0;
	double averageSearchtime = 0.0;
	for (int iQuery = 0; iQuery < querySize; iQuery++)
	{
		std::vector<RankedResult*> _results;
		FeaturesBase* query = _queries.at(iQuery);

		averageSearchtime = sortModelRandomToQuery(query, _model, _distance, _results);

		double averagePrecision = computeAP(query, _results, querySize, precAtValues, precAtKValues);
		meanAveragePrecision += averagePrecision;

		precisionRecallAtK.push_back(precAtKValues);

		guard.lock();
		showProgress("Group: " + std::to_string(_id), iQuery + 1, querySize);
		guard.unlock();

		for (std::vector< RankedResult* >::iterator it = _results.begin(); it != _results.end(); ++it)
		{
			delete (*it);
		}
		_results.clear();
	}

	meanAveragePrecision = meanAveragePrecision / double(querySize);

	addMapValues(_id, meanAveragePrecision);
	addCompTimeValues(_id, averageSearchtime);
	addPrecisionRecallCurveValues(precAtValues, querySize);
	addPrecisionRecallAtKValues(_id, precisionRecallAtK, querySize);

	this->mAAMap = meanAveragePrecision;

	guard.lock();
	LOG_INFO("Group finished: " << _id << " Radnom MAP@" << ": " << meanAveragePrecision);
	guard.unlock();

	return meanAveragePrecision;
}

double defuse::Valuator::sortModelToQuery(
	FeaturesBase* _query, std::vector<FeaturesBase*> _model,
	DistanceBase* _distance, std::vector<RankedResult*>& _results)
{
	int modelSize = int(_model.size());
	//std::vector<defuse::RankedResult*> results;
	_results.reserve(modelSize);

	double avgSearchTime = 0.0;
	for (int iElem = 0; iElem < modelSize; iElem++)
	{
		FeaturesBase* element = _model.at(iElem);

		double e1_start = double(cv::getTickCount());
		float distance = _distance->compute(*(_query), *element);
		double e1_end = double(cv::getTickCount());

		if (distance < 0)
		{
			LOG_FATAL("Fatal Error: The Distance is smaller than zero: " << distance);
		}

		double tickFrequency = double(cv::getTickFrequency());
		double searchTime = (e1_end - e1_start) / tickFrequency;
		avgSearchTime += searchTime;

		RankedResult* result = new RankedResult(element->mID);
		result->mMatchingCriteria = element->mMatchingCriteria;
		result->mDistance = distance;
		result->mSearchTime = searchTime;

		_results.push_back(result);
	}

	std::sort(_results.begin(), _results.end(), [](const RankedResult* s1, const RankedResult* s2)
	{
		return (s1->mDistance < s2->mDistance);
	});

	//return results
	//_results.assign(results.begin(), results.end());

	return avgSearchTime = avgSearchTime / double(modelSize);;
}

double defuse::Valuator::sortModelRandomToQuery(FeaturesBase* _query, std::vector<FeaturesBase*> _model, DistanceBase* _distance, std::vector<RankedResult*>& _results)
{
	int modelSize = int(_model.size());
	_results.reserve(modelSize);

	for (int iElem = 0; iElem < modelSize; iElem++)
	{
		FeaturesBase* element = _model.at(iElem);

		float distance = 0.0;

		if (distance < 0)
		{
			LOG_FATAL("Fatal Error: The Distance is smaller than zero: " << distance);
		}

		double tickFrequency = double(cv::getTickFrequency());

		RankedResult* result = new RankedResult(element->mID);
		result->mMatchingCriteria = element->mMatchingCriteria;
		result->mDistance = 0.0;
		result->mSearchTime = 0.0;

		_results.push_back(result);
	}

	srand(time(0));
	std::random_shuffle(_results.begin(), _results.end(), [](int n) { return rand() % n; });

	auto engine = std::default_random_engine{};
	std::shuffle(_results.begin(), _results.end(), engine);

	return 0.0;
}

double defuse::Valuator::computeAPAtK(
	int k, FeaturesBase* _query, std::vector<RankedResult*> _results,
	int _maxMatchings, std::map<std::tuple<int, double>, double>& _precProgress,
	std::tuple<int, double, double, double>& _precAtK)
{

	int matches = 0;

	double avgPrecisionAsSum = 0;

	int resultSize = int(_results.size());

	for (int iResult = 1; iResult < k + 1; iResult++)
	{
		RankedResult* rankedresult = _results.at(iResult - 1);

		if (rankedresult->mMatchingCriteria == _query->mMatchingCriteria)
		{
			matches++;
			double precision = static_cast<double>(matches) / static_cast<double>(iResult);
			avgPrecisionAsSum += precision;

			double recall = matches / float(_maxMatchings);
			std::tuple<int, double> index = std::make_tuple(_query->mMatchingCriteria, recall);
			_precProgress[index] += precision;
		}
	}

	
	int min = std::min(_maxMatchings, k);
	double averagePrecision = (avgPrecisionAsSum / static_cast<double>(min));

	double precisionAtK = (matches / float(k));

	double recallAtK = (matches / float(_maxMatchings));

	_precAtK = std::make_tuple(_query->mMatchingCriteria, precisionAtK, recallAtK, averagePrecision);

	return averagePrecision;
}


double defuse::Valuator::computeAP(
	FeaturesBase* _query, std::vector<RankedResult*> _results,
	int _maxMatchings, std::map<std::tuple<int, double>, double>& _precProgress,
	std::tuple<int, double, double, double>& _precAtK)
{
	int k = int(_results.size());
	double result = computeAPAtK(k, _query, _results, _maxMatchings, _precProgress, _precAtK);
	return result;
}

double defuse::Valuator::computeMAPAtK(
	int k, int _id, std::vector<FeaturesBase*> _queries,
	DistanceBase* _distance, std::vector<FeaturesBase*> _model)
{
	std::unique_lock<std::mutex> guard(locking());
	LOG_INFO("Group started: " << _id << " MAP@" << k << " ... ");
	guard.unlock();

	std::vector<std::tuple<int, double, double, double>> precisionRecallAtK;

	int querySize = int(_queries.size());

	std::map<std::tuple<int, double>, double> precAtValues;
	std::tuple<int, double, double, double> precAtKValues;

	double meanAveragePrecision = 0.0;
	double averageSearchtime = 0.0;
	for (int iQuery = 0; iQuery < querySize; iQuery++)
	{
		std::vector<RankedResult*> _results;
		FeaturesBase* query = _queries.at(iQuery);
		averageSearchtime = sortModelToQuery(query, _model, _distance, _results);
		double averagePrecision = computeAPAtK(k, query, _results, querySize, precAtValues, precAtKValues);
		meanAveragePrecision += averagePrecision;

		precisionRecallAtK.push_back(precAtKValues);

		guard.lock();
		showProgress("Group: " + std::to_string(_id), iQuery + 1, querySize);
		guard.unlock();

		for (std::vector< RankedResult* >::iterator it = _results.begin(); it != _results.end(); ++it)
		{
			delete (*it);
		}
		_results.clear();
	}

	meanAveragePrecision = meanAveragePrecision / double(querySize);

	addMapValues(_id, meanAveragePrecision);
	addCompTimeValues(_id, averageSearchtime);
	addPrecisionRecallCurveValues(precAtValues, querySize);
	addPrecisionRecallAtKValues(_id, precisionRecallAtK, querySize);

	this->mAAMap = meanAveragePrecision;

	guard.lock();
	LOG_INFO("Group finished: " << _id << " MAP@" << k << ": " << meanAveragePrecision);
	guard.unlock();

	return meanAveragePrecision;
}

void defuse::Valuator::addMapValues(int _id, double _map)
{
	mMaps.push_back(std::make_pair(_id, _map));
}

void defuse::Valuator::addCompTimeValues(int _id, double _time)
{
	mCompDistanceTimes.push_back(std::make_pair(_id, _time));
}

void defuse::Valuator::addPrecisionRecallCurveValues(std::map<std::tuple<int, double>, double> _precisionRecall, int _values)
{
	for (auto it = _precisionRecall.begin(); it != _precisionRecall.end(); ++it) {
		(*it).second = (*it).second / double(_values);
	}

	mPrecisionRecallCurveValues.push_back(_precisionRecall);
}

void defuse::Valuator::addPrecisionRecallAtKValues(int _id, std::vector<std::tuple<int, double, double, double>> _precisionRecallAtK, int _values)
{
	double recallAtK = 0.0;
	double precisionAtK = 0.0;
	double avgPrecisionAtK = 0.0;

	int size = int(_precisionRecallAtK.size());
	for (int i = 0; i < size; i++)
	{
		recallAtK += std::get<1>(_precisionRecallAtK.at(i));
		precisionAtK += std::get<2>(_precisionRecallAtK.at(i));
		avgPrecisionAtK += std::get<3>(_precisionRecallAtK.at(i));
	}

	recallAtK = recallAtK / double(_values);
	precisionAtK = precisionAtK / double(_values);
	avgPrecisionAtK = avgPrecisionAtK / double(_values);

	std::tuple<int, double, double, double> tuple = std::make_tuple(_id, recallAtK, precisionAtK, avgPrecisionAtK);
	mPrecisionRecallAtK.push_back(tuple);

	mPrecisionAtK.push_back(std::make_pair(_id, precisionAtK));
	mRecallAtK.push_back(std::make_pair(_id, recallAtK));
	mAvgPrecisionAtK.push_back(std::make_pair(_id, avgPrecisionAtK));
}

bool defuse::Valuator::writeValuesToCSVTemplate(File* _template, std::string _type, std::vector<std::pair<int, double>> _values)
{
	std::ifstream csvfileIn;
	csvfileIn.open(_template->getFile().c_str(), std::ios_base::in);

	if (!csvfileIn.is_open())
	{
		LOG_ERROR("Error: Cannot open CSV File: " << _template->getFile());
		return false;
	}

	std::string line;
	csvfileIn >> line;
	csvfileIn.close();

	std::ofstream csvfileOut;
	csvfileOut.open(_template->getFile().c_str(), std::ios_base::app);
	std::vector<std::string> elems = cplusutil::String::split(line, ';');

	std::vector<std::string> newLine(elems);
	newLine.at(0) = mModelname + ";";
	newLine.at(0) += _type + ";";

	for (int i = 1; i < elems.size(); i++)
	{
		int elem = std::atoi(elems.at(i).c_str());

		for (int j = 0; j < _values.size(); j++)
		{
			if (_values.at(j).first == elem)
			{
				std::string value = std::to_string(_values.at(j).second) + ";";
				newLine.at(i) = value;
				break;
			}

		}

	}
	newLine.push_back(std::to_string(0.0));

	for (int i = 0; i < newLine.size(); i++)
	{
		csvfileOut << newLine.at(i);
	}

	csvfileOut << std::endl;
	csvfileOut.close();
	return true;
}

bool defuse::Valuator::writePrecesionRecallValues(File* _file, std::vector<std::map<std::tuple<int, double>, double>> _precisionRecallValues)
{
	std::string header;
	std::string line;

	std::ofstream outfile;

	//_file->extendFileName("prec");

	outfile.open(_file->getFile(), std::ofstream::out);

	if (!outfile.is_open())
	{
		LOG_ERROR("Error: Cannot open CSV File: " << _file->getFile());
		return false;
	}

	header = ";";

	int max = 0;
	int pos = 0;
	for (int i = 0; i < _precisionRecallValues.size(); i++)
	{
		int nr = _precisionRecallValues.at(i).size();

		if(nr > max)
		{
			max = nr;
			pos = i;
		}
	}

	std::map<std::tuple<int, double>, double>::iterator iterator;
	std::map<std::tuple<int, double>, double> highestmap = _precisionRecallValues.at(pos);

	//write header
	for (iterator = highestmap.begin(); iterator != highestmap.end(); ++iterator)
	{
		std::stringstream t1;
		t1 << std::get<1>((*iterator).first) << ";";
		header += t1.str();
	}

	for (int i = 0; i < _precisionRecallValues.size(); i++)
	{

		std::map<std::tuple<int, double>, double>::iterator it;
		std::map<std::tuple<int, double>, double> map = _precisionRecallValues.at(i);

		int counter = 0;

		for (it = map.begin(); it != map.end(); ++it)
		{
			////write header
			//if (i == 0)
			//{
			//	std::stringstream t1;
			//	t1 << std::get<1>((*it).first) << ";";
			//	header += t1.str();
			//}

			//write class into the first row
			if (counter == 0)
			{
				std::stringstream t2;
				t2 << std::get<0>((*it).first) << ";";
				line += t2.str();
				t2.str("");

			}

			std::stringstream t3;
			t3 << (*it).second;
			line += t3.str();
			line += ";";
			t3.str("");


			counter++;
		}


		std::stringstream t4;
		t4 << std::endl;
		line += t4.str();
		t4.str("");

	}

	outfile << header << std::endl;
	outfile << line;
	outfile.close();
	return true;
}

void defuse::Valuator::interpolateRecallTo11Steps(
	std::vector<std::map<std::tuple<int, double>, double>> _precisionRecall, 
	std::vector<std::map<std::tuple<int, double>, double>>& _interpolatedPrecisionRecall)
{
	std::vector<std::tuple<int, std::map<double, double>>> recallAt11Steps;
	
	std::map<double, double> recallLevels;
	for (double recallStep = 0.0; recallStep <= 1.0; recallStep+=0.1)
	{
		recallLevels[recallStep] = 0.0;
	}

	std::map<std::tuple<int, double>, double> tmp;
	std::vector < std::map<std::tuple<int, double>, double>> tmp2;
	for (int i = 0; i < _precisionRecall.size(); i++)
	{
		std::map<double, double>::iterator itRecallLevels;
		std::map<std::tuple<int, double>, double> tmp;
		int gid = 0;
		for (itRecallLevels = recallLevels.begin(); itRecallLevels != recallLevels.end(); ++itRecallLevels)
		{
			//get desired recallLevel
			double recallLevel = (*itRecallLevels).first;

			double maxPrecision = 0.0;
			
			//find maximum precision value with recall higher than recallLevel
			std::map<std::tuple<int, double>, double>::iterator it;
			std::map<std::tuple<int, double>, double> map = _precisionRecall.at(i);
			for (it = map.begin(); it != map.end(); ++it)
			{
				double recall = std::get<1>((*it).first);

				gid = std::get<0>((*it).first);

				int recallInt = int((std::floor(recall * 10 + 0.5) / 10.0) * 100);
				int recallLevelInt = int((std::floor(recallLevel * 10 + 0.5) / 10.0) * 100);

				if (recallInt >= recallLevelInt)
				{
					double precision = (*it).second;
					if(precision > maxPrecision)
					{
						maxPrecision = precision;
					}
				}
			}
			//recallLevels[recallLevel] = maxPrecision;
			tmp[std::make_tuple(gid, recallLevel)] = maxPrecision;
		}

		//recallAt11Steps.push_back(std::make_tuple(gid,recallLevels));
		tmp2.push_back(tmp);
	}

	_interpolatedPrecisionRecall.assign(tmp2.begin(), tmp2.end());
}

void defuse::Valuator::writeCompTimeValues(File* _file, std::vector<std::pair<int, double>> _compTimeValues)
{
	std::ofstream outfile;
	outfile.open(_file->getFile(), std::ofstream::out | std::ofstream::app);

	if (!outfile.is_open())
	{
		LOG_ERROR("Error: Cannot open CSV File: " << _file->getFile());
	}

	for(int i = 0; i < _compTimeValues.size(); i++)
	{
		outfile << _compTimeValues.at(i).first << ", " << _compTimeValues.at(i).second << "\n";
	}

	outfile.close();
}

std::unique_lock<std::mutex> defuse::Valuator::locking()
{
	std::unique_lock<std::mutex> guard(mLock);
	return std::move(guard);
}

void defuse::Valuator::showProgress(std::string label, int _step, int _total) const
{
	int barWidth = 70;
	float progress = float(_step / float(_total));

	barWidth = barWidth - label.size();

	std::cout << label << " [";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
}
