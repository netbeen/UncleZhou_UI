#include "viewpatchdistributecavvas.h"

#include <QPainter>
#include <QMouseEvent>
#include <cmath>

ViewPatchDistributeCavvas::ViewPatchDistributeCavvas(QWidget* parent, const std::vector<PatchInfoNode>& patchInfo):QWidget(parent){
    this->setMinimumSize(600,600);
    this->setCursor(Qt::CrossCursor);

    this->patchInfo = patchInfo;
    this->drawList = std::vector<int>();
}

void ViewPatchDistributeCavvas::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QColor(255,255,255));
    painter.drawRect(rect());

    painter.setBrush(QColor(255,0,0));
    for(int i = 0; i < this->patchInfo.size(); i++){
        painter.drawEllipse(QPoint(this->patchInfo.at(i).x*this->width(),this->patchInfo.at(i).y*this->height()),4,4);
    }



    for(size_t elem =0; elem < this->patchInfo.size(); elem ++){
    //for(int elem : this->drawList){
        QImage image(patchInfo.at(elem).image.data, patchInfo.at(elem).image.cols, patchInfo.at(elem).image.rows, patchInfo.at(elem).image.step, QImage::Format_RGB888);
        painter.drawImage(this->patchInfo.at(elem).x*this->width(),this->patchInfo.at(elem).y*this->height(),image);
    }
}

void ViewPatchDistributeCavvas::mouseReleaseEvent(QMouseEvent *e){
    const float mouseX = (float)(e->pos().x())/this->width();
    const float mouseY = (float)(e->pos().y())/this->height();

    std::cout <<"mouse " << mouseX << " " << mouseY << std::endl;

    for(size_t i = 0; i < this->patchInfo.size(); i++){
        float distanceX = std::pow(mouseX-patchInfo.at(i).x,2);
        float distanceY = std::pow(mouseY-patchInfo.at(i).y,2);
        float distance = distanceX + distanceY;

        if(distance < 0.001){
            std::cout << "选中了"<<   i  << " " << "坐标"<< patchInfo.at(i).x << " " << patchInfo.at(i).y  << " 距离："<< distance<< std::endl;
            this->drawList.push_back(i);
            break;
        }
    }
    this->update();
}
