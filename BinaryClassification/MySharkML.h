#pragma once

#include <opencv2/opencv.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

#include <shark/Data/Csv.h> //importing the file
//###end<includes>

using namespace std; 
using namespace shark;

class CMySharkML
{
public:
	CMySharkML(void);
	CMySharkML(int numTree);
	~CMySharkML(void);

	void RFClassification(cv::Mat &trainingFeat, std::vector<int> &trainingLabel, 
		cv::Mat &TestFeat, std::vector<int> &OutputLabel, std::vector<float> &OutputConf);

protected:
	void Features2SharkData(LabeledData<RealVector, unsigned int> &dataset,	cv::Mat &features, std::vector<int> &v_label);
	void GetPredictionLabelandConfidence(std::vector<int> &predictLable, std::vector<float> &predictConf, Data<RealVector> &predictions);

private:
	int m_numTrees;
};

