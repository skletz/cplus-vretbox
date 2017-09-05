#ifndef _FUZZIFICATOR_H_
#define _FUZZIFICATOR_H_

#include <opencv2/opencv.hpp>

class Fuzzificator
{
	struct Fuzzyrules
	{
		int input1;
		int input2;
		int input3;
		int output;
	};

	//32 values
	std::vector<double> HUE_MEMBERSHIP_VALUES{
		0, 0, 5, 10, 5, 10, 
		35, 50, 35, 50, 70, 
		85, 70, 85, 150, 165, 
		150, 165, 195, 205, 
		195, 205, 265, 280, 
		265, 280, 315, 330, 
		315, 330, 360, 360
	};

	//8 values
	std::vector<double> SATURATION_10BIN_MEMBERSHIP_VALUES{
		0, 0, 10, 75, 10, 75, 255, 255
	};

	//8 values
	std::vector<double> SATURATION_24BIN_MEMBERSHIP_VALUES{
		0, 0, 68, 188, 68, 188, 255, 255
	};

	//12 values
	std::vector<double> VALUE_10BIN_MEMBERSHIP_VALUES{
		0, 0, 10, 75, 10, 75, 180, 220, 
		180, 220, 255, 255
	};

	//8 values
	std::vector<double> VALUE_24BIN_MEMBERSHIP_VALUES{
		0, 0, 68, 188, 68, 188, 255, 255
	};

	int FUZZY_10BIN_RULES_DEFINITION[48][4] = {
		{ 0, 0, 0, 2 }, { 0, 1, 0, 2 }, 
		{ 0, 0, 2, 0 }, { 0, 0, 1, 1 },
		{ 1, 0, 0, 2 }, { 1, 1, 0, 2 }, 
		{ 1, 0, 2, 0 }, { 1, 0, 1, 1 },
		{ 2, 0, 0, 2 }, { 2, 1, 0, 2 }, 
		{ 2, 0, 2, 0 }, { 2, 0, 1, 1 },
		{ 3, 0, 0, 2 }, { 3, 1, 0, 2 }, 
		{ 3, 0, 2, 0 }, { 3, 0, 1, 1 },
		{ 4, 0, 0, 2 }, { 4, 1, 0, 2 }, 
		{ 4, 0, 2, 0 }, { 4, 0, 1, 1 },
		{ 5, 0, 0, 2 }, { 5, 1, 0, 2 }, 
		{ 5, 0, 2, 0 }, { 5, 0, 1, 1 },
		{ 6, 0, 0, 2 }, { 6, 1, 0, 2 }, 
		{ 6, 0, 2, 0 }, { 6, 0, 1, 1 },
		{ 7, 0, 0, 2 }, { 7, 1, 0, 2 }, 
		{ 7, 0, 2, 0 }, { 7, 0, 1, 1 },
		{ 0, 1, 1, 3 }, { 0, 1, 2, 3 }, 
		{ 1, 1, 1, 4 }, { 1, 1, 2, 4 },
		{ 2, 1, 1, 5 }, { 2, 1, 2, 5 }, 
		{ 3, 1, 1, 6 }, { 3, 1, 2, 6 },
		{ 4, 1, 1, 7 }, { 4, 1, 2, 7 }, 
		{ 5, 1, 1, 8 },	{ 5, 1, 2, 8 },
		{ 6, 1, 1, 9 }, { 6, 1, 2, 9 }, 
		{ 7, 1, 1, 3 }, { 7, 1, 2, 3 }
	};

	int FUZZY_24BIN_RULES_DEFINITION[4][3] = {
		{ 1, 1, 1 }, { 0, 0, 2 },
		{ 0, 1, 0 }, { 1, 0, 2 }
	};

	//48 Values
	std::vector<Fuzzyrules*> mFuzzy10BinRules;

	//4 Values
	std::vector<Fuzzyrules*> mFuzzy24BinRules;

	//8 Values
	std::vector<double> mHueActivation;

	//2 Values
	std::vector<double> mSaturationActivation;

	//3 Values
	std::vector<double> mValueActivation;

	//10 Values
	std::vector<double> mFuzzy10BinHisto;

	//24 Values
	std::vector<double> mFuzzy24BinHisto;


public:
	Fuzzificator();

	~Fuzzificator();

	void calculateFuzzy10BinHisto(
		double hue, double saturation, 
		double value, cv::Mat& fuzzy10BinHisto);

	void calculateFuzzy24BinHisto(
		double hue, double saturation,
		double value, cv::Mat& colorValues,
		cv::Mat& fuzzy24BinHisto);
	
	void findMembershipValueForTriangles(
		double input, std::vector<double>& triangles, 
		std::vector<double>& memfunction);

	void mMultiParticipateDefazzificator(
		std::vector<Fuzzyrules*>& rules, std::vector<double>& input1,
		std::vector<double>& input2, std::vector<double>& input3, 
		std::vector<double>& resultTable);

	void mMultiParticipateDefazzificator(
		std::vector<Fuzzyrules*>& rules, std::vector<double>& input1,
		std::vector<double>& input2, std::vector<double>& resultTable);
};

#endif //_FUZZIFICATOR_H_