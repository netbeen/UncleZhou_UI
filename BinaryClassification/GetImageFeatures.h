#pragma once

#include <opencv2/opencv.hpp>
#include <iostream> 
#include <fstream>

using namespace std; 

void Label2BGR(int label, cv::Vec3b &pColor);
int BGR2Label(const cv::Vec3b &pColor);

class CGetImageFeatures
{
public:
	CGetImageFeatures(void);
	CGetImageFeatures(int patchSize, int numBins);
	~CGetImageFeatures(void);

	void PrepareFeatures(cv::Mat &SrcImg, cv::Mat &colorMask, cv::Mat &trainingFeat, std::vector<int> &trainingLabel, cv::Mat &TestFeat);

	void OutputFeatures(std::string filepath, cv::Mat &features);
	void OutputLabel(std::string filepath, std::vector<int> &v_label);
	void GetFeatures_ColorHist_3Channels(cv::Mat &features, cv::Mat &img, int patchSize, int numBinsPerChannel, bool flag_Normalize = false);
	void GetTrainingSet(cv::Mat &trainingFeat, std::vector<int> &trainlabel, cv::Mat &allFeat, std::vector<int> &v_label);
	void ColorImg2Label(std::vector<int> &label, cv::Mat &img);
	void Label2ColorImage(std::vector<int> &label, cv::Mat &img);
	void LabelConvertforBinaryClassification(std::vector<int> &label, int BK_Label, int Cur_Label);
	void LabelConvertforOutputBinaryClass(std::vector<int> &label, int BK_Label, int Cur_Label);

	inline double round(double x) {return double((int (x+0.5))); }
	void Label2Class(std::vector<int> &label, std::vector<int> &v_class);
	void LabelConvert(std::vector<int> &label, int n, int *L1, int *L2);

private:
	int m_numBins;
	int m_patchSize;
};