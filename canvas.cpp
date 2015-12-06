#include "canvas.h"

#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QSize>

#include <assert.h>

/**
 * @brief Canvas::Canvas
 * @brief Canvas的构造函数。负责初始化图层管理器，载入背景图片，生成半透明模式图片
 * @param parent 作为canvas对象的父对象
 * @return 没有返回值
 */
Canvas::Canvas(QWidget* parent) : QWidget(parent), scaleFactor(1.0),operationType(config::None),isInited(false),isShowBackground(false),pencilRadius(10),eraserRadius(10)
{
    this->layerManager = LayerManager::getInstance();
    QObject::connect(this->layerManager, &LayerManager::displayLayerChanged, this, &Canvas::receiveDisplayLayerChanged);

    this->backgroundImage = QImage("sourceImage.png");
    this->backgroundPixmap = QPixmap::fromImage(this->backgroundImage);

    this->alpha = QImage(this->backgroundImage.width(), this->backgroundImage.height(), QImage::Format_RGBA8888);
    alpha.fill(QColor::fromRgbF(0.0,0.0,0.0,0.5));

    this->color = QColor(255,0,0);

}

/**
 * @brief Canvas::paintEvent
 * @brief 重写virtual的绘制函数，系统自动调用。负责更新canvas图像，绘制半透明背景，显示画笔等工具的绘制结果。
 * @param e
 * @return 没有返回值
 */
void Canvas::paintEvent(QPaintEvent* e){
    if(this->surfaceImage.isNull()){
        return;
    }

    this->surfaceImage = this->layerManager->getDisplayLayerItem()->image;
    if(this->isShowBackground == true){
        this->surfaceImage.setAlphaChannel(this->alpha.alphaChannel());     //将当前图形变为半透明
    }
    this->surfacePixmap = QPixmap::fromImage(this->surfaceImage);

    QSize imageSize = this->surfacePixmap.size();
    imageSize = imageSize*this->scaleFactor;

    if(this->isInited == false){
        this->setImageToTheCenter();
        this->isInited = true;
    }

    QPainter painter(this);     //声明绘制器
    if(this->isShowBackground == true){
            painter.drawPixmap(this->topLeftPoint, backgroundPixmap.scaled(imageSize, Qt::KeepAspectRatio));   //绘制背景
    }
    painter.drawPixmap(this->topLeftPoint, this->surfacePixmap.scaled(imageSize, Qt::KeepAspectRatio));   //绘制，制定左上角，绘制pixmap
    emit this->canvasUpdatedSignal();

}

/**
 * @brief Canvas::receiveDisplayLayerChanged
 * @brief 作为slot函数，接受图层管理器发出的显示图层改变的信号。遂更新Canvas的保存图像，并刷新canvas。
 * @param 没有参数
 * @return 没有返回值
 */
void Canvas::receiveDisplayLayerChanged(){
    this->surfaceImage = this->layerManager->getDisplayLayerItem()->image;
    this->update();
}


void Canvas::mousePressEvent(QMouseEvent *e){
    if(e->buttons() & Qt::LeftButton ){
        switch (this->operationType) {
            case config::None:
                return;
            case config::Move:
                this->moveStartPoint = e->pos();
                this->topLeftPointBackup = topLeftPoint;
                //qDebug() << "moveStartPoint=" << e->pos().x() << " " << e->pos().y();
                break;
            case config::ZoomIn:
                this->setScale(this->scaleFactor*1.1);
                this->update();
                //qDebug() << "Current scale factor = " << this->scaleFactor;
                break;
            case config::ZoomOut:
                this->setScale(this->scaleFactor*0.9);
                this->update();
                //qDebug() << "Current scale factor = " << this->scaleFactor;
                break;
            default:
                break;
        }
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::None:
                    return;
                case config::Pencil:
                    this->paint(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                    break;
                case config::Eraser:
                    this->erase(this->mapToPixmap(e->pos()),this->eraserRadius);
                    break;
                case config::Bucket:
                    this->bucket(this->color);
                    break;
                case config::Polygon:
                    this->polygon(this->mapToPixmap(e->pos()),this->color);
                    break;
                default:
                    break;
            }
        }    //以上操作只有在画布内部才有效

    }
}


void Canvas::mouseMoveEvent(QMouseEvent *e){
    if( e->buttons() & Qt::LeftButton ){
        QPoint delta;
        switch (this->operationType) {
            case config::None:
                return;
            case config::Move:{
                delta = e->pos() - this->moveStartPoint;
                this->topLeftPoint = topLeftPointBackup + delta;
                this->update();
                break;
            default:
                break;
            }
        }
        if(this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::None:
                    return;
                case config::Pencil:
                    this->paint(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                    break;
                case config::Eraser:
                    this->erase(this->mapToPixmap(e->pos()),this->eraserRadius);
                    break;
                default:
                    break;
            }
        }    //以上操作只有在画布内部才有效

    }
}


bool Canvas::isContained(const QPoint testPoint) const{
    assert(this->surfaceImage.isNull() == false);
    QSize canvasSize = QSize(this->surfaceImage.width()*this->scaleFactor, this->surfaceImage.height()*this->scaleFactor);
    QRect canvasRect = QRect(this->topLeftPoint, canvasSize);
    return canvasRect.contains(testPoint);
}

void Canvas::setOperationType(config::operationType inputOperationType){
    this->operationType = inputOperationType;
}


/**
 * @brief Canvas::paint
 * @brief 铅笔工具的绘制函数
 * @param center 表示鼠标点击的中心
 * @param radius 表示铅笔的有效半径
 * @param color 表示铅笔的绘制颜色
 * @return 没有返回值
 */
void Canvas::paint(const QPoint center, const int radius,const QColor color){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    for(int x = center.x()-radius; x < center.x()+radius; x++){
        for(int y = center.y()-radius; y < center.y()+radius; y++){
            if(Util::calcL2Distance(center, QPoint(x,y)) < radius){
                currentDisplayLayerItem->image.setPixel(x, y,color.rgb());
            }
        }
    }

    this->update();
}



void Canvas::polygon(const QPoint center, const QColor color){  //多边形工具
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    if(this->polygonStarted == false){
        //初始化
        this->polygonPointVector.clear();
        this->beforePolygonBackup = currentDisplayLayerItem->image;
        this->polygonStarted = true;
    }else{
        float distance = Util::calcL2Distance(center, this->polygonPointVector.front());
        if(distance < 8){
            cv::Mat_<cv::Vec3b> cvImage;
            Util::convertQImageToMat(this->beforePolygonBackup,cvImage);
            int pointNumber = this->polygonPointVector.size();

            cv::Point rook_points[1][pointNumber];
            for(int i = 0; i < pointNumber; i++){
                rook_points[0][i] = cv::Point( this->polygonPointVector.at(i).x(), this->polygonPointVector.at(i).y() );
            }
            const cv::Point* ppt[1] = { rook_points[0] };
            int npt[] = {pointNumber};
            cv::fillPoly(cvImage,ppt,npt,1,cv::Scalar(color.blue(),color.green(),color.red()));
            Util::convertMattoQImage(cvImage,this->beforePolygonBackup);
            currentDisplayLayerItem->image = this->beforePolygonBackup;
            this->update();

            this->polygonStarted = false;
            return;
        }
    }

    this->polygonPointVector.push_back(center);

    for(QPoint elem : this->polygonPointVector){
        for(int r = 10; r > 0; r--){
            if(r%2==0){
                this->paint(elem,r,QColor(0,0,0));
            }else{
                this->paint(elem,r,QColor(255,255,255));
            }
        }
    }
    this->paint(this->polygonPointVector.front(),6,color);

    this->update();
}


/**
 * @brief Canvas::erase
 * @brief 橡皮工具的绘制函数
 * @param center 表示橡皮的鼠标中心
 * @param radius 表示橡皮的有效半径
 * @return 没有返回值
 */
void Canvas::erase(const QPoint center, const int radius){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    for(int x = center.x()-radius; x < center.x()+radius; x++){
        for(int y = center.y()-radius; y < center.y()+radius; y++){
            if(Util::calcL2Distance(center, QPoint(x,y)) < radius){
                currentDisplayLayerItem->image.setPixel(x, y,QColor(255,255,255).rgb());
            }
        }
    }

    this->update();
}


/**
 * @brief Canvas::bucket
 * @brief 颜料桶工具的绘制函数，用特定的颜色填充整个图像区域
 * @param color 表示所绘制的颜色
 * @return 没有返回值
 */
void Canvas::bucket(const QColor color){
    QImage* image = &(this->layerManager->getDisplayLayerItem()->image);

    for(int x = 0; x < image->width(); x++){
        for(int y = 0; y < image->height(); y++){
                image->setPixel(x, y,color.rgb());
        }
    }

    this->update();
}


/**
 * @brief Canvas::mapToPixmap
 * @brief 将canvas中的一个点映射至实际图像的某一个像素上
 * @param screenPoint   canvas坐标中的点
 * @return 返回实际像素点的位置
 */
QPoint Canvas::mapToPixmap(QPoint screenPoint){
    QPoint finalPosition;
    finalPosition.setX((screenPoint.x() - this->topLeftPoint.x())/this->scaleFactor);
    finalPosition.setY((screenPoint.y() - this->topLeftPoint.y())/this->scaleFactor);
    return finalPosition;
}



/**
 * @brief Canvas::receiveShowBackground
 * @brief 作为slot函数，接收layerDock传来的“显示透明背景”的信号
 * @param isShow 作为是否显示背景的bool值
 * @return 没有返回值
 */
void Canvas::receiveShowBackground(bool isShow){
    this->isShowBackground = isShow;
    this->update();
}


void Canvas::setPencilRadius(int inputRadius){
    this->pencilRadius = inputRadius;
}


void Canvas::setEraserRadius(int inputRadius){
    this->eraserRadius = inputRadius;
}

void Canvas::setColor(QColor inputColor){
    this->color = inputColor;
}

void Canvas::setScale(float inputScaleFactor){
    this->scaleFactor = inputScaleFactor;
    emit this->scaleFactorChanged(this->scaleFactor);
}

void Canvas::resetScale(){
    this->scaleFactor = 1.0;
    emit this->scaleFactorChanged(this->scaleFactor);
    this->setImageToTheCenter();
    this->update();
}

void Canvas::setImageToTheCenter(){
    QSize imageSize = this->surfacePixmap.size();
    imageSize = imageSize*this->scaleFactor;

    this->topLeftPoint.setX((this->width() - imageSize.width()) / 2);
    this->topLeftPoint.setY((this->height() - imageSize.height()) / 2);
}
