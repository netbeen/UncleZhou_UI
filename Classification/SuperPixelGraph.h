#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include "MyClassification.h"
#include "gco-v3.0/GCoptimization.h"
using namespace std;
using namespace cv;

class SuperPixelGraph
{
public:
	SuperPixelGraph(void);
	SuperPixelGraph(vector<vector<Point> >m_superpixelCoords, Mat m_superpixelID, Mat m_predictConf);
	~SuperPixelGraph(void);
	//void readSuperPixelData(Mat img, string dataDir);
	int  isContour(int curLabel, int row, int col);
	bool superPixelDFS(Mat &flags, int row, int col, vector<Point> &contour, vector<int> &neighborLabel);
	//void outputGraph();
	void buildGraph();

	// the following function is copy from gco-v3
	// set up a neighborhood system, general graph cut
	void GeneralGraph_DArraySArraySpatVarying(double smoothRatio);
	//void PrintGraphResult();
public:
	vector<int> resultLabel;
private:
	int numOfSuperPixels, numOfClasses;
	Mat pixelLabel;
	Mat predictConf;
	vector< vector<Point> > superPixelDat;
	vector<vector<cv::Point > > superPixelContour;
	vector<vector<int> > neighborSuperPixelLabel;
	vector<vector<int> > adjacentEdgeLength;
};

