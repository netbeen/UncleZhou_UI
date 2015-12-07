#include "util.h"

#include <cmath>
#include <fstream>
#include <QImage>

using namespace cv;
using namespace std;

double Util::PI = 3.14159265;


void Util::convertQImageToMat( QImage &img_qt,  Mat_<Vec3b>& img_cv){
    img_cv.create(img_qt.height(), img_qt.width());
    img_qt.convertToFormat(QImage::Format_RGB32);
    int lineNum = 0;
    int height = img_qt.height();
    int width = img_qt.width();
    uchar *imgBits = img_qt.bits();
    for(int i=0; i<height; i++){
        lineNum = i* width *4;
        for(int j=0; j<width; j++){
            img_cv(i, j)[2] = imgBits[lineNum + j*4 + 2];
            img_cv(i, j)[1] = imgBits[lineNum + j*4 + 1];
            img_cv(i, j)[0] = imgBits[lineNum + j*4 + 0];
        }
    }
}


void Util::convertMattoQImage( Mat_<Vec3b>& img_cv, QImage &img_qt){
    img_qt.convertToFormat(QImage::Format_RGB32); // 将QImage 转换成32位格式 32位格式是最便于处理
    img_qt.scaled(img_cv.rows, img_cv.cols);
    int lineNum = 0;
    int height = img_cv.rows;
    int width =  img_cv.cols;
    uchar* imgBits = img_qt.bits();  // img_qt 的首字节地址
    for( int i=0; i<height; i++ ){
        lineNum = i* width*4;
        for(int j=0; j<width; j++){
            imgBits[lineNum + j*4 + 2] = img_cv(i, j)[2];
            imgBits[lineNum + j*4 + 1] = img_cv(i, j)[1];
            imgBits[lineNum + j*4 + 0] = img_cv(i, j)[0];
        }
    }
}


void Util::replaceColorBlockDFS(cv::Mat_<cv::Vec3b>& image,const cv::Point startPoint,const cv::Vec3b newColor){
    cv::Vec3b currentColor = image.at<cv::Vec3b>(startPoint.y,startPoint.x);
    image.at<cv::Vec3b>(startPoint.y,startPoint.x) = newColor;

    //DFS
    if( startPoint.x+1 < image.cols && image.at<cv::Vec3b>(startPoint.y,startPoint.x+1) == currentColor){
        Util::replaceColorBlockDFS(image,cv::Point(startPoint.x+1,startPoint.y),newColor);
    }
    if(startPoint.x-1 >= 0 && image.at<cv::Vec3b>(startPoint.y,startPoint.x-1) == currentColor){
        Util::replaceColorBlockDFS(image,cv::Point(startPoint.x-1,startPoint.y),newColor);
    }
    if(startPoint.y+1 < image.rows && image.at<cv::Vec3b>(startPoint.y+1,startPoint.x) == currentColor){
        Util::replaceColorBlockDFS(image,cv::Point(startPoint.x,startPoint.y+1),newColor);
    }
    if(startPoint.y-1 >= 0 && image.at<cv::Vec3b>(startPoint.y-1,startPoint.x) == currentColor){
        Util::replaceColorBlockDFS(image,cv::Point(startPoint.x,startPoint.y-1),newColor);
    }
}

/**
      * @brief Util::guidanceChannel
      * @brief 静态的vector，用于保存引导图片的类型
      */
std::vector<QString> Util::guidanceChannel = std::vector<QString>();


/**
      * @brief Util::init
      * @brief 为util类进行初始化（初始化guidanceChannel这个vector）
      * @param 没有参数
      * @return 没有返回值
      */
void Util::init(){
    Util::guidanceChannel.push_back("LabelChannel");
    Util::guidanceChannel.push_back("FeatureChannel");
    Util::guidanceChannel.push_back("GradientChannel");
}


/**
      * @brief Util::calcL2Distance
      * @brief 计算两个点的L2距离
      * @param point1 表示第一个点
      * @param point2 表示第二个点
      * @return 两个点的L2距离
      */
float Util::calcL2Distance(const QPoint point1, const QPoint point2){
    return std::sqrt(std::pow(point1.x() - point2.x(),2)+std::pow(point1.y() - point2.y(),2));
}

void Util::generateFeatureFromFile(const QString filename, cv::Mat& features){
    std::cout << "WARNING: load featrue from file! Cannot display picture!" << std::endl;
    ifstream file(filename.toUtf8().constData());

    const int ROWS = 45;
    const int COLS = 701;

    features = cv::Mat(ROWS,COLS,CV_32F);
    double tempD;
    for(int row_index = 0; row_index < ROWS; row_index++){
        for(int col_index = 0; col_index < COLS; col_index++){
            file >> tempD;
            features.at<float>(row_index,col_index) = tempD;
        }
    }
}


void Util::generateGaborFeatureFromImage(const cv::Mat& sourceImage, cv::Mat& features){
    assert(sourceImage.channels() == 3);

    cv::Mat blueValue = cv::Mat(sourceImage.rows, sourceImage.cols, CV_8U);
    cv::Mat greenValue = cv::Mat(sourceImage.rows, sourceImage.cols, CV_8U);
    cv::Mat redValue = cv::Mat(sourceImage.rows, sourceImage.cols, CV_8U);

    for(int offset_y = 0; offset_y < sourceImage.rows; offset_y++){
        for(int offset_x = 0; offset_x < sourceImage.cols; offset_x++){
            blueValue.at<uchar>(offset_y,offset_x) = sourceImage.at<cv::Vec3b>(offset_y,offset_x)[0];
            greenValue.at<uchar>(offset_y,offset_x) = sourceImage.at<cv::Vec3b>(offset_y,offset_x)[1];
            redValue.at<uchar>(offset_y,offset_x) = sourceImage.at<cv::Vec3b>(offset_y,offset_x)[2];
        }
    }

    Mat blue_filterd_image = Util::gabor_filter(blueValue, 0);
    Mat green_filterd_image = Util::gabor_filter(greenValue, 0);
    Mat red_filterd_image = Util::gabor_filter(redValue, 0);

    cv::Mat blue_features = Util::generateFeature(blue_filterd_image);
    cv::Mat green_features = Util::generateFeature(green_filterd_image);
    cv::Mat red_features = Util::generateFeature(red_filterd_image);

    features = cv::Mat(blue_features.rows, blue_features.cols*3, blue_features.type());
    for(int offset_y = 0; offset_y < blue_features.rows; offset_y++){
        for(int offset_x = 0; offset_x < blue_features.cols; offset_x++){
            int step = blue_features.cols;
            features.at<float>(offset_y,offset_x+step*0) = blue_features.at<float>(offset_y,offset_x);
            features.at<float>(offset_y,offset_x+step*1) = green_features.at<float>(offset_y,offset_x);
            features.at<float>(offset_y,offset_x+step*2) = red_features.at<float>(offset_y,offset_x);
        }
    }

    return;
}


void Util::generateRgbFeatureFromImage(const cv::Mat& sourceImage, cv::Mat& features){
    assert(sourceImage.channels() == 3);

    features = cv::Mat(sourceImage.rows*sourceImage.cols, 3, CV_32F);

    for(int offset_y = 0; offset_y < sourceImage.rows; offset_y++){
        for(int offset_x = 0; offset_x < sourceImage.cols; offset_x++){
            features.at<float>(offset_y*sourceImage.cols+offset_x, 0) = (float)sourceImage.at<cv::Vec3b>(offset_y,offset_x)[2];
            features.at<float>(offset_y*sourceImage.cols+offset_x, 1) = (float)sourceImage.at<cv::Vec3b>(offset_y,offset_x)[1];
            features.at<float>(offset_y*sourceImage.cols+offset_x, 2) = (float)sourceImage.at<cv::Vec3b>(offset_y,offset_x)[0];
        }
    }
    return;
}

void Util::generateRgbFeatureFromImageWithPatch(const cv::Mat& sourceImage, cv::Mat& features, int patchSize){
    assert(sourceImage.channels() == 3);

    features = cv::Mat(sourceImage.rows*sourceImage.cols, 3*patchSize*patchSize, CV_32F);

    for(int offset_y = 0; offset_y < sourceImage.rows; offset_y++){
        for(int offset_x = 0; offset_x < sourceImage.cols; offset_x++){
            int true_y = offset_y;
            int true_x = offset_x;

            if(true_y > sourceImage.rows-1-patchSize){
                true_y = sourceImage.rows-1-patchSize;
            }
            if(true_x > sourceImage.cols-1-patchSize){
                true_x = sourceImage.cols-1-patchSize;
            }

            for(int patch_y = 0; patch_y < patchSize; patch_y++){
                for(int patch_x = 0; patch_x < patchSize; patch_x++){
                    features.at<float>(offset_y*sourceImage.cols+offset_x, (patch_y*patchSize+patch_x)*3 + 0 ) = (float)sourceImage.at<cv::Vec3b>(offset_y+patch_y,offset_x+patch_x)[2];
                    features.at<float>(offset_y*sourceImage.cols+offset_x, (patch_y*patchSize+patch_x)*3 + 1 ) = (float)sourceImage.at<cv::Vec3b>(offset_y+patch_y,offset_x+patch_x)[1];
                    features.at<float>(offset_y*sourceImage.cols+offset_x, (patch_y*patchSize+patch_x)*3 + 2 ) = (float)sourceImage.at<cv::Vec3b>(offset_y+patch_y,offset_x+patch_x)[0];
                }
            }
        }
    }
    return;
}


cv::Mat Util::generateFeature(const cv::Mat& gaborImg){

    const int imgWidth = gaborImg.cols/8;
    const int imgHeight = gaborImg.rows/5;
    const int featureDimension = 8*5;

    cv::Mat features = cv::Mat(imgWidth*imgHeight, featureDimension, CV_32F);

    for(int offset_y = 0; offset_y < imgHeight; offset_y++){
        for(int offset_x = 0; offset_x < imgWidth; offset_x++){
            for(int dimension_index = 0; dimension_index < featureDimension; dimension_index++){
                const int gaborFeatureX = dimension_index%8;
                const int gaborFeatureY = dimension_index/8;

                float targetGrayValue = gaborImg.at<float>(gaborFeatureY*imgHeight + offset_y,  gaborFeatureX*imgWidth+ offset_x);
                features.at<float>(offset_y * imgWidth + offset_x,  gaborFeatureY*8+gaborFeatureX) = targetGrayValue;
            }
        }
    }

    return features;
}


cv::Mat Util::getMyGabor(int width, int height, int U, int V, double Kmax, double f,double sigma, int ktype, const string kernel_name)
{

    int half_width = width / 2;
    int half_height = height / 2;
    double Qu = Util::PI*U/8;
    double sqsigma = sigma*sigma;
    double Kv = Kmax/pow(f,V);
    double postmean = exp(-sqsigma/2);

    Mat kernel_re(width, height, ktype);
    Mat kernel_im(width, height, ktype);
    Mat kernel_mag(width, height, ktype);

    double tmp1, tmp2, tmp3;
    for(int j = -half_height; j <= half_height; j++){
        for(int i = -half_width; i <= half_width; i++){
            tmp1 = exp(-(Kv*Kv*(j*j+i*i))/(2*sqsigma));
            tmp2 = cos(Kv*cos(Qu)*i + Kv*sin(Qu)*j) - postmean;
            tmp3 = sin(Kv*cos(Qu)*i + Kv*sin(Qu)*j);

            if(ktype == CV_32F)
                kernel_re.at<float>(j+half_height, i+half_width) =
                        (float)(Kv*Kv*tmp1*tmp2/sqsigma);
            else
                kernel_re.at<double>(j+half_height, i+half_width) =
                        (double)(Kv*Kv*tmp1*tmp2/sqsigma);

            if(ktype == CV_32F)
                kernel_im.at<float>(j+half_height, i+half_width) =
                        (float)(Kv*Kv*tmp1*tmp3/sqsigma);
            else
                kernel_im.at<double>(j+half_height, i+half_width) =
                        (double)(Kv*Kv*tmp1*tmp3/sqsigma);
        }
    }

    magnitude(kernel_re, kernel_im, kernel_mag);

    if(kernel_name.compare("real") == 0)
        return kernel_re;
    else if(kernel_name.compare("imag") == 0)
        return kernel_im;
    else{
        printf("Invalid kernel name!\n");
        return kernel_mag;
    }
}

void Util::construct_gabor_bank()
{
    const int kernel_size = 69;
    double Kmax = PI/2;
    double f = sqrt(2.0);
    double sigma = 2*PI;
    int U = 0;
    int V = 0;
    int GaborH = kernel_size;
    int GaborW = kernel_size;
    int UStart = 0, UEnd = 8;
    int VStart = -1, VEnd = 4;

    Mat kernel;
    Mat totalMat;
    for(U = UStart; U < UEnd; U++){
        Mat colMat;
        for(V = VStart; V < VEnd; V++){
            kernel = getMyGabor(GaborW, GaborH, U, V,
                                Kmax, f, sigma, CV_64F, "real");

            //show gabor kernel
            normalize(kernel, kernel, 0, 1, CV_MINMAX);
            printf("U%dV%d\n", U, V);

            if(V == VStart)
                colMat = kernel;
            else
                vconcat(colMat, kernel, colMat);
        }
        if(U == UStart)
            totalMat = colMat;
        else
            hconcat(totalMat, colMat, totalMat);
    }

    imshow("gabor bank", totalMat);
    waitKey(0);
}

void Util::labelOtherPoints(std::vector<ClusteringPoints>& v_points, std::vector<std::pair<int, double> >& v_density_Descend){
    // assign labels for other points
    // query points through the density list of descending order
    std::vector<std::pair<int, double> >::iterator iter_d_descend = v_density_Descend.begin();
    iter_d_descend = v_density_Descend.begin();
    for(int i=0; i<v_points.size(); i++, iter_d_descend++) {
        int curID = iter_d_descend->first;
        if(v_points[curID].label != -1)
            continue;

        int nNNHD = v_points[curID].nNNHD;
        v_points[curID].label = v_points[nNNHD].label;
    }
}


Mat Util::gabor_filter(Mat& img, int type)
{
    const int kernel_size = 7; // should be odd
    // variables for gabor filter
    double Kmax = PI/2;
    double f = sqrt(2.0);
    double sigma = 2*PI;
    int U = 7;
    int V = 4;
    int GaborH = kernel_size;
    int GaborW = kernel_size;
    int UStart = 0, UEnd = 8;
    int VStart = -1, VEnd = 4;

    //
    Mat kernel_re, kernel_im;
    Mat dst_re, dst_im, dst_mag;

    // variables for filter2D
    Point archor(-1,-1);
    int ddepth = CV_64F;//CV_64F
    double delta = 0;

    // filter image with gabor bank
    Mat totalMat, totalMat_re, totalMat_im;
    for(U = UStart; U < UEnd; U++){
        Mat colMat, colMat_re, colMat_im;
        for(V = VStart; V < VEnd; V++){
            kernel_re = getMyGabor(GaborW, GaborH, U, V,
                                   Kmax, f, sigma, CV_64F, "real");
            kernel_im = getMyGabor(GaborW, GaborH, U, V,
                                   Kmax, f, sigma, CV_64F, "imag");

            filter2D(img, dst_re, ddepth, kernel_re);
            filter2D(img, dst_im, ddepth, kernel_im);

            dst_mag.create(img.rows, img.cols, CV_64FC1);
            magnitude(Mat_<float>(dst_re),Mat_<float>(dst_im),
                      dst_mag);

            //show gabor kernel
            normalize(dst_mag, dst_mag, 0, 1, CV_MINMAX);
            normalize(dst_re, dst_re, 0, 1, CV_MINMAX);
            normalize(dst_im, dst_im, 0, 1, CV_MINMAX);


            if(V == VStart){
                colMat = dst_mag;
                colMat_re = dst_re;
                colMat_im = dst_im;
            }
            else{
                vconcat(colMat, dst_mag, colMat);
                vconcat(colMat_re, dst_re, colMat_re);
                vconcat(colMat_im, dst_im, colMat_im);
            }
        }
        if(U == UStart){
            totalMat = colMat;
            totalMat_re = colMat_re;
            totalMat_im = colMat_im;
        }
        else{
            hconcat(totalMat, colMat, totalMat);
            hconcat(totalMat_re, colMat_re, totalMat_re);
            hconcat(totalMat_im, colMat_im, totalMat_im);
        }
    }

    // return
    switch(type){
        case 0:
            return totalMat;
        case 1:
            return totalMat_re;
        case 2:
            return totalMat_im;
        default:
            return totalMat;
    }
}


void Util::imgUndoScale(const cv::Mat& scaled, cv::Mat& withoutScale, int scaleFactor){
    assert(scaled.type() == CV_8UC3);
    withoutScale = cv::Mat(scaled.rows*scaleFactor,scaled.cols*scaleFactor,CV_8UC3);
    for(int i = 0; i < scaled.rows; i++){
        for(int j = 0; j < scaled.cols; j++){
            for(int patchX = 0; patchX < scaleFactor; patchX++){
                for(int patchY = 0; patchY < scaleFactor; patchY++){
                    withoutScale.at<cv::Vec3b>(i*scaleFactor + patchX, j *scaleFactor +patchY)[0] = scaled.at<cv::Vec3b>(i,j)[0];
                    withoutScale.at<cv::Vec3b>(i*scaleFactor + patchX, j *scaleFactor +patchY)[1] = scaled.at<cv::Vec3b>(i,j)[1];
                    withoutScale.at<cv::Vec3b>(i*scaleFactor + patchX, j *scaleFactor +patchY)[2] = scaled.at<cv::Vec3b>(i,j)[2];
                }
            }
        }
    }
}


void Util::calcDistanceAndDelta(const cv::Mat features, std::vector<ClusteringPoints>& v_points, std::vector<std::pair<int, double> >& v_density_Descend){
    v_points = std::vector<ClusteringPoints>();
    v_density_Descend = std::vector<std::pair<int, double> >();

    // 0. 全局参数
    int nMaxSearch = 128;
    int numPts = features.rows;
    int DIMENSION = features.cols;

    // 3. 构建KDTree，计算search radius
    cv::flann::Index myKdTree;
    myKdTree.build(features, cv::flann::KDTreeIndexParams(nMaxSearch));

    double search_radius = Util::GetSearchRadius(myKdTree, features, nMaxSearch, 0.10);
    std::cout << " Search Radius: " << search_radius << std::endl;

    // 4. 计算Density - radius search
    float* pf = (float*) features.data;

    ClusteringPoints pt;
    v_points.assign(numPts, pt);

    v_density_Descend.assign(numPts, std::pair<int, double>(-1, 0.0));

    for(int i=0; i<numPts; i++) {
        std::vector<float> queryPos(DIMENSION, 0.0);
        for(int k=0; k<DIMENSION; k++)
            queryPos[k] = pf[i*DIMENSION+k];

        std::vector<int> indices;
        std::vector<float> dists;
        int nofNeighbors = myKdTree.radiusSearch(queryPos, indices, dists, search_radius, numPts, cv::flann::SearchParams(nMaxSearch));
        if(nofNeighbors < 1) {
            cout<<"KdTree failed, current i =" << i <<endl;
            exit(0);
        }
        v_density_Descend[i] = std::pair<int, double> (i, (double) nofNeighbors-1.0);

        v_points[i].nID = i;
        v_points[i].density = v_density_Descend[i].second;
    }

    // 5. Sort density in descending order
    std::sort(v_density_Descend.begin(), v_density_Descend.end(), Util::cmp);

    std::vector<int> rankinDesityDescend; // record the rank of the i-th point in the descending order density list
    rankinDesityDescend.assign(numPts, -1);
    for(int i=0; i<numPts; i++) {
        int index = v_density_Descend[i].first;
        rankinDesityDescend[index] = i;
    }

    int nglobalMax = v_density_Descend.begin()->first;
    v_points[nglobalMax].label = 0;

    // 6. 计算 dis2NNHD
    std::vector<ClusteringPoints>::iterator iter = v_points.begin();
    for(int i=0; i<numPts; i++, iter++) {

        if(iter->label == 0) // global maximum density
            continue;

        std::vector<float> queryPos(DIMENSION, 0.0);
        for(int k=0; k<DIMENSION; k++)
            queryPos[k] = pf[i*DIMENSION+k];

        int knn = 16;
        int startindex = 1;
        int curRank = rankinDesityDescend[i];

        while (1) {
            if (curRank < 2*knn || knn >= 512) { // no need to search in knn neighborhood
                double minDis = 999999.0;
                int nNNHD = -1;
                for(int j=0; j<curRank; j++) {
                    int index = v_density_Descend[j].first;
                    double dis = 0.0;
                    for(int k=0; k<DIMENSION; k++) {
                        double d = pf[index*DIMENSION+k] - queryPos[k];
                        dis +=  d*d;
                    }
                    if(dis < minDis) {
                        minDis = dis;
                        nNNHD = index;
                    }
                }
                iter->nNNHD = nNNHD;
                iter->dis2NNHD = minDis;

                break;
            }
            else {
                std::vector<int> indices;
                std::vector<float> dists;
                int nchecks = nMaxSearch;
                if(nchecks < 2*knn)
                    nchecks = 2*knn;
                myKdTree.knnSearch(queryPos, indices, dists, knn, cv::flann::SearchParams(nchecks)); //, cv::flann::SearchParams(knn+1));
                bool bfound = false;
                double distest1 = dists[0];
                for(int j=startindex; j<knn; j++) {
                    int nnIndex = indices[j];
                    if(nnIndex < 0 || nnIndex >= numPts) {
                        cout<<"knn search error"<<endl;
                        exit(0);
                    }

                    double distest2 = dists[j];
                    double dis = 0.0;

                    if(rankinDesityDescend[nnIndex] < curRank) { //have found the nearest neighbor of higher density
                        iter->nNNHD = nnIndex;
                        iter->dis2NNHD = distest2;
                        bfound = true;
                        break;
                    }
                }
                if(bfound)
                    break;
                else {
                    startindex = knn;
                    knn = knn*2 < numPts ? knn*2 : numPts;
                }
            }
        }
    }
}



int Util::cmp(const std::pair<int, double> &x, const std::pair<int, double> &y )
{
    return x.second > y.second;
}


double Util::GetSearchRadius(cv::flann::Index &myKdTree, const cv::Mat &features, int nMaxSearch, float percent = 0.02f)
{
    //3.5 计算radius,目标2%
    double maxDistanceSum = 0.0;
    int numpts = features.rows;
    int DIMENSION = features.cols;
    int knn = numpts*percent;
    if(knn > nMaxSearch)
        knn = nMaxSearch;

    int nchecks = 2*knn > nMaxSearch ? 2*knn : nMaxSearch;
    for(int i = 0; i <  numpts; i++){
        cv::Mat indices;
        cv::Mat dists;
        myKdTree.knnSearch(features.row(i), indices, dists, knn, cv::flann::SearchParams(nchecks));
        //std::cout << distances << std::endl;
        float localMax = dists.at<float>(0, knn-1);

        //std::cout << localMax << std::endl;
        maxDistanceSum += localMax;
    }
    double search_radius = maxDistanceSum / numpts; //average
    return search_radius;
}
