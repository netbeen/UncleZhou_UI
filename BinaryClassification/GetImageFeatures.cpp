#include "GetImageFeatures.h"

CGetImageFeatures::CGetImageFeatures(void)
{
	m_patchSize = 16;
	m_numBins = 16;
}

CGetImageFeatures::CGetImageFeatures(int patchSize, int numBins)
{
	m_patchSize = patchSize;
	m_numBins = numBins;
}


CGetImageFeatures::~CGetImageFeatures(void)
{
}

void CGetImageFeatures::PrepareFeatures(cv::Mat &SrcImg, cv::Mat &colorMask, cv::Mat &trainingFeat, std::vector<int> &trainingLabel, cv::Mat &TestFeat)
{
	GetFeatures_ColorHist_3Channels(TestFeat, SrcImg, m_patchSize, m_numBins);
	
	int rows = colorMask.rows;
	int cols = colorMask.cols;
	cv::Mat mask_crop = colorMask(cv::Range(0, rows-m_patchSize+1), cv::Range(0, cols-m_patchSize+1));
	std::vector<int> labels;
	ColorImg2Label(labels, mask_crop);

	int L1[5] = {0, 2, 6, 20, 24};
	int L2[5] = {9999, 1, 0, 0, 0};
	LabelConvert(labels, 5, L1, L2);

	GetTrainingSet(trainingFeat, trainingLabel, TestFeat, labels);
}

void CGetImageFeatures::OutputFeatures(std::string filepath, cv::Mat &features)
{
	float* pf = (float*) features.data;
	ofstream Dout(filepath, ios::out);
	for(int i=0; i<features.rows; i++) {
		for(int j=0; j<features.cols; j++, pf++) 
			Dout<<*pf<<" ";
		Dout<<endl;
	}
	Dout.close();
}

void CGetImageFeatures::OutputLabel(std::string filepath, std::vector<int> &v_label)
{
	ofstream Dout(filepath, ios::out);
	for(int i=0; i<v_label.size(); i++)
		Dout<<v_label[i]<<endl;

	Dout.close();
}


void CGetImageFeatures::GetFeatures_ColorHist_3Channels(cv::Mat &features, cv::Mat &img, int patchSize, int numBinsPerChannel, bool flag_Normalize)
{
	int imgW = img.cols - patchSize + 1;
	int imgH = img.rows - patchSize + 1;
	int numPts = imgW*imgH;
	int nChannels = img.channels();
	int DIMENSION = nChannels*numBinsPerChannel;

	cv::Mat feat = cv::Mat(numPts, DIMENSION, CV_32F);
	memset(feat.data, 0, sizeof(float)*numPts*DIMENSION);

	int BinWidth = 256 / numBinsPerChannel;

	int pf = 0;
	for(int i=0; i<imgH; i++) {
		for(int j=0; j<imgW; j++, pf++) {

			for(int y = 0; y<patchSize; y++) {
				for(int x = 0; x<patchSize; x++) {
					cv::Vec3b &pp = img.at<cv::Vec3b>(i+y, j+x);
					for(int k=0; k<nChannels; k++) {
						int intensity = (int) pp[nChannels-1-k]; //data[((i+y)*img.cols+(j+x))*nChannels + k];
						int BinID = intensity*(numBinsPerChannel-1) / 255.0 + 0.5;
						//						int BinID = int(intensity / BinWidth + 0.5);	
						feat.at<float>(pf, k*numBinsPerChannel + BinID) += 1.0; 
					}
				}
			}

			//Normalize
			if(!flag_Normalize)
				continue;
			int numPatchPixels =  patchSize*patchSize;
			for(int k=0; k<DIMENSION; k++) 
				feat.at<float>(pf, k) /= (float) numPatchPixels;
		}
	}


	features = feat;
}

void CGetImageFeatures::GetTrainingSet(cv::Mat &trainingFeat, std::vector<int> &trainlabel, cv::Mat &allFeat, std::vector<int> &v_label)
{
	int rows = allFeat.rows;
	int DIMENSION = allFeat.cols;
	if(rows != v_label.size()) {
		cout<<"ERROR: size error!"<<endl;
		return;
	}

	std::vector<int>::iterator iter_l = v_label.begin();
	trainlabel.clear();
	for(int i=0; i<rows; i++, iter_l++) {
		if(*iter_l == 9999) //Unlabeled Data
			continue;
		trainlabel.push_back(*iter_l);
	}
	std::size_t numPts_train = trainlabel.size();
	trainingFeat = cv::Mat(numPts_train, DIMENSION, CV_32F);

	float* pf_all = (float*) allFeat.data;
	float* pf_train = (float*) trainingFeat.data;
	iter_l = v_label.begin();
	for(int i=0; iter_l != v_label.end(); iter_l++, i++, pf_all+=DIMENSION) {
		if(*iter_l == 9999) //Unlabeled Data
			continue;

		memcpy(pf_train, pf_all, sizeof(float)*DIMENSION); //Copy a line
		pf_train += DIMENSION;
	}
}

void CGetImageFeatures::LabelConvertforBinaryClassification(std::vector<int> &label, int BK_Label, int Cur_Label)
{
	for(int i=0; i<label.size(); i++) {
		if(label[i] == BK_Label)
			label[i] = 9999;
		else if(label[i] ==Cur_Label)
			label[i] = 1;
		else
			label[i] = 0;
	}
}

void CGetImageFeatures::LabelConvertforOutputBinaryClass(std::vector<int> &label, int BK_Label, int Cur_Label)
{
	for(int i=0; i<label.size(); i++) {
		if(label[i] == 0)
			label[i] = BK_Label;
		else
			label[i] = Cur_Label;
	}
}

void CGetImageFeatures::LabelConvert(std::vector<int> &label, int n, int *L1, int *L2)
{
	//int L1[5] = {0, 2, 6, 20, 24}; // convert [2, 6, 20, 24] to [1, 2, 3, 4]
	//int L2[5] = {-1, 0, 1, 2, 3};
	std::vector<int>::iterator iter_l = label.begin();
	for(int i=0; i<label.size(); i++, iter_l++) {
		for(int j=0; j<n; j++) {
			if(*iter_l == L1[j])
				*iter_l = L2[j];
		}
	}
}

void CGetImageFeatures::Label2Class(std::vector<int> &label, std::vector<int> &v_class)
{
	std::vector<int> sortL(label);
	sort(sortL.begin(), sortL.end());

	std::vector<int>::iterator iter = unique(sortL.begin(), sortL.end());
	v_class.assign(sortL.begin(), iter);
}

void CGetImageFeatures::ColorImg2Label(std::vector<int> &label, cv::Mat &img)
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

void CGetImageFeatures::Label2ColorImage(std::vector<int> &label, cv::Mat &img)
{
	int nChannels = img.channels();
	std::vector<int>::iterator iter_l = label.begin();
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, iter_l++) {
			 Label2BGR(*iter_l, img.at<cv::Vec3b>(i, j));
		}
	}
}

int BGR2Label(const cv::Vec3b &pColor)
{
	// NOTE: OpenCV is bgr
	float r = (float) pColor[2];
	float g = (float) pColor[1];
	float b = (float) pColor[0];
	return int(r /127.0 + 0.5)*9 + int(g/127.0 + 0.5)*3 + int(b/127.0 + 0.5);
}

void Label2BGR(int label, cv::Vec3b &pColor)
{
	// NOTE: opencv is bgr
	cv::Vec3b &bgr = pColor;
	bgr[2] = uchar ( (int) ( int(label/9.0) * 127.5 ));
	label = label - int(label/9.0)*9;
	bgr[1] = uchar ( (int) ( int(label/3.0)*127.5 ));
	label = label - int(label/3.0)*3;
	bgr[0]  = uchar ( (int) ( label*127.5 ));
}