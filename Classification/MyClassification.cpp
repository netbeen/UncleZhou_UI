#include "MyClassification.h"
#include "MySharkML.h"
#include "GetImageFeatures.h"
#include "SuperPixelGraph.h"

#include <algorithm>

CMyClassification::CMyClassification(void)
{
	m_patchSize = 16;
	m_numBins = 16;
	m_numTrees = 100;

	m_BK_Label = 26;
	m_bCropped = false;

	m_bPCA = false;
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

void CMyClassification::SetNumBinsPerChannel(std::vector<int> &v_numbins, bool bPCA)
{
	m_bPCA = bPCA;
	m_numBinsPerFeat = v_numbins;
}

void CMyClassification::SetBackgroundColor(cv::Vec3b BK_Color)
{
	m_BK_Label = BGR2Label(BK_Color);
}

void CMyClassification::RandomForestClassification(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg)
{
	//GetLabels
	m_ImgRows = inputImg.rows;
	m_ImgCols = inputImg.cols;
	cv::Mat mask_crop = colorMask(cv::Range(0, m_ImgRows-m_patchSize+1), cv::Range(0, m_ImgCols-m_patchSize+1));
	std::vector<int> allabels;
	ColorImg2Label(allabels, mask_crop);
	m_bCropped = true; //the output has been cropped;

	// GetFeatures
	CGetImageFeatures myfeat;
	myfeat.GetFeatures_ColorHist_3Channels(m_imgFeatures, inputImg, m_patchSize, m_numBins);

	// Run Classification
	RunRandomForest(allabels, m_imgFeatures, outputImg);
}

void CMyClassification::RandomForest_SuperPixel(std::vector<std::string> strFeatImgs, std::vector<int> &v_channels,
												cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat)
{
	if(strFeatImgs.size() < 1) {
		cout<<"No feature images!"<<endl;
		return;
	}

	std::vector<cv::Mat> featImgs;
	for(int i=0; i<strFeatImgs.size(); i++) {
		cv::Mat featimg;
		if(v_channels[i] == 1)
			featimg = cv::imread(strFeatImgs[i], CV_LOAD_IMAGE_GRAYSCALE);
		else 
			featimg = cv::imread(strFeatImgs[i], CV_LOAD_IMAGE_COLOR);

		featImgs.push_back(featimg);
	}
	
	RandomForest_SuperPixel(featImgs, colorMask, outputImg, dirSuperPixelDat);
}

void CMyClassification::RandomForest_SuperPixel(std::vector<cv::Mat> FeatImgs, 
	cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat)
{
	if(FeatImgs.size() < 1) {
		cout<<"No feature images!"<<endl;
		return;
	}

	//Read superpixels file
	m_ImgRows = FeatImgs[0].rows;
	m_ImgCols = FeatImgs[0].cols;
	if(!ReadInSuperPixelLabel(dirSuperPixelDat)) {
		cout<<"ERROR in read Superpixel ID file";
		return;
	}

	//Get Labels
	std::vector<int> allLabel; //labels
	ColorImg2Label_superpixel(allLabel, colorMask);


	//GetFeatures - super pixels features
	CGetImageFeatures myfeat;
	if(m_numBinsPerFeat.size() < 1) {
		for(int i=0; i<FeatImgs.size(); i++)
			m_numBinsPerFeat.push_back(m_numBins);
	}
	
	myfeat.GetImageFeatures_Histograms_SuperPixel(m_imgFeatures, FeatImgs, m_superpixelCoords, m_numBinsPerFeat);

	
	// Run Classification
	RunRandomForest(allLabel, m_imgFeatures, outputImg);
}

void CMyClassification::RandomForest_SuperPixel(cv::Mat &inputImg, cv::Mat &colorMask, cv::Mat &outputImg, std::string dirSuperPixelDat)
{
	//Read superpixels file
	m_ImgRows = inputImg.rows;
	m_ImgCols = inputImg.cols;
	if(!ReadInSuperPixelLabel(dirSuperPixelDat)) {
		cout<<"ERROR in read Superpixel ID file";
		return;
	}

	//Get Labels
	std::vector<int> allLabel; //labels
	ColorImg2Label_superpixel(allLabel, colorMask);

	
	//GetFeatures - super pixels features
	CGetImageFeatures myfeat;
	myfeat.GetFeat_ColorHist_SuperPixel(m_imgFeatures, inputImg, m_superpixelCoords, m_numBins);

	
	// Run Classification
	RunRandomForest(allLabel, m_imgFeatures, outputImg);
}


void CMyClassification::RunRandomForest(std::vector<int> allabels, cv::Mat &TestFeat, cv::Mat &outputImg)
{
	CGetImageFeatures myfeat;

	if(m_bPCA)
		TestFeat = myfeat.DimensionReduction_PCA(TestFeat, 0.333333);


	//////////////////////////////////////////////////////////////////////////
	LabelConvertforClassification(allabels);

	//Select Training Set
	cv::Mat trainingFeat;
	std::vector<int> trainingLabel;
	myfeat.GetTrainingSet(trainingFeat, trainingLabel, TestFeat, allabels);

	if(trainingFeat.rows < 1){
		cout<<"training feature's dimension is illegal !"<<endl;
		return ;
	}

	//Run Random Forest
	CMySharkML myshark(m_numTrees);
	myshark.RFClassification(trainingFeat, trainingLabel, TestFeat, m_predictLabel, m_predictConf);

	// Hard constraint for masked area
	for(int i=0; i<allabels.size(); i++) {
		int L = allabels[i];
		if(L == 9999)
			continue;
		m_predictLabel[i] = L;
		for(int j=0; j<m_predictConf.cols; j++) 
			m_predictConf.at<float>(i, j) = 0.0f;
		m_predictConf.at<float>(i, L) = 1.0f;
	}

	//Output Label
	LabelConvertforOutput(m_predictLabel);
	//////////////////////////////////////////////////////////////////////////


	// Output Image
	outputImg = cv::Mat(m_ImgRows, m_ImgCols, CV_8UC3);
	if(m_bCropped) {
		cv::Mat predictImg(m_ImgRows-m_patchSize+1, m_ImgCols-m_patchSize+1, CV_8UC3);
		Label2ColorImage(m_predictLabel, predictImg);

		outputImg = cv::Mat(m_ImgRows, m_ImgCols, CV_8UC3);
		for(int i=0; i<outputImg.rows; i++) {
			for(int j=0; j<outputImg.cols; j++) {
				if(i<predictImg.rows && j<predictImg.cols)
					outputImg.at<cv::Vec3b>(i, j) = predictImg.at<cv::Vec3b>(i, j);
				else
					Label2BGR(m_BK_Label, outputImg.at<cv::Vec3b>(i, j) );
			}
		}
	}
	else
		Label2ColorImg_SuperPixel(m_predictLabel, outputImg);
}

void CMyClassification::RunGraphCut(cv::Mat &resultImg, double smoothRatio)
{
	SuperPixelGraph spGraph(m_superpixelCoords, m_superpixelID, m_predictConf);
	spGraph.buildGraph();

	//spGraph.outputGraph();
	spGraph.GeneralGraph_DArraySArraySpatVarying(smoothRatio);
	//spGraph.PrintGraphResult();
	resultImg = Mat(m_ImgRows, m_ImgCols, CV_8UC3, Scalar(0));
	LabelConvertforOutput(spGraph.resultLabel);
	Label2ColorImg_SuperPixel(spGraph.resultLabel, resultImg);
	//imshow("the image after GC", resultImg);
}


void CMyClassification::LabelConvertforClassification(std::vector<int> &label)
{
	m_classes_input.clear();
	Label2Class(label, m_classes_input);
	
	m_classes_converted.clear();
	int j = 0;
	for(int i=0; i<m_classes_input.size(); i++) {
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
	if(pf==nullptr)
		return false;

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

			std::vector<cv::Point> v_pixels;
			DFTraverse(temLabel, i, j, curlabel, BKLabel, v_pixels);

			m_superpixelCoords.push_back(v_pixels);
		}
	}

	// ensure the superpixelID is compatible with the index in coods vector
	std::vector< std::vector<cv::Point> >::iterator iter_spCoords = m_superpixelCoords.begin();
	for(int i=0; i<m_superpixelCoords.size(); i++, iter_spCoords++) {
		std::vector<cv::Point>::iterator iter_pts = iter_spCoords->begin();
		for(int j=0; j<iter_spCoords->size(); j++, iter_pts++) {
			m_superpixelID.at<int>(iter_pts->y, iter_pts->x) = i;
		}
	}
}

void CMyClassification::DFTraverse(cv::Mat &labelmap, int i, int j, const int curlabel, const int BKLabel, std::vector<cv::Point> &pixels)
{
	if(i>=labelmap.rows || i<0 || j<0 || j>=labelmap.cols)
		return;

	int l = labelmap.at<int>(i, j);
	if(l==curlabel) {
		labelmap.at<int>(i, j) = BKLabel;
		cv::Point pt(j, i); //x, y
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

	std::vector< std::vector<cv::Point> >::iterator iter_spCoords = m_superpixelCoords.begin();
	for(int i=0; i<m_superpixelCoords.size(); i++, iter_spCoords++) {
		memset(numpixels, 0, sizeof(int)*numlabels);
		std::vector<cv::Point>::iterator iter_pts = iter_spCoords->begin();
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
	 std::vector< std::vector<cv::Point> >::iterator iter = m_superpixelCoords.begin();
	 for(; iter != m_superpixelCoords.end(); iter++, superPixelId++) {
	 	std::vector<cv::Point >::iterator iiter = iter->begin();
	 	for(; iiter!=iter->end(); iiter++)
	 		Label2BGR(label[superPixelId], img.at<cv::Vec3b>(iiter->y, iiter->x));
	 }
}
