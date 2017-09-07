#include "fdysigxtractor.hpp"
#include <cpluslogger.hpp>

#define _USE_MATH_DEFINES 
#include <math.h>

/**
 * Feature Signatures Extractor
 *
 */
defuse::FlowDySIGXtractor::FlowDySIGXtractor(
	std::vector<cv::Point2f> _samplepoints,
	int _maxFrames,
	int _initSeeds,
	int _initialCentroids, int _iterations, 
	int _minClusterSize, float _minDistance,
	float _dropThreshold,
	int _grayscaleBits, int _windowRadius,
	bool _resetTracking)
	: SIGXtractor(_samplepoints)
{
	mSamplepoints = _samplepoints;

	mInitSeeds = _initSeeds;
	mMaxClusters = _initialCentroids;
	mIterations = _iterations;
	mMinimalClusterSize = _minClusterSize;
	mMinimalDistance = _minDistance;
	mClusterDropThreshold = _dropThreshold;

	mGrayscaleBits = _grayscaleBits;
	mWindowRadius = _windowRadius;

	mMaxFrames = _maxFrames;
	mResetTracking = _resetTracking;

	indices.x2 = 7;
	indices.y2 = 8;
	indices.w = 9;
	indices.dims = 10;
}

defuse::FeaturesBase* defuse::FlowDySIGXtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture stream(_videobase->mFile->getFile());

	if (!stream.isOpened())
	{
		LOG_FATAL("Error: Video Stream cannot be opened: " << _videobase->mFile->getFile());
	}

	//Output
	cv::Mat vectors;

	//Measure extraction time
	size_t e1Start = double(cv::getTickCount());
	computeDynamicSignatures(stream, _videobase->mFile->getFile(), vectors);
	size_t e1End = double(cv::getTickCount());
	double elapsedSecs = (e1End - e1Start) / double(cv::getTickFrequency());

	FeaturesBase* features = new FeaturesBase(_videobase->mFile->getFilename(), vectors);
	features->mExtractionTime = elapsedSecs;

	stream.release();
	return features;
}


bool defuse::FlowDySIGXtractor::computeDynamicSignatures(cv::VideoCapture& _video, std::string filename, cv::OutputArray _signatures)
{
	int framecnt = _video.get(CV_CAP_PROP_FRAME_COUNT);
	int width = _video.get(CV_CAP_PROP_FRAME_WIDTH);
	int height = _video.get(CV_CAP_PROP_FRAME_HEIGHT);

	int frameSize = 0;
	int step = 0;

	getFrameRatio(framecnt, mMaxFrames, step, frameSize);

	std::vector<cv::Point2f> currPoints;
	std::vector<cv::Point2f> initPoints;

	deNormalizePoints(mSamplepoints, width, height, initPoints);
	std::vector<cv::Point2f> prevPoints(initPoints);

	std::vector<cv::Mat> spatialsamples;
	spatialsamples.reserve(framecnt);

	cv::Mat samples;
	samples.create(initPoints.size(), indices.dims, CV_32F);

	cv::Mat frame, prevFrame, prevGrayFrame, grayFrame;
	int counter = 0;
	int curFrameNr = 0, prevFrameNr = 0;
	for (int iFrame = 0; iFrame < frameSize; iFrame = iFrame + step)
	{
		curFrameNr = iFrame;
		counter++;

		showProgress(iFrame, frameSize);

		_video.set(CV_CAP_PROP_POS_FRAMES, iFrame);
		_video.grab();
		_video.retrieve(frame);

		if (frame.empty()) 
			continue;

		cv::cvtColor(frame, grayFrame, CV_BGR2GRAY);

		if (mResetTracking)
		{
			prevPoints = std::vector<cv::Point2f>(initPoints);
		}

		SIGXtractor::getSamples(frame, prevPoints, samples);

		if (iFrame > 0)
		{
			//Indicates wheter the flow for the corresponding features has been found
			std::vector<uchar> statusVector;
			statusVector.resize(prevPoints.size());
			//Indicates the error for the corresponding feature
			std::vector<float> errorVector;
			errorVector.resize(prevPoints.size());

			cv::calcOpticalFlowPyrLK(prevGrayFrame, grayFrame, prevPoints, currPoints, statusVector, errorVector);

			getMotionSamples(statusVector, errorVector, prevPoints, currPoints, height, width, samples);

			if(mDisaply || mSaveDisplay)
			{
				cv::Mat sampleImage, motionImage, residualImage, dynamicImage;

				drawSamples(frame, samples, sampleImage);
				drawResidualImage(grayFrame, prevGrayFrame, residualImage);
				drawMotionDirection(frame, samples, motionImage);
				drawDynamicSamples(frame, samples, dynamicImage);

				char keyPressed = 0;
				if(mDisaply)
				{
					showImage(prevFrame, cv::Size(width, height), 0, 0, "Previous Frame");
					showImage(frame, cv::Size(width, height), 0, height, "Current Frame");
					showImage(sampleImage, cv::Size(width, height), width, 0, "Current Samples");
					showImage(motionImage, cv::Size(width, height), width, height, "Motion Direction");
					showImage(residualImage, cv::Size(width, height), width * 2, height, "Residual Image");
					showImage(dynamicImage, cv::Size(width, height), width * 3, 0, "Dynamic Samples");
					keyPressed = cv::waitKey(0);
				}

				if (keyPressed == 'p' || mSaveDisplay)
				{
					File file(filename);
					file.setPath(mOutput->getPath());
					std::string prevnr = std::to_string(prevFrameNr);
					std::string curnr = std::to_string(curFrameNr);
					std::string nr = std::to_string(iFrame);
					saveImage(file.getFile(), ".jpg", "frame_" + prevnr, prevFrame);
					saveImage(file.getFile(), ".jpg", "frame_" + curnr, frame);
					saveImage(file.getFile(), ".png", "sampleImage_" + nr, sampleImage);
					saveImage(file.getFile(), ".png", "motionImage_" + nr, motionImage);
					saveImage(file.getFile(), ".png", "residualImage_" + nr, residualImage);
					saveImage(file.getFile(), ".png", "dynamicImage_" + nr, dynamicImage);
				}
			}

			cv::Mat copy;
			samples.copyTo(copy);
			spatialsamples.push_back(copy);
		}
		else
		{
			currPoints = std::vector<cv::Point2f>(prevPoints);
		}

		prevGrayFrame = grayFrame.clone();
		prevFrame = frame.clone();
		prevPoints = std::vector<cv::Point2f>(currPoints);
		prevFrameNr = curFrameNr;


	}

	cv::Mat temporalsamples(cv::Size(samples.cols, spatialsamples.size() * samples.rows), samples.type());

	for (int i = 0; i < spatialsamples.size(); i++)
	{
		cv::Mat tmp = spatialsamples.at(i);

		for (int j = 0; j < tmp.rows; j++)
		{
			int shift = tmp.rows;
			temporalsamples.at<float>((i*shift) + j, indices.x) = tmp.at<float>(j, indices.x);
			temporalsamples.at<float>((i*shift) + j, indices.y) = tmp.at<float>(j, indices.y);
			temporalsamples.at<float>((i*shift) + j, indices.l) = tmp.at<float>(j, indices.l);
			temporalsamples.at<float>((i*shift) + j, indices.a) = tmp.at<float>(j, indices.a);
			temporalsamples.at<float>((i*shift) + j, indices.b) = tmp.at<float>(j, indices.b);
			temporalsamples.at<float>((i*shift) + j, indices.c) = tmp.at<float>(j, indices.c);
			temporalsamples.at<float>((i*shift) + j, indices.e) = tmp.at<float>(j, indices.e);
			temporalsamples.at<float>((i*shift) + j, indices.x2) = tmp.at<float>(j, indices.x2);
			temporalsamples.at<float>((i*shift) + j, indices.y2) = tmp.at<float>(j, indices.y2);
		}
	}

	cv::Mat signatures;
	getClusters(temporalsamples, signatures);

	if (mDisaply || mSaveDisplay)
	{
		cv::Mat signaturesImage;
		drawSignatures(frame, signatures, signaturesImage);

		char keyPressed = 0;
		if (mDisaply)
		{
			showImage(signaturesImage, cv::Size(width, height), width * 2, height * 2, "Flow-based Signatures");
			keyPressed = cv::waitKey(0);
		}

		if (keyPressed == 'p' || mSaveDisplay)
		{
			File file(filename);
			file.setPath(mOutput->getPath());
			saveImage(file.getFile(), ".png", "signaturesImage", signaturesImage);
		}
	}

	signatures.copyTo(_signatures);
	return true;
}

void defuse::FlowDySIGXtractor::drawMotionDirection(const cv::Mat _source, cv::Mat _samples, cv::Mat& _result)
{
	//if (_source.empty() || _samples.empty())
	//	return;

	cv::Mat source, result, samples;
	_source.copyTo(source);
	result.create(source.size(), source.type());

	float maxLength = 0;
	float minLength = std::numeric_limits<float>::max();
	for (int i = 0; i < _samples.rows; i++)
	{
		float x1 = _samples.at<float>(i, indices.x);
		float y1 = _samples.at<float>(i, indices.y);
		float x2 = _samples.at<float>(i, indices.x2);
		float y2 = _samples.at<float>(i, indices.y2);
		cv::Point p = cv::Point(x1 * source.cols, y1 * source.rows);
		cv::Point q = cv::Point(x2 * source.cols, y2 * source.rows);

		float mvX = x2 - x1;
		float mvY = y2 - y1;
		float length = std::sqrt(std::pow(mvX, 2) + std::pow(mvY, 2));

		if(length > maxLength)
		{
			maxLength = length;
		}

		if (length < minLength)
		{
			minLength = length;
		}
	}

	for (int i = 0; i < _samples.rows; i++)
	{
		float x1 = _samples.at<float>(i, indices.x);
		float y1 = _samples.at<float>(i, indices.y);
		float x2 = _samples.at<float>(i, indices.x2);
		float y2 = _samples.at<float>(i, indices.y2);
		cv::Point p = cv::Point(x1 * source.cols, y1 * source.rows);
		cv::Point q = cv::Point(x2 * source.cols, y2 * source.rows);

		float mvX = x2 - x1;
		float mvY = y2 - y1;
		float length = std::sqrt(std::pow(mvX,2) + std::pow(mvY, 2));
		
		float intens1 = maxLength / float(4);
		float intens2 = ((maxLength / float(4)) * 2.0);
		float intens3 = ((maxLength / float(4)) * 3.0);
		int scale = 1;
		if(length == 0)
		{
			scale = 0;
		}else if(length >= minLength && length < intens1)
		{
			scale = 1;
		}else if(length >= intens1 && length < intens2)
		{
			scale = 3;
		}
		else if (length >= intens2 && length < intens3)
		{
			scale = 6;
		}
		else
		{
			scale = 9;
		}

		//double a = atan2(y1,x1) - atan2(y2, x2);
		//a = a * 360.0 / double(2 * M_PI);
		//a = (a < 0) ? a + 360.0 : a;

		//Note: h is in range [0,360] and l and s is in [0,1]
		cv::Mat labColor(1, 1, CV_32FC3, cv::Scalar(0.5, mvX, mvY));
		cv::Mat rgbColor;
		cv::cvtColor(labColor, rgbColor, cv::COLOR_Lab2BGR);
		cv::Scalar lineColor = cv::Scalar(rgbColor.data[0], rgbColor.data[1], rgbColor.data[2], 255);
		int lineThickness = 1;

		double angle = atan2(double(p.y) - q.y, double(p.x) - q.x);
		double hypotenuse = sqrt(square(p.y - q.y) + square(p.x - q.x));

		q.x = int(p.x - length * hypotenuse * cos(angle));
		q.y = int(p.y - length * hypotenuse * sin(angle));
		//draw length
		line(result, p, q, lineColor, lineThickness);

		p.x = int(q.x + scale * 3 * cos(angle + M_PI / 4));
		p.y = int(q.y + scale * 3 * sin(angle + M_PI / 4));
		//draw arrow bottom 
		line(result, p, q, lineColor, lineThickness);
		
		p.x = int(q.x + scale * 3 * cos(angle - M_PI / 4));
		p.y = int(q.y + scale * 3 * sin(angle - M_PI / 4));
		//draw arrow top 
		line(result, p, q, lineColor, lineThickness);

	}

	result.copyTo(_result);

}

void defuse::FlowDySIGXtractor::drawResidualImage(const cv::Mat _current, const cv::Mat _previous, cv::Mat& _result)
{
	if (_current.empty() || _current.empty() || _previous.empty() || _previous.empty())
		return;

	cv::Mat current, previous, result;
	_current.copyTo(current);
	_previous.copyTo(previous);
	result.create(current.size(), current.type());

	//result = previous - current;
	//cv::Mat diff1, diff2;
	cv::absdiff(current, previous, result);
	//cv::absdiff(previous, current, diff2);
	//result = diff1 + diff2;

	result.copyTo(_result);
}

void defuse::FlowDySIGXtractor::drawDynamicSamples(const cv::Mat _source, const cv::Mat _samples, cv::Mat& _result)
{
		if (_source.empty() || _samples.empty())
		return;

	cv::Mat source, result, samples;
	_source.copyTo(source);
	_result.create(source.size(), source.type());
	_result.copyTo(result);
	_samples.copyTo(samples);

	// compute max radius = one eigth of the length of the shorter dimension
	int maxRadius = ((source.rows < source.cols) ? source.rows : source.cols) / 8;

	// draw signature
	for (int i = 0; i < samples.rows; i++)
	{
		cv::Vec3f labColor(
			_samples.at<float>(i, indices.l) * ranges.l,
			_samples.at<float>(i, indices.a) * ranges.a,
			_samples.at<float>(i, indices.b) * ranges.b);

		cv::Mat labPixel(1, 1, CV_32FC3);
		labPixel.at<cv::Vec3f>(0, 0) = labColor;
		cv::Mat rgbPixel;
		cvtColor(labPixel, rgbPixel, cv::COLOR_Lab2BGR);
		rgbPixel.convertTo(rgbPixel, CV_8UC4, 255);
		rgbPixel.convertTo(rgbPixel, cv::COLOR_BGR2BGRA);
		cv::Vec4b rgbColor = rgbPixel.at<cv::Vec4b>(0, 0);	// end
		rgbColor[3] = 255;

		// precompute variables
		cv::Point center(int(samples.at<float>(i, indices.x) * source.cols), int(samples.at<float>(i, indices.y) * source.rows));

		int radius = 1;
		if (samples.at<float>(i, indices.w) < 0)
			radius = int(maxRadius * 0.05);
		else
			radius = maxRadius * samples.at<float>(i, indices.w);

		cv::Vec3b borderColor(0, 0, 0);
		int borderThickness(0);

		// draw filled circle
		//circle(result, center, radius, rgbColor, -1);
		// draw circle outline
		//circle(result, center, radius, borderColor, borderThickness);
		cv::Point axis(int(samples.at<float>(i, indices.x2) * source.cols), int(samples.at<float>(i, indices.y2) * source.rows));

		float mvX = std::abs(axis.x - center.x);
		float mvY = std::abs(axis.y - center.y);

		double angle = angleBetween(center, axis);

		cv::ellipse(result, center, cv::Size(mvX, mvY), angle, 0, 360, rgbColor);
	}

	result.copyTo(_result);
}

float defuse::FlowDySIGXtractor::angleBetween(const cv::Point& v1, const cv::Point& v2)
{

	float len1 = sqrt(v1.x * v1.x + v1.y * v1.y);
	float len2 = sqrt(v2.x * v2.x + v2.y * v2.y);

	float dot = v1.x * v2.x + v1.y * v2.y;

	float a = dot / (len1 * len2);

	if (a >= 1.0)
		return 0.0;
	else if (a <= -1.0)
		return M_PI;
	else
		return acos(a); // 0..PI
}


void defuse::FlowDySIGXtractor::getMotionSamples(
	std::vector<uchar>& statusVector, std::vector<float>& errorVector, 
	std::vector<cv::Point2f>& prevPoints, std::vector<cv::Point2f>& currPoints, 
	int height, int width, cv::Mat& out)
{
	cv::Mat samples;
	out.copyTo(samples);

	for (int iStatus = 0; iStatus < statusVector.size(); iStatus++)
	{
		cv::Point2f point = currPoints[iStatus];
		if (statusVector[iStatus] != 0)
		{
			samples.at<float>(iStatus, indices.x2) = point.x / float(width);
			samples.at<float>(iStatus, indices.y2) = point.y / float(height);

		}else
		{
			samples.at<float>(iStatus, indices.x2) = 0;
			samples.at<float>(iStatus, indices.y2) = 0;
		}

		if (point.x > width)
		{
			samples.at<float>(iStatus, indices.x2) = 1.0;
			point.x = width;
		}
		else if (point.x < 0)
		{
			samples.at<float>(iStatus, indices.x2) = 0;
			point.x = 0;
		}

		if (point.y > height)
		{
			samples.at<float>(iStatus, indices.y2) = 1.0;
			point.y = height;
		}
		else if (point.y < 0)
		{
			samples.at<float>(iStatus, indices.y2) = 0;
			point.y = 0;
		}

		currPoints[iStatus] = point;
	}

	samples.copyTo(out);
}

std::string defuse::FlowDySIGXtractor::toString()
{
	std::string base = SIGXtractor::toString();
	std::stringstream st;

	st << "; ";
	st << "xtractor: flow-based signatures, ";
	st << "maxFrames: ";
	st << mMaxFrames << ", ";
	st << "resetTracking: ";
	st << mResetTracking << ", ";
	std::string frameSelection;
	frameSelection = getFrameRatioAsString();
	base += frameSelection;
	base += st.str();
	return base;
}

std::string defuse::FlowDySIGXtractor::getXtractorID() const
{
	std::string base = SIGXtractor::getXtractorID();
	std::stringstream st;

	st << "_FFS";
	st << "_";
	st << mMaxFrames;
	st << "_";
	st << mResetTracking;
	st << "_";
	st << mFrameSelection;

	base += st.str();
	return base;
}

double defuse::FlowDySIGXtractor::square(int a) const
{
	return a * a;
}


