#include "readsuperpixeldat.h"
#include <cmath>
#include <fstream>

ReadSuperPixelDat::ReadSuperPixelDat()
{

}

#include <string>
#include "SuperPixel/SLICSuperpixels/SLIC.h"
#include "util.h"

void ReadSuperPixelDat::authorSegment(int superPixelCount)
{
    const int width = this->image.cols;
    const int height = this->image.rows;
    // unsigned int (32 bits) to hold a pixel in ARGB format as follows:
    // from left to right,
    // the first 8 bits are for the alpha channel (and are ignored)
    // the next 8 bits are for the red channel
    // the next 8 bits are for the green channel
    // the last 8 bits are for the blue channel
    //unsigned int* pbuff = new UINT[sz];
    ReadImage(pbuff, width, height);//YOUR own function to read an image into the ARGB format

    //----------------------------------
    // Initialize parameters
    //----------------------------------
    int k = superPixelCount;//Desired number of superpixels. 在此修改分成多少个super pixel
    double m = 40;//Compactness factor. use a value ranging from 10 to 40 depending on your needs. Default is 10
    int* klabels = new int[width*height];
    int numlabels(0);
    string filename = "yourfilename.jpg";
    string savepath = "yourpathname";
    //----------------------------------
    // Perform SLIC on the image buffer
    //----------------------------------
    SLIC segment;
    segment.PerformSLICO_ForGivenK(pbuff, width, height, klabels, numlabels, k, m);
    // Alternately one can also use the function PerformSLICO_ForGivenStepSize() for a desired superpixel size
    //----------------------------------
    // Save the labels to a text file
    //----------------------------------
    segment.SaveSuperpixelLabels(klabels, width, height, filename, savepath);
    //----------------------------------
    // Draw boundaries around segments
    //----------------------------------
    segment.DrawContoursAroundSegments(pbuff, klabels, width, height, 0xff0000);
    //----------------------------------
    // Save the image with segment boundaries.
    //----------------------------------
    SaveSegmentedImageFile(pbuff, width, height);//YOUR own function to save an ARGB buffer as an image
    //----------------------------------
    // Clean up
    //----------------------------------
    if(pbuff) delete [] pbuff;
    if(klabels) delete [] klabels;
}

void ReadSuperPixelDat::segmentSourceImage(){
    this->image = cv::imread((Util::dirName+"/sourceImage.png").toUtf8().data());
    this->size = this->image.cols * this->image.rows;

    this->authorSegment();
    this->authorRead();
}

void ReadSuperPixelDat::ReadImage(unsigned int* pbuff, int width,int height){//YOUR own function to read an image into the ARGB format
    //std::cout << "ReadImage start" << std::endl;
    const int sz = this->size;
    this->pbuff = new unsigned int[sz];
    //std::cout << "pbuff's address = " <<(pbuff) << std::endl;

    unsigned int* ptr = this->pbuff;
    for(int y_offset = 0; y_offset < this->image.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->image.cols; x_offset++){
            cv::Vec3b color = this->image.at<cv::Vec3b>(y_offset,x_offset);
            unsigned int tempResult = 1;
            tempResult += color[0];
            tempResult += color[1]*std::pow(2,8);
            tempResult += color[2]*std::pow(2,16);
            (*ptr) = (unsigned int)tempResult;
            ptr++;
        }
    }

    //std::cout << "ReadImage end" << std::endl;
}


void ReadSuperPixelDat::SaveSegmentedImageFile(unsigned int* pbuff, int width,int height){//YOUR own function to save an ARGB buffer as an image
    std::cout << "SaveSegmentedImageFile start" << std::endl;
    this->result = cv::Mat(this->image.size(), CV_8UC3);

    unsigned int* ptr = this->pbuff;
    for(int y_offset = 0; y_offset < this->image.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->image.cols; x_offset++){
            //std::cout << y_offset << " " << x_offset << std::endl;
            unsigned int tempResult1 = *ptr;
            int blue = tempResult1%(int)std::pow(2,8);
            int green = (tempResult1/(int)std::pow(2,8))%(int)std::pow(2,8);
            int red = (tempResult1/(int)std::pow(2,16))%(int)std::pow(2,8);
            this->result.at<cv::Vec3b>(y_offset,x_offset) = cv::Vec3b(blue,green,red);
            ptr++;
        }
    }

    std::cout << "SaveSegmentedImageFile end" << std::endl;
}

void ReadSuperPixelDat::authorRead(){
    FILE* pf = fopen((Util::dirName+"/SLICOutput.dat").toUtf8().data(), "r");
    int sz = this->size;
    int* vals = new int[sz];
    int elread = fread((char*)vals, sizeof(int), sz, pf);
    for( int j = 0; j < this->image.rows; j++ ){
        for( int k = 0; k < this->image.cols; k++ ){
            int i = j*this->image.cols+k;
            labels[i] = vals[i];
        }
    }
    delete [] vals;
    fclose(pf);
}



int ReadSuperPixelDat::cvtRGB2ColorCode(int red, int green, int blue){
    int result = 0;
    result += (red/127)*9;
    result += (green/127)*3;
    result += blue/127;
    return result;
}

void ReadSuperPixelDat::searchUnwhiteArea(){
    for(int y_offset = 0; y_offset < this->maskDFS.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->maskDFS.cols; x_offset++){
            cv::Vec3b currentColor = this->maskDFS.at<cv::Vec3b>(y_offset,x_offset);

            cv::Vec3b white = cv::Vec3b(255,255,255);
            if(currentColor != white){
                int colorCode = this->cvtRGB2ColorCode(currentColor[2],currentColor[1],currentColor[0]);
                this->colorCodeVector.push_back(colorCode);

                std::cout << "Find unwhite area: " << y_offset << " " << x_offset  << " ColorCode = " << colorCode<< std::endl;
                this->coordinates = std::vector< std::pair<int,int> >();
                this->dfs(std::pair<int,int>(y_offset,x_offset));
                this->maskID2Coodinates.push_back(this->coordinates);
            }
        }
    }
}

void ReadSuperPixelDat::dfs(std::pair<int,int> coordinate){
    this->coordinates.push_back(coordinate);
    cv::Vec3b currentMaskColor = this->maskDFS.at<cv::Vec3b>(coordinate.first, coordinate.second);

    cv::Vec3b white = cv::Vec3b(255,255,255);

    this->maskDFS.at<cv::Vec3b>(coordinate.first,coordinate.second) = white;

    if(coordinate.first+1 < this->maskDFS.rows && this->maskDFS.at<cv::Vec3b>(coordinate.first+1,coordinate.second) == currentMaskColor)
        this->dfs(std::pair<int,int>(coordinate.first+1,coordinate.second));
    if(coordinate.second-1 >=0 && this->maskDFS.at<cv::Vec3b>(coordinate.first,coordinate.second-1) == currentMaskColor)
        this->dfs(std::pair<int,int>(coordinate.first,coordinate.second-1));
    if(coordinate.first-1 >= 0 && this->maskDFS.at<cv::Vec3b>(coordinate.first-1,coordinate.second) == currentMaskColor)
        this->dfs(std::pair<int,int>(coordinate.first-1,coordinate.second));
    if(coordinate.second+1 < this->maskDFS.cols && this->maskDFS.at<cv::Vec3b>(coordinate.first,coordinate.second+1) == currentMaskColor)
        this->dfs(std::pair<int,int>(coordinate.first,coordinate.second+1));
}
