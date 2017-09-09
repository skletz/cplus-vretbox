/** VRETBOX (Version 1.0) ******************************
* ******************************************************
*       _    _      ()_()
*      | |  | |    |(o o)
*   ___| | _| | ooO--`o'--Ooo
*  / __| |/ / |/ _ \ __|_  /
*  \__ \   <| |  __/ |_ / /
*  |___/_|\_\_|\___|\__/___|
*
* ******************************************************
* Purpose:
* Input/Output:

* @author skletz
* @version 1.0 26/07/17
*
**/

//#include <boost/asio.hpp>
#include "valuation.hpp"
#include <unordered_map>
#include <boost/thread/thread.hpp>
#include "threadpool.hpp"


vretbox::VRETBOXValuation::VRETBOXValuation()
	: mValuationTimes(nullptr), mDistance(nullptr),
	  mXtractor(nullptr), mValuator(nullptr),
	  mInput(nullptr), mIndex(nullptr), mQueryIndex(nullptr),
	  mOutputMAPFile(nullptr), mAtK(0), mMaxThreads(0), mOutput(nullptr), mRandom(false)
{
	mArgs = nullptr;
}

bool vretbox::VRETBOXValuation::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool areArgsValid = true;

	mIndex = new File(mArgs["infile"].as< std::string >());
	mInput = new Directory(mArgs["indir"].as< std::string >());
	mQueryIndex = new File(mArgs["General.queryfile"].as< std::string >());

	mOutput = new Directory(mArgs["outdir"].as< std::string >());

	mOutputMAPFile = new File(mArgs["outfile"].as< std::string >());
	mValuationTimes = new File(mArgs["General.measurements"].as< std::string >());
	
	mAtK = mArgs["General.atK"].as< int >();

	mMaxThreads = mArgs["maxThreads"].as< int >();

	mRandom = mArgs["General.random"].as< bool >();

	if (mArgs["General.distance"].as< std::string >() == "smd")
	{
		mDistance = initSMDistance();
	}
	else if (mArgs["General.distance"].as< std::string >() == "sqfd")
	{
		mDistance = initSQFDistance();
	}
	else if (mArgs["General.distance"].as< std::string >() == "lp")
	{
		mDistance = initMinkowskiDistance();
	}
	else if (mArgs["General.distance"].as< std::string >() == "hamming")
	{
		mDistance = initHammingDistance();
	}
	else
	{
		LOG_FATAL("Distance " << mArgs["General.distance"].as< std::string >() << " is not defined");
		mDistance = nullptr;
	}

	if (mDistance == nullptr)
		areArgsValid = false;

	std::unordered_map<std::string, int> modelIndex = initIndex(mIndex);
	LOG_INFO("Model index initialized ... " << modelIndex.size());
	std::unordered_map<std::string, int> queryIndex = initIndex(mQueryIndex);
	LOG_INFO("Query index initialized ... " << queryIndex.size());

	LOG_INFO("Load model ... ");
	initModel(modelIndex);
	LOG_INFO("Model loaded ... " << mModel.size());

	LOG_INFO("Load queries ... ");
	initQueries(queryIndex);
	LOG_INFO("Queries loaded ... " << mQueries.size());

	if(modelIndex.empty() || queryIndex.empty())
	{
		LOG_ERROR("Error: No model or queries are found ...");
		areArgsValid = false;
	}
		

	if (mAtK == 0)
	{
		mAtK = int(mModel.size());
		LOG_ERROR("Evaluation input at k=0 => k@" << mAtK);
	}else
	{
		LOG_ERROR("Evaluation input k@" << mAtK);
	}
		

	std::string modelname = mInput->directories.at(mInput->directories.size() - 1);
	std::string distanceID = mDistance->getDistanceID();

	if(mRandom)
	{
		modelname = modelname + "-" + "RANDOM";
	}else
	{
		modelname = modelname + "-" + distanceID;
	}

	//Init output
	//Extend outputdirectory with modelname
	mOutput->addDirectory(modelname);
	mValuationTimes->extendFileName(modelname);
	mValuator = new defuse::Valuator();
	mValuator->mModelname = modelname;

	LOG_INFO("**** " << "Retrieval Feature Evaluation Tool " << "**** ");
	LOG_INFO("**** " << "Settings");
	LOG_INFO("**** " << mDistance->toString());
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

	return areArgsValid;
}

void vretbox::VRETBOXValuation::run()
{

	ThreadPool* pool = new ThreadPool(mMaxThreads);

	int elementcnt = 0;

	LOG_INFO("Evaluate " << mQueries.size() << "  groups ...");
	for (auto iQueryGroup = mQueries.begin(); iQueryGroup != mQueries.end(); ++iQueryGroup)
	{
		int group = (*iQueryGroup).first;
		int groupSize = int(((*iQueryGroup).second).size());
		
		std::unique_lock<std::mutex> guard(locking());
		LOG_INFO("Group " << group << " Size: " << groupSize);
		guard.unlock();

		std::string groupnr = std::to_string((*iQueryGroup).first);

		elementcnt += groupSize;
		
		//mValuator->computeMAPAtK(mAtK, group, (*iQueryGroup).second, mDistance, mModel);

		if(mRandom)
		{
			pool->addTask(boost::bind(&defuse::Valuator::computeRandomMAPs, mValuator, group, (*iQueryGroup).second, mDistance, mModel));
		}else
		{
			pool->addTask(boost::bind(&defuse::Valuator::computeMAPAtK, mValuator, mAtK, group, (*iQueryGroup).second, mDistance, mModel));

			//mValuator->computeMAPAtK(mAtK, group, (*iQueryGroup).second, mDistance, mModel);
		}

	
	}
	pool->createAndJoinThreads();

	LOG_INFO("Save MAP values ...");

	//Save evaluation values
	std::string pos = std::to_string(mAtK);
	//Copy the template of MAP values storage to store P,R@K in a separte file
	File* pAtkFile = new File(mOutputMAPFile->getFile());
	pAtkFile->extendFileName("atK");
	if(!cplusutil::FileIO::isValidPathToFile(pAtkFile->getFile()))
	{
		cplusutil::FileIO::copyFiles(mOutputMAPFile->getFile(), pAtkFile->getFile());
	}

	mValuator->writeValuesToCSVTemplate(mOutputMAPFile, "MAP@"+ pos, mValuator->mMaps);
	mValuator->writeValuesToCSVTemplate(pAtkFile, "P@" + pos, mValuator->mPrecisionAtK);
	mValuator->writeValuesToCSVTemplate(pAtkFile, "R@" + pos, mValuator->mRecallAtK);
	mValuator->writeValuesToCSVTemplate(pAtkFile, "AvgP@" + pos, mValuator->mAvgPrecisionAtK);
	
	LOG_INFO("Save interpolated precision recall values ...");
	std::vector<std::map<std::tuple<int, double>, double>> interpolated;
	mValuator->interpolateRecallTo11Steps(mValuator->mPrecisionRecallCurveValues, interpolated);

	File* interpolatedPrecFile = new File(mOutputMAPFile->getFile());
	interpolatedPrecFile->extendFileName("at"+pos+"-prec11interpol");
	interpolatedPrecFile->addDirectoryToPath(mValuator->mModelname);
	mValuator->writePrecesionRecallValues(interpolatedPrecFile, interpolated);

	LOG_INFO("Save precision recall values ...");
	File* precValues = new File(mOutputMAPFile->getFile());
	precValues->extendFileName("at" + pos + "-prec");
	precValues->addDirectoryToPath(mValuator->mModelname);
	mValuator->writePrecesionRecallValues(precValues, mValuator->mPrecisionRecallCurveValues);

	if(!mRandom)
	{
		mValuator->writeCompTimeValues(mValuationTimes, mValuator->mCompDistanceTimes);
	}

}

defuse::SMD* vretbox::VRETBOXValuation::initSMDistance() const
{
	defuse::SMD* smd;

	defuse::Minkowski* gddistance = initMinkowskiDistance();

	if(gddistance == nullptr)
		smd = nullptr;

	int direction;


	if (mArgs["Cfg.smd.direction"].as<std::string>() == "bidirectional")
	{
		direction = 0;
	}
	else if (mArgs["Cfg.smd.direction"].as<std::string>() == "asymmetric-query")
	{
		direction = 1;
	}
	else if (mArgs["Cfg.smd.direction"].as<std::string>() == "asymmetric-database")
	{
		direction = 2;
	}
	else
	{
		direction = 1;
		LOG_FATAL("Cfg.smd.direction " << mArgs["Cfg.smd.direction"].as< std::string >() << " is not defined");
		smd = nullptr;
	}


	float lambda = mArgs["Cfg.smd.lambda"].as<float>();
	
	smd = new defuse::SMD(gddistance, direction, lambda);

	return smd;
}

defuse::SQFD* vretbox::VRETBOXValuation::initSQFDistance() const
{
	defuse::SQFD* sqfd;

	defuse::Minkowski* gddistance = initMinkowskiDistance();

	if (gddistance == nullptr)
		sqfd = nullptr;

	int similairty;


	if (mArgs["Cfg.sqfd.similarity"].as<std::string>() == "heuristic")
	{
		similairty = 0;
	}else
	{
		similairty = 1;
		LOG_FATAL("Cfg.sqfd.similarity " << mArgs["Cfg.sqfd.similarity"].as< std::string >() << " is not defined");
		sqfd = nullptr;
	}


	float alpha = mArgs["Cfg.sqfd.alpha"].as<float>();

	sqfd = new defuse::SQFD(gddistance, similairty, alpha);

	return sqfd;
}

defuse::Minkowski* vretbox::VRETBOXValuation::initMinkowskiDistance() const
{
	defuse::Minkowski* gddistance;
	int grounddistance;
	if (mArgs["General.grounddistance"].as<std::string>() == "L1")
	{
		grounddistance = 1;
	}
	else if (mArgs["General.grounddistance"].as<std::string>() == "L2")
	{
		grounddistance = 2;
	}
	else
	{
		grounddistance = 2;
		LOG_FATAL("General.grounddistance " << mArgs["General.grounddistance"].as< std::string >() << " is not defined");
		gddistance = nullptr;
	}

	gddistance = new defuse::Minkowski(grounddistance);
	return gddistance;
}

defuse::Hamming* vretbox::VRETBOXValuation::initHammingDistance() const
{
	return new defuse::Hamming();
}

std::unique_lock<std::mutex> vretbox::VRETBOXValuation::locking()
{
	std::unique_lock<std::mutex> guard(mLock);
	return std::move(guard);
}

void vretbox::VRETBOXValuation::initModel(std::unordered_map<std::string, int>& _index)
{
	std::string entrie = cplusutil::FileIO::getFirstFileFromDirectory(mInput->getPath());

	LOG_INFO("Feature files in the format of: " << entrie);

	std::unordered_map<int, std::vector<defuse::FeaturesBase*>> queries;
	File file(entrie);

	LOG_INFO("File converted in: " << file.getFile());
	int counter = 0;
	int total = _index.size();
	for(auto it = _index.begin(); it != _index.end(); ++it)
	{
		std::string filename = it->first;
		file.setFilename(filename);

		//LOG_INFO("Looking for: " << file.getFile());

		defuse::FeaturesBase* features = new defuse::FeaturesBase();
		features->deserialize(file.getFile());

		if (!features->mVectors.empty())
		{
			File f(file);
			features->mID = f.getFilename();
			features->mMatchingCriteria = _index[f.getFilename()];
			mModel.push_back(features);
			
		}else
		{
			LOG_ERROR("Fatal Error: Feature file cannot be deserialized: " << file.getFile());
		}

		showProgress(counter + 1, total);
		counter++;
	}
}

std::unordered_map<std::string, int> vretbox::VRETBOXValuation::initIndex(File* _file) const
{
	char tmp[256];
	std::string vid = "";
	int gid;
	std::unordered_map<std::string, int> index;

	//fetch all extracted features from the videos
	FILE *file = fopen(_file->getFile().c_str(), "r");

	if (file == NULL)
	{
		LOG_ERROR("Error: Index File cannot be opened: " << _file->getFile());
	}

	while (fscanf(file, "%d%*c%[^\n]%*c",&gid, &tmp) == 2)
	{
		
		vid = tmp;
		index[vid] = gid;
		//LOG_INFO("Vid: " << vid  << " Gid: " << gid);
	}
	fclose(file);

	return index;
}

void vretbox::VRETBOXValuation::initQueries(std::unordered_map<std::string, int>& _index)
{
	std::unordered_map<std::string, int> copy;
	copy = _index;

	int modelSize = int(mModel.size());
	for (int iModel = 0; iModel < modelSize; iModel++)
	{
		defuse::FeaturesBase* element = mModel.at(iModel);

		if(copy.find(element->mID) != copy.end())
		{
			mQueries[element->mMatchingCriteria].push_back(element);
		}

		showProgress(iModel + 1, modelSize);
	}
}


void vretbox::VRETBOXValuation::showProgress(int _step, int _total) const
{
	int barWidth = 70;
	float progress = float(_step / float(_total));

	std::cout << "[";
	int pos = barWidth * progress;
	for (int i = 0; i < barWidth; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] " << int(progress * 100.0) << " %\r";
	std::cout.flush();
}

vretbox::VRETBOXValuation::~VRETBOXValuation()
{
	delete mValuationTimes;
	//delete mDistance;
	delete mXtractor;
	delete mValuator;
	delete mInput;
	delete mIndex;
	delete mQueryIndex;
	delete mOutputMAPFile;
}