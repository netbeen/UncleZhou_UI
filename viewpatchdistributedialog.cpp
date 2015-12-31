#include "viewpatchdistributedialog.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <fstream>
#include <climits>

ViewPatchDistributeDialog::ViewPatchDistributeDialog(QWidget* parent):QDialog(parent),patchSize(10){
    this->initData();
    this->initDialogLayout();
}

void ViewPatchDistributeDialog::initData(){
    this->sourceImage = cv::imread("/home/netbeen/桌面/周叔项目/exemplar.png");
    cv::cvtColor(this->sourceImage, this->sourceImage, cv::COLOR_BGR2RGB);
    this->patchInfo = std::vector<PatchInfoNode>(2100);

    std::ifstream inputFile("/home/netbeen/桌面/周叔项目/newCoord-20151223-2.txt");

    const int patchCountX = this->sourceImage.cols/this->patchSize;
    const int patchCountY = this->sourceImage.rows/this->patchSize;

    std::cout << patchCountX << " " << patchCountY << std::endl;

    for(size_t i = 0; i <this->patchInfo.size(); i++){
        float x,y;
        inputFile >> x >> y;
        PatchInfoNode patchInfoNode;
        cv::Rect rect = cv::Rect(i/patchCountX*this->patchSize,i%patchCountY*this->patchSize,this->patchSize,this->patchSize);
        cv::Mat image = sourceImage(rect);

        cv::resize(image,image,cv::Size(30,30));

        patchInfoNode.id = i;
        patchInfoNode.x = x;
        patchInfoNode.y = y;
        patchInfoNode.image = image;
        this->patchInfo.at(i) = patchInfoNode;
    }
    inputFile.close();

    //normalize start
    float xMin = INT32_MAX;
    float yMin = INT32_MAX;
    float xMax = INT32_MIN;
    float yMax = INT32_MIN;

    //找出x坐标与y坐标的最大值
    for(size_t i = 0; i < patchInfo.size(); i++){
        xMin = std::min(xMin,this->patchInfo.at(i).x);
        xMax = std::max(xMax,this->patchInfo.at(i).x);
        yMin = std::min(yMin, this->patchInfo.at(i).y);
        yMax = std::max(yMax, this->patchInfo.at(i).y);
    }
    std::cout << xMax << " " << xMin << " " << yMax << " " << yMin << std::endl;
    for(size_t i = 0; i < patchInfo.size(); i++){
        patchInfo.at(i).x -= xMin;
        patchInfo.at(i).y -= yMin;
    }
    float xRange = xMax - xMin;
    float yRange = yMax - yMin;
    for(size_t i = 0; i < patchInfo.size(); i++){
        patchInfo.at(i).x /= xRange;
        patchInfo.at(i).y /= yRange;
    }

    this->mapReturn2OriginSize();
}

void ViewPatchDistributeDialog::mapReturn2OriginSize(){
    cv::Mat visualizeResult = cv::Mat(this->sourceImage.size(), CV_8UC3, cv::Scalar(0,0,255));

    int patchCountPerRow = this->sourceImage.cols/this->patchSize;
    int patchCountPerColumn = this->sourceImage.rows/this->patchSize;

    for(int i = 0; i < this->patchInfo.size();i++){
        int patchRowIndex = i/patchCountPerRow;
        int patchColumnIndex = i%patchCountPerRow;
        int patchUpleftX = patchColumnIndex*this->patchSize;
        int patchUpleftY = patchRowIndex*this->patchSize;

        float xValue  = this->patchInfo.at(i).x;
        xValue = 1- xValue;
        cv::Vec3b color = cv::Vec3b(255*xValue,255*xValue,255*xValue);
        for(int y_offset_patch = 0; y_offset_patch < this->patchSize; y_offset_patch++){
            for(int x_offset_patch = 0; x_offset_patch < this->patchSize; x_offset_patch++){
                visualizeResult.at<cv::Vec3b>(patchUpleftY+y_offset_patch, patchUpleftX+x_offset_patch) = color;
            }
        }
    }
    cv::imshow("visualizeResult",visualizeResult);
    cv::imwrite("debug-visualizeResult.png",visualizeResult);
    cv::waitKey();



}


void ViewPatchDistributeDialog::initDialogLayout(){
    this->setWindowTitle("View Patch Distribute");

    this->setMinimumSize(1100,600);

    QHBoxLayout* mainHBoxLayout = new QHBoxLayout(this);
    this->setLayout(mainHBoxLayout);
    QVBoxLayout* leftLayout = new QVBoxLayout(this);
    mainHBoxLayout->addLayout(leftLayout);
    QVBoxLayout* rightLayout = new QVBoxLayout(this);
    mainHBoxLayout->addLayout(rightLayout);

    this->viewPatchDistributeCavvas = new ViewPatchDistributeCavvas(this,this->patchInfo);
    leftLayout->addWidget(this->viewPatchDistributeCavvas);
}
