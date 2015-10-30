#include "densitypeakcanvas.h"


#include <QPainter>
#include <QMouseEvent>

const int PADDING = 25;

DensityPeakCanvas::DensityPeakCanvas(QWidget* parent) : QWidget(parent), inited(false),selectionValid(false),densityMax(-1),deltaMax(-1){
    this->setMinimumSize(600,600);
    this->setCursor(Qt::CrossCursor);
    this->selectPointIndex = std::vector<int>();

}

void DensityPeakCanvas::mousePressEvent(QMouseEvent *e){
    this->selectionTopleft.setX(e->pos().x());
    this->selectionTopleft.setY(e->pos().y());
    this->selectionValid = true;
}

void DensityPeakCanvas::mouseMoveEvent(QMouseEvent *e){
    this->selectionButtomDown.setX(e->pos().x());
    this->selectionButtomDown.setY(e->pos().y());
    this->update();
}

void DensityPeakCanvas::mouseReleaseEvent(QMouseEvent *e){
    int densityLow = std::min(this->selectionTopleft.x(),this->selectionButtomDown.x());
    densityLow = (double)(densityLow-PADDING)/(this->width()-PADDING*2)*this->densityMax;
    int densityHigh = std::max(this->selectionTopleft.x(),this->selectionButtomDown.x());
    densityHigh = (double)(densityHigh-PADDING)/(this->width()-PADDING*2)*this->densityMax;
    double deltaLow = std::max(this->selectionTopleft.y(),this->selectionButtomDown.y());
    deltaLow = -((deltaLow + PADDING-this->height()) / (this->height()-PADDING*2)*this->deltaMax);
    double deltaHigh = std::min(this->selectionTopleft.y(),this->selectionButtomDown.y());
    deltaHigh = -((deltaHigh + PADDING-this->height()) / (this->height()-PADDING*2)*this->deltaMax);

    selectPointIndex.clear();
    for(int i = 0; i < this->v_points.size(); i++){
        if(v_points.at(i).density >= densityLow && v_points.at(i).density <= densityHigh && v_points.at(i).dis2NNHD >= deltaLow && v_points.at(i).dis2NNHD <= deltaHigh){
            selectPointIndex.push_back(i);
        }
    }
    this->update();
    if(this->selectPointIndex.empty() == false){
        emit this->selectCompetedSignal(this->selectPointIndex);
    }
}

void DensityPeakCanvas::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter.setBrush(QColor(192,192,192));
    painter.drawRect(rect());

    painter.drawLine(QPoint(0+PADDING,this->height()-1-PADDING),QPoint(this->width()-1-PADDING,this->height()-1-PADDING));    //density轴
    painter.drawLine(QPoint(0+PADDING,0+PADDING),QPoint(0+PADDING,this->height()-1-PADDING)); //delta轴

    if(this->inited == true){
        painter.setBrush(QColor(0,0,255));
        for(ClusteringPoints elem : this->v_points){
            painter.drawEllipse(QPoint(elem.density/this->densityMax*(this->width()-PADDING*2)+PADDING,this->height() - elem.dis2NNHD/this->deltaMax*(this->height() - PADDING *2)-PADDING),4,4);
        }
        if(this->selectionValid == true){
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(Qt::green, 2, Qt::DashLine));
            painter.drawRect(QRect(this->selectionTopleft, this->selectionButtomDown));
            painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
            painter.setBrush(QColor(255,0,0));
            for(int index : this->selectPointIndex){
                painter.drawEllipse(QPoint(v_points.at(index).density/this->densityMax*(this->width()-PADDING*2)+PADDING,this->height() - v_points.at(index).dis2NNHD/this->deltaMax*(this->height() - PADDING *2)-PADDING),4,4);
            }
        }
    }
}

void DensityPeakCanvas::init(const std::vector<ClusteringPoints>& v_points){
    this->v_points = v_points;
    double secondDelta = v_points.at(0).dis2NNHD;
    int maxDeltaIndex = -1;
    for(int i = 0; i < v_points.size(); i++){
        if(this->densityMax < v_points.at(i).density){
            this->densityMax = v_points.at(i).density;
            this->maxDensityIndex = i;
        }
        if(this->deltaMax < v_points.at(i).dis2NNHD){
            secondDelta = this->deltaMax;
            //std::cout << "second :" << secondDelta << " , max :" << v_points.at(i).dis2NNHD << std::endl;
            this->deltaMax = v_points.at(i).dis2NNHD;
            maxDeltaIndex = i;
        }
    }
    this->v_points.at(maxDeltaIndex).dis2NNHD = secondDelta;
    this->deltaMax = secondDelta;
    this->inited = true;
}
