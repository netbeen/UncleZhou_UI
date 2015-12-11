#include "MyBinaryClassification.h"
#include "MySharkML.h"
#include "GetImageFeatures.h"


CMyBinaryClassification::CMyBinaryClassification(void)
{
	m_patchSize = 16;
	m_numBins = 16;
	m_numTrees = 100;
}


CMyBinaryClassification::~CMyBinaryClassification(void)
{
}

void CMyBinaryClassification::SetParametes(int patchSize, int numBins, int numTrees)
{
	m_patchSize = patchSize;
	m_numBins = numBins;
	m_numTrees = numTrees;
}
void CMyBinaryClassification::RandomForestBinaryClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg, cv::Vec3b BK_Color, cv::Vec3b Cur_Color)
{
	int BK_Label = BGR2Label(BK_Color);
	int Cur_Label = BGR2Label(Cur_Color);
	// Data Structure
	cv::Mat trainingFeat, TestFeat; //features
	std::vector<int> trainingLabel, predictLabel; //labels
	std::vector<float> predictConf; //confidence


	//GetFeatures
	CGetImageFeatures myfeat(m_patchSize, m_numBins);
	myfeat.GetFeatures_ColorHist_3Channels(TestFeat, inputImg, m_patchSize, m_numBins);

	//GetLabels
	int rows = colorMask.rows;
	int cols = colorMask.cols;
	cv::Mat mask_crop = colorMask(cv::Range(0, rows-m_patchSize+1), cv::Range(0, cols-m_patchSize+1));
	std::vector<int> labels;
	myfeat.ColorImg2Label(labels, mask_crop);
	myfeat.LabelConvertforBinaryClassification(labels, BK_Label, Cur_Label);

	//Select Training Set
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, labels);


	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, predictLabel, predictConf);

	//Output
	myfeat.LabelConvertforOutputBinaryClass(predictLabel, BK_Label, Cur_Label);
	cv::Mat predictImg(inputImg.rows-m_patchSize+1, inputImg.cols-m_patchSize+1, CV_8UC3);
	myfeat.Label2ColorImage(predictLabel, predictImg);

	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3);
	for(int i=0; i<outputImg.rows; i++) {
		for(int j=0; j<outputImg.cols; j++) {
			if(i<predictImg.rows && j<predictImg.cols)
				outputImg.at<cv::Vec3b>(i, j) = predictImg.at<cv::Vec3b>(i, j);
			else
				outputImg.at<cv::Vec3b>(i, j) = BK_Color;
		}
	}
}


void CMyBinaryClassification::RandomForestBinaryClassification(cv::Mat &inputImg, cv::Mat &outputImg, cv::Vec3b BK_Color, cv::Vec3b Cur_Color, string dirSuperPixelDat)
{
	int BK_Label = BGR2Label(BK_Color);
	int Cur_Label = BGR2Label(Cur_Color);
	// Data Structure
	cv::Mat trainingFeat, TestFeat; //features
	std::vector<int> trainingLabel, predictLabel; //labels
	std::vector<float> predictConf; //confidence


	//GetFeatures
	CGetImageFeatures myfeat(m_patchSize, m_numBins);

	//test super pixels features
	cv::Mat trainFeat;
	std::vector<int> allLabel;
	myfeat.GetSuperPixelFeat(inputImg, m_numBins, TestFeat, trainFeat, allLabel, BK_Label, dirSuperPixelDat);

	myfeat.LabelConvertforBinaryClassification(allLabel, BK_Label, Cur_Label);
	//Select Training Set
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, allLabel);
	if(trainingFeat.rows < 1){
		cout<<"training feature's dimension is illegal !"<<endl;
		return ;
	}
	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, predictLabel, predictConf);

	//Output
	myfeat.LabelConvertforOutputBinaryClass(predictLabel, BK_Label, Cur_Label);
	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3);

	myfeat.Label2ColorSuperPixelImage(predictLabel, outputImg);
}