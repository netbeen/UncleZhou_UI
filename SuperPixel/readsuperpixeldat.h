#ifndef READSUPERPIXELDAT_H
#define READSUPERPIXELDAT_H

#include <opencv2/opencv.hpp>
#include <QString>


class ReadSuperPixelDat
{
public:
    ReadSuperPixelDat();

    void segmentSourceImage();
    void authorSegment(int superPixelCount = 4000);
    void authorRead();
    int cvtRGB2ColorCode(int red, int green, int blue);



private:
    cv::Mat image;
    cv::Mat result;
    int size;
    unsigned int* pbuff;
    void ReadImage(unsigned int* pbuff, int width,int height);
    void SaveSegmentedImageFile(unsigned int* pbuff, int width,int height);
    int labels[99999999];

    void searchUnwhiteArea();
    cv::Mat maskDFS;
    cv::Mat mask;

    void dfs(std::pair<int,int> coordinate);

    std::vector<std::vector< std::pair<int,int> > > maskID2Coodinates;
    std::vector< std::pair<int,int> > coordinates;

    std::vector<int> colorCodeVector;
};

#endif // READSUPERPIXELDAT_H
