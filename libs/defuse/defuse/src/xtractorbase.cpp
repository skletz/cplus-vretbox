#include "xtractorbase.hpp"
#include <iostream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video/tracking.hpp>

void defuse::XtractorBase::showProgress(int _step, int _total) const
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

int defuse::XtractorBase::getKeyframeNumber(int framecnt) const
{
	int keyframe = 1;
	if (mKeyFrameSelection == MiddleFrame)
	{
		keyframe = int(framecnt / float(2));
	}
	else if (mKeyFrameSelection == FirstFrame)
	{
		keyframe = 1;
	}
	else if (mKeyFrameSelection == LastFrame)
	{
		keyframe = framecnt - 1;
	}
	else
	{
		LOG_FATAL("Keyframe selection " << mKeyFrameSelection << " not implemented: Use middle, first or last frame");
	}

	return keyframe;
}

void defuse::XtractorBase::getFrameRatio(int framecnt, int maxframes, int& step, int& frameSize) const
{
	int tmpStep = 1;
	int tmpFrameSize = framecnt;
	if (mFrameSelection == FramesPerVideo)
	{
		tmpStep = static_cast<int>(framecnt / float(maxframes));
		tmpFrameSize = (tmpStep * maxframes) - 1;

	}
	else if (mFrameSelection == FramesPerSecond)
	{
		tmpFrameSize = framecnt;
		if (maxframes < framecnt)
		{
			tmpStep = maxframes;
		}
	}
	else
	{
		LOG_INFO("All frames are used");
		tmpFrameSize = framecnt;
		tmpStep = 1;
	}

	if (tmpStep == 0)
	{
		tmpFrameSize = framecnt;
		tmpStep = 1;
	}

	step = tmpStep;
	frameSize = tmpFrameSize;
}

std::string defuse::XtractorBase::getKeyframeSelectionAsString() const
{
	std::stringstream st;
	st << "keyframeselection: ";

	if (mKeyFrameSelection == FirstFrame)
	{
		st << "first";
	}
	else if (mKeyFrameSelection == MiddleFrame)
	{
		st << "middle";
	}
	else if (mKeyFrameSelection == LastFrame)
	{
		st << "last";
	}

	return st.str();
}

std::string defuse::XtractorBase::getFrameRatioAsString() const
{
	std::stringstream st;
	st << "frameSelection: ";
	if (mFrameSelection == FramesPerVideo)
	{
		st << "framespervideo";
	}
	else if (mFrameSelection == FramesPerSecond)
	{
		st << "framespersecond";
	}else if(mFrameSelection == All)
	{
		st << "all";
	}
	return st.str();
}

void defuse::XtractorBase::showImage(const cv::Mat _image, cv::Size _size, int x, int y, std::string name) const
{
	cv::Mat resImage;

	cv::resize(_image, resImage, _size);
	cv::imshow(name, resImage);
	cv::moveWindow(name, x, y);
}

void defuse::XtractorBase::saveImage(std::string file, std::string fileextension, std::string name, cv::Mat& image) const
{
	File f(file);
	f.setFilenameWithoutExtension(f.getFilename() + "_" + name);
	f.setFileExtension(fileextension);
	cv::imwrite(f.getFile(), image);
}
