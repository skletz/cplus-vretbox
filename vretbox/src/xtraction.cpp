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

#include "xtraction.hpp"

vretbox::VRETBOXXtraction::VRETBOXXtraction()
	: mXtractor(nullptr), mVideo(nullptr), mFeatures(nullptr),
	  mXtractionTimes(nullptr), mSamplepoints(nullptr), mOutpudir(nullptr), 
	  mDisplay(false), mSaveDisplay(false)
{
	mArgs = nullptr;
}

bool vretbox::VRETBOXXtraction::init(boost::program_options::variables_map _args)
{
	mArgs = _args;
	bool areArgsValid = true;

	mVideo = new File(mArgs["infile"].as< std::string >());
	mFeatures = new File(mArgs["outfile"].as< std::string >());
	mXtractionTimes = new File(mArgs["General.measurements"].as< std::string >());
	mOutpudir = new Directory(mArgs["outdir"].as< std::string >());
	mDisplay = mArgs["display"].as< bool >();
	mSaveDisplay = mArgs["autosave"].as< bool >();

	std::string desc = mArgs["General.descriptor"].as< std::string >();
	bool samplepoints = false;
	if (desc == "sfs" || desc == "ffs" || desc == "tfs")
	{
		areArgsValid = initStaticSignatures();
		samplepoints = true;
	}

	if (desc == "ffs")
	{
		areArgsValid = initFlowbasedSignatures();
		samplepoints = true;
	}

	if (desc == "como")
	{
		areArgsValid = initComoDescriptor();
		samplepoints = false;
	}

	if (desc == "brisk")
	{
		areArgsValid = initBRISKDescriptor();
		samplepoints = false;
	}

	if (desc == "hog")
	{
		areArgsValid = initHOGDescriptor();
		samplepoints = false;
	}

	if (desc == "ceed")
	{
		areArgsValid = initCEEDDescriptor();
		samplepoints = false;
	}

	//Add modelname to all output destinations
	std::string xtractorID = mXtractor->getXtractorID();

	std::string modelname = "";
	if(samplepoints)
	{
		std::string samplepointID = mSamplepoints->getFilename();
		modelname = xtractorID + "-" + samplepointID;
	}else
	{
		modelname = xtractorID;
	}

	mXtractionTimes->extendFileName(modelname);
	mFeatures->addDirectoryToPath(modelname);

	if(mXtractor->mDisaply || mXtractor->mSaveDisplay)
		mXtractor->mOutput->addDirectory(modelname);

	LOG_INFO("**** " << "Retrieval Feature Extraction Tool " << "**** ");
	LOG_INFO("**** " << "Settings");
	LOG_INFO("**** " << mXtractor->toString());
	LOG_INFO("**** " << ((samplepoints) ? mSamplepoints->getFilename() : ""));
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
	LOG_INFO("**** " << "Video: " << mVideo->getFile());
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initStaticSignatures()
{
	bool areArgsValid = true;

	//Common parameters for feature signatures
	defuse::SIGXtractor::KeyFrameSelection keyframeSelection;
	defuse::SamplePoints::Distribution distribution;

	int initSeeds = mArgs["Cfg.fs.initSeeds"].as<int>();
	int initialCentroids = mArgs["Cfg.fs.initialCentroids"].as<int>();
	int iterations = mArgs["Cfg.fs.iterations"].as<int>();
	int minClusterSize = mArgs["Cfg.fs.minClusterSize"].as<int>();
	float minDistance = mArgs["Cfg.fs.minDistance"].as<float>();
	float dropThreshold = mArgs["Cfg.fs.dropThreshold"].as<float>();
	int grayscaleBits = mArgs["Cfg.fs.grayscaleBits"].as<int>();
	int windowRadius = mArgs["Cfg.fs.windowRadius"].as<int>();
	float lpDistance = mArgs["Cfg.fs.lpdistance"].as<float>();
	float mean = mArgs["Cfg.fs.mean"].as<float>();
	float stddev = mArgs["Cfg.fs.stddev"].as<float>();

	bool overrideSamples = mArgs["Cfg.fs.overrideSamples"].as< bool >();

	//Process sampling
	if (mArgs["Cfg.fs.distribution"].as<std::string>() == "random")
	{
		distribution = defuse::SamplePoints::Distribution::RANDOM;
	}
	else if (mArgs["Cfg.fs.distribution"].as<std::string>() == "regular")
	{
		distribution = defuse::SamplePoints::Distribution::REGULAR;
	}
	else if (mArgs["Cfg.fs.distribution"].as<std::string>() == "gaussian")
	{
		distribution = defuse::SamplePoints::Distribution::GAUSSIAN;
	}
	else
	{
		distribution = defuse::SamplePoints::Distribution::REGULAR;
		LOG_FATAL("Cfg.fs.distribution " << mArgs["Cfg.ffs.distribution"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	//Process samplepoints
	std::string samplepointdir = mArgs["Cfg.fs.samplepointdir"].as<std::string>();
	std::string samplepointfileformat = mArgs["Cfg.fs.samplepointfileformat"].as<std::string>();

	std::vector<cv::Point2f> samplepoints;
	bool deserializationSuccessful = false;
	if(samplepointdir != "")
	{
		deserializationSuccessful = deserializeSamplepoints(samplepointdir, samplepointfileformat, initSeeds, distribution, mean, stddev, samplepoints);
	}

	if(overrideSamples || (!deserializationSuccessful && samplepointdir != ""))
	{
		initSamplepoints(initSeeds, distribution, mean, stddev, samplepoints);
		serializeSamplepoints(samplepointdir, samplepointfileformat, distribution, mean, stddev, samplepoints);
	}

	if (samplepointdir == "")
	{
		initSamplepoints(initSeeds, distribution, mean, stddev, samplepoints);
	}


	//Init xtractor
	mXtractor = new defuse::SIGXtractor(samplepoints, initSeeds, initialCentroids, lpDistance);
	mXtractor->mDisaply = mDisplay;
	mXtractor->mSaveDisplay = mSaveDisplay;
	mXtractor->mOutput = mOutpudir;

	static_cast<defuse::SIGXtractor *>(mXtractor)->mIterations = iterations;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mMinimalClusterSize = minClusterSize;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mGrayscaleBits = grayscaleBits;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mWindowRadius = windowRadius;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mMinimalDistance = minDistance;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mClusterDropThreshold = dropThreshold;

	//how to choose the keyframe for static feature signatures
	if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "MiddleFrame")
	{
		keyframeSelection = defuse::SIGXtractor::KeyFrameSelection::MiddleFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "FirstFrame")
	{
		keyframeSelection = defuse::SIGXtractor::KeyFrameSelection::FirstFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "LastFrame")
	{
		keyframeSelection = defuse::SIGXtractor::KeyFrameSelection::LastFrame;
	}
	else
	{
		keyframeSelection = defuse::SIGXtractor::KeyFrameSelection::MiddleFrame;
		LOG_FATAL("Cfg.static.keyframeSelection " << mArgs["Cfg.static.keyframeSelection"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	static_cast<defuse::SIGXtractor *>(mXtractor)->mKeyFrameSelection = keyframeSelection;

	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initFlowbasedSignatures()
{
	bool areArgsValid = true;

	int maxFrames = mArgs["Cfg.dfs.maxFrames"].as<int>();
	bool resetTracking = mArgs["Cfg.dfs.resetTracking"].as<bool>();
	defuse::FlowDySIGXtractor::FrameSelection frameSelection;

	defuse::SIGXtractor* xtractor = static_cast<defuse::SIGXtractor *>(mXtractor);
	mXtractor = new defuse::FlowDySIGXtractor(xtractor->mSamplepoints);
	mXtractor->mDisaply = xtractor->mDisaply;
	mXtractor->mSaveDisplay = xtractor->mSaveDisplay;
	mXtractor->mOutput = xtractor->mOutput;

	static_cast<defuse::SIGXtractor *>(mXtractor)->mInitSeeds = xtractor->mInitSeeds;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mMaxClusters = xtractor->mMaxClusters;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mDistance = xtractor->mDistance;

	static_cast<defuse::SIGXtractor *>(mXtractor)->setLpDistance(xtractor->getLpDistance());

	static_cast<defuse::SIGXtractor *>(mXtractor)->mIterations = xtractor->mIterations;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mMinimalClusterSize = xtractor->mMinimalClusterSize;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mGrayscaleBits = xtractor->mGrayscaleBits;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mWindowRadius = xtractor->mWindowRadius;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mMinimalDistance = xtractor->mMinimalDistance;
	static_cast<defuse::SIGXtractor *>(mXtractor)->mClusterDropThreshold = xtractor->mClusterDropThreshold;

	static_cast<defuse::FlowDySIGXtractor *>(mXtractor)->mMaxFrames = maxFrames;
	static_cast<defuse::FlowDySIGXtractor *>(mXtractor)->mResetTracking = resetTracking;

	if (mArgs["Cfg.dfs.frameSelection"].as<std::string>() == "FramesPerVideo")
	{
		frameSelection = defuse::FlowDySIGXtractor::FrameSelection::FramesPerVideo;
	}
	else if (mArgs["Cfg.dfs.frameSelection"].as<std::string>() == "FramesPerSecond")
	{
		frameSelection = defuse::FlowDySIGXtractor::FrameSelection::FramesPerSecond;
	}
	else if (mArgs["Cfg.dfs.frameSelection"].as<std::string>() == "All")
	{
		frameSelection = defuse::FlowDySIGXtractor::FrameSelection::All;
	}
	else
	{
		frameSelection = defuse::FlowDySIGXtractor::FrameSelection::All;
		LOG_FATAL("Cfg.dfs.frameSelection " << mArgs["Cfg.dfs.frameSelection"].as<std::string>() << " is not defined");
		areArgsValid = false;
	}
	static_cast<defuse::FlowDySIGXtractor *>(mXtractor)->mFrameSelection = frameSelection;
	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initComoDescriptor()
{
	bool areArgsValid = true;
	//Init xtractor
	mXtractor = new defuse::COMOXtractor();
	mXtractor->mDisaply = mDisplay;
	mXtractor->mSaveDisplay = mSaveDisplay;
	mXtractor->mOutput = mOutpudir;

	defuse::COMOXtractor::KeyFrameSelection keyframeSelection;

	//how to choose the keyframe for static feature signatures
	if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "MiddleFrame")
	{
		keyframeSelection = defuse::COMOXtractor::KeyFrameSelection::MiddleFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "FirstFrame")
	{
		keyframeSelection = defuse::COMOXtractor::KeyFrameSelection::FirstFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "LastFrame")
	{
		keyframeSelection = defuse::COMOXtractor::KeyFrameSelection::LastFrame;
	}
	else
	{
		keyframeSelection = defuse::COMOXtractor::KeyFrameSelection::MiddleFrame;
		LOG_FATAL("Cfg.static.keyframeSelection " << mArgs["Cfg.static.keyframeSelection"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initBRISKDescriptor()
{
	bool areArgsValid = true;
	//Init xtractor
	mXtractor = new defuse::BRISKXtractor();
	mXtractor->mDisaply = mDisplay;
	mXtractor->mSaveDisplay = mSaveDisplay;
	mXtractor->mOutput = mOutpudir;

	defuse::BRISKXtractor::KeyFrameSelection keyframeSelection;

	//how to choose the keyframe for static feature signatures
	if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "MiddleFrame")
	{
		keyframeSelection = defuse::BRISKXtractor::KeyFrameSelection::MiddleFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "FirstFrame")
	{
		keyframeSelection = defuse::BRISKXtractor::KeyFrameSelection::FirstFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "LastFrame")
	{
		keyframeSelection = defuse::BRISKXtractor::KeyFrameSelection::LastFrame;
	}
	else
	{
		keyframeSelection = defuse::BRISKXtractor::KeyFrameSelection::MiddleFrame;
		LOG_FATAL("Cfg.static.keyframeSelection " << mArgs["Cfg.static.keyframeSelection"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initHOGDescriptor()
{
	bool areArgsValid = true;
	//Init xtractor
	mXtractor = new defuse::HOGXtractor();
	mXtractor->mDisaply = mDisplay;
	mXtractor->mSaveDisplay = mSaveDisplay;
	mXtractor->mOutput = mOutpudir;

	defuse::HOGXtractor::KeyFrameSelection keyframeSelection;

	//how to choose the keyframe for static feature signatures
	if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "MiddleFrame")
	{
		keyframeSelection = defuse::HOGXtractor::KeyFrameSelection::MiddleFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "FirstFrame")
	{
		keyframeSelection = defuse::HOGXtractor::KeyFrameSelection::FirstFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "LastFrame")
	{
		keyframeSelection = defuse::HOGXtractor::KeyFrameSelection::LastFrame;
	}
	else
	{
		keyframeSelection = defuse::HOGXtractor::KeyFrameSelection::MiddleFrame;
		LOG_FATAL("Cfg.static.keyframeSelection " << mArgs["Cfg.static.keyframeSelection"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	return areArgsValid;
}

bool vretbox::VRETBOXXtraction::initCEEDDescriptor()
{
	bool areArgsValid = true;
	//Init xtractor
	mXtractor = new defuse::CEEDXtractor();
	mXtractor->mDisaply = mDisplay;
	mXtractor->mSaveDisplay = mSaveDisplay;
	mXtractor->mOutput = mOutpudir;

	defuse::CEEDXtractor::KeyFrameSelection keyframeSelection;

	//how to choose the keyframe for static feature signatures
	if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "MiddleFrame")
	{
		keyframeSelection = defuse::CEEDXtractor::KeyFrameSelection::MiddleFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "FirstFrame")
	{
		keyframeSelection = defuse::CEEDXtractor::KeyFrameSelection::FirstFrame;
	}
	else if (mArgs["Cfg.static.keyframeSelection"].as<std::string>() == "LastFrame")
	{
		keyframeSelection = defuse::CEEDXtractor::KeyFrameSelection::LastFrame;
	}
	else
	{
		keyframeSelection = defuse::CEEDXtractor::KeyFrameSelection::MiddleFrame;
		LOG_FATAL("Cfg.static.keyframeSelection " << mArgs["Cfg.static.keyframeSelection"].as< std::string >() << " is not defined");
		areArgsValid = false;
	}

	return areArgsValid;
}

void vretbox::VRETBOXXtraction::initSamplepoints(int initSeeds, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f>& samplepoints)
{
	defuse::SamplePoints* samples = new defuse::SamplePoints(initSeeds, distribution, mean, stddev, true);
	//return result
	samplepoints = samples->getPoints();
	File* file = new File(samples->getSamplePointFileName());
	mSamplepoints = file;

}

void vretbox::VRETBOXXtraction::serializeSamplepoints(std::string samplepointdir, std::string fileextension, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f> samplepoints)
{
	Directory samplepointdirectory(samplepointdir);
	defuse::SamplePoints* samples = new defuse::SamplePoints(samplepoints, distribution, mean, stddev);
	File* sampleFile = new File(samplepointdirectory.getPath(), samples->getSamplePointFileName(), fileextension);

	//return results
	samples->serialize(sampleFile->getFile());
	mSamplepoints = sampleFile;
}

bool vretbox::VRETBOXXtraction::deserializeSamplepoints(std::string samplepointdir, std::string fileextension, int initSeeds, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f>& samplepoints)
{
	//return status, if the samplepoints cannot be deserialized return false 
	bool creationSuccessful = true;

	Directory samplepointdirectory(samplepointdir);
	defuse::SamplePoints* samples = new defuse::SamplePoints(initSeeds, distribution, mean, stddev, false);
	File* sampleFile = new File(samplepointdirectory.getPath(), samples->getSamplePointFileName(), fileextension);

	bool status = false;
	bool fileExists = cplusutil::FileIO::isValidPathToFile(sampleFile->getFile());

	int deserializedSize = 0;
	if(fileExists)
	{
		status = samples->deserialize(sampleFile->getFile());
		deserializedSize = int(samples->getPoints().size());

	}
	
	//if the file does not exist, or the file canno be deserialized or the desirelized is not the same as initSeeds
	if (!fileExists || !status || initSeeds != deserializedSize)
	{
		LOG_FATAL("Fatal error: Sample count: " << initSeeds << " do not match with desirelized samplepoints: " << deserializedSize);
		creationSuccessful = false;
	}

	samplepoints = samples->getPoints();

	mSamplepoints = sampleFile;
	return creationSuccessful;
}

void vretbox::VRETBOXXtraction::run()
{
	defuse::VideoBase* segment = new defuse::VideoBase(mVideo);

	defuse::FeaturesBase* features = mXtractor->xtract(segment);
	LOG_INFO("**** " << "Save Features");
	LOG_INFO("**** " << "Features File: " << mFeatures->getFile());
	//Save extraction time
	std::ofstream of(mXtractionTimes->getFile(), std::ofstream::out | std::ofstream::app);
	of << segment->mFile->getFilename() << ", " << segment->mFile->getFileExtension() << ", " << features->mExtractionTime << "\n";
	
	//Save features
	features->serialize(mFeatures->getFile());
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");

	delete segment;
}

vretbox::VRETBOXXtraction::~VRETBOXXtraction()
{
	delete mXtractor;
	delete mVideo;
	delete mFeatures;
	delete mXtractionTimes;
	delete mSamplepoints;
	delete mOutpudir;
}


