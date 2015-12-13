#include "GetImageFeatures.h"

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


void CGetImageFeatures::GetFeatures_ColorHist_3Channels(cv::Mat &features, cv::Mat &img, bool flag_Normalize)
{
	int imgW = img.cols - m_patchSize + 1;
	int imgH = img.rows - m_patchSize + 1;
	int numPts = imgW*imgH;
	int nChannels = img.channels();
	int DIMENSION = nChannels*m_numBins;

	cv::Mat feat = cv::Mat(numPts, DIMENSION, CV_32F);
	memset(feat.data, 0, sizeof(float)*numPts*DIMENSION);

	int BinWidth = 256 / m_numBins;

	int pf = 0;
	for(int i=0; i<imgH; i++) {
		for(int j=0; j<imgW; j++, pf++) {

			for(int y = 0; y<m_patchSize; y++) {
				for(int x = 0; x<m_patchSize; x++) {
					cv::Vec3b &pp = img.at<cv::Vec3b>(i+y, j+x);
					for(int k=0; k<nChannels; k++) {
						int intensity = (int) pp[nChannels-1-k]; //data[((i+y)*img.cols+(j+x))*nChannels + k];
						int BinID = intensity*(m_numBins-1) / 255.0 + 0.5;
						//						int BinID = int(intensity / BinWidth + 0.5);	
						feat.at<float>(pf, k*m_numBins + BinID) += 1.0; 
					}
				}
			}

			//Normalize
			if(!flag_Normalize)
				continue;
			int numPatchPixels =  m_patchSize*m_patchSize;
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


void CGetImageFeatures::GetSuperPixelFeat(cv::Mat &features, cv::Mat &img, std::vector< std::vector<cv::Point2d> > &superPixelDat)
{
	int DIMENSION = m_numBins*3;
	int numSuperPixels = superPixelDat.size();
	features = cv::Mat(numSuperPixels, DIMENSION, CV_32F);
	memset(features.data, 0, sizeof(float)*numSuperPixels*DIMENSION);
	
	int pf=0;
	int nChannels = img.channels();
	std::vector<std::vector<cv::Point2d> >::iterator iter= superPixelDat.begin();
	for(; iter != superPixelDat.end(); iter++, pf++){

		std::vector<cv::Point2d >::iterator iiter=iter->begin();
		for(; iiter!=iter->end(); iiter++){
			cv::Vec3b pp = img.at<cv::Vec3b>( iiter->y, iiter->x);
			for(int k=0; k<nChannels; k++) {
				int intensity = (int) pp[nChannels-1-k];
				int BinID = intensity*(m_numBins-1) / 255.0 + 0.5;
				features.at<float>(pf, k*m_numBins + BinID) += 1.0;
			}
		}

		for(int k=0; k<DIMENSION; k++) 
			features.at<float>(pf, k) /= (float) iter->size();
	}
}


/*
	//my test about super pixels
	vector<cv::Point> spRowsDat; //each row in super pixel data
	vector<vector<int> > maskDat; //mask data
	vector<int> maskIdDat; //each row id data
	int id, numRowsDat, x,y,numSuperPixels;
	ifstream readDat(dirSuperPixelDat);
	if(!readDat.good()){
		cout<<"error! open the super pixels data fail."<<endl;
		return;
	}
	string line;
	getline(readDat, line);
	if( line == "#labels" ){
		getline(readDat, line);
		istringstream ss(line);
		ss>>numSuperPixels;
		cout<<numSuperPixels<<endl;
		for(int i = 0; i<numSuperPixels; i++){
			getline(readDat, line);
			istringstream ss(line);
			ss>>id>>numRowsDat;
			for(int j=0;j<numRowsDat;j++){
				ss>>y>>x;
				spRowsDat.push_back(cv::Point(x,y));
			}
			this->superPixelDat.push_back(spRowsDat);
            {
                vector<cv::Point> swapVector;
                spRowsDat.swap(swapVector);
            }
		}
	}

	int numMask, spId, color, numMaskSuperPixels = 0;
	vector<int> maskColor;
	getline(readDat, line);
	if( line == "#masks" ){
		getline(readDat, line);
		istringstream ss(line);
		ss>>numMask;
		cout<<numMask<<endl;
		for(int i = 0; i<numMask; i++){
			getline(readDat, line);
			istringstream ss(line);
			ss>>id>>color>>numRowsDat;
			maskColor.push_back(color);
			for(int j=0;j<numRowsDat;j++, numMaskSuperPixels++){
				ss>>spId;
				maskIdDat.push_back(spId);
			}
			maskDat.push_back(maskIdDat);
            {
                vector<int> swapVector;
                maskIdDat.swap(swapVector);
            }

		}
	}
    readDat.close();
	*/