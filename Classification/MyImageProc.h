#pragma once

#include <opencv2/opencv.hpp>

class CMyImageProc
{
public:
	CMyImageProc(void);
	~CMyImageProc(void);

	cv::Mat GenSaliencyMap(cv::Mat &img);
	cv::Mat GenTextonMap(cv::Mat &img, int clusterCount = 256);
};

