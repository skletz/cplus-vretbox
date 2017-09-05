#ifndef _DEFUSE_COMOXTRACTOR_HPP_
#define _DEFUSE_COMOXTRACTOR_HPP_

#include <opencv2/opencv.hpp>
#include "xtractorbase.hpp"
#include "fuzzificator.hpp"

/**
* COMO Extractor (Color Moments)
* This class implements the extraction of color moments.
* Parts of the source code are based on the JAVA implemenation of COMO using LIRE.
*
* Original implementation of COMO:
* @title: COMO source code
* @author Savvas Chatzichristofis, Nektarios Anagnostopoulos
* @date 2017
* @version 1.0
* @availability http://bit.ly/2x6tIMF
*
* Adapted and modified:
* @author skletz
* @version 1.0 05/09/2017
*/
namespace defuse {
	
	class COMOXtractor : public XtractorBase
	{
		double TEXTURE_DEFINITION_TABLE[6][7] = {
			{ 0.0012801877159444332, 1.2811997974150548E-8, 2.931517692794886E-11, 2.3361585476863737E-11, 1.0229383520567805E-21, -4.741043401863473E-16, 3.489450463009515E-23 },
			{ 0.030098519274221386, 9.553200793114547E-6, 2.7312098265223947E-7, 1.5846418079328335E-6, -2.6894242032236695E-12, 5.431901588135399E-9, -1.6868751727422065E-12 },
			{ 0.0018139530164278453, 5.941872634495257E-8, 1.4200444288697612E-10, 1.5023611260922216E-10, 1.3373151439243982E-20, 1.1760743231129731E-14, 1.3487898261349881E-20 },
			{ 0.004440612110292505, 6.320931310602498E-7, 5.4004430112564E-9, 1.0423679617819742E-8, -2.7085204326667915E-16, 1.7090726156377653E-11, 9.51706439192856E-16 },
			{ 9.639630464555355E-4, 7.859161865855281E-9, 1.1230404627214495E-11, 8.291087328844257E-12, 1.372616300158933E-22, -4.3513261260527616E-16, -3.862637133906026E-24 },
			{ 0.10135058748498203, 4.290771163627242E-5, 6.178005148446455E-6, 1.9548008476574724E-5, 4.4473620282147234E-11, 1.6752667650020017E-7, 2.758163163459048E-11 }
		};

		double THRESHOLDS_TABLE[6] = {
			0.005, 0.005, 0.005, 0.0195, 0.15, 0.99
		};

		double QUANTIZATION_TABLE[6][8] = {
			{ 0.0, 2.2979840251251352E-4, 0.003657613514320107, 0.010357706103550541, 0.022976494474541424, 0.04752827531647437, 0.09151056108704082, 0.1615389408959318 },
			{ 0.0, 1.861677234061732E-9, 3.883162107311353E-5, 2.645481834894222E-4, 4.989157432933257E-4, 0.0010012281919558304, 0.0022903602362873057, 0.005932084363707513 },
			{ 0.0, 1.3951623165035215E-4, 0.0020837868450434803, 0.006022801764285895, 0.014272156711079073, 0.03090212121501537, 0.062053031549716815, 0.11730419112808481 },
			{ 0.0, 8.89367145342922E-5, 0.0019703656430397226, 0.0062326663361094966, 0.014831325860695401, 0.03025253877556776, 0.056160026587483626, 0.10925562299842911 },
			{ 0.0, 7.645076258669699E-4, 0.009060830664973839, 0.022816928919939736, 0.04554064383860326, 0.08050841264707713, 0.13494983145101114, 0.2189523376653257 },
			{ 0.0, 1.2210519150091045E-7, 9.734493518238497E-4, 0.003109401139347163, 0.007469476414828149, 0.01950707582271682, 0.04556422468997043, 0.08878828670561877 }
		};

		static int const BLOCKSIZE_MAX = 40;

		static int const BLOCKSIZE_MIN = 20;

		static int const TEXTURE_HISTSIZE = 6;

		static int const FUZZY_HISTSIZE = 24;

		static int const GRAYSCALE_HISTSIZE = 256;

		Fuzzificator* mFuzzificator;

	public:
		enum KeyFrameSelection { FirstFrame, MiddleFrame, LastFrame };

		//Default Setting of flow-based sampling
		KeyFrameSelection mKeyFrameSelection = MiddleFrame;

		std::string mName = "Static COMO Descriptor Xtractor";


		COMOXtractor();

		FeaturesBase* xtract(VideoBase* _videobase) override;

		std::string toString() const override;

		std::string getXtractorID() const override;

		void showProgress(int _step, int _total) const override;

		bool computeCOMODescriptor(cv::VideoCapture& _video, std::string filename, cv::OutputArray _descriptor);

		/**
		* \brief Describes each block of an image. The image is subdivided into 40*40 blocks (max) or 20*20 blocks (min). If the block contains enough texture information, the color (HSV) as well as texture (HuMoments) are calculated and summed up in the descriptor.
		* \param image 8-bit input image
		* \param descriptor output como descriptor
		* \return false on failure
		*/
		bool describe(cv::Mat &image, cv::Mat &descriptor);

		/**
		* \brief Extract como features of a block
		* \param imageBlock input block of an image
		* \param features
		* \return false on failure
		*/
		bool extractFromBlock(cv::Mat &imageBlock, cv::Mat &features);

		void convertToGrayscale(cv::Mat& image, cv::Mat& gray);

		void calculateGrayscaleHistogram(cv::Mat& gray, cv::Mat& hist);

		void calculateHuMoments(cv::Mat& gray, cv::Mat& huMoments);

		float calculateEntropy(cv::Mat& grayscaleHist, int area);

		void calculateDistances(cv::Mat& huMoments, cv::Mat& huTable, std::vector<double>& distances);

		void quantify(cv::Mat& descriptor, cv::Mat& output);

		void showHistogram(std::string name, cv::MatND &hist, int height, int width);

		void showImage(const cv::Mat _image, cv::Size _size, int x, int y, std::string name) const;

	};
}


#endif //_DEFUSE_COMOXTRACTOR_HPP_
