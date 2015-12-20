#include "MyImageProc.h"
#include "Saliency.h"
#include "MyMR8FilterBank.h"

CMyImageProc::CMyImageProc(void)
{
}


CMyImageProc::~CMyImageProc(void)
{
}

cv::Mat CMyImageProc::GenSaliencyMap(cv::Mat &img)
{
	cv::Mat saliencyImg;

	std::vector<UINT> temImg;
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++) {
			cv::Vec3b &pf = img.at<cv::Vec3b>(i, j);
			UINT rgb = ( (UINT) ( (pf[0] | ((unsigned short)( pf[1] )<<8)) | (((unsigned long) pf[2])<<16) ) );
			temImg.push_back(rgb);
		}
	}
	Saliency ss;
	std::vector<double> salmap;
	ss.GetSaliencyMap(temImg, img.cols, img.rows, salmap);

	saliencyImg = cv::Mat(img.rows, img.cols, CV_8U);
	int n=0;
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, n++) {
			int val = int (salmap[n] + 0.5);
			saliencyImg.at<uchar>(i, j) = (uchar) val;
		}
	}

	return saliencyImg;
}

cv::Mat CMyImageProc::GenTextonMap(cv::Mat &img, int clusterCount )
{
	int n_sigmas, n_orientations;
	mH2 filterbank;
	vector<vector<Mat> > response;
	cv::Mat inputImg(img.rows, img.cols, CV_8UC1);
	cv::cvtColor(img, inputImg, COLOR_BGR2GRAY); //imread("src.png");
	n_sigmas = 3;
	n_orientations = 6;
	//int clusterCount = 32;
	createFilterbank(filterbank, n_sigmas, n_orientations);
	apply_filterbank(inputImg, filterbank, response, n_sigmas, n_orientations);

	//cout<<float(response[0][0].at<uchar>(0,0))<<endl;
	cv::Mat samples(inputImg.rows * inputImg.cols, 8, CV_32F); //MR8
	cv::Mat centers, labels;
	int  attempts = 5;
	for(int k=0, i=0; i<inputImg.rows; i++){
		for(int j=0; j<inputImg.cols; j++, k++){
			for(int d=0, ii = 0; ii<3; ii++){
				for(int jj=0; jj<response[ii].size(); jj++, d++){
					samples.at<float>(k,d) = float(response[ii][jj].at<uchar>(i,j));
				}
			}
		}
	}
	//cout<<"samples transfer over!"<<endl;

	// Apply KMeans
	cv::kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 1.0), attempts, KMEANS_PP_CENTERS, centers);

	cv:: Mat textonImg(inputImg.rows, inputImg.cols, CV_8UC1, Scalar(uchar(0)));
	for(int k=0, i=0; i<inputImg.rows; i++){
		for(int j=0; j<inputImg.cols; j++, k++){
			int l = labels.at<int>(k,0);
			double f_l = l / (double)clusterCount;
			textonImg.at<uchar>(i,j) = (uchar) (int (f_l*255.0+0.5));
		}
	}
	//imshow("textons", texton);
	//imwrite("texton_map.png", texton);
	
	return textonImg;
}

cv::Mat CMyImageProc::GenColorTexton_regular(cv::Mat &img, int numBinsPerChannel )
{
	if(numBinsPerChannel > 6) {
		cout<<"NumBins per channel can't be larger than 6"<<endl;
		return cv::Mat();
	}

	cv::Mat labelImg(img.rows, img.cols, CV_8UC1);
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++) {
			cv::Vec3b &pf = img.at<cv::Vec3b>(i, j);
			// You can now access the pixel value with cv::Vec3b
			float b = (float)pf[0];
			float g = (float)pf[1];
			float r = (float)pf[2];

			// this is the bin assuming all bins are present
			int binID = (int)(floor(b/256.0 *(float)numBinsPerChannel) + (float)numBinsPerChannel * floor(g/256.0*(float)numBinsPerChannel) 
				+ (float)numBinsPerChannel * (float)numBinsPerChannel * floor(r/256.0*(float)numBinsPerChannel)); 

			//Normalize for restore
			double f_binID = binID / double (numBinsPerChannel*numBinsPerChannel*numBinsPerChannel);
			labelImg.at<uchar>(i, j) = uchar( int (f_binID*255.0)+0.5);
		}
	}

	return labelImg;
}

cv::Mat CMyImageProc::GenColorTexton_kmeans(cv::Mat &img, int clusterCount )
{
	if(img.channels() != 3) {
		cout<<"Only color images are supported!"<<endl;
		return cv::Mat();
	}

	cv::Mat samples(img.rows*img.cols, 3, CV_32F);
	cv::Mat centers, labels;
	int p = 0;
	for(int i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, p++) {
			cv::Vec3b &pf = img.at<cv::Vec3b>(i, j);
			// You can now access the pixel value with cv::Vec3b
			float b = (float)pf[0];
			float g = (float)pf[1];
			float r = (float)pf[2];

			samples.at<float>(p, 0) = r;
			samples.at<float>(p, 1) = g;
			samples.at<float>(p, 2) = b;
		}
	}

	int attempts = 5;
	cv::kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 1.0), attempts, KMEANS_PP_CENTERS, centers);

	cv:: Mat textonImg(img.rows, img.cols, CV_8UC1, Scalar(uchar(0)));
	for(int k=0, i=0; i<img.rows; i++) {
		for(int j=0; j<img.cols; j++, k++) {
			int l = labels.at<int>(k,0);
			double f_l = l / (double)clusterCount;
			textonImg.at<uchar>(i,j) = (uchar) (int (f_l*255.0+0.5));
		}
	}

	return textonImg;
}