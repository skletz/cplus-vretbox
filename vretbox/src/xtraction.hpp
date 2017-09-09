#ifndef _VRETBOXXTRACTION_HPP_
#define  _VRETBOXXTRACTION_HPP_

#include "toolbase.hpp"
#include <defuse.hpp>

namespace vretbox {

	/**
	* \brief Surgical action retrieval extraction tool
	*/
	class VRETBOXXtraction : public vretbox::ToolBase
	{

		defuse::XtractorBase* mXtractor;

		File* mVideo;

		File* mFeatures;

		File* mXtractionTimes;

		File* mSamplepoints;

		Directory* mOutpudir;

		bool mDisplay;

		bool mSaveDisplay;

	public:

		/**
		 * \brief Constructor
		 */
		VRETBOXXtraction();

		/**
		 * \brief Initialize all member variables using boost program options.
		 * \param _args program_options::variables_map
		 * \return true, if initialization was successful
		 */
		bool init(boost::program_options::variables_map _args) override;

		/**
		 * \brief Run extraction tool after initialization
		 */
		void run() override;

		/**
		 * \brief Destructor
		 */
		~VRETBOXXtraction() override;

	private:

		bool initStaticSignatures();

		bool initFlowbasedSignatures();

		bool initComoDescriptor();

		bool initBRISKDescriptor();

		bool initHOGDescriptor();

		bool initCEEDDescriptor();

		bool deserializeSamplepoints(std::string samplepointdir, std::string fileextension, int initSeeds, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f>& samplepoints);

		void initSamplepoints(int initSeeds, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f>& samplepoints);

		void serializeSamplepoints(std::string samplepointdir, std::string fileextension, defuse::SamplePoints::Distribution distribution, float mean, float stddev, std::vector<cv::Point2f> samplepoints);

	};
}

#endif //_VRETBOXXTRACTION_HPP_