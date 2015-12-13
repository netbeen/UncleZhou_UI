#include "MyClassification.h"
#include "MySharkML.h"
#include "GetImageFeatures.h"
#include <algorithm>

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
	std::vector<int> allabels, trainingLabel; //labels

	//GetFeatures
	CGetImageFeatures myfeat(m_patchSize, m_numBins);
	myfeat.GetFeatures_ColorHist_3Channels(TestFeat, inputImg);

	//GetLabels
	int rows = colorMask.rows;
	int cols = colorMask.cols;
	cv::Mat mask_crop = colorMask(cv::Range(0, rows-m_patchSize+1), cv::Range(0, cols-m_patchSize+1));
	ColorImg2Label(allabels, mask_crop);


	//////////////////////////////////////////////////////////////////////////
	LabelConvertforClassification(allabels);

	//Select Training Set
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, allabels);
	if(trainingFeat.rows < 1){
		cout<<"training feature's dimension is illegal !"<<endl;
		return ;
	}

	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, m_predictLabel, m_predictConf);

	//Output
	LabelConvertforOutput(m_predictLabel);
	//////////////////////////////////////////////////////////////////////////


	cv::Mat predictImg(inputImg.rows-m_patchSize+1, inputImg.cols-m_patchSize+1, CV_8UC3);
	Label2ColorImage(m_predictLabel, predictImg);

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

void CMyClassification::RandomForest_SuperPixel(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat)
{
	// Data Structure
	cv::Mat trainingFeat, TestFeat; //features
	std::vector<int> allLabel, trainingLabel; //labels

	//Read superpixels file
	m_ImgRows = inputImg.rows;
	m_ImgCols = inputImg.cols;
	if(!ReadInSuperPixelLabel(dirSuperPixelDat)) {
		cout<<"ERROR in read Superpixel ID file";
		return;
	}
	
	//GetFeatures - super pixels features
	CGetImageFeatures myfeat(m_patchSize, m_numBins);
	myfeat.GetSuperPixelFeat(TestFeat, inputImg, m_superpixelCoords);

	//Get Labels
	ColorImg2Label_superpixel(allLabel, colorMask);


	//////////////////////////////////////////////////////////////////////////
	LabelConvertforClassification(allLabel);

	//Select Training Set
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, allLabel);
	if(trainingFeat.rows < 1){
		cout<<"training feature's dimension is illegal !"<<endl;
		return ;
	}
	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, m_predictLabel, m_predictConf);

	//Output
	LabelConvertforOutput(m_predictLabel);
	//////////////////////////////////////////////////////////////////////////


	outputImg = cv::Mat(inputImg.rows, inputImg.cols, CV_8UC3);

	Label2ColorImg_SuperPixel(m_predictLabel, outputImg);
}


void CMyClassification::LabelConvertforClassification(std::vector<int> &label)
{
	m_classes_input.clear();
	Label2Class(label, m_classes_input);
	
	m_classes_converted.clear();
	for(int i=0, j=0; i<m_classes_input.size(); i++) {
		if(m_classes_input[i] == m_BK_Label)
			m_classes_converted.push_back(9999);
		else {
			m_classes_converted.push_back(j);
			j++;
		}
	}

	LabelConvert(label, m_classes_input, m_classes_converted);
}

void CMyClassification::LabelConvertforOutput(std::vector<int> &label)
{
	LabelConvert(label, m_classes_converted, m_classes_input);
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
}

void CMyClassification::Label2ColorImage(std::vector<int> &label, cv::Mat &img)
{
	int nChannels = img.channels();
	std::vector<int>::iterator iter_l = label.begin();
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, iter_l++) {
			Label2BGR(*iter_l, img.at<cv::Vec3b>(i, j));
		}
	}
}

bool CMyClassification::ReadInSuperPixelLabel(std::string dirSuperPixelDat)
{
	FILE* pf = fopen(dirSuperPixelDat.c_str(), "r");
	int sz = m_ImgRows*m_ImgCols;
	int* vals = new int[sz];
	int elread = fread((char*)vals, sizeof(int), sz, pf);
	
	m_superpixelID.release();
	m_superpixelID = cv::Mat(m_ImgRows, m_ImgCols, CV_32S);
	memcpy((int*)m_superpixelID.data, vals, sizeof(int)*sz);

	delete [] vals;
	fclose(pf);

	GetPointsofSuperPixels();

	return true;
}

void CMyClassification::GetPointsofSuperPixels()
{
	m_superpixelCoords.clear();
	int BKLabel = -9999;
	cv::Mat temLabel = m_superpixelID.clone();
	for(int i=0; i<m_ImgRows; i++) {
		for(int j=0; j<m_ImgCols; j++) {
			int curlabel = temLabel.at<int>(i, j);
			if(curlabel == BKLabel)
				continue;

			std::vector<cv::Point2d> v_pixels;
			DFTraverse(temLabel, i, j, curlabel, BKLabel, v_pixels);

			m_superpixelCoords.push_back(v_pixels);
		}
	}

	// ensure the superpixelID is compatible with the index in coods vector
	std::vector< std::vector<cv::Point2d> >::iterator iter_spCoords = m_superpixelCoords.begin();
	for(int i=0; i<m_superpixelCoords.size(); i++, iter_spCoords++) {
		std::vector<cv::Point2d>::iterator iter_pts = iter_spCoords->begin();
		for(int j=0; j<iter_spCoords->size(); j++, iter_pts++) {
			m_superpixelID.at<int>(iter_pts->y, iter_pts->x) = i;
		}
	}
}

void CMyClassification::DFTraverse(cv::Mat &labelmap, int i, int j, const int curlabel, const int BKLabel, std::vector<cv::Point2d> &pixels)
{
	if(i>=labelmap.rows || i<0 || j<0 || j>=labelmap.cols)
		return;

	int l = labelmap.at<int>(i, j);
	if(l==curlabel) {
		labelmap.at<int>(i, j) = BKLabel;
		cv::Point2d pt(j, i); //x, y
		pixels.push_back(pt);

		DFTraverse(labelmap, i, j+1, curlabel, BKLabel, pixels);
		DFTraverse(labelmap, i+1, j, curlabel, BKLabel, pixels);
		DFTraverse(labelmap, i-1, j, curlabel, BKLabel, pixels);
		DFTraverse(labelmap, i, j-1, curlabel, BKLabel, pixels);
	}
	return;
}

void CMyClassification::ColorImg2Label_superpixel(std::vector<int> &allLabels, cv::Mat &mask)
{
	std::vector<int> pixelLabels;
	ColorImg2Label(pixelLabels, mask);
	Label2Class(pixelLabels, m_classes_input);
	if(m_classes_input.size() < 2) {
		cout<<"ERROR: less than 2 classes"<<endl;
		return;
	}

	allLabels.clear();
	int numlabels = m_classes_input.size();
	int *numpixels = new int[numlabels];

	std::vector< std::vector<cv::Point2d> >::iterator iter_spCoords = m_superpixelCoords.begin();
	for(int i=0; i<m_superpixelCoords.size(); i++, iter_spCoords++) {
		memset(numpixels, 0, sizeof(int)*numlabels);
		std::vector<cv::Point2d>::iterator iter_pts = iter_spCoords->begin();
		for(int j=0; j<iter_spCoords->size(); j++, iter_pts++) {
			int l = BGR2Label(mask.at<cv::Vec3b>(iter_pts->y, iter_pts->x));
			for(int k=0; k<numlabels; k++) {
				if(l==m_classes_input[k])
					numpixels[k]++;
			}
		}

		int Label = m_classes_input[0];
		int maxnum = numpixels[0];
		for(int k=1; k<numlabels; k++) {
			if(numpixels[k] > maxnum) {
				maxnum = numpixels[k];
				Label = m_classes_input[k];
			}
		}
		
		allLabels.push_back(Label);
	}

	delete[] numpixels;
}

void CMyClassification::Label2ColorImg_SuperPixel(std::vector<int> &label, cv::Mat &img)
{
	 int nChannels = img.channels();
	 int superPixelId = 0;
	 std::vector< std::vector<cv::Point2d> >::iterator iter = m_superpixelCoords.begin();
	 for(; iter != m_superpixelCoords.end(); iter++, superPixelId++) {
	 	std::vector<cv::Point2d >::iterator iiter = iter->begin();
	 	for(; iiter!=iter->end(); iiter++)
	 		Label2BGR(label[superPixelId], img.at<cv::Vec3b>(iiter->y, iiter->x));
	 }
}
