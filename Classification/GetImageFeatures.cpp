#include "GetImageFeatures.h"

#include "Saliency.h"

#include "Eigen/Dense"
using namespace Eigen;

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

void CGetImageFeatures::GetImageFeatures_Histograms_SuperPixel(cv::Mat &features, std::vector<cv::Mat> v_featureImgs,
															   std::vector< std::vector<cv::Point> > &superPixelDat, std::vector<int> v_numBins)
{
	int icount = v_featureImgs.size();
	std::vector<cv::Mat> feat;
	int totalChannel = 0;
	for(int i=0; i<icount; i++) {
		if(v_featureImgs[i].rows < 1)
			continue;

		int channel_i = v_featureImgs[i].channels();
		totalChannel += channel_i;
		cv::Mat feat_i;
		GetFeat_ColorHist_SuperPixel(feat_i, v_featureImgs[i], superPixelDat, v_numBins[i]);
		feat.push_back(feat_i);
	}

	features = Cat(feat);
}

cv::Mat CGetImageFeatures::Cat(std::vector<cv::Mat> &feats)
{
	int numSuperPixels = feats[0].rows;
	if(numSuperPixels < 1)
		return cv::Mat();

	int totalCols = 0;
	for(int i=0; i<feats.size(); i++) {
		totalCols += feats[i].cols;
		assert(feats[i].rows == numSuperPixels);
	}

	cv::Mat features = cv::Mat(numSuperPixels, totalCols, CV_32F);
	float* pf_all = (float*)features.data;

	for(int j=0; j<numSuperPixels; j++) {
		for(int i=0; i<feats.size();  i++) {
			int cols_i= feats[i].cols;

			float *pf_i = (float *) feats[i].data + j*cols_i;
			memcpy(pf_all, pf_i, sizeof(float)*cols_i); //Copy a line

			pf_all += cols_i;
		}
	}

	return features;
}

void CGetImageFeatures::GetFeat_ColorHist_SuperPixel(cv::Mat &features, cv::Mat &img, std::vector< std::vector<cv::Point> > &superPixelDat, int numBins)
{
	int nChannels = img.channels();
	int DIMENSION = numBins*nChannels;
	int numSuperPixels = superPixelDat.size();
	features = cv::Mat(numSuperPixels, DIMENSION, CV_32F);
	memset(features.data, 0, sizeof(float)*numSuperPixels*DIMENSION);
	
	int pf=0;
	std::vector<std::vector<cv::Point> >::iterator iter= superPixelDat.begin();
	for(; iter != superPixelDat.end(); iter++, pf++){

		std::vector<cv::Point >::iterator iiter=iter->begin();
		for(; iiter!=iter->end(); iiter++) {
			uchar *pp = nullptr;
			if(nChannels == 3)
				pp = &img.at<cv::Vec3b>( iiter->y, iiter->x)[0];
			else if(nChannels == 1)
				pp = &img.at<uchar>( iiter->y, iiter->x);
			for(int k=0; k<nChannels; k++) {
				int intensity = (int) pp[nChannels-1-k];
				int BinID = intensity*(numBins-1) / 255.0 + 0.5;
				features.at<float>(pf, k*numBins + BinID) += 1.0;
			}
		}

		for(int k=0; k<DIMENSION; k++) 
			features.at<float>(pf, k) /= (float) iter->size();
	}
}

cv::Mat CGetImageFeatures::DimensionReduction_PCA(cv::Mat &features, float r_ratio)
{
	assert(r_ratio>0 && r_ratio<=1);
	int numSamples = features.rows;
	int dim_org = features.cols;
	int dim = int (dim_org * r_ratio + 0.5);

	Eigen::MatrixXd newFeat = Eigen::MatrixXd::Zero(numSamples, dim_org);

	for(int j=0; j<dim_org; j++) {
		double ave = 0.0;
		for(int i=0; i<numSamples; i++)
			ave += features.at<float>(i, j);
		
		ave /= (double) numSamples;

		for(int i=0; i<numSamples; i++)
			newFeat(i, j) = features.at<float>(i, j) - ave;
	}

	Eigen::JacobiSVD<Eigen::MatrixXd> svd(newFeat, Eigen::ComputeThinV);
	newFeat *= svd.matrixV();
//	newFeat *= eigV;

	cv::Mat reducedFeat(numSamples, dim, CV_32F);
	for(int i=0; i<numSamples; i++) {
		for(int j=0; j<dim; j++) {
			reducedFeat.at<float>(i, j) = (float) newFeat(i, j); //EigenVec[index+j*numsamples];
		}
	}	
	return reducedFeat;
}
