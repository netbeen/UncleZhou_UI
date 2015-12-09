#ifndef GRAPHCUT_H
#define GRAPHCUT_H

#include <opencv2/opencv.hpp>


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include "gco-v3.0/GCoptimization.h"

class GraphCut
{
public:
    GraphCut();
    void main();

private:
    int CLASS_NUMBER;
    int scaleFactor = 1;
    cv::Mat rawImage;

    cv::Mat initGuess;
    cv::Mat resultLabel;
    std::vector<cv::Vec3b> label2Value;
    cv::Mat GMMProbability;
    cv::Mat initGuessMask;

    bool checkUserMarkValid(const cv::Mat& userMark);
    void GridGraph_Individually(int width,int height,int num_pixels,int num_labels);
    void generateGMMProbability();
    void generateInitGuessMask();
};

#endif // GRAPHCUT_H
