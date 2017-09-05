#include "comoxtractor.hpp"


defuse::COMOXtractor::COMOXtractor()
{
	mFuzzificator = new Fuzzificator();
}

defuse::FeaturesBase* defuse::COMOXtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture stream(_videobase->mFile->getFile());

	if (!stream.isOpened())
	{
		LOG_FATAL("Error: Video Stream cannot be opened: " << _videobase->mFile->getFile());
	}

	//Output
	cv::Mat vectors;

	//Measure extraction time
	double elapsedSecs = computeCOMODescriptor(stream, _videobase->mFile->getFile(), vectors);
	FeaturesBase* features = new FeaturesBase(_videobase->mFile->getFilename(), vectors);
	features->mExtractionTime = float(elapsedSecs);

	stream.release();
	return features;
}

std::string defuse::COMOXtractor::toString() const
{
	std::stringstream st;

	st << "xtractor: como, ";
	st << "max blocksize: ";
	st << BLOCKSIZE_MAX << ", ";
	st << "max blocksize: ";
	st << BLOCKSIZE_MIN << ", ";
	st << "texture histsize: ";
	st << TEXTURE_HISTSIZE << ", ";
	st << "fuzzy histsize: ";
	st << FUZZY_HISTSIZE << ", ";
	st << "grayscale histsize: ";
	st << GRAYSCALE_HISTSIZE << ", ";
	st << "keyframeselection: ";

	if (mKeyFrameSelection == KeyFrameSelection::FirstFrame)
	{
		st << "first";
	}
	else if (mKeyFrameSelection == KeyFrameSelection::MiddleFrame)
	{
		st << "middle";
	}
	else if (mKeyFrameSelection == KeyFrameSelection::LastFrame)
	{
		st << "last";
	}

	return st.str();
}

std::string defuse::COMOXtractor::getXtractorID() const
{
	std::stringstream st;

	st << "COMO";
	st << "_";
	st << BLOCKSIZE_MAX;
	st << "_";
	st << BLOCKSIZE_MIN;
	st << "_";
	st << TEXTURE_HISTSIZE;
	st << "_";
	st << FUZZY_HISTSIZE;
	st << "_";
	st << GRAYSCALE_HISTSIZE;
	st << "_";
	st << mKeyFrameSelection;

	return st.str();
}

void defuse::COMOXtractor::showProgress(int _step, int _total) const
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

double defuse::COMOXtractor::computeCOMODescriptor(cv::VideoCapture& _video, std::string filename, cv::OutputArray _descriptor)
{
	int framecnt = int(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int width = int(_video.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = int(_video.get(CV_CAP_PROP_FRAME_HEIGHT));

	int framenr = framecnt;

	if (mKeyFrameSelection == MiddleFrame) //use middle frame
	{
		framenr = int(framecnt / float(2));
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else if (mKeyFrameSelection == FirstFrame) //use first frame
	{
		framenr = 1;
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else if (mKeyFrameSelection == LastFrame) //use last frame
	{
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else
	{
		LOG_FATAL("Keyframe selection " << mKeyFrameSelection << " not implemented: Use middle, first or last frame");
		return false;
	}

	cv::Mat image, como;

	_video.grab();
	_video.retrieve(image);

	//Measure extraction time
	double e1Start = double(cv::getTickCount());
	describe(image, como);
	double e1End = double(cv::getTickCount());
	double elapsedSecs = (e1End - e1Start) / double(cv::getTickFrequency());

	como.copyTo(_descriptor);

	return elapsedSecs;
}

bool defuse::COMOXtractor::describe(cv::Mat& image, cv::Mat& descriptor)
{
	cv::Mat comoDescriptor = cv::Mat::zeros(1, 144, CV_32F);
	cv::Mat comoDescriptorQuantized = cv::Mat::zeros(1, 144, CV_8UC1);

	int width = image.cols;
	int height = image.rows;

	//1. Calculate grayscale histogram
	cv::Mat grayImage;
	convertToGrayscale(image, grayImage);

	int NumberOfBlocks = -1;
	if (std::min(width, height) >= 80)
		NumberOfBlocks = BLOCKSIZE_MAX * BLOCKSIZE_MAX;
	else if (std::min(width, height) >= 40)
		NumberOfBlocks = BLOCKSIZE_MIN * BLOCKSIZE_MIN;

	int Step_X = 2, Step_Y = 2, TemoMAX_X, TemoMAX_Y;
	if (NumberOfBlocks > 0) {
		double sqrtNumberOfBlocks = std::sqrt(NumberOfBlocks);
		Step_X = int(std::floor(width / sqrtNumberOfBlocks));
		Step_Y = int(std::floor(height / sqrtNumberOfBlocks));

		if ((Step_X % 2) != 0) {
			Step_X = Step_X - 1;
		}
		if ((Step_Y % 2) != 0) {
			Step_Y = Step_Y - 1;
		}

		TemoMAX_X = Step_X * int(sqrtNumberOfBlocks);
		TemoMAX_Y = Step_Y * int(sqrtNumberOfBlocks);
	}
	else {
		TemoMAX_X = Step_X * int(std::floor(width >> 1));
		TemoMAX_Y = Step_Y * int(std::floor(height >> 1));
	}

	for (int y = 0; y < TemoMAX_Y; y += Step_Y)
	{
		for (int x = 0; x < TemoMAX_X; x += Step_X)
		{
			cv::Mat imageBlock, grayBlock, huMoments, grayscaleHist;
			cv::Rect mask = cv::Rect(x, y, Step_X, Step_Y);
			grayImage(mask).copyTo(grayBlock);
			image(mask).copyTo(imageBlock);

			calculateGrayscaleHistogram(grayBlock, grayscaleHist);
			float entropy = calculateEntropy(grayscaleHist, grayBlock.rows * grayBlock.cols);

			if (entropy >= 1)
			{
				extractFromBlock(imageBlock, grayBlock, comoDescriptor);
			}
		}
	}

	double sum = cv::sum(comoDescriptor)[0];
	comoDescriptor /= sum;
	//this->quantify(comoDescriptor, comoDescriptorQuantized);
	//comoDescriptorQuantized.copyTo(descriptor);
	comoDescriptor.copyTo(descriptor);
	return true;
}

bool defuse::COMOXtractor::extractFromBlock(cv::Mat& imageBlock, cv::Mat& grayBlock, cv::Mat& features)
{
	int area = imageBlock.cols * imageBlock.rows;
	cv::Mat fuzzy10BinResult, fuzzy24BinResult;

	int huTableRows = sizeof(TEXTURE_DEFINITION_TABLE) / sizeof(TEXTURE_DEFINITION_TABLE[0]);
	int huTableCols = sizeof(TEXTURE_DEFINITION_TABLE[0]) / sizeof(TEXTURE_DEFINITION_TABLE[0][0]);
	cv::Mat huTable = cv::Mat(huTableRows, huTableCols, CV_64FC1, &TEXTURE_DEFINITION_TABLE);

	std::vector<double> distances = std::vector<double>(huTableRows);
	std::vector<double> HSV = std::vector<double>(huTableRows);
	std::vector<double> edgeHist = std::vector<double>(huTableRows);

	std::vector<cv::Mat> channels;
	cv::Mat huMoments;
	calculateHuMoments(grayBlock, huMoments);
	calculateDistances(huMoments, huTable, distances);

	//The type of texture the current block belongs to is determined.
	//A block can participate in more than one type of texture. 
	int t = -1;
	for (int iDistance = 0; iDistance < distances.size(); iDistance++)
	{
		if (distances.at(iDistance) < THRESHOLDS_TABLE[iDistance])
		{
			int pos = ++t;
			edgeHist[pos] = iDistance;
		}
	}

	cv::Scalar means = cv::mean(imageBlock);
	cv::Mat bgrmeans(1, 1, CV_8UC3, means);
	bgrmeans.convertTo(bgrmeans, CV_32F, 1. / 255);

	cv::Mat hsvmeans;
	cv::cvtColor(bgrmeans, hsvmeans, CV_BGR2HSV);

	cv::split(hsvmeans, channels);
	int h = channels[0].at<float>(0);
	int s = channels[1].at<float>(0) * 255;
	int v = channels[2].at<float>(0) * 255;

	mFuzzificator->calculateFuzzy10BinHisto(
		h, s, v, fuzzy10BinResult);
	mFuzzificator->calculateFuzzy24BinHisto(
		h, s, v, fuzzy10BinResult, fuzzy24BinResult);

	//t indicates, how many types of textures
	for (int i = 0; i <= t; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			if (fuzzy24BinResult.at<double>(j) > 0)
			{
				//the index depends on the texture type, in which the fuzzy histogram will be stored.
				int index = 24 * edgeHist[i] + j;
				double fuzzy24Value = fuzzy24BinResult.at<double>(j);
				features.at<float>(index) += fuzzy24Value;
			}
		}
	}

	return true;
}

void defuse::COMOXtractor::convertToGrayscale(cv::Mat& image, cv::Mat& gray)
{
	cv::cvtColor(image, gray, CV_BGR2GRAY);
}

void defuse::COMOXtractor::calculateGrayscaleHistogram(cv::Mat& gray, cv::Mat& hist)
{
	float range[] = { 0, GRAYSCALE_HISTSIZE - 1 };
	const float *ranges[] = { range };

	cv::Mat tmp;
	cv::calcHist(&gray, 1, 0, cv::Mat(), tmp, 1, &GRAYSCALE_HISTSIZE, ranges, true, false);

	//transpose histogram
	hist = tmp.col(0).t();
}

void defuse::COMOXtractor::calculateHuMoments(cv::Mat& gray, cv::Mat& huMoments)
{
	double huMomArray[7]; //There are 7 HuMoments
	cv::HuMoments(cv::moments(gray), huMomArray);

	//transpose huMoments
	cv::Mat(1, 7, CV_64FC1, &huMomArray).copyTo(huMoments);
}

float defuse::COMOXtractor::calculateEntropy(cv::Mat& grayscaleHist, int area)
{
	float entropy = 0.0;

	for (int iHist = 0; iHist < grayscaleHist.cols; iHist++)
	{
		float val = grayscaleHist.at<float>(iHist);
		if (val > 0)
		{
			val /= static_cast<float>(area);
			entropy -= val * (std::log(val) / std::log(2));
		}
	}

	return entropy;
}

void defuse::COMOXtractor::calculateDistances(cv::Mat& huMoments, cv::Mat& huTable, std::vector<double>& distances)
{
	int huTableRows = huTable.rows;
	double maxDistance = (-1 * std::numeric_limits<double>::max());
	for (int iRow = 0; iRow < huTableRows; iRow++)
	{
		cv::Mat row = huTable.row(iRow);
		double distance = cv::norm(huMoments, row);
		distances.at(iRow) = distance;
		if (distance > maxDistance)
		{
			maxDistance = distances.at(iRow);
		}
	}

	for (int i = 0; i < distances.size(); i++)
	{
		distances.at(i) /= maxDistance;
	}
}

void defuse::COMOXtractor::quantify(cv::Mat& descriptor, cv::Mat& output)
{
	int size = descriptor.cols;
	std::vector<uint8_t> result = std::vector<uint8_t>(size);
	output.reserve(TEXTURE_HISTSIZE * FUZZY_HISTSIZE);

	double min, temp;

	for (int iTexture = 0; iTexture < TEXTURE_HISTSIZE; iTexture++)
	{

		for (int iFuzzy = (iTexture * FUZZY_HISTSIZE); iFuzzy < ((iTexture + 1) * FUZZY_HISTSIZE); iFuzzy++)
		{

			min = std::abs(descriptor.at<float>(iFuzzy) - QUANTIZATION_TABLE[iTexture][0]);
			result[iFuzzy] = 0;

			for (uint8_t j = 1; ((j < 8) && (min != 0.0)); j++)
			{
				temp = std::abs(descriptor.at<float>(iFuzzy) - QUANTIZATION_TABLE[iTexture][j]);
				if (temp < min)
				{
					min = temp;
					result[iFuzzy] = j;
				}
			}
		}

	}
	cv::Mat out(1, result.size(), CV_8UC1, result.data());
	out.copyTo(output);
}

void defuse::COMOXtractor::showHistogram(std::string name, cv::MatND& hist, int height, int width)
{
	//get the maximal height of the histogram
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	cv::Mat histImg = cv::Mat::zeros(GRAYSCALE_HISTSIZE, maxVal * scale, CV_8UC1);

	for (int iEntry = 0; iEntry < GRAYSCALE_HISTSIZE; iEntry++) {
		float binVal = hist.at<float>(iEntry, 0);
		int intensity = cvRound(binVal * (GRAYSCALE_HISTSIZE - 1) / maxVal);
		rectangle(histImg, cv::Point(iEntry*scale, histImg.rows), cv::Point((iEntry + 1)*scale - 1, histImg.rows - intensity), cv::Scalar::all(255), CV_FILLED);
	}

	cv::namedWindow(name);
	cv::imshow(name, histImg);
}

void defuse::COMOXtractor::showImage(const cv::Mat _image, cv::Size _size, int x, int y, std::string name) const
{
	cv::namedWindow(name);
	cv::imshow(name, _image);
}
