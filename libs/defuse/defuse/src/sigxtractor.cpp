#include "sigxtractor.hpp"
#include <cpluslogger.hpp>

#define _USE_MATH_DEFINES 
#include <math.h>

/**
 * Feature Signatures Extractor
 *
 */
defuse::SIGXtractor::SIGXtractor(
	std::vector<cv::Point2f> _samplepoints,
	int _initSeeds,
	int _initialCentroids,
	float lpdistance,
	int _iterations,
	int _minClusterSize,
	float _minDistance,
	float _dropThreshold,
	int _grayscaleBits,
	int _windowRadius)
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

	indices.x = 0;
	indices.y = 1;
	indices.l = 2;
	indices.a = 3;
	indices.b = 4;
	indices.c = 5;
	indices.e = 6;
	indices.w = 7;
	indices.dims = 8;

	ranges.l = 100;
	ranges.a = 127;
	ranges.b = 127;

	normalizer.c = 25.0f;
	normalizer.e = 4.0f;

	for (int i = 0; i < indices.dims; i++)
	{
		mWeights.push_back(1.0);
		mTranslations.push_back(0.0);
	}

	mP = lpdistance;
	if (mP == 1.0)
	{
		mDistance = &SIGXtractor::computeL1;
	}
	else if (mP == 2.0)
	{
		mDistance = &SIGXtractor::computeL2;
	}
	else
	{
		mDistance = &SIGXtractor::computeLp;
	}
}

defuse::FeaturesBase* defuse::SIGXtractor::xtract(VideoBase* _videobase)
{
	cv::VideoCapture stream(_videobase->mFile->getFile());

	if (!stream.isOpened())
	{
		LOG_FATAL("Error: Video Stream cannot be opened: " << _videobase->mFile->getFile());
	}

	//Output
	cv::Mat vectors;

	//Measure extraction time
	double e1Start = double(cv::getTickCount());
	computeStaticSignatures(stream, _videobase->mFile->getFile(), vectors);
	double e1End = double(cv::getTickCount());
	double elapsedSecs = (e1End - e1Start) / double(cv::getTickFrequency());

	FeaturesBase* features = new FeaturesBase(_videobase->mFile->getFilename(), vectors);
	features->mExtractionTime = float(elapsedSecs);

	stream.release();
	return features;
}

void defuse::SIGXtractor::setLpDistance(float lpdistance)
{
	mP = lpdistance;
	if (mP == 1.0)
	{
		mDistance = &SIGXtractor::computeL1;
	}
	else if (mP == 2.0)
	{
		mDistance = &SIGXtractor::computeL2;
	}
	else
	{
		mDistance = &SIGXtractor::computeLp;
	}
}

float defuse::SIGXtractor::getLpDistance()
{
	return mP;
}

bool defuse::SIGXtractor::computeStaticSignatures(cv::VideoCapture& _video, std::string filename, cv::OutputArray _signatures) const
{
	int framecnt = int(_video.get(CV_CAP_PROP_FRAME_COUNT));
	int width = int(_video.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = int(_video.get(CV_CAP_PROP_FRAME_HEIGHT));

	int framenr = framecnt;

	if (mKeyFrameSelection == SIGXtractor::KeyFrameSelection::MiddleFrame) //use middle frame
	{
		framenr = int(framecnt / float(2));
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else if (mKeyFrameSelection == SIGXtractor::KeyFrameSelection::FirstFrame) //use first frame
	{
		framenr = 1;
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else if (mKeyFrameSelection == SIGXtractor::KeyFrameSelection::LastFrame) //use last frame
	{
		_video.set(CV_CAP_PROP_POS_FRAMES, framenr);
	}
	else
	{
		LOG_FATAL("Keyframe selection " << mKeyFrameSelection << " not implemented: Use middle, first or last frame");
		return false;
	}

	cv::Mat image, signatures;

	_video.grab();
	_video.retrieve(image);

	std::vector<cv::Point2f> initPoints;
	deNormalizePoints(mSamplepoints, width, height, initPoints);

	cv::Mat samples;
	samples.create(int(initPoints.size()), indices.dims, CV_32F);

	getSamples(image, initPoints, samples);
	getClusters(samples, signatures);
	signatures.copyTo(_signatures);

	if (mDisaply || mSaveDisplay)
	{
		cv::Mat sampleImage, signaturesImage;
		drawSamples(image, samples, sampleImage);
		drawSignatures(image, signatures, signaturesImage);

		char keyPressed = 0;
		if (mDisaply)
		{
			showImage(image, cv::Size(width, height), 0, 0, "Frame");
			showImage(sampleImage, cv::Size(width, height), width, 0, "Samples");
			showImage(signaturesImage, cv::Size(width, height), width * 2, 0, "Signatures");
			keyPressed = cv::waitKey(0);
		}

		if (keyPressed == 'p' || mSaveDisplay)
		{
			File file(filename);
			file.setPath(mOutput->getPath());
			saveImage(file.getFile(), ".jpg", "image", image);
			saveImage(file.getFile(), ".png", "sampleImage", sampleImage);
			saveImage(file.getFile(), ".png", "signaturesImage", signaturesImage);
		}
	}

	return true;
}

void defuse::SIGXtractor::deNormalizePoints(std::vector<cv::Point2f> samplepoints, int width, int height, std::vector<cv::Point2f>& output) const
{
	output.resize(samplepoints.size());

	//De-Normalize
	for (int i = 0; i < samplepoints.size(); i++)
	{
		cv::Point p(0, 0);
		p.x = static_cast<int>(samplepoints.at(i).x * (width - 1) + 0.5);
		p.y = static_cast<int>(samplepoints.at(i).y * (height - 1) + 0.5);
		output.at(i) = p;
	}
}


void defuse::SIGXtractor::getSamples(cv::Mat& frame, std::vector<cv::Point2f> points, cv::Mat& out) const
{
	cv::Mat samples;
	out.copyTo(samples);

	//cv::xfeatures2d::pct_signatures::GrayscaleBitmap grayscaleBitmap(frame, mGrayscaleBits);

	std::vector<std::uint32_t> histogram;
	std::vector<std::uint32_t> data;
	initGrayscaleBitmap(frame, data, histogram);

	std::vector<cv::Point2f> corners;
	
	for (int iSample = 0; iSample < points.size(); iSample++)
	{
		int x = static_cast<int>(points[iSample].x);
		int y = static_cast<int>(points[iSample].y);

		if (x == frame.cols)
			x = x - 1;
		if (y == frame.rows)
			y = y - 1;

		samples.at<float>(iSample, indices.x) = static_cast<float>(x / static_cast<float>(frame.cols) * mWeights[indices.x] + mTranslations[indices.x]);
		samples.at<float>(iSample, indices.y) = static_cast<float>(y / static_cast<float>(frame.rows) * mWeights[indices.y] + mTranslations[indices.y]);

		cv::Mat rgbPixel(frame, cv::Rect(x, y, 1, 1));
		cv::Mat labPixel;
		rgbPixel.convertTo(rgbPixel, CV_32FC3, 1.0 / 255);
		cv::cvtColor(rgbPixel, labPixel, cv::COLOR_BGR2Lab);
		cv::Vec3f labColor = labPixel.at<cv::Vec3f>(0, 0);

		samples.at<float>(iSample, indices.l) = static_cast<float>(std::floor(labColor[0] + 0.5) / static_cast<float>(ranges.l) * mWeights[indices.l] + mTranslations[indices.l]);
		samples.at<float>(iSample, indices.a) = static_cast<float>(std::floor(labColor[1] + 0.5) / static_cast<float>(ranges.a) * mWeights[indices.a] + mTranslations[indices.a]);
		samples.at<float>(iSample, indices.b) = static_cast<float>(std::floor(labColor[2] + 0.5) / static_cast<float>(ranges.b) * mWeights[indices.b] + mTranslations[indices.b]);

		double contrast = 0.0, entropy = 0.0;
		getContrastEntropy(x, y, frame.cols, frame.rows, data, histogram, contrast, entropy, mWindowRadius);
		samples.at<float>(iSample, indices.c) = static_cast<float>(contrast / static_cast<float>(normalizer.c) * mWeights[indices.c] + mTranslations[indices.c]);
		samples.at<float>(iSample, indices.e) = static_cast<float>(entropy / static_cast<float>(normalizer.e) * mWeights[indices.e] + mTranslations[indices.e]);

		samples.copyTo(out);
	}

}

void defuse::SIGXtractor::getClusters(cv::Mat& samples, cv::Mat& signatures) const
{

	// Prepare initial centroids.
	cv::Mat clusters;
	// make seeds from the first mInitSeeds samples
	samples(cv::Rect(0, 0, samples.cols, mMaxClusters)).copyTo(clusters);
	// set initial weight to 1
	clusters(cv::Rect(indices.w, 0, 1, clusters.rows)) = 1;					

	joinCloseClusters(clusters);
	dropLightPoints(clusters);

	// Main iterations cycle. Our implementation has fixed number of iterations.
	for (int iteration = 0; iteration < this->mIterations; iteration++)
	{
		// Prepare space for new centroid values.
		cv::Mat tmpCentroids(clusters.size(), clusters.type());
		tmpCentroids = 0;

		// Clear weights for new iteration.
		clusters(cv::Rect(indices.w, 0, 1, clusters.rows)) = 0;

		// Compute affiliation of points and sum new coordinates for centroids.
		for (int iSample = 0; iSample < samples.rows; iSample++)
		{
			int iClosest = findClosestCluster(clusters, samples, iSample);
			for (int iDimension = 0; iDimension < indices.dims - 1; iDimension++)
			{
				tmpCentroids.at<float>(iClosest, iDimension) += samples.at<float>(iSample, iDimension);
			}
			clusters.at<float>(iClosest, indices.w)++;
		}

		// Compute average from tmp coordinates and throw away too small clusters.
		int lastIdx = 0;
		for (int i = 0; (int)i < tmpCentroids.rows; ++i)
		{
			// Skip clusters that are too small (large-enough clusters are packed right away)
			if (clusters.at<float>(i, indices.w) >(iteration + 1) * this->mMinimalClusterSize)
			{
				for (int d = 0; d < indices.dims - 1; d++)
				{
					clusters.at<float>(lastIdx, d) = tmpCentroids.at<float>(i, d) / clusters.at<float>(i, indices.w);
				}
				// weights must be compacted as well
				clusters.at<float>(lastIdx, indices.w) = clusters.at<float>(i, indices.w);
				lastIdx++;
			}
		}

		// Crop the arrays if some centroids were dropped.
		clusters.resize(lastIdx);
		if (clusters.rows == 0)
		{
			break;
		}

		// Finally join clusters with too close centroids.
		joinCloseClusters(clusters);
		dropLightPoints(clusters);
	}

	// The result must not be empty!
	if (clusters.rows == 0)
	{
		singleClusterFallback(samples, clusters);
	}

	cropClusters(clusters);

	normalizeWeights(clusters);

	// save the result
	signatures.create(clusters.rows, indices.dims, clusters.type());
	clusters.copyTo(signatures);
}

void defuse::SIGXtractor::joinCloseClusters(cv::Mat clusters) const
{
	for (int i = 0; i < clusters.rows - 1; i++)
	{
		if (clusters.at<float>(i, indices.w) == 0)
		{
			continue;
		}

		for (int j = i + 1; j < clusters.rows; j++)
		{
			if (clusters.at<float>(j, indices.w) > 0 && (*this.*mDistance)(clusters, i, clusters, j) <= mMinimalDistance)
			{
				clusters.at<float>(i, indices.w) = 0;
				break;
			}
		}
	}
}

void defuse::SIGXtractor::dropLightPoints(cv::Mat& clusters) const
{
	int frontIdx = 0;

	// Skip leading continuous part of weighted-enough points.
	while (frontIdx < clusters.rows && clusters.at<float>(frontIdx, indices.w) > mClusterDropThreshold)
	{
		++frontIdx;
	}

	// Mark first emptied position and advance front index.
	int tailIdx = frontIdx++;

	while (frontIdx < clusters.rows)
	{
		if (clusters.at<float>(frontIdx, indices.w) > mClusterDropThreshold)
		{
			// Current (front) item is not dropped -> copy it to the tail.
			clusters.row(frontIdx).copyTo(clusters.row(tailIdx));
			++tailIdx;	// grow the tail
		}
		++frontIdx;
	}
	clusters.resize(tailIdx);
}

int defuse::SIGXtractor::findClosestCluster(cv::Mat& clusters, cv::Mat& points, int pointIdx) const
{
	int iClosest = 0;
	float minDistance = (*this.*mDistance)(clusters, 0, points, pointIdx);
	for (int iCluster = 1; iCluster < clusters.rows; iCluster++)
	{
		float distance = (*this.*mDistance)(clusters, iCluster, points, pointIdx);
		if (distance < minDistance)
		{
			iClosest = iCluster;
			minDistance = distance;
		}
	}
	return iClosest;
}

void defuse::SIGXtractor::normalizeWeights(cv::Mat& clusters) const
{
	// get max weight
	float maxWeight = clusters.at<float>(0, indices.w);
	for (int i = 1; i < clusters.rows; i++)
	{
		if (clusters.at<float>(i, indices.w) > maxWeight)
		{
			maxWeight = clusters.at<float>(i, indices.w);
		}
	}

	// normalize weight
	float weightNormalizer = 1 / maxWeight;
	for (int i = 0; i < clusters.rows; i++)
	{
		clusters.at<float>(i, indices.w) = clusters.at<float>(i, indices.w) * weightNormalizer;
	}
}

void defuse::SIGXtractor::singleClusterFallback(const cv::Mat& points, cv::Mat& clusters) const
{
	if (clusters.rows != 0)
	{
		return;
	}

	// Initialize clusters.
	clusters.resize(1);
	clusters.at<float>(0, indices.w) = static_cast<float>(points.rows);

	// Sum all points.
	for (int i = 0; i < points.rows; ++i)
	{
		for (int d = 0; d < indices.dims - 1; d++)
		{
			clusters.at<float>(0, d) += points.at<float>(i, d);
		}
	}

	// Divide centroid by number of points -> compute average in each dimension.
	for (int i = 0; i < points.rows; ++i)
	{
		for (int d = 0; d < indices.dims - 1; d++)
		{
			clusters.at<float>(0, d) = clusters.at<float>(0, d) / clusters.at<float>(0, indices.w);
		}
	}
}

void defuse::SIGXtractor::cropClusters(cv::Mat& clusters) const
{
	if (clusters.rows > mMaxClusters)
	{
		cv::Mat duplicate;						// save original clusters
		clusters.copyTo(duplicate);

		cv::Mat sortedIdx;						// sort using weight column
		sortIdx(clusters(cv::Rect(indices.dims - 1, 0, 1, clusters.rows)), sortedIdx, cv::SORT_EVERY_COLUMN + cv::SORT_DESCENDING);

		clusters.resize(mMaxClusters);		// crop to max clusters
		for (int i = 0; i < mMaxClusters; i++)
		{									// copy sorted rows
			duplicate.row(sortedIdx.at<int>(i, 0)).copyTo(clusters.row(i));
		}
	}
}

float defuse::SIGXtractor::computeL1(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const
{
	double result = 0.0;

	//without weights - 1!
	for (int i = 0; i < _f1.cols - 1; i++)
	{
		double distance = _f1.at<float>(idx1, i) - _f2.at<float>(idx2, i);
		result += std::abs(distance);
	}

	return float(result);
}

float defuse::SIGXtractor::computeL2(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const
{
	double result = 0.0;

	//without weights - 1!
	for (int i = 0; i < _f1.cols - 1; i++)
	{
		double distance = _f1.at<float>(idx1, i) - _f2.at<float>(idx2, i);
		result += distance * distance;
	}

	result = std::sqrt(result);
	return float(result);
}

float defuse::SIGXtractor::computeLp(cv::Mat& _f1, int idx1, cv::Mat& _f2, int idx2) const
{
	double result = 0.0;

	//without weights - 1!
	for (int i = 0; i < _f1.cols - 1; i++)
	{
		double distance = _f1.at<float>(idx1, i) - _f2.at<float>(idx2, i);
		result += std::pow(std::abs(distance),1);
	}

	result = std::pow(result, float(1.0) / 1);
	return float(result);
}

void defuse::SIGXtractor::showProgress(int _step, int _total) const
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

std::string defuse::SIGXtractor::toString() const
{
	std::stringstream st;

	st << "xtractor: static signatures, ";
	st << "initSeeds: ";
	st << mInitSeeds << ", ";
	st << "initialCentroids: ";
	st << mMaxClusters << ", ";
	st << "lpDistance: ";
	st << mP << ", ";
	st << "iterations: ";
	st << mIterations << ", ";
	st << "minClusterSize: ";
	st << mMinimalClusterSize << ", ";
	st << "minDistance: ";
	st << mMinimalDistance << ", ";
	st << "dropThreshold: ";
	st << mClusterDropThreshold << ", ";
	st << "grayscaleBits: ";
	st << mGrayscaleBits << ", ";
	st << "windowRadius: ";
	st << mWindowRadius << ", ";
	st << "keyframeselection: ";

	if(mKeyFrameSelection == KeyFrameSelection::FirstFrame)
	{
		st << "first";
	}
	else if(mKeyFrameSelection == KeyFrameSelection::MiddleFrame)
	{
		st << "middle";
	}
	else if (mKeyFrameSelection == KeyFrameSelection::LastFrame)
	{
		st << "last";
	}

	return st.str();
}

std::string defuse::SIGXtractor::getXtractorID() const
{
	std::stringstream st;

	st << "SFS";
	st << "_";
	st << mInitSeeds;
	st << "_";
	st << mMaxClusters;
	st << "_";
	st << mP;
	st << "_";
	st << mIterations;
	st << "_";
	st << mMinimalClusterSize;
	st << "_";
	st << mMinimalDistance;
	st << "_";
	st << mClusterDropThreshold;
	st << "_";
	st << mGrayscaleBits;
	st << "_";
	st << mWindowRadius;
	st << "_";
	st << mKeyFrameSelection;

	return st.str();
}

void defuse::SIGXtractor::initGrayscaleBitmap(cv::Mat image, std::vector<std::uint32_t>& data, std::vector<std::uint32_t>& histogram) const
{
	cv::Mat grayscaleBitmap;
	cv::Mat bitmap;
	image.copyTo(bitmap);
	bitmap.convertTo(bitmap, CV_16U, 257);

	cvtColor(bitmap, grayscaleBitmap, cv::COLOR_BGR2GRAY);

	int width = bitmap.cols;
	int height = bitmap.rows;

	std::size_t pixelsPerItem = 32 / mGrayscaleBits;
	data.resize((width * height + pixelsPerItem - 1) / pixelsPerItem);

	for (std::size_t y = 0; y < height; y++)
	{
		for (std::size_t x = 0; x < width; x++)
		{
			std::uint32_t grayVal = static_cast<std::uint32_t>(grayscaleBitmap.at<std::uint16_t>(static_cast<int>(y), static_cast<int>(x))) >> (16 - mGrayscaleBits);
			setPixel(x, y, width, data, grayVal);
		}
	}

	histogram.resize(1 << (mGrayscaleBits * 2));
}

void defuse::SIGXtractor::getContrastEntropy(std::size_t x, std::size_t y, int width, int height, std::vector<std::uint32_t>& data, 
	std::vector<std::uint32_t>& histogram, double& contrast, double& entropy, std::size_t windowRadius) const
{
	std::size_t fromX = (x > windowRadius) ? x - windowRadius : 0;
	std::size_t fromY = (y > windowRadius) ? y - windowRadius : 0;
	std::size_t toX = std::min<std::size_t>(width - 1, x + windowRadius + 1);
	std::size_t toY = std::min<std::size_t>(height - 1, y + windowRadius + 1);

	for (std::size_t j = fromY; j < toY; ++j)
	{
		for (std::size_t i = fromX; i < toX; ++i)							// for each pixel in the window
		{
			updateHistogram(getPixel(i, j, width, data), getPixel(i, j + 1, width, data), histogram);			// match every pixel with all 8 its neighbours
			updateHistogram(getPixel(i, j, width, data), getPixel(i + 1, j, width, data), histogram);
			updateHistogram(getPixel(i, j, width, data), getPixel(i + 1, j + 1, width, data), histogram);
			updateHistogram(getPixel(i + 1, j, width, data), getPixel(i, j + 1, width, data), histogram);		// 4 updates per pixel in the window
		}
	}

	contrast = 0.0;
	entropy = 0.0;

	std::uint32_t pixelsScale = 1 << mGrayscaleBits;
	double normalizer = (double)((toX - fromX) * (toY - fromY) * 4);				// four increments per pixel in the window (see above)
	for (std::size_t j = 0; j < pixelsScale; ++j)								// iterate row in a 2D histogram
	{
		for (std::size_t i = 0; i <= j; ++i)									// iterate column up to the diagonal in 2D histogram
		{
			if (histogram[j*pixelsScale + i] != 0) 							// consider only non-zero values
			{
				double value = (double)histogram[j*pixelsScale + i] / normalizer;	// normalize value by number of histogram updates
				contrast += (i - j) * (i - j) * value;		// compute contrast
				entropy -= value * std::log(value);			// compute entropy
				histogram[j*pixelsScale + i] = 0;			// clear the histogram array for the next computation
			}
		}
	}
}

std::uint32_t defuse::SIGXtractor::getPixel(std::size_t x, std::size_t y, int width, std::vector<std::uint32_t>& data) const
{
	std::size_t pixelsPerItem = 32 / mGrayscaleBits;
	std::size_t offset = y * width + x;
	std::size_t shift = (offset % pixelsPerItem) * mGrayscaleBits;
	std::uint32_t mask = (1 << mGrayscaleBits) - 1;
	return (data[offset / pixelsPerItem] >> shift) & mask;
}

void defuse::SIGXtractor::setPixel(std::size_t x, std::size_t y, int width, std::vector<std::uint32_t>& data, std::uint32_t val) const
{
	std::size_t pixelsPerItem = 32 / mGrayscaleBits;
	std::size_t offset = y * width + x;
	std::size_t shift = (offset % pixelsPerItem) * mGrayscaleBits;
	std::uint32_t mask = (1 << mGrayscaleBits) - 1;
	val &= mask;
	data[offset / pixelsPerItem] &= ~(mask << shift);
	data[offset / pixelsPerItem] |= val << shift;
}

void defuse::SIGXtractor::updateHistogram(std::uint32_t a, std::uint32_t b, std::vector<std::uint32_t>& histogram) const
{
	int offset = (int)((a > b) ? (a << mGrayscaleBits) + b : a + (b << mGrayscaleBits));	// merge to a variable with greater higher bits
	histogram[offset]++;												// to accumulate just in a triangle in 2D histogram for efficiency
}

void defuse::SIGXtractor::drawSamples(const cv::Mat _source, const cv::Mat _samples, cv::Mat& _result) const
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
		circle(result, center, radius, rgbColor, -1);
		// draw circle outline
		//circle(result, center, radius, borderColor, borderThickness);
	}

	result.copyTo(_result);
}

void defuse::SIGXtractor::drawSignatures(const cv::Mat _source, const cv::Mat _signature, cv::Mat& _result) const
{
	if (_source.empty() || _signature.empty())
		return;

	cv::Mat result;

	_result.copyTo(result);

	drawSamples(_source, _signature, result);
	result.copyTo(_result);
}

void defuse::SIGXtractor::showImage(const cv::Mat _image, cv::Size _size, int x, int y, std::string name) const
{
	cv::Mat resImage;

	cv::resize(_image, resImage, _size);
	cv::imshow(name, resImage);
	cv::moveWindow(name, x, y);
}

void defuse::SIGXtractor::saveImage(std::string file, std::string fileextension, std::string name,cv::Mat& image) const
{
	File f(file);
	f.setFilenameWithoutExtension(f.getFilename() + "_" + name);
	f.setFileExtension(fileextension);
	cv::imwrite(f.getFile(), image);
}
