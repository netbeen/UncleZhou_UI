#include <opencv2/opencv.hpp>

#include "MyClassification.h"

#include "GetImageFeatures.h"
#include "MyImageProc.h"

#include <fstream>
#include <string>
#include <iostream>
#include <time.h>

using namespace std;


void main()
{
    long start_time=clock();

    std::string dir_folder = "E:/Codes/Clustering/SharkTest2/SharkTest/data/";
    std::string dir_image = dir_folder + "stone_floor/";
    std::string input_image = "src";
    std::string ex_image = "png";
    std::string input_mask = "mask-color";
    std::string ex_mask = "png";
    std::string output_image = "trg";
    std::string dirSuperPixelDat = dir_image + "output.dat";
    float imgscale = 1.0;
    int patchSize = 16;
    int numBins = 16;
    cv::Vec3b BK_Color(uchar(255), uchar(255), uchar(255)); //black
    cv::Vec3b Cur_Color(uchar(255), 0, 0); //blue

    // Read Source Image
    std::string imgfile = dir_image + input_image + "." + ex_image;
    cv::Mat img = cv::imread(imgfile);

    // Read Mask
    std::string maskfile = dir_image + input_mask + "." + ex_mask;
    cv::Mat mask = cv::imread(maskfile);

	// Read Edgemap, Saliency map
	std::string input_texton = dir_image + "src_texton.png";
	std::string input_edgemap = dir_image + "src_gpb.png";
	std::string input_saliencymap = dir_image + "src_saliency.png";
	cv::Mat edgemap = cv::imread(input_edgemap, CV_8UC1);
	CMyImageProc tt;
	cv::Mat salmap = tt.GenSaliencyMap(img);
	cv::imwrite(input_saliencymap, salmap);

// 	cv::Mat textonImg = tt.GenTextonMap(img);
// 	cv::imwrite(input_texton, textonImg);

	// Run RFBinaryClassification
	std::vector<std::string> morefeatimgs;
	morefeatimgs.push_back(input_texton);
	morefeatimgs.push_back(input_edgemap);
	morefeatimgs.push_back(input_saliencymap);

	cv::Mat outputImg;
    CMyClassification myTest;
    myTest.RandomForest_SuperPixel(img, morefeatimgs, mask, outputImg, dirSuperPixelDat);
	myTest.RunGraphCut(outputImg);

	//myTest.RandomForestClassification(img, mask, outputImg);
	//myTest.RandomForestClassification(img, mask, outputImg);
    cv::imwrite(dir_image+output_image+"."+ex_mask, outputImg);

    long end_time =clock();
    cout<< "Total running time is: "<<static_cast<double>(end_time-start_time)/CLOCKS_PER_SEC<<"ms"<<endl;
    ///system("pause");
}


	/*
// Win32TestPure.cpp : 定义控制台应用程序的入口点。  
#include <vector>  
#include <deque>  
#include <iomanip>  
#include <stdexcept>  
#include <string>  
#include <iostream>  
#include <fstream>  
#include <direct.h>//_mkdir()  
#include "opencv2\opencv.hpp"  
#include "MyGaborFilter.h"  
using namespace std;  
using namespace cv;  

int main()  
{   
	//Mat M = getGaborKernel(Size(9,9),2*CV_PI,u*CV_PI/8, 2*CV_PI/pow(2,CV_PI*(v+2)/2),1,0);  
	Mat saveM;  
	//s8-4  
	//s1-5  
	//s1中年男人  
	Mat I=imread("E:\\Codes\\Clustering\\SharkTest2\\SharkTest\\data\\stone_floor\\src.png"); //("H:\\pic\\s1-5.bmp",-1);  
	normalize(I,I,1,0,CV_MINMAX,CV_32F);  
	int tl = I.type();
	Mat showM,showMM;Mat M,MatTemp1,MatTemp2;  
	Mat line;  
	int iSize=50;//如果数值比较大，比如50则接近论文中所述的情况了！估计大小和处理的源图像一样！  
	for(int i=0;i<8;i++)  
	{  
		showM.release();  
		for(int j=0;j<5;j++)  
		{  
			Mat M1= GaborFR::getRealGaborKernel(Size(iSize,iSize),2*CV_PI,i*CV_PI/8+CV_PI/2, j,1, CV_32F);  
			Mat M2 = GaborFR::getImagGaborKernel(Size(iSize,iSize),2*CV_PI,i*CV_PI/8+CV_PI/2, j,1, CV_32F);  
			//加了CV_PI/2才和大部分文献的图形一样，不知道为什么！  
			Mat outR,outI;  
			GaborFR::getFilterRealImagPart(I,M1,M2,outR,outI);  
			//          M=GaborFR::getPhase(M1,M2);  
			//          M=GaborFR::getMagnitude(M1,M2);  
			//          M=GaborFR::getPhase(outR,outI);  
			//          M=GaborFR::getMagnitude(outR,outI);  
			//         M=GaborFR::getMagnitude(outR,outI);  
			//          MatTemp2=GaborFR::getPhase(outR,outI);  
			//          M=outR;  
			M=M1;  
			//      resize(M,M,Size(100,100));  
			normalize(M,M,0,255,CV_MINMAX,CV_8U);  
			showM.push_back(M);  
			line=Mat::ones(4,M.cols,M.type())*255;  
			showM.push_back(line);  
		}  
		showM=showM.t();  
		line=Mat::ones(4,showM.cols,showM.type())*255;  
		showMM.push_back(showM);  
		showMM.push_back(line);  
	}  
	showMM=showMM.t();  
	//  bool flag=imwrite("H:\\out.bmp",showMM);  
	imshow("saveMM",showMM);waitKey(0);  
	return 0;  
}//endof   main()  
*/