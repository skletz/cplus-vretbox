#include "samplepoints.hpp"
#include <cpluslogger.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <random>

#define _USE_MATH_DEFINES 
#include <math.h>
#include <iomanip>

defuse::SamplePoints::SamplePoints(int _samplesCnt, Distribution _distribution, float mean, float stddev, bool _initializeSamplepoints)
	:mMean(mean), mStddev(stddev)
{
	mSampleCnt = _samplesCnt;
	mDistribution = _distribution;

	if (_initializeSamplepoints)
	{
		initSamplePoints(_distribution, _samplesCnt);
	}

}

defuse::SamplePoints::SamplePoints(std::vector<cv::Point2f> _samplepoints, Distribution _distribution, float mean, float stddev)
{
	mPoints = _samplepoints;
	mDistribution = _distribution;
	mSampleCnt = int(mPoints.size());
	mMean = mean;
	mStddev = stddev;
}

void defuse::SamplePoints::initSamplePoints(Distribution distribution, int samplesCnt)
{
	mSampleCnt = samplesCnt;
	mDistribution = distribution;

	cv::RNG random;
	random.state = cv::getTickCount();
	mPoints.clear();
	mPoints.resize(samplesCnt);

	switch (distribution)
	{
	case RANDOM:
		for (int i = 0; i < samplesCnt; i++)
		{
			mPoints[i] = (cv::Point2f(random.uniform(float(0.0), float(1.0)), random.uniform(float(0.0), float(1.0))));
		}
		break;
		//ADDED option for regular choosen keypoints
	case REGULAR:
	{
		float result = float(sqrt(samplesCnt));
		for (int i = 0; i < int(result); i++)
		{
			for (int j = 0; j < int(result); j++)
			{
				uint64_t pos = uint64_t(i*result + j);
				mPoints[pos] = cv::Point2f(float(i / result), float(j / result));
			}

		}
		break;
	}
	case GAUSSIAN:
	{

		//http://www.alanzucconi.com/2015/09/16/how-to-sample-from-a-gaussian-distribution/
		//Box-Muller transform
		for (int i = 0; i < samplesCnt; i++)
		{
			//Generate two random numbers u1, u2 ~Unif(0,1)
			double u1 = random.uniform(double(0.0), double(1.0));
			double u2 = random.uniform(double(0.0), double(1.0));;

			//Create the radius
			double r = sqrt(-2 * log(u1));
			//Create the angle
			double theta = 2 * M_PI * u2;
			//Convert from polar to Cartesian coordinates
			double x = r * cos(theta);
			double y = r * sin(theta);

			//Results ranges from [-1,+1][a,b]
			x = mMean + x * mStddev;
			if(x < 0.0 || x > 1.0)
			{
				i--;
				continue;
			}
			
			y = mMean + y * mStddev;
			if (y < 0.0 || y > 1.0)
			{
				i--;
				continue;
			}
			

			mPoints[i] = cv::Point2f(float(x),float(y));
		}
		break;

	}
	default:
		LOG_ERROR("Generation of this samplepoint distribution is not implemented!");
		break;
	}
}

int defuse::SamplePoints::getSampleCnt() const
{
	return mSampleCnt;
}

std::vector<cv::Point2f> defuse::SamplePoints::getPoints() const
{
	return mPoints;
}

defuse::SamplePoints::Distribution defuse::SamplePoints::getDistribution() const
{
	return mDistribution;
}

std::string defuse::SamplePoints::getSamplePointFileName() const
{
	std::string sampleCnt = cplusutil::String::toStirng(mSampleCnt);
	std::string name = cplusutil::String::concatCStrings("_", 3, "samplepoints", defuse::distributionToString.at(mDistribution), sampleCnt.c_str());

	if(mDistribution == GAUSSIAN)
	{
		std::stringstream stream;
		stream << name << "_" << std::setprecision(2) << mMean << "_" << std::setprecision(2) << mStddev;
		name = stream.str();
	}

	return name;
}

void defuse::SamplePoints::restoreDistribution(std::string _filename)
{
	std::vector<std::string> parts = cplusutil::String::split(_filename.c_str(), '_');
	if(parts.size() < 3)
	{
		LOG_ERROR("Fatal error: The filename does not correspond to the standard: [samplepoints_distribution_samplecnt]" << _filename << " ");
		return;
	}

	std::string distribution = parts.at(1);
	
	for (int i = 0; i < distributionToString.size(); i++)
	{
		if(distribution == distributionToString.at(static_cast<Distribution>(i)))
		{
			mDistribution = static_cast<Distribution>(i);
		}
	}

}

bool defuse::SamplePoints::serialize(std::string _destinationfile)
{
	bool result = false;
	File tmp(_destinationfile);

	if (tmp.getFileExtension() == ".yml" || tmp.getFileExtension() == ".xml")
	{
		result = writeOpenCVData(_destinationfile);
	}
	else if (tmp.getFileExtension() == ".txt")
	{
		result = writeTextfile(_destinationfile);
	}
	else if (tmp.getFileExtension() == ".bin")
	{
		result = writeBinary(_destinationfile);
	}
	return result;
}


bool defuse::SamplePoints::deserialize(std::string _sourcefile)
{
	bool result = false;
	File tmp(_sourcefile);

	if(tmp.getFileExtension() == ".yml" || tmp.getFileExtension() == ".xml")
	{
		result = readOpenCVData(_sourcefile);
	}
	else if(tmp.getFileExtension() == ".txt")
	{
		result = readTextfile(_sourcefile);
	}
	else if (tmp.getFileExtension() == ".bin")
	{
		result = readBinary(_sourcefile);
	}

	if(tmp.getFileExtension() == ".bin" || tmp.getFileExtension() == ".txt")
	{
		restoreDistribution(tmp.getFilename());
	}

	return result;
}

bool defuse::SamplePoints::writeBinary(std::string _file)
{
	std::ofstream stream(_file, std::ofstream::binary |  std::ios::out);
	
	for (int i = 0; i < mPoints.size(); i++)
	{
		//stream << mPoints.at(i).x << "," << mPoints.at(i).y << std::endl;
		float x = mPoints.at(i).x;
		float y = mPoints.at(i).y;
		stream.write(reinterpret_cast<const char*>(&x), sizeof(float));
		stream.write(reinterpret_cast<const char*>(&y), sizeof(float));
	}

	stream.close();
	return true;
}

bool defuse::SamplePoints::readBinary(std::string _file)
{
	std::vector<cv::Point2f> points;
	std::ifstream stream(_file, std::ofstream::binary | std::ios::in);

	while (stream.good())
	{
		float x;
		float y;
		stream.read(reinterpret_cast<char*>(&x), sizeof(float));
		stream.read(reinterpret_cast<char*>(&y), sizeof(float));

		points.push_back(cv::Point2f(x, y));
	}

	stream.close();

	if (points.empty())
	{
		LOG_ERROR("Fatal Error: The File is empty! " << _file);
		return false;
	}

	//while loop with stream.good() reads the last line twice
	//so remove the last element
	points.pop_back();

	mPoints.assign(points.begin(), points.end());
	return true;
}

bool defuse::SamplePoints::writeTextfile(std::string _file)
{
	std::ofstream stream(_file);

	if (!stream.is_open()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	for (int i = 0; i < mPoints.size(); i++)
	{
		stream << mPoints.at(i).x << " " << mPoints.at(i).y << std::endl;
	}

	stream.close();
	return true;
}

bool defuse::SamplePoints::readTextfile(std::string _file)
{
	std::vector<cv::Point2f> points;
	FILE *gtfile = fopen(_file.c_str(), "r");

	float x, y;
	while (fscanf(gtfile, "%f%*c%f%*c", &x, &y) == 2)
	{
		points.push_back(cv::Point2f(x, y));
	}
	fclose(gtfile);

	if (points.empty())
	{
		LOG_ERROR("Fatal Error: The File is empty! " << _file);
		return false;
	}

	mPoints.assign(points.begin(), points.end());
	return true;
}

bool defuse::SamplePoints::writeOpenCVData(std::string _file) const
{
	cv::FileStorage fs(_file, cv::FileStorage::WRITE);
	if (!fs.isOpened()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	fs << "SamplePoints" << *this;
	fs.release();
	return true;
}

bool defuse::SamplePoints::readOpenCVData(std::string _file)
{
	cv::FileStorage fs(_file, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	if (fs["SamplePoints"].empty())
	{
		LOG_ERROR("Fatal Error: The File is empty! " << _file);
		return false;
	}

	fs["SamplePoints"] >> *this;
	fs.release();
	return true;
}

void defuse::SamplePoints::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Distribution" << mDistribution
		<< "SamplesCnt" << static_cast<int>(mSampleCnt)
		<< "SamplePoints" << mPoints;
	fs << "}";

}

void defuse::SamplePoints::read(const cv::FileNode& node)
{
	int distributionEnum;
	node["Distribution"] >> distributionEnum;
	mDistribution = static_cast<Distribution>(distributionEnum);
	node["SamplesCnt"] >> mSampleCnt;
	node["SamplePoints"] >> mPoints;
}

