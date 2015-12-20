#pragma once

#include <opencv2/opencv.hpp>

class CMyImageProc
{
public:
	CMyImageProc(void);
	~CMyImageProc(void);

	cv::Mat GenSaliencyMap(cv::Mat &img);
	cv::Mat GenTextonMap(cv::Mat &img, int clusterCount = 256);
	cv::Mat GenColorTexton_regular(cv::Mat &img, int numBinsPerChannel = 4);
	cv::Mat GenColorTexton_kmeans(cv::Mat &img, int clusterCount = 256);
};

