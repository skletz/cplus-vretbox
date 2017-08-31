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
* @version 1.0 24/05/17
*
**/

#include "../include/vretbox.hpp"

#include <cpluslogger.hpp>
#include <cplusutil.hpp>

#include <boost/program_options.hpp>
#include "xtraction.hpp"
#include "valuation.hpp"


static std::string PROGNAME = "VRETBOX";
static std::string PROGDESCRIPTION= "Video Retrieval Evaluation Toolbox";

boost::program_options::variables_map processProgramOptions(const int argc, const char *const argv[]);
void showDescription();
void showHelp(const boost::program_options::options_description& desc);

int main(int argc, char const *argv[]) {

	showDescription();
	boost::program_options::variables_map args;
	try
	{
		args = processProgramOptions(argc, argv);
	}
	catch (std::exception& e)
	{
		LOG_ERROR(PROGNAME << " Error: Programm options cannot be used." << " Exception: " << e.what());
	}

	vretbox::ToolBase* tool = nullptr;

	if (args["tool"].as< std::string >() == "xtraction")
	{
		tool = new vretbox::VRETBOXXtraction();

	}
	else if (args["tool"].as< std::string >() == "valuation")
	{
		tool = new vretbox::VRETBOXValuation();
	}
	else
	{
		LOG_FATAL(PROGNAME << " Error: Tool "<< args["tool"].as< std::string >() << " is not defined.");
	}

	if(tool != nullptr && tool->init(args))
	{
		//try
		//{
			tool->run();
		//}
		//catch (std::exception& e)
		//{
		//	LOG_FATAL(args["tool"].as< std::string >() << " Error: File cannot be handled: " << args["infile"].as< std::string >() << " Exception: " << e.what());
		//}

	}else
	{
		LOG_ERROR(args["tool"].as< std::string >() << " Error: Tool initialization failed.");
	}
		

	try
	{
		delete tool;
	}catch(std::exception& e)
	{
		LOG_ERROR(args["tool"].as< std::string >() << " Error: Deleting tool failed." << " Exception: " << e.what());
	}
	
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
	return EXIT_SUCCESS;
}

void showDescription()
{
	LOG_INFO("* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *");
	LOG_INFO("**** " << PROGNAME << " PROGRAM " << "(" << PROGDESCRIPTION << ")" << " - " << "VERSION 1.0 -" <<  " 2017 " << "****" );
}

void showHelp(const boost::program_options::options_description& desc)
{
	LOG_INFO(desc);
}

boost::program_options::variables_map processProgramOptions(const int argc, const char *const argv[])
{
	boost::program_options::options_description generic("Generic options");
	generic.add_options()
		("help,h", "Print options")
		("tool,t", boost::program_options::value<std::string>()->default_value("extraction"), "The tool to be used")
		("config", boost::program_options::value<std::string>()->default_value("default.ini"), "Configuration file for the settings to be used")
		("infile,i", boost::program_options::value<std::string>(), "Input file")
		("indir", boost::program_options::value<std::string>(), "Input directory")
		("outfile,o", boost::program_options::value<std::string >(), "Output file")
		("outdir", boost::program_options::value<std::string >(), "Output directory")
		("display", boost::program_options::value<bool>()->default_value(false),"display intermediate steps")
		("autosave", boost::program_options::value<bool>()->default_value(false),"save displayed images")
		("maxThreads", boost::program_options::value<int>()->default_value(5), "how many threads to run concurrently")
		;

	//All possible options that will be allowed in config file for the extraction tool
	boost::program_options::options_description config("Configuration");
	config.add_options()
		("General.descriptor", boost::program_options::value<std::string>()->default_value("ffs"),
			"which features should be extracted")
		("General.measurements", boost::program_options::value<std::string>(),
			"in which file should times are stored")
		("General.queryfile", boost::program_options::value<std::string>(),
			"which queries should be evaluated")
		("General.atK", boost::program_options::value<int>()->default_value(0),
			"which max-position should be included into the results; 0=all")

		("Cfg.dfs.maxFrames", boost::program_options::value<int>()->default_value(5), 
			"how many frames should be used")
		("Cfg.dfs.frameSelection", boost::program_options::value<std::string>()->default_value("FramesPerVideo"), 
			"how should the frames selected: FramesPerVideo, FramesPerSecond")
		("Cfg.dfs.resetTracking", boost::program_options::value<bool>()->default_value(true), 
			"should the tracking samplepoints newly initialized for each frame")

		("Cfg.fs.keyframeSelection", boost::program_options::value<std::string>()->default_value("MiddleFrame"),
			"which frame should be selected: MiddleFrame")
		("Cfg.fs.initSeeds", boost::program_options::value<int>()->default_value(100), 
			"how many samplepoints should be clustered")
		("Cfg.fs.initialCentroids", boost::program_options::value<int>()->default_value(10), 
			"how many clusters should maximal created")

		("Cfg.fs.iterations", boost::program_options::value<int>()->default_value(5), 
			"how many iteration should be clustered")
		("Cfg.fs.minClusterSize", boost::program_options::value<int>()->default_value(2), 
			"what is the minimum number of samples per cluster")
		("Cfg.fs.minDistance", boost::program_options::value<float>()->default_value(0.01f), 
			"what is the minimal joining distance of clusters")
		("Cfg.fs.dropThreshold", boost::program_options::value<float>()->default_value(0.0), 
			"what is the number of samples per cluster to drop it")
		
		("Cfg.fs.samplepointdir", boost::program_options::value<std::string>(), 
			"directory where the samplepoints are stored")
		("Cfg.fs.samplepointfileformat", boost::program_options::value<std::string>(),
			"fileformat of the samplepoints")
		("Cfg.fs.distribution", boost::program_options::value<std::string>()->default_value("regular"), 
			"what is the distribution: random, regular")

		("Cfg.fs.overrideSamples", boost::program_options::value<bool>()->default_value(false),
			"override stored sample points")

		("Cfg.fs.mean", boost::program_options::value<float>()->default_value(0.0),
			"mean value for gaussian distribution")
		("Cfg.fs.stddev", boost::program_options::value<float>()->default_value(1.0),
			"standard deviation value for gaussian distribution")

		("Cfg.fs.grayscaleBits", boost::program_options::value<int>()->default_value(5),
			"grayscaleBits")
		("Cfg.fs.windowRadius", boost::program_options::value<int>()->default_value(4),
			"windowRadius")

		("Cfg.fs.lpdistance", boost::program_options::value<float>()->default_value(1.0),
			"p value for minkowski distance")

		("General.distance", boost::program_options::value<std::string>()->default_value("smd"),
			"which distance should be used")

		("General.grounddistance", boost::program_options::value<std::string>()->default_value("L2"),
			"which groundistance should be used with in connection with smd")

		("Cfg.smd.matchingstrategy", boost::program_options::value<std::string>()->default_value("nearest-neighbor"),
			"which matchingstrategy should be used in smd")
		("Cfg.smd.direction", boost::program_options::value<std::string>()->default_value("asymmetric-query"),
			"which direction should be calculated in smd")
		("Cfg.smd.costfunction", boost::program_options::value<std::string>()->default_value("weighted-distance"),
			"which costfunction should be used in smd")
		("Cfg.smd.lambda", boost::program_options::value<float>()->default_value(1.0),
			"which value of lambda should be used with smd (only neceassary with bidirectional matching strategy)")
		
		("Cfg.sqfd.alpha", boost::program_options::value<float>()->default_value(1.0),
			"which value of alpha should be used with sqfd")
		("Cfg.sqfd.similarity", boost::program_options::value<std::string>()->default_value("heuristic"),
			"which similarity should be used in sqfd")
		;


	boost::program_options::positional_options_description positional;
	positional.add("outfile", -1);

	boost::program_options::options_description cmdlineOptions;
	cmdlineOptions.add(generic).add(config);

	boost::program_options::options_description configfileOptions;
	configfileOptions.add(config);

	boost::program_options::options_description visible("Allowed options");
	visible.add(generic).add(config);

	if (argc < 2)
	{
		LOG_ERROR("For the execution of " << PROGNAME << " there are too few command line arguments");
		showHelp(visible);
		exit(EXIT_SUCCESS);
	}

	boost::program_options::variables_map args;
	boost::program_options::variables_map configfileArgs;

	try
	{
		store(boost::program_options::command_line_parser(argc, argv).options(cmdlineOptions).positional(positional).run(), args);
	}
	catch (boost::program_options::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(visible);
		exit(EXIT_FAILURE);
	}

	std::ifstream ifs(args["config"].as< std::string >());
	if (!ifs.is_open())
	{
		LOG_ERROR("Configuration file  " << args["config"].as< std::string >() << " cannot be found");
		showHelp(visible);
		exit(EXIT_SUCCESS);
	}

	try
	{
		store(parse_config_file(ifs, visible), args);
	}
	catch (boost::program_options::error const& e)
	{
		LOG_ERROR(e.what());
		showHelp(visible);
		exit(EXIT_FAILURE);
	}

	notify(args);


	if (args.count("help")) {
		showHelp(visible);
	}

	return args;
}
