#include "readsuperpixeldat.h"
#include <cmath>
#include <fstream>

ReadSuperPixelDat::ReadSuperPixelDat()
{

}

#include <string>
#include "SuperPixel/SLICSuperpixels/SLIC.h"

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


void ReadSuperPixelDat::main(){
    this->image = cv::imread("/home/netbeen/桌面/周叔项目/stone.png");
    this->size = this->image.cols * this->image.rows;
    std::cout << this->size << std::endl;

    this->authorSegment();
    this->authorRead();
    std::ofstream labelsFile("labels.txt");
    for(int i = 0; i < this->size; i++){
        //std::cout << i << std::endl;
        labelsFile << labels[i] << std::endl;
    }
    labelsFile.close();

    //输出分析文件
    this->analyseLabelFile();

    std::cout << "Done!" << std::endl;
}

void ReadSuperPixelDat::segmentSourceImage(){
    this->image = cv::imread("sourceImage.png");
    this->size = this->image.cols * this->image.rows;
    std::cout << this->size << std::endl;

    this->authorSegment();
    this->authorRead();
    std::ofstream labelsFile("labels.txt");
    for(int i = 0; i < this->size; i++){
        labelsFile << labels[i] << std::endl;
    }
    labelsFile.close();
}

void ReadSuperPixelDat::main(QString filename, int superPixelCount, float threshold){
    this->image = cv::imread(filename.toUtf8().data());
    this->size = this->image.cols * this->image.rows;
    std::cout << this->size << std::endl;

    this->authorSegment(superPixelCount);
    this->authorRead();
    std::ofstream labelsFile("labels.txt");
    for(int i = 0; i < this->size; i++){
        //std::cout << i << std::endl;
        labelsFile << labels[i] << std::endl;
    }
    labelsFile.close();

    this->analyseLabelFile(threshold);

    std::cout << "Done!" << std::endl;
}

void ReadSuperPixelDat::ReadImage(unsigned int* pbuff, int width,int height){//YOUR own function to read an image into the ARGB format
    std::cout << "ReadImage start" << std::endl;
    const int sz = this->size;
    this->pbuff = new unsigned int[sz];
    std::cout << "pbuff's address = " <<(pbuff) << std::endl;

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

    std::cout << "ReadImage end" << std::endl;
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
    cv::imwrite("result.png",this->result);

    std::cout << "SaveSegmentedImageFile end" << std::endl;
}

void ReadSuperPixelDat::authorRead(){
    FILE* pf = fopen("output.dat", "r");
    int sz = this->size;
    int* vals = new int[sz];
    int elread = fread((char*)vals, sizeof(int), sz, pf);
    for( int j = 0; j < this->image.rows; j++ )
    {
        for( int k = 0; k < this->image.cols; k++ )
        {
            int i = j*this->image.cols+k;
            labels[i] = vals[i];
        }
    }
    delete [] vals;
    fclose(pf);
}


void ReadSuperPixelDat::analyseLabelFile(float THRESHOLD){
    std::ifstream labelFile("labels.txt");
    std::vector<int> labels;
    int minLabel = INT32_MAX;
    int maxLabel= INT32_MIN;
    for(int i = 0; i < this->size; i++){
        int tempLabel;
        labelFile >> tempLabel;
        labels.push_back(tempLabel);
        minLabel = std::min(minLabel, tempLabel);
        maxLabel = std::max(maxLabel, tempLabel);
    }
    labelFile.close();
    std::cout << minLabel << " " << maxLabel << std::endl;
    //    for(int i = 0; i < this->size; i++){
    //        std::cout << i <<" " << labels.at(i) << std::endl;
    //    }

    std::vector<std::vector< std::pair<int,int> > > label2Coodinates(maxLabel+1);
    std::vector<int> label2Count(maxLabel+1,0);
    cv::Mat coodinate2Label = cv::Mat(this->image.rows, this->image.cols, CV_32S);

    for(int i = 0; i < this->size; i++){
        int y_offset = i/this->image.cols;
        int x_offset = i%this->image.cols;

        coodinate2Label.at<int>(y_offset,x_offset) = labels.at(i);      //CHECKED
        label2Count.at(labels.at(i))++;     //CHECKED
        label2Coodinates.at(labels.at(i)).push_back(std::pair<int,int>(y_offset,x_offset));     //CHECKED
    }

    this->mask = cv::imread("sourceGuidanceLabelChannel.png");

    this->maskID2Coodinates = std::vector<std::vector< std::pair<int,int> > >(0);
    this->maskDFS = mask.clone();

    this->colorCodeVector = std::vector<int>();
    //this->searchUnblackArea();
    std::cout << "this->searchUnwhiteArea(); start" << std::endl;
    this->searchUnwhiteArea();
    std::cout << "this->searchUnwhiteArea(); end" << std::endl;

    //std::cout << this->maskID2Coodinates.size() << std::endl;

    std::ofstream analyseResult("analyseResult.txt");
    analyseResult << "#labels" << std::endl;
    analyseResult << label2Coodinates.size() << std::endl;
    for(size_t i = 0 ; i < label2Coodinates.size(); i++){
        analyseResult << i << "\t"<< label2Coodinates.at(i).size() <<"\t";
        for(std::pair<int,int> elem : label2Coodinates.at(i)){
            analyseResult << elem.first << " " << elem.second << "\t";
        }
        analyseResult << std::endl;
    }
    //#masks(maskID,color,coverSuperPixelCount,superIDs...)
    analyseResult << "#masks" << std::endl;
    analyseResult << maskID2Coodinates.size() << std::endl;

    //const float THRESHOLD = 0.5;

    std::vector<std::vector<int> > resultVector;  //用于测试
    std::cout << "maskID2Coodinates.size():" <<maskID2Coodinates.size() << "  this->colorCodeVector.size():" << this->colorCodeVector.size() << std::endl;
    assert(maskID2Coodinates.size() == this->colorCodeVector.size());
    for(size_t i = 0 ; i < maskID2Coodinates.size(); i++){
        analyseResult << i << "\t"  << this->colorCodeVector.at(i) << "\t";
        std::vector<int> coverLabelCount(maxLabel+1,0);
        for(std::pair<int,int> elem : maskID2Coodinates.at(i)){
            int currentLabel = coodinate2Label.at<int>(elem.first, elem.second);
            if(currentLabel > maxLabel || currentLabel < 0){
                std::cout << "错误1:" << std::endl;
                std::cout << "currentLabel:" << currentLabel << std::endl;
                std::cout << "elem.first, elem.second: " << elem.first << " " << elem.second << std::endl;
            }
            coverLabelCount.at(currentLabel)++;
        }
        assert(coverLabelCount.size() == label2Count.size());
        std::vector<int> result;
        //遍历所有label，检测阈值比例是否符合
        for(size_t j = 0; j < coverLabelCount.size(); j++){
            if((float)coverLabelCount.at(j) / (float)label2Count.at(j) > THRESHOLD){
                result.push_back(j);
            }
        }
        analyseResult << result.size() << "\t" ;
        for(int elem : result){
            analyseResult << elem << "\t" ;
        }
        resultVector.push_back(result);
        analyseResult << std::endl;
    }


    /////////////////////////////////////////////////

    std::cout << "初始化maskImage" << std::endl;
    cv::Mat maskImage = cv::Mat(this->image.size(), this->image.type());
    for(int y_offset = 0; y_offset < maskImage.rows; y_offset++){
        for(int x_offset = 0; x_offset < maskImage.cols; x_offset++){
            maskImage.at<cv::Vec3b>(y_offset,x_offset)[0] = 0;
        }
    }
    for(size_t i = 0 ; i < maskID2Coodinates.size(); i++){
        std::vector<std::pair<int,int> > tempCoordinates = maskID2Coodinates.at(i);
        for(std::pair<int,int> elem : tempCoordinates){
            maskImage.at<cv::Vec3b>(elem.first,elem.second) = this->image.at<cv::Vec3b>(elem.first,elem.second);
        }
    }

    cv::Mat SPImage = cv::Mat(this->image.size(), this->image.type());
    for(int y_offset = 0; y_offset < SPImage.rows; y_offset++){
        for(int x_offset = 0; x_offset < SPImage.cols; x_offset++){
            SPImage.at<cv::Vec3b>(y_offset,x_offset)[0] = 0;
        }
    }
    for(std::vector<int> result : resultVector){
        for(int elem : result){
            std::vector< std::pair<int,int> > tempCoords = label2Coodinates.at(elem);
            for(std::pair<int,int> tempCoord : tempCoords){
                SPImage.at<cv::Vec3b>(tempCoord.first,tempCoord.second) = this->image.at<cv::Vec3b>(tempCoord.first,tempCoord.second);
            }
        }
    }

    analyseResult.close();
}

int ReadSuperPixelDat::cvtRGB2ColorCode(int red, int green, int blue){
    int result = 0;
    result += (red/127)*9;
    result += (green/127)*3;
    result += blue/127;
    return result;
}

void ReadSuperPixelDat::searchUnblackArea(){
    for(int y_offset = 0; y_offset < this->maskDFS.rows; y_offset++){
        for(int x_offset = 0; x_offset < this->maskDFS.cols; x_offset++){
            cv::Vec3b currentColor = this->maskDFS.at<cv::Vec3b>(y_offset,x_offset);


            if(currentColor[0] != 0 || currentColor[1] != 0 || currentColor[2] != 0){
                int colorCode = this->cvtRGB2ColorCode(currentColor[2],currentColor[1],currentColor[0]);
                this->colorCodeVector.push_back(colorCode);

                std::cout << "Find unblack area: " << y_offset << " " << x_offset  << " ColorCode = " << colorCode<< std::endl;
                this->coordinates = std::vector< std::pair<int,int> >();
                this->dfs(std::pair<int,int>(y_offset,x_offset));
                this->maskID2Coodinates.push_back(this->coordinates);
                std::cout << "This area of mask has point count: " << this->coordinates.size() << std::endl;
            }
        }
    }
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
                //std::cout << "This area of mask has point count: " << this->coordinates.size() << std::endl;
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
