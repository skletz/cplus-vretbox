#include "ceedxtractor.hpp"
#include <cpluslogger.hpp>

defuse::CEEDXtractor::CEEDXtractor()
{
	mFuzzificator = new Fuzzificator();
	T0 = 14.0;
	T1 = 0.68;
	T2 = 0.98;
	T3 = 0.98;
}

defuse::FeaturesBase* defuse::CEEDXtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture stream(_videobase->mFile->getFile());

	if (!stream.isOpened())
	{
		LOG_FATAL("Error: Video Stream cannot be opened: " << _videobase->mFile->getFile());
	}

	//Output
	cv::Mat vectors;

	//Measure extraction time
	double elapsedSecs = computeCEEDDescriptor(stream, _videobase->mFile->getFile(), vectors);
	FeaturesBase* features = new FeaturesBase(_videobase->mFile->getFilename(), vectors);
	features->mExtractionTime = float(elapsedSecs);

	stream.release();
	return features;
}

std::string defuse::CEEDXtractor::toString()
{
	std::stringstream st;

	st << "xtractor: como, ";
	st << "max blocksize: ";
	st << BLOCKSIZE_MAX << ", ";
	st << "min blocksize: ";
	st << BLOCKSIZE_MIN << ", ";
	std::string tmp = st.str();
	tmp += getKeyframeSelectionAsString();

	return tmp;
}

std::string defuse::CEEDXtractor::getXtractorID() const
{
	std::stringstream st;

	st << "CEED";
	st << "_";
	st << BLOCKSIZE_MAX;
	st << "_";
	st << BLOCKSIZE_MIN;
	st << "_";
	st << mKeyFrameSelection;

	return st.str();
}

double defuse::CEEDXtractor::computeCEEDDescriptor(cv::VideoCapture& _video, std::string filename, cv::OutputArray _descriptor)
{
	int framecnt = int(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int width = int(_video.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = int(_video.get(CV_CAP_PROP_FRAME_HEIGHT));

	int framenr = getKeyframeNumber(framecnt);
	_video.set(CV_CAP_PROP_POS_FRAMES, framenr);

	cv::Mat image, ceed;

	_video.grab();
	_video.retrieve(image);

	//Measure extraction time
	double e1Start = double(cv::getTickCount());
	describe(image, ceed);
	double e1End = double(cv::getTickCount());
	double elapsedSecs = (e1End - e1Start) / double(cv::getTickFrequency());

	ceed.copyTo(_descriptor);

	return elapsedSecs;
}

bool defuse::CEEDXtractor::describe(cv::Mat& image, cv::Mat& descriptor)
{
	cv::Mat ceedDescriptor = cv::Mat::zeros(1, 144, CV_32F);
	int width = image.cols;
	int height = image.rows;

	//1. Calculate grayscale histogram
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

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

			extractFromBlock(imageBlock, grayBlock, ceedDescriptor);

		}
	}

	double sum = cv::sum(ceedDescriptor)[0];
	ceedDescriptor /= sum;
	ceedDescriptor.copyTo(descriptor);
	return true;
}

bool defuse::CEEDXtractor::extractFromBlock(cv::Mat& imageBlock, cv::Mat& grayBlock, cv::Mat& features)
{
	int width = imageBlock.cols;
	int height = imageBlock.rows;

	cv::Mat fuzzy10BinResult, fuzzy24BinResult;
	std::vector<double> HSV = std::vector<double>(3);
	std::vector<double> edgeHist = std::vector<double>(6);

	double Max = 0;

	Neighborhood neighbours;

	neighbours.area1 = 0;
	neighbours.area2 = 0;
	neighbours.area3 = 0;
	neighbours.area4 = 0;

	Mask maskValues;
	maskValues.mask1 = 0;
	maskValues.mask2 = 0;
	maskValues.mask3 = 0;
	maskValues.mask4 = 0;
	maskValues.mask5 = 0;

	for (int i = 0; i < edgeHist.size(); i++) {
		edgeHist[i] = -1;
	}
	//1. Calculate grayscale histogram
	cv::Mat grayImage;
	cv::cvtColor(imageBlock, grayImage, CV_BGR2GRAY);


	for(int i = 0; i < width; i++)
	{
		for(int j = 0; j < height; j++)
		{
			if (j < (i + width / 2) && i < (j + height / 2))
				neighbours.area1 += grayImage.at<uchar>(j, i);
			if (j >= (i + width / 2) && i < (j + height / 2))
				neighbours.area2 += grayImage.at<uchar>(j, i);
			if (j < (i + width / 2) && i >= (j + height / 2))
				neighbours.area3 += grayImage.at<uchar>(j, i);
			if (j >= (i + width / 2) && i >= (j + height / 2))
				neighbours.area4 += grayImage.at<uchar>(j, i);
		}
	}

	neighbours.area1 = int(neighbours.area1 * (4.0 / (width * height)));
	neighbours.area2 = int(neighbours.area2 * (4.0 / (width * height)));
	neighbours.area3 = int(neighbours.area3 * (4.0 / (width * height)));
	neighbours.area4 = int(neighbours.area4 * (4.0 / (width * height)));

	//obtain the respective edge magnitudes
	maskValues.mask1 = std::abs(
		neighbours.area1 * 2 + neighbours.area2 * -2 + neighbours.area3 * -2 + neighbours.area4 * 2);

	maskValues.mask2 = std::abs(
		neighbours.area1 * 1 + neighbours.area2 * 1 + neighbours.area3 * -1 + neighbours.area4 * -1);

	maskValues.mask3 = std::abs(
		neighbours.area1 * 1 + neighbours.area2 * -1 + neighbours.area3 * 1 + neighbours.area4 * -1);

	maskValues.mask4 = std::abs(
		neighbours.area1 * std::sqrt(2) + neighbours.area2 * 0 + neighbours.area3 * 0 + neighbours.area4 * -std::sqrt(2));

	maskValues.mask5 = std::abs(
		neighbours.area1 * 0 + neighbours.area2 * std::sqrt(2) + neighbours.area3 * -std::sqrt(2) + neighbours.area4 * 0);

	//calculate the maximum from calculated edge magnitudes
	Max = std::max(maskValues.mask1, std::max(maskValues.mask2, std::max(maskValues.mask3, std::max(maskValues.mask4, maskValues.mask5))));

	//Normalize values using the maximum
	maskValues.mask1 = maskValues.mask1 / Max;
	maskValues.mask2 = maskValues.mask2 / Max;
	maskValues.mask3 = maskValues.mask3 / Max;
	maskValues.mask4 = maskValues.mask4 / Max;
	maskValues.mask5 = maskValues.mask5 / Max;

	//count values according to heuristic pentagon diagram
	int t = -1;

	//if the max value is greater than the given threshold, the image block is classifiedas texture block
	if (Max < T0) {
		edgeHist[0] = 0; //non-directional (texture-less block)
		t = 0;
	}
	else {
		t = -1;

		if (maskValues.mask1 > T1) {
			t++;
			edgeHist[t] = 1;
		}
		if (maskValues.mask2 > T2) {
			t++;
			edgeHist[t] = 2;
		}
		if (maskValues.mask3 > T2) {
			t++;
			edgeHist[t] = 3;
		}
		if (maskValues.mask4 > T3) {
			t++;
			edgeHist[t] = 4;
		}
		if (maskValues.mask5 > T3) {
			t++;
			edgeHist[t] = 5;
		}

	}
	//?
	cv::Scalar means = cv::mean(imageBlock);
	cv::Mat bgrmeans(1, 1, CV_8UC3, means);
	bgrmeans.convertTo(bgrmeans, CV_32F, 1. / 255);

	cv::Mat hsvmeans;
	cv::cvtColor(bgrmeans, hsvmeans, CV_BGR2HSV);

	std::vector<cv::Mat> channels;
	cv::split(hsvmeans, channels);
	int h = channels[0].at<float>(0);
	int s = channels[1].at<float>(0) * 255;
	int v = channels[2].at<float>(0) * 255;

	mFuzzificator->calculateFuzzy10BinHisto(
		h, s, v, fuzzy10BinResult);
	mFuzzificator->calculateFuzzy24BinHisto(
		h, s, v, fuzzy10BinResult, fuzzy24BinResult);

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
