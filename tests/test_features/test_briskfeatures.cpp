#include "stdafx.h"
#include "CppUnitTest.h"
#include <defuse.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

//std::string BRISKFILE = "../../../../../testdata/trecvid16_data/features/cvBRISK_1/35345_2_267-674_14.99_320x240.txt";

//Errors
std::string BRISKFILE = "../../../../../testdata/trecvid16_data/features/cvBRISK_1/39013_28_15545-15601_29.97_320x240.txt";
//std::string BRISKFILE = "../../../../../testdata/trecvid16_data/features/cvBRISK_1/38024_80_8079-8130_29.92_320x240.txt";

namespace vretbox
{
	TEST_CLASS(CNNFeatures)
	{
	public:

		TEST_METHOD(ReadBRISKFile)
		{
			File* file = new File(BRISKFILE);
			defuse::FeaturesBase* feature = new defuse::FeaturesBase();

			defuse::FeaturesBase* features = new defuse::FeaturesBase();
			features->deserialize(file->getFile());

			defuse::Hamming* distance = new defuse::Hamming();
			float dissimilairty = distance->compute(*features, *features);
			bool dist = dissimilairty == 0;
			Assert::IsTrue(dist, L"Self-similarity is wrong!", LINE_INFO());

			bool des = !features->mVectors.empty();
			Assert::IsTrue(des, L"Feature file cannot be desirelized!", LINE_INFO());
		}

	};
}