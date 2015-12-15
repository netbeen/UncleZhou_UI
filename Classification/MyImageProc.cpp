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
	cv::Mat samples(inputImg.rows * inputImg.cols, 8, CV_32F);
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

	cv:: Mat textonImg(inputImg.rows, inputImg.cols, CV_8UC1, Scalar(uchar(0)));
	// Apply KMeans
	cv::kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 1.0), attempts, KMEANS_PP_CENTERS, centers);

	for(int k=0, i=0; i<inputImg.rows; i++){
		for(int j=0; j<inputImg.cols; j++, k++){
			textonImg.at<uchar>(i,j) = (uchar) labels.at<int>(k,0);
		}
	}
	//imshow("textons", texton);
	//imwrite("texton_map.png", texton);
	
	return textonImg;
}