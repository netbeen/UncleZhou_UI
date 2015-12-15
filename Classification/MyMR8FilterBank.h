#include "opencv2/opencv.hpp"
using namespace std;
using namespace cv;

typedef unsigned int uint;
const float PI = 3.1415;

typedef vector<float> m1;
typedef vector<m1> m2;
typedef vector<m2> m3;

typedef vector<Mat> mH1;
typedef vector<mH1> mH2;


void  apply_filterbank(Mat &img, vector<vector<Mat> >filterbank, vector<vector<Mat> > &response, int n_sigmas, int n_orientations);

void createFilterbank(mH2 &filterbank, int &n_sigmas, int &n_orientations);

