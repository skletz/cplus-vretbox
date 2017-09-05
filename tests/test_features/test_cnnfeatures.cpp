#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string FEATUREFILE = "../../../../../testdata/trecvid-data/features/CNN_Features/35345_1.csv";

namespace vretbox
{
	TEST_CLASS(CNNFeatures)
	{
	public:

		TEST_METHOD(ReadCSVFile)
		{
			File* file = new File(FEATUREFILE);
			defuse::FeaturesBase* feature = new defuse::FeaturesBase();

			defuse::FeaturesBase* features = new defuse::FeaturesBase();
			features->deserialize(file->getFile());

			bool des = !features->mVectors.empty();
			Assert::IsTrue(des, L"Feature file cannot be desirelized!", LINE_INFO());
		}

	};
}