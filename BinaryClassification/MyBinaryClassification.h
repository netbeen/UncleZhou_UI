#pragma once

#include <opencv2/opencv.hpp>

class CMyBinaryClassification
{
public:
	CMyBinaryClassification(void);
	~CMyBinaryClassification(void);

	void RandomForestBinaryClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg, cv::Vec3b  BK_Label, cv::Vec3b  Cur_Label);
	void SetParametes(int patchSize, int numBins, int m_numTrees = 100);

private:
	int m_patchSize, m_numBins, m_numTrees;
};

