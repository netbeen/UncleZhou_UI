#include "MyClassification.h"
#include "MySharkML.h"
#include "GetImageFeatures.h"

CMyClassification::CMyClassification(void)
{
	m_patchSize = 16;
	m_numBins = 16;
	m_numTrees = 100;

	m_BK_Label = 26;
}


CMyClassification::~CMyClassification(void)
{
}

void CMyClassification::SetParametes(int patchSize, int numBins, int numTrees)
{
	m_patchSize = patchSize;
	m_numBins = numBins;
	m_numTrees = numTrees;
}

void CMyClassification::SetBackgroundColor(cv::Vec3b BK_Color)
{
	m_BK_Label = BGR2Label(BK_Color);
}

void CMyClassification::RandomForestClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg)
{
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
	ColorImg2Label(labels, mask_crop);

	//Select Training Set
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, labels);

	if(trainingFeat.rows < 1)
		return;

	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, predictLabel, predictConf);

	//Output
	cv::Mat predictImg(inputImg.rows-m_patchSize+1, inputImg.cols-m_patchSize+1, CV_8UC3);
	Label2ColorImage(predictLabel, predictImg);

	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3);
	for(int i=0; i<outputImg.rows; i++) {
		for(int j=0; j<outputImg.cols; j++) {
			if(i<predictImg.rows && j<predictImg.cols)
				outputImg.at<cv::Vec3b>(i, j) = predictImg.at<cv::Vec3b>(i, j);
			else
				Label2BGR(m_BK_Label, outputImg.at<cv::Vec3b>(i, j) );
		}
	}
}

void CMyClassification::RandomForest_SuperPixel(cv::Mat &inputImg, cv::Mat &outputImg, string dirSuperPixelDat)
{
	// Data Structure
	cv::Mat trainingFeat, TestFeat; //features
	std::vector<int> trainingLabel, predictLabel; //labels
	std::vector<float> predictConf; //confidence
	
	//GetFeatures
	CGetImageFeatures myfeat(m_patchSize, m_numBins);

	//test super pixels features
	cv::Mat trainFeat;
	std::vector<int> allLabel;
	myfeat.GetSuperPixelFeat(inputImg, m_numBins, TestFeat, trainFeat, allLabel, m_BK_Label, dirSuperPixelDat);

	//myfeat.LabelConvertforBinaryClassification(allLabel, BK_Label, Cur_Label);
	LabelConvertforClassification(allLabel, m_BK_Label);

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
	LabelConvert(predictLabel, m_classes_converted, m_classes_input);
	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3);

	myfeat.Label2ColorSuperPixelImage(predictLabel, outputImg);
}


void CMyClassification::LabelConvertforClassification(std::vector<int> &label,  int BK_Label)
{
	m_classes_input.clear();
	Label2Class(label, m_classes_input);
	
	m_classes_converted.clear();
	for(int i=0, j=0; i<m_classes_input.size(); i++) {
		if(m_classes_input[i] == BK_Label)
			m_classes_converted.push_back(9999);
		else {
			m_classes_converted.push_back(j);
			j++;
		}
	}

	LabelConvert(label, m_classes_input, m_classes_converted);
}


void CMyClassification::LabelConvert(std::vector<int> &label, std::vector<int> L1, std::vector<int> L2)
{
	assert(L1.size() == L2.size());
	std::vector<int>::iterator iter_l = label.begin();
	int n = L1.size();
	for(int i=0; i<label.size(); i++, iter_l++) {
		for(int j=0; j<n; j++) {
			if(*iter_l == L1[j]) {
				*iter_l = L2[j];
				break;
			}
		}
	}
}

void CMyClassification::Label2Class(std::vector<int> &label, std::vector<int> &v_class)
{
	std::vector<int> sortL(label);
	sort(sortL.begin(), sortL.end());

	std::vector<int>::iterator iter = unique(sortL.begin(), sortL.end());
	v_class.assign(sortL.begin(), iter);
}


void CMyClassification::ColorImg2Label(std::vector<int> &label, cv::Mat &img)
{
	label.clear();
	int numpoints = img.rows*img.cols;
	int nChannels = img.channels();
	if(nChannels != 3) {
		cout<<"ERROR: not a colorful image!"<<endl;
		return;
	}

	cv::Mat testimg(img.rows, img.cols, CV_8U);
	memset(testimg.data, 0, img.rows*img.cols);

	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++) {	
			label.push_back(BGR2Label(img.at<cv::Vec3b>(i, j)));
		}
	}

	LabelConvertforClassification(label, m_BK_Label);
}

void CMyClassification::Label2ColorImage(std::vector<int> &label, cv::Mat &img)
{
	LabelConvert(label, m_classes_converted, m_classes_input);

	int nChannels = img.channels();
	std::vector<int>::iterator iter_l = label.begin();
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, iter_l++) {
			Label2BGR(*iter_l, img.at<cv::Vec3b>(i, j));
		}
	}
}
