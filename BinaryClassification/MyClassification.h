#pragma once

#include <opencv2/opencv.hpp>


class CMyClassification
{
public:
	CMyClassification(void);
	~CMyClassification(void);

	void RandomForestClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg);
	void RandomForest_SuperPixel(cv::Mat &inputImg, cv::Mat &outputImg, std::string dirSuperPixelDat);
	void SetParametes(int patchSize, int numBins, int m_numTrees = 100);
	void SetBackgroundColor(cv::Vec3b  BK_Color);

	// Label Processing
	void ColorImg2Label(std::vector<int> &label, cv::Mat &img);
	void Label2ColorImage(std::vector<int> &label, cv::Mat &img);

	void LabelConvertforClassification(std::vector<int> &label,  int BK_Label);
	void LabelConvert_OutPut(std::vector<int> &label,  int BK_Label);

	void Label2Class(std::vector<int> &label, std::vector<int> &v_class);
	void LabelConvert(std::vector<int> &label, std::vector<int> L1, std::vector<int> L2);

private:
	int m_patchSize, m_numBins, m_numTrees;
	int m_BK_Label;
	std::vector<int> m_classes_input;
	std::vector<int> m_classes_converted;
};

