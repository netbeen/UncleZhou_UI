#include "densitypeakdialog.h"
#include "QGridLayout"
#include <QHBoxLayout>
#include <QVBoxLayout>


DensityPeakDialog::DensityPeakDialog(QWidget* parent) : QDialog(parent),scaleFactor(4.0)
{
    this->initColorTabel();
    this->initDialogLayout();

    this->sourceImageMat = cv::imread("./sourceImage.png");
    cv::resize(sourceImageMat,sourceImageMat,cv::Size(sourceImageMat.cols/this->scaleFactor,sourceImageMat.rows/this->scaleFactor) ,cv::INTER_LANCZOS4);
    //cv::imshow("sourceImageMat",sourceImageMat);

    cv::Mat features;
    Util::generateGaborFeatureFromImage(this->sourceImageMat,features);

    Util::calcDistanceAndDelta(features,this->v_points, this->v_density_Descend);

    std::cout << this->v_points.size() << std::endl;
    this->densityPeakCanvas->init(this->v_points);

    QObject::connect(this->densityPeakCanvas, &DensityPeakCanvas::selectCompetedSignal, this, &DensityPeakDialog::receiveSelectionCompeted);
}

// re-label image
void DensityPeakDialog::receiveSelectionCompeted(std::vector<int> selectPointIndex){
    std::cout <<"DensityPeakDialog::receiveSelectionCompeted total = " << selectPointIndex.size() << std::endl;
    if(selectPointIndex.size() > 27){
        std::cout << "DENY! selectPointIndex > 27" << std::endl;
        return;
    }
    assert(selectPointIndex.size() > 0);
    for(int i = 0; i < this->v_points.size(); i ++){
        v_points.at(i).label = -1;
    }
    for(int i = 0; i < selectPointIndex.size(); i ++){
        this->v_points.at(selectPointIndex.at(i)).label = i+1;
    }
    if(this->v_points.at(this->v_density_Descend.front().first).label == -1){
        std::cout << "DENY! MAX DENSITY DOES NOT BEEN SELECT!" << std::endl;
        return;
    }
    Util::labelOtherPoints(this->v_points, this->v_density_Descend);

    cv::Mat labelImageMat = cv::Mat(this->sourceImageMat.size(), CV_8UC3);

    int step = this->colorTabel.size()/selectPointIndex.size();
    int tempLabel;
    for(int i = 0; i < this->v_points.size(); i++){
        assert(v_points.at(i).label != -1);
        tempLabel = this->v_points.at(i).label;
        labelImageMat.at<cv::Vec3b>(i/labelImageMat.cols, i%labelImageMat.cols)[0] = this->colorTabel.at((tempLabel-1)*step).blue();
        labelImageMat.at<cv::Vec3b>(i/labelImageMat.cols, i%labelImageMat.cols)[1] = this->colorTabel.at((tempLabel-1)*step).green();
        labelImageMat.at<cv::Vec3b>(i/labelImageMat.cols, i%labelImageMat.cols)[2] = this->colorTabel.at((tempLabel-1)*step).red();
    }

    //cv::imshow("labelImageMat",labelImageMat);
    cv::Mat imgWithoutScale;
    Util::imgUndoScale(labelImageMat,imgWithoutScale,this->scaleFactor);
    cv::imwrite("./previewGuidance.png", imgWithoutScale);
    this->previewGuidanceImage->update();


}

void DensityPeakDialog::initColorTabel(){
    std::vector<int> enumColor = std::vector<int>{0,127,255};

    for(int i = 0; i < 3 ; i++){
        for(int j = 0; j < 3; j ++){
            for(int k = 0; k < 3; k++){
                this->colorTabel.push_back(QColor(enumColor.at(i),enumColor.at(j),enumColor.at(k)));
            }
        }
    }
}

void DensityPeakDialog::initDialogLayout(){
    this->setWindowTitle("Density Peak Dialog");

    this->setMinimumSize(1100,600);

    QHBoxLayout* mainHBoxLayout = new QHBoxLayout(this);
    this->setLayout(mainHBoxLayout);
    QVBoxLayout* leftLayout = new QVBoxLayout(this);
    mainHBoxLayout->addLayout(leftLayout);
    QVBoxLayout* rightLayout = new QVBoxLayout(this);
    mainHBoxLayout->addLayout(rightLayout);

    this->previewOriginImage = new ReadonlyCanvas("./sourceImage.png",this);
    rightLayout->addWidget(previewOriginImage);

    this->previewGuidanceImage = new ReadonlyCanvas("./previewGuidance.png",this);
    rightLayout->addWidget(previewGuidanceImage);

    this->densityPeakCanvas = new DensityPeakCanvas(this);
    leftLayout->addWidget(densityPeakCanvas);

}
