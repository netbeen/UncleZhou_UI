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
	void GetFeatures_ColorHist_3Channels(cv::Mat &features, cv::Mat &img, bool flag_Normalize = false);
	void GetTrainingSet(cv::Mat &trainingFeat, std::vector<int> &trainlabel, cv::Mat &allFeat, std::vector<int> &v_label);


	//processing the super pixels
	void GetFeat_ColorHist_SuperPixel(cv::Mat &features, cv::Mat &img, std::vector< std::vector<cv::Point> > &superPixelDat);
	
	void GetImageFeatures_Histograms_SuperPixel(cv::Mat &features, std::vector<cv::Mat> v_featureImgs,
		std::vector< std::vector<cv::Point> > &superPixelDat);
	
	cv::Mat Cat(std::vector<cv::Mat> &feats);

private:
	int m_numBins;
	int m_patchSize;
};