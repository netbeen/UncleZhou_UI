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

	void OutputFeatures(std::string filepath, cv::Mat &features);
	void OutputLabel(std::string filepath, std::vector<int> &v_label);
	void GetFeatures_ColorHist_3Channels(cv::Mat &features, cv::Mat &img, int patchSize, int numBinsPerChannel, bool flag_Normalize = false);
	void GetTrainingSet(cv::Mat &trainingFeat, std::vector<int> &trainlabel, cv::Mat &allFeat, std::vector<int> &v_label);


	//processing the super pixels
	//void ReadSuperPixelDat(std::string filepath, cv::Mat &features);
	void GetSuperPixelFeat(cv::Mat img, int numBinsPerChannel, cv::Mat &trainFeats, cv::Mat &trainFeat, vector<int> &trainLabel, int BK_Label, string dirSuperPixelDat);
	void Label2ColorSuperPixelImage(std::vector<int> &label, cv::Mat &img);

	inline double round(double x) {return double((int (x+0.5))); }
	void Label2Class(std::vector<int> &label, std::vector<int> &v_class);
	void LabelConvert(std::vector<int> &label, int n, int *L1, int *L2);

private:
	vector<vector<cv::Point> > superPixelDat; 
	int m_numBins;
	int m_patchSize;
};