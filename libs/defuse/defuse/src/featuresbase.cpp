#include "featuresbase.hpp"
#include <fstream>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <cplusutil.hpp>

defuse::FeaturesBase::FeaturesBase(std::string _id)
{
	mID = _id;
	mVectors = cv::Mat();
	mMatchingCriteria = 0;
	mExtractionTime = 0.0;
}

defuse::FeaturesBase::FeaturesBase(std::string _id, cv::Mat _features)
{
	mID = _id;
	mVectors = _features;
	mExtractionTime = 0.0;
	mMatchingCriteria = 0;
}

defuse::FeaturesBase::FeaturesBase(cv::Mat _features)
{
	mID = "";
	mVectors = _features;
	mExtractionTime = 0.0;
	mMatchingCriteria = 0;
}

defuse::FeaturesBase::FeaturesBase()
{
	mID = "";
	mVectors = cv::Mat();
	mExtractionTime = 0.0;
	mMatchingCriteria = 0;
}

defuse::FeaturesBase::~FeaturesBase(){}

std::string defuse::FeaturesBase::getMatType(cv::Mat inputMat) const
{
	int inttype = inputMat.type();

	std::string r;
	uchar depth = inttype & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (inttype >> CV_CN_SHIFT);
	switch (depth) {
	case CV_32F: r = "32F";break;
	case CV_64F: r = "64F";break;
	case CV_8U: r = "8U"; break;
	case CV_32S: r = "32S"; break;
	default:     r = "User";break;
	}
	r += "C";
	r += (chans + '0');
	return r;
}

int defuse::FeaturesBase::getMatType(std::string type) const
{
	int r;
	std::stringstream t;
	for(char& c : type)
	{
		t << c;
		if(c == 'U' || c == 'S' || c == 'F')
		{
			break;
		}
	}
	std::string matType = t.str();

	if (matType == "32F")
	{
		r = CV_32F;
	}
	else if (matType == "64F")
	{
		r = CV_64F;
	}
	else if (matType == "8U")
	{
		r = CV_8U;
	}
	else if (matType == "32S")
	{
		r = CV_32S;
	}
	else
	{
		r = -1;
	}

	return r;
}

int defuse::FeaturesBase::getMatDepth(std::string type) const
{
	std::stringstream t;
	for (char& c : type)
	{
		if (c == 'C')
		{
			t << c;;
		}
	}
	std::string matDepth = t.str();

	int depth = std::stoi(matDepth);
	return depth;
}

bool defuse::FeaturesBase::serialize(std::string _destinationfile)
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
	else if (tmp.getFileExtension() == ".csv")
	{
		result = writeCSVFile(_destinationfile);
	}
	return result;
}

bool defuse::FeaturesBase::deserialize(std::string _sourcefile)
{
	bool result = false;
	File tmp(_sourcefile);

	if (tmp.getFileExtension() == ".yml" || tmp.getFileExtension() == ".xml")
	{
		result = readOpenCVData(_sourcefile);
	}
	else if (tmp.getFileExtension() == ".txt")
	{
		result = readTextfile(_sourcefile);
	}
	else if (tmp.getFileExtension() == ".bin")
	{
		result = readBinary(_sourcefile);
	}
	else if (tmp.getFileExtension() == ".csv")
	{
		result = readCSVFile(_sourcefile);
	}

	return result;
}

bool defuse::FeaturesBase::writeBinary(std::string _file)
{
	std::ofstream outstream(_file, std::ios_base::out);
	outstream.close();
	outstream.open(_file, std::ios::binary | std::ios::out);

	boost::archive::binary_oarchive oa(outstream);

	int cols, rows, type;
	cols = mVectors.cols; rows = mVectors.rows; type = mVectors.type();
	bool continuous = mVectors.isContinuous();

	oa & cols & rows & type & continuous;

	if (continuous) {
		const unsigned int data_size = rows * cols * int(mVectors.elemSize());
		oa & boost::serialization::make_array(mVectors.ptr(), data_size);
	}
	else {
		const unsigned int row_size = cols * int(mVectors.elemSize());
		for (int i = 0; i < rows; i++) {
			oa & boost::serialization::make_array(mVectors.ptr(i), row_size);
		}
	}

	return true;
}

bool defuse::FeaturesBase::readBinary(std::string _file)
{
	std::ifstream instream(_file, std::ios::binary | std::ios::in);

	boost::archive::binary_iarchive ia(instream);

	cv::Mat vectors;
	int cols, rows, type;
	bool continuous;

	ia & cols & rows & type & continuous;

	vectors.create(rows, cols, type);

	const unsigned int row_size = cols * int(vectors.elemSize());
	for (int i = 0; i < rows; i++) {
		ia & boost::serialization::make_array(vectors.ptr(i), row_size);
	}

	mVectors = vectors;
	return true;
}

bool defuse::FeaturesBase::writeTextfile(std::string _file)
{
	std::ofstream stream(_file);

	if (!stream.is_open()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}
	std::string mattype = getMatType(mVectors);
	stream << mVectors.rows << " " << mVectors.cols << " " << mattype << std::endl;

	if(mVectors.type() == CV_8U)
	{
		mVectors.convertTo(mVectors, CV_32S);
	}

	for (int i = 0; i < mVectors.rows; i++)
	{
		for (int j = 0; j < mVectors.cols; j++)
		{
			if(mVectors.type() == CV_32F)
				stream << mVectors.at<float>(i,j) << " " ;
			else if (mVectors.type() == CV_64F)
				stream << mVectors.at<double>(i, j) << " ";
			else if (mVectors.type() == CV_32S)
				stream << mVectors.at<int>(i, j) << " ";
			else
				LOG_ERROR("Error: writeTextFile: Mat type not supported!");
		}
		stream << std::endl;
	}

	stream.close();
	return true;
}

bool defuse::FeaturesBase::readTextfile(std::string _file)
{
	cv::Mat features;
	std::ifstream stream(_file);

	if (!stream.is_open()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	std::string line;
	std::getline(stream, line);
	int width, height;
	std::string type;
	std::istringstream linestream(line);
	linestream >> width >> height >> type;
	int matType = getMatType(type);

	bool convertToUChar = false;
	if (matType == CV_32S)
		convertToUChar = true;

	cv::Mat mat(width, height, matType);
	for (int i = 0; i < width; i++)
	{
		if(std::getline(stream, line))
		{
			std::stringstream values(line);
			for (int j = 0; j < height; j++)
			{
				float val;
				values >> val;
				if (matType == CV_32F)
					mat.at<float>(i, j) = val;
				else if (matType == CV_64F)
					mat.at<double>(i, j) = val;
				else if (matType == CV_32S)
					mat.at<int>(i, j) = val;
				else
					LOG_ERROR("Error: readTextFile: Mat type not supported!");
			}
		}

	}

	if(convertToUChar)
		mat.convertTo(mat, CV_8U);

	mVectors = mat;

	return true;
}

bool defuse::FeaturesBase::writeCSVFile(std::string _file)
{
	std::ofstream stream(_file);

	if (!stream.is_open()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	for (int i = 0; i < mVectors.rows; i++)
	{
		for (int j = 0; j < mVectors.cols; j++)
		{
			stream << mVectors.at<float>(i, j) << "; ";
		}
		stream << std::endl;
	}

	stream.close();
	return true;
}

bool defuse::FeaturesBase::readCSVFile(std::string _file)
{
	std::ifstream stream(_file);
	std::vector<float> floats;

	if (!stream) {
		LOG_ERROR("Error in serialization of Video: Invalid file " << "");
		return false;
	}

	float value = 0.0;
	std::stringstream strStream;
	strStream << stream.rdbuf();

	std::vector<std::string> values = cplusutil::String::split(strStream.str(), ';');


	for (int i = 0; i < values.size(); i++)
	{
		floats.push_back(std::stof(values.at(i)));
	}

	mVectors = cv::Mat(1, floats.size(), CV_32F);
	memcpy(mVectors.data, floats.data(), floats.size() * sizeof(float));
	return true;
}

bool defuse::FeaturesBase::writeOpenCVData(std::string _file) const
{
	cv::FileStorage fs(_file, cv::FileStorage::WRITE);

	if (!fs.isOpened()) {
		LOG_ERROR("Error in serialization of Video: Invalid file " << "");
		return false;
	}

	fs << "Features" << *this;
	fs.release();
	return true;
}

bool defuse::FeaturesBase::readOpenCVData(std::string _file)
{
	cv::FileStorage fs(_file, cv::FileStorage::READ);

	if (!fs.isOpened()) {
		LOG_ERROR("Fatal Error: The File cannot be opened! " << _file);
		return false;
	}

	if (fs["Features"].empty())
	{
		LOG_ERROR("Fatal Error: The File is empty! " << _file);
		return false;
	}

	fs["Features"] >> *this;
	fs.release();
	return true;
}

void defuse::FeaturesBase::write(cv::FileStorage& fs) const
{
	fs << "{"
		<< "Features" << mVectors
		<< "}";
}

void defuse::FeaturesBase::read(const cv::FileNode& node)
{
	node["Features"] >> mVectors;
}

