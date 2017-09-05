#ifndef _VRETBOXVALUATION_HPP_
#define  _VRETBOXVALUATION_HPP_

#include "toolbase.hpp"
#include <defuse.hpp>
#include <unordered_map>

namespace vretbox {

	/**
	* \brief Surgical action retrieval evaluation tool
	*/
	class VRETBOXValuation : public vretbox::ToolBase
	{
		
	public:

		File* mValuationTimes;

		defuse::DistanceBase* mDistance;

		defuse::XtractorBase* mXtractor;

		defuse::Valuator* mValuator;

		std::vector<defuse::FeaturesBase*> mModel;

		std::map<int, std::vector<defuse::FeaturesBase*>> mQueries;
		
		Directory* mInput;

		Directory* mOutput;

		File* mIndex;

		File* mQueryIndex;

		File* mOutputMAPFile;

		int mAtK;

		int mMaxThreads;

		bool mRandom;

		std::mutex mLock;

		/**
		* \brief Constructor
		*/
		VRETBOXValuation();

		/**
		* \brief  Initialize all member variables using boost program options
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
		~VRETBOXValuation() override;

	private:
		std::unordered_map<std::string, int> initIndex(File* _file) const;

		void initModel(std::unordered_map<std::string, int>& _index);

		void initQueries(std::unordered_map<std::string, int>& _index);

		defuse::SMD* initSMDistance() const;

		defuse::SQFD* initSQFDistance() const;

		defuse::Minkowski* initMinkowskiDistance() const;

		std::unique_lock<std::mutex> locking();

		void showProgress(int _step, int _total) const;
	};

}

#endif //_VRETBOXVALUATION_HPP_