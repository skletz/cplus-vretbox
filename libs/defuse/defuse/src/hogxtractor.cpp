#include "hogxtractor.hpp"
#include <cpluslogger.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

defuse::HOGXtractor::HOGXtractor()
{
	mWinSize = cv::Size(128,64);
	mBlockSize = cv::Size(16, 16);
	mBlockStride = cv::Size(8, 8);
	mCellSize = cv::Size(8, 8);
}

defuse::FeaturesBase* defuse::HOGXtractor::xtract(VideoBase* _videobase)
{

	cv::VideoCapture stream(_videobase->mFile->getFile());

	if (!stream.isOpened())
	{
		LOG_FATAL("Error: Video Stream cannot be opened: " << _videobase->mFile->getFile());
	}

	//Output
	cv::Mat vectors;

	//Measure extraction time
	double elapsedSecs = computecvHOGDescriptor(stream, _videobase->mFile->getFile(), vectors);

	FeaturesBase* features = new FeaturesBase(_videobase->mFile->getFilename(), vectors);
	features->mExtractionTime = float(elapsedSecs);

	stream.release();
	return features;
}

std::string defuse::HOGXtractor::toString()
{
	std::string result;
	result += "xtractor: HOG, ";
	result += getKeyframeSelectionAsString();
	return result;
}

std::string defuse::HOGXtractor::getXtractorID() const
{
	std::stringstream st;
	st << "HOG_";
	st << mKeyFrameSelection;
	return st.str();
}

float defuse::HOGXtractor::computecvHOGDescriptor(cv::VideoCapture& _videobase, std::string filename, cv::OutputArray& _descriptor)
{
	int framecnt = int(_videobase.get(CV_CAP_PROP_FRAME_COUNT));
	int width = int(_videobase.get(CV_CAP_PROP_FRAME_WIDTH));
	int height = int(_videobase.get(CV_CAP_PROP_FRAME_HEIGHT));

	int framenr = getKeyframeNumber(framecnt);
	_videobase.set(CV_CAP_PROP_POS_FRAMES, framenr);

	cv::Mat image, hog;

	_videobase.grab();
	_videobase.retrieve(image);

	//Measure extraction time
	double elapsedSecs = computeHogFeatures(image, hog, filename);

	hog.copyTo(_descriptor);

	return elapsedSecs;
}

float defuse::HOGXtractor::computeHogFeatures(cv::Mat& image, cv::Mat& descriptor, std::string filename)
{
	cv::Mat tmpImg;
	cv::Size imgSize = mWinSize;
	if (image.cols != mWinSize.width && image.rows != mWinSize.height) {
		resize(image, tmpImg, imgSize);
	}
	/*image.copyTo(tmpImg);*/

	cv::HOGDescriptor hog;
	hog.winSize = imgSize;

	std::vector<float> descriptorsValues;
	std::vector<cv::Point> locations;
	cv::Mat gray;
	cv::cvtColor(tmpImg, gray, CV_BGR2GRAY);

	//Measure extraction time
	double e1Start = double(cv::getTickCount());
	hog.compute(gray, descriptorsValues, cv::Size(), cv::Size(), locations);

	cv::Mat hogfeat(1, descriptorsValues.size(), CV_32FC1);
	for(int i = 0; i < descriptorsValues.size(); i++)
	{
		hogfeat.at<float>(0,i) = descriptorsValues.at(i);
	}
	double e1End = double(cv::getTickCount());
	double elapsedSecs = (e1End - e1Start) / double(cv::getTickFrequency());

	if (mDisaply || mSaveDisplay)
	{
		cv::Mat hogImage;
		visualizeHOGDescriptor(tmpImg, descriptorsValues, imgSize, mCellSize, 2, 1, hogImage);

		char keyPressed = 0;
		if (mDisaply)
		{
			showImage(tmpImg, imgSize, 0, 0, "Frame");
			showImage(hogImage, cv::Size(hogImage.cols, hogImage.rows), image.cols, 0, "HOG Features");

			keyPressed = cv::waitKey(0);
		}

		if (keyPressed == 'p' || mSaveDisplay)
		{
			File file(filename);
			file.setPath(mOutput->getPath());
			saveImage(file.getFile(), ".jpg", "image", tmpImg);
			saveImage(file.getFile(), ".jpg", "hogFeatures", hogImage);
		}
	}

	hogfeat.copyTo(descriptor);
	return elapsedSecs;
}

void defuse::HOGXtractor::visualizeHOGDescriptor(
	cv::Mat& origImg, std::vector<float>& descriptorValues, 
	cv::Size winSize, cv::Size cellSize, int scaleFactor, 
	double viz_factor, cv::Mat& image)
{
	cv::Mat visual_image;
	resize(origImg, visual_image, cv::Size(origImg.cols*scaleFactor, origImg.rows*scaleFactor));

	int gradientBinSize = 9;
	// dividing 180° into 9 bins, how large (in rad) is one bin?
	float radRangeForOneBin = 3.14 / (float)gradientBinSize;

	// prepare data structure: 9 orientation / gradient strenghts for each cell
	int cells_in_x_dir = winSize.width / cellSize.width;
	int cells_in_y_dir = winSize.height / cellSize.height;
	int totalnrofcells = cells_in_x_dir * cells_in_y_dir;
	float*** gradientStrengths = new float**[cells_in_y_dir];
	int** cellUpdateCounter = new int*[cells_in_y_dir];
	for (int y = 0; y<cells_in_y_dir; y++)
	{
		gradientStrengths[y] = new float*[cells_in_x_dir];
		cellUpdateCounter[y] = new int[cells_in_x_dir];
		for (int x = 0; x<cells_in_x_dir; x++)
		{
			gradientStrengths[y][x] = new float[gradientBinSize];
			cellUpdateCounter[y][x] = 0;
			for (int bin = 0; bin<gradientBinSize; bin++)
				gradientStrengths[y][x][bin] = 0.0;
		}
	}
	// nr of blocks = nr of cells - 1
	// since there is a new block on each cell (overlapping blocks!) but the last one
	int blocks_in_x_dir = cells_in_x_dir - 1;
	int blocks_in_y_dir = cells_in_y_dir - 1;
	// compute gradient strengths per cell
	int descriptorDataIdx = 0;
	int cellx = 0;
	int celly = 0;

	for (int blockx = 0; blockx<blocks_in_x_dir; blockx++)
	{
		for (int blocky = 0; blocky<blocks_in_y_dir; blocky++)
		{
			// 4 cells per block ...
			for (int cellNr = 0; cellNr<4; cellNr++)
			{
				// compute corresponding cell nr
				int cellx = blockx;
				int celly = blocky;
				if (cellNr == 1) celly++;
				if (cellNr == 2) cellx++;
				if (cellNr == 3)
				{
					cellx++;
					celly++;
				}

				for (int bin = 0; bin<gradientBinSize; bin++)
				{
					float gradientStrength = descriptorValues[descriptorDataIdx];
					descriptorDataIdx++;
					gradientStrengths[celly][cellx][bin] += gradientStrength;
				} // for (all bins)

				  // note: overlapping blocks lead to multiple updates of this sum!
				  // we therefore keep track how often a cell was updated,
				  // to compute average gradient strengths
				cellUpdateCounter[celly][cellx]++;

			} // for (all cells)
		} // for (all block x pos)
	} // for (all block y pos)

	  // compute average gradient strengths
	for (int celly = 0; celly<cells_in_y_dir; celly++)
	{
		for (int cellx = 0; cellx<cells_in_x_dir; cellx++)
		{
			float NrUpdatesForThisCell = (float)cellUpdateCounter[celly][cellx];
			// compute average gradient strenghts for each gradient bin direction
			for (int bin = 0; bin<gradientBinSize; bin++)
			{
				gradientStrengths[celly][cellx][bin] /= NrUpdatesForThisCell;
			}
		}
	}
	std::cout << "descriptorDataIdx = " << descriptorDataIdx << std::endl;

	// draw cells
	for (int celly = 0; celly<cells_in_y_dir; celly++)
	{
		for (int cellx = 0; cellx<cells_in_x_dir; cellx++)
		{
			int drawX = cellx * cellSize.width;
			int drawY = celly * cellSize.height;

			int mx = drawX + cellSize.width / 2;
			int my = drawY + cellSize.height / 2;

			cv::rectangle(visual_image,
			              cv::Point(drawX*scaleFactor, drawY*scaleFactor),
			              cv::Point((drawX + cellSize.width)*scaleFactor,
				(drawY + cellSize.height)*scaleFactor),
				CV_RGB(100, 100, 100),
				1);

			// draw in each cell all 9 gradient strengths
			for (int bin = 0; bin<gradientBinSize; bin++)
			{
				float currentGradStrength = gradientStrengths[celly][cellx][bin];

				// no line to draw?
				if (currentGradStrength == 0)
					continue;

				float currRad = bin * radRangeForOneBin + radRangeForOneBin / 2;

				float dirVecX = cos(currRad);
				float dirVecY = sin(currRad);
				float maxVecLen = cellSize.width / 2;
				float scale = viz_factor; // just a visual_imagealization scale,
										  // to see the lines better

										  // compute line coordinates
				float x1 = mx - dirVecX * currentGradStrength * maxVecLen * scale;
				float y1 = my - dirVecY * currentGradStrength * maxVecLen * scale;
				float x2 = mx + dirVecX * currentGradStrength * maxVecLen * scale;
				float y2 = my + dirVecY * currentGradStrength * maxVecLen * scale;

				// draw gradient visual_imagealization
				line(visual_image,
				     cv::Point(x1*scaleFactor, y1*scaleFactor),
				     cv::Point(x2*scaleFactor, y2*scaleFactor),
					CV_RGB(0, 0, 255),
					1);

			} // for (all bins)

		} // for (cellx)
	} // for (celly)

	  // don't forget to free memory allocated by helper data structures!
	for (int y = 0; y<cells_in_y_dir; y++)
	{
		for (int x = 0; x<cells_in_x_dir; x++)
		{
			delete[] gradientStrengths[y][x];
		}
		delete[] gradientStrengths[y];
		delete[] cellUpdateCounter[y];
	}
	delete[] gradientStrengths;
	delete[] cellUpdateCounter;
	visual_image.copyTo(image);
}
