#pragma once

#include <opencv2/opencv.hpp>


class CMyClassification
{
public:
	CMyClassification(void);
	~CMyClassification(void);

	void RandomForest_SuperPixel(cv::Mat &inputImg, std::vector<std::string> moreFeatImgs, 
		cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat);


	void RandomForestClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg);
	void RandomForest_SuperPixel(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat);
	void RandomForest_SuperPixel(cv::Mat &inputImg, std::vector<cv::Mat> moreFeatImgs, cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat);
	
	void SetParametes(int patchSize, int numBins, int m_numTrees = 100);
	void SetBackgroundColor(cv::Vec3b  BK_Color);

	void RunRandomForest(std::vector<int> allabels, cv::Mat &TestFeat, cv::Mat &outpuImg);

	void RunGraphCut(cv::Mat &resultImg, double smoothRatio = 0.325f);


public:
	cv::Mat m_superpixelID;
	std::vector< std::vector<cv::Point> > m_superpixelCoords;
	std::vector<int> m_predictLabel;
	cv::Mat m_predictConf;

private:

	// Label Processing
	void ColorImg2Label(std::vector<int> &label, cv::Mat &img);
	void Label2ColorImage(std::vector<int> &label, cv::Mat &img);

	void ColorImg2Label_superpixel(std::vector<int> &allLabels, cv::Mat &mask);
	void Label2ColorImg_SuperPixel(std::vector<int> &label, cv::Mat &img);

	void LabelConvertforClassification(std::vector<int> &label);
	void LabelConvertforOutput(std::vector<int> &label);

	void Label2Class(std::vector<int> &label, std::vector<int> &v_class);
	void LabelConvert(std::vector<int> &label, std::vector<int> L1, std::vector<int> L2);


	int m_patchSize, m_numBins, m_numTrees, m_numClasses;
	bool m_bCropped;

	int m_ImgRows, m_ImgCols;
	int m_BK_Label;

	std::vector<int> m_classes_input;
	std::vector<int> m_classes_converted;


	void GetPointsofSuperPixels();
	bool ReadInSuperPixelLabel(std::string dirSuperPixelDat);
	void DFTraverse(cv::Mat &labelmap, int i, int j, const int curlabel, const int BKLabel, std::vector<cv::Point> &pixels);
};

