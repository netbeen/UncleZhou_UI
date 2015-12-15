#include "MyMR8FilterBank.h"

//response = np.exp(-x ** 2 / (2. * sigma ** 2))
void func1(float *response, float *lengths, float sigma, int size) {
	for(int i = 0; i < size; i++)
		response[i] = exp(- lengths[i] * lengths[i] / (2 * sigma * sigma));
}

//response = -response * x
void func2(float *response, float *lengths, int size) {
	for(int i = 0; i < size; i++)
		response[i] = -response[i] * lengths[i];
}

//response = response * (x ** 2 - sigma ** 2)
void func3(float *response, float *lengths, float sigma, int size) {
	for(int i = 0; i < size; i++)
		response[i] = response[i] * (lengths[i] * lengths[i] - sigma * sigma);
}

// response /= np.abs(response).sum()
void normalize(float *response, int size) {
	float summ = 0;
	for(int i = 0; i < size; i++)
		summ += std::abs(response[i]);
	for(int i = 0; i < size; i++)
		response[i] /= summ;
}

void make_gaussian_filter(float *response, float *lengths, float sigma, int size, int order=0) {
	assert(order <= 2);//, "Only orders up to 2 are supported"

	// compute unnormalized Gaussian response
	func1(response, lengths, sigma, size);
	if (order == 1)
		func2(response, lengths, size);
	else if (order == 2)
		func3(response, lengths, sigma, size);

	normalize(response, size);
}

void getX(float *xCoords, Point2f* pts, int size) {
	for(int i = 0; i < size; i++)
		xCoords[i] = pts[i].x;
}

void getY(float *yCoords, Point2f* pts, int size) {
	for(int i = 0; i < size; i++)
		yCoords[i] = pts[i].y;
}

void multiplyArrays(float *gx, float *gy, float *response, int size) {
	for(int i = 0; i < size; i++)
		response[i] = gx[i] * gy[i];
}

void makeFilter(float scale, int phasey, Point2f* pts, float *response, int size) {
	float *xCoords = new float[size];
	float *yCoords = new float[size];
	getX(xCoords, pts, size);
	getY(yCoords, pts, size);

	float *gx = new float[size];
	float *gy = new float[size];
	make_gaussian_filter(gx, xCoords, 3 * scale, size);
	make_gaussian_filter(gy, yCoords, scale, size, phasey);
	multiplyArrays(gx, gy, response, size);
	normalize(response, size);
}

void createPointsArray(Point2f *pointsArray, int radius) {
	int index = 0;
	for(int x = -radius; x <= radius; x++)
		for(int y = -radius; y <= radius; y++)
		{
			pointsArray[index] = Point2f(x,y);
			index++;
		}
}

void rotatePoints(float s, float c, Point2f *pointsArray, Point2f *rotatedPointsArray, int size) {
	for(int i = 0; i < size; i++)
	{
		rotatedPointsArray[i].x = c * pointsArray[i].x - s * pointsArray[i].y;
		rotatedPointsArray[i].y = s * pointsArray[i].x - c * pointsArray[i].y;
	}
}

void computeLength(Point2f *pointsArray, float *length, int size) {
	for(int i = 0; i < size; i++)
		length[i] = sqrt(pointsArray[i].x * pointsArray[i].x + pointsArray[i].y * pointsArray[i].y);
}

void toMat(float *edgeThis, Mat &edgeThisMat, int support) {
	edgeThisMat = Mat::zeros(support, support, CV_32FC1);
	float* nextPts = (float*)edgeThisMat.data;
	for(int i = 0; i < support * support; i++)
	{
		nextPts[i] = edgeThis[i];
	}
}

void makeRFSfilters(vector<Mat>& edge, vector<Mat >& bar, vector<Mat>& rot, vector<float> &sigmas, int n_orientations=6, int radius=24) {
	int support = 2 * radius + 1;
	int size = support * support;
	Point2f *orgpts = new Point2f[size];
	createPointsArray(orgpts, radius);

	for(uint sigmaIndex = 0; sigmaIndex < sigmas.size(); sigmaIndex++)
		for(int orient = 0; orient < n_orientations; orient++)
		{
			float sigma = sigmas[sigmaIndex];
			//Not 2pi as filters have symmetry
			float angle = PI * orient / n_orientations;
			float c = cos(angle);
			float s = sin(angle);
			Point2f *rotpts = new Point2f[size];
			rotatePoints(s, c, orgpts, rotpts, size);
			float *edgeThis = new float[size];
			makeFilter(sigma, 1, rotpts, edgeThis, size);
			float *barThis = new float[size];
			makeFilter(sigma, 2, rotpts, barThis, size);
			Mat edgeThisMat;
			Mat barThisMat;
			toMat(edgeThis, edgeThisMat, support);
			toMat(barThis, barThisMat, support);

			edge.push_back(edgeThisMat);
			bar.push_back(barThisMat);
		}

	float *length = new float[size];
	computeLength(orgpts, length, size);

	float *rotThis1 = new float[size];
	float *rotThis2 = new float[size];
	make_gaussian_filter(rotThis1, length, 10, size);
	make_gaussian_filter(rotThis2, length, 10, size, 2);

	Mat rotThis1Mat;
	Mat rotThis2Mat;
	toMat(rotThis1, rotThis1Mat, support);
	toMat(rotThis2, rotThis2Mat, support);
	rot.push_back(rotThis1Mat);
	rot.push_back(rotThis2Mat);
}

void  apply_filterbank(Mat &img, vector<vector<Mat> >filterbank, vector<vector<Mat> > &response, int n_sigmas, int n_orientations) {
	response.resize(3);
	vector<Mat>& edges = filterbank[0];
	vector<Mat>& bar = filterbank[1];
	vector<Mat>& rot = filterbank[2];

	// Apply Edge Filters //
	int i = 0;
	for(int sigmaIndex = 0; sigmaIndex < n_sigmas; sigmaIndex++)
	{
		cv::Mat newMat = Mat::zeros(img.rows, img.cols, img.type());
		for(int orient = 0; orient < n_orientations; orient++)
		{
			Mat dst;
			filter2D(img, dst,  -1, edges[i], Point( -1, -1 ), 0, BORDER_DEFAULT );
			newMat = cv::max(dst, newMat);
			i++;
		}
		Mat newMatUchar;
		newMat = cv::abs(newMat);
		newMat.convertTo(newMatUchar, CV_8UC1);
		response[0].push_back(newMatUchar);
	}

	// Apply Bar Filters //
	i = 0;
	for(int sigmaIndex = 0; sigmaIndex < n_sigmas; sigmaIndex++)
	{
		Mat newMat = Mat::zeros(img.rows, img.cols, img.type());
		for(int orient = 0; orient < n_orientations; orient++)
		{
			Mat dst;
			cv::filter2D(img, dst,  -1 , bar[i], Point( -1, -1 ), 0, BORDER_DEFAULT );
			newMat = max(dst, newMat);
			i++;
		}
		Mat newMatUchar;
		newMat = cv::abs(newMat);
		newMat.convertTo(newMatUchar, CV_8UC1);
		response[1].push_back(newMatUchar);
	}

	// Apply Gaussian and LoG Filters //
	for(uint i = 0; i < 2; i++)
	{
		Mat newMat = Mat::zeros(img.rows, img.cols, img.type());
		Mat dst;
		cv::filter2D(img, dst,  -1 , rot[i], Point( -1, -1 ), 0, BORDER_DEFAULT );
		newMat = max(dst, newMat);
		Mat newMatUchar;
		newMat = cv::abs(newMat);
		newMat.convertTo(newMatUchar, CV_8UC1);
		response[2].push_back(newMatUchar);

	  }
  cout <<"leaving apply filterbank" << endl;
}

void createFilterbank(mH2 &filterbank, int &n_sigmas, int &n_orientations){
  vector<float> sigmas;
  sigmas.push_back(1.0f);
  sigmas.push_back(2.0f);
  sigmas.push_back(4.0f);
  n_sigmas = sigmas.size();
  n_orientations = 6;

  vector<Mat > edge, bar, rot;
  makeRFSfilters(edge, bar, rot, sigmas, n_orientations);

  // Store created filters in filter bank 2d vector<Mat>
  filterbank.push_back(edge);
  filterbank.push_back(bar);
  filterbank.push_back(rot);
}


/*

int main(){

	int n_sigmas, n_orientations;
	mH2 filterbank;
	vector<vector<Mat> > response;
	Mat inputImg = imread("src.png");
	n_sigmas = 3;
	n_orientations = 6;
	int clusterCount = 32;
	createFilterbank(filterbank, n_sigmas, n_orientations);
	apply_filterbank(inputImg, filterbank, response, n_sigmas, n_orientations);

	//cout<<float(response[0][0].at<uchar>(0,0))<<endl;
	Mat samples(inputImg.rows * inputImg.cols, 8, CV_32F);
	Mat centers, labels;
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

	Mat texton(inputImg.rows, inputImg.cols, CV_8UC1, Scalar(uchar(0)));
	// Apply KMeans
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 1.0), attempts, KMEANS_PP_CENTERS, centers);

	for(int k=0, i=0; i<inputImg.rows; i++){
		for(int j=0; j<inputImg.cols; j++, k++){
			texton.at<uchar>(i,j) = (uchar) labels.at<int>(k,0);
		}
	}
	//imshow("textons", texton);
	imwrite("texton_map.png", texton);
	cout<<"over!!"<<endl;
	cout<<"labels :\n"<<labels.size()<<endl;
	waitKey(0);
	return 0;
}
*/