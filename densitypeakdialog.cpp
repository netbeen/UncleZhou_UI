#include "densitypeakdialog.h"
#include "QGridLayout"


DensityPeakDialog::DensityPeakDialog(QWidget* parent) : QDialog(parent),scaleFactor(4.0)
{
    this->initDialogLayout();

    this->sourceImageMat = cv::imread("./sourceImage.png");
    cv::resize(sourceImageMat,sourceImageMat,cv::Size(sourceImageMat.cols/this->scaleFactor,sourceImageMat.rows/this->scaleFactor) ,cv::INTER_LANCZOS4);
    //cv::imshow("sourceImageMat",sourceImageMat);

    cv::Mat features;
    Util::generateGaborFeatureFromImage(this->sourceImageMat,features);

    std::vector<ClusteringPoints> v_points;
    Util::calcDistanceAndDelta(features,v_points);

    std::cout << v_points.size() << std::endl;
    this->densityPeakCanvas->init(v_points);
}

void DensityPeakDialog::initDialogLayout(){
    this->setWindowTitle("Density Peak Dialog");

    this->setMinimumSize(1100,600);
    QGridLayout* mainGridLayout = new QGridLayout(this);
    this->setLayout(mainGridLayout);

    this->previewOriginImage = new ReadonlyCanvas("./sourceImage.png",this);
    mainGridLayout->addWidget(this->previewOriginImage,0,1);

    this->previewGuidanceImage = new ReadonlyCanvas("./previewGuidance.png",this);
    mainGridLayout->addWidget(this->previewGuidanceImage,1,1);

    this->densityPeakCanvas = new DensityPeakCanvas(this);
    mainGridLayout->addWidget(this->previewGuidanceImage,0,0,2,1);
}
