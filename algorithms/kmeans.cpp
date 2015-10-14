#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

cv::Mat clustering(const cv::Mat src, const int clusterNum)
{
    const int row = src.rows;
    const int col = src.cols;
    const unsigned long int size = row*col;

    cv::Mat clusters(size, 1, CV_32SC1);	//clustering Mat, save class label at every location;

    cv::Mat srcPoint(size, 1, CV_32FC3);//convert src Mat to sample srcPoint.

    cv::Vec3f* srcPoint_p = (cv::Vec3f*)srcPoint.data;//////////////////////////////////////////////
    cv::Vec3f* src_p = (cv::Vec3f*)src.data;

    for(unsigned long int i = 0;i < size; i++)
    {
        *srcPoint_p = *src_p;
        srcPoint_p++;
        src_p++;
    }
    cv::Mat center;   //center存储着聚类中心的坐标

    //K-M聚类
    cv::kmeans(srcPoint, clusterNum, clusters,cv::TermCriteria (CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 0.1),clusterNum,cv::KMEANS_PP_CENTERS , center);

    std::cout<<"center row:"<<center.rows<<" col:"<<center.cols<<std::endl;
    for (int y = 0; y < center.rows; y++)
    {
        cv::Vec3f* imgData = center.ptr<cv::Vec3f>(y);
        std::cout<<imgData->val[0]<<" "<<imgData->val[1]<<" "<<imgData->val[2]<<std::endl;
        std::cout<<std::endl;
    }


    //得出聚类结果的最大和最小值
    double minH,maxH;
    cv::minMaxLoc(clusters, &minH, &maxH);
    std::cout<<"H-channel min:"<<minH<<" max:"<<maxH<<std::endl;


    int* clusters_p = (int*)clusters.data;
    //show label mat
    cv::Mat label(src.size(), CV_32SC1);
    int* label_p = (int*)label.data;
    //assign the clusters to Mat label
    for(unsigned long int i = 0;i < size; i++)
    {
        *label_p = *clusters_p;
        label_p++;
        clusters_p++;
    }



    return label;
}

int main()
{
    cv::Mat img=cv::imread("/home/netbeen/桌面/img.png");
    cv::imshow("src",img);

    cv::GaussianBlur(img,img,cv::Size(5,5),0);
    img.convertTo(img,CV_32FC3);

    cv::Mat label=clustering(img,2);

    label.convertTo(label,CV_8UC1);
    cv::normalize(label,label,255,0,CV_MINMAX);
    //cv::erode(label,label,cv::Mat(),cv::Point(-1,-1),3);
    //cv::dilate(label,label,cv::Mat(),cv::Point(-1,-1),3);
    cv::imshow("label",label);

    cv::waitKey();
}

