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

    std::ifstream inputFile("/home/netbeen/桌面/周叔项目/HistoEMD_newCoord.txt");

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
