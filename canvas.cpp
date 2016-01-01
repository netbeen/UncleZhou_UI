#include "canvas.h"

#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QSize>
#include <imageeditwindow.h>

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

    this->backgroundImage = QImage(Util::dirName+"/sourceImage.png");
    this->backgroundPixmap = QPixmap::fromImage(this->backgroundImage);

    this->alpha = QImage(this->backgroundImage.width(), this->backgroundImage.height(), QImage::Format_RGBA8888);
    alpha.fill(QColor::fromRgbF(0.0,0.0,0.0,0.5));

    this->color = QColor(255,0,0);

    this->undoStack = UndoStack::getInstance();

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

/**
 * @brief Canvas::mousePressEvent
 * @brief 鼠标点击响应函数
 * @param e
 * @return 没有返回值
 */
void Canvas::mousePressEvent(QMouseEvent *e){
    if(e->buttons() & Qt::LeftButton ){
        switch (this->operationType) {
            case config::None:
                return;
            case config::Move:
                this->moveStartPoint = e->pos();
                this->topLeftPointBackup = topLeftPoint;
                break;
            case config::ZoomIn:
                this->setScale(this->scaleFactor*1.1);
                this->update();
                break;
            case config::ZoomOut:
                this->setScale(this->scaleFactor*0.9);
                this->update();
                break;
            default:
                break;
        }
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::None:
                    return;
                case config::Pencil:
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->paint(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                    break;
                case config::Eraser:
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->erase(this->mapToPixmap(e->pos()),this->eraserRadius);
                    break;
                case config::Bucket:
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->bucket(this->color);
                    break;
                case config::Polygon:
                    this->polygon(this->mapToPixmap(e->pos()),this->color);
                    break;
                case config::BrokenLine:
                    this->brokenLine(this->mapToPixmap(e->pos()));
                    break;
                case config::MagicEraser:
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->magicErase(this->mapToPixmap(e->pos()));
                    break;
                case config::Gaussian:
                    emit this->sendCoordinateSignal(this->mapToPixmap(e->pos()).x(), this->mapToPixmap(e->pos()).y());
                    break;
                case config::VectorField:
                    if(this->brokenLineStarted == false){
                        this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                        this->layerManager->getDisplayLayerItem()->image = Util::drawVectorFieldBackgroundBackup;
                        this->vectorFieldCurve.clear();
                    }
                    this->vectorFieldCurve.push_back(this->mapToPixmap(e->pos()));
                    this->brokenLine(this->mapToPixmap(e->pos()));
                    break;
                default:
                    break;
            }

        }    //以上操作只有在画布内部才有效
    }else if(e->buttons() & Qt::RightButton){
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::Eraser:    //右键状态下，橡皮擦变为魔术橡皮擦
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->magicErase(this->mapToPixmap(e->pos()));
                    break;
                case config::BrokenLine:    //右键状态下，折线工具调用折线结束的功能
                    this->brokenLineEnd();
                    break;
                case config::VectorField:
                    Util::vectorFieldCurves.push_back(this->vectorFieldCurve);
                    this->brokenLineEnd();
                    this->repaint();        //强制刷新画布
                    Util::drawVectorFieldBackgroundBackup = this->layerManager->getDisplayLayerItem()->image;
                    emit this->calcVectorFieldSignal();
                    break;
                default:
                    break;
            }
        }//以上操作只有在画布内部才有效
    }else if(e->buttons() & Qt::MiddleButton){
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::VectorField:
                    this->undoStack->push(this->layerManager->getDisplayLayerItem()->image);
                    this->layerManager->getDisplayLayerItem()->image = Util::drawVectorFieldBackgroundBackup;
                    this->paint(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                    this->vectorFieldCurve.clear();
                default:
                    break;
            }
        }//以上操作只有在画布内部才有效
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *e){
    if(e->button() == Qt::LeftButton ){
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
        }    //以上操作只有在画布内部才有效
        switch (this->operationType) {
            case config::Pencil:
                emit this->updateColorButtonLayoutSignal();
                break;
            case config::Eraser:
                emit this->updateColorButtonLayoutSignal();
                break;
            case config::MagicEraser:
                emit this->updateColorButtonLayoutSignal();
                break;
            case config::Polygon:
                emit this->updateColorButtonLayoutSignal();
                break;
            case config::BrokenLine:
                emit this->updateColorButtonLayoutSignal();
                break;
            default:
                break;
        }
    }else if(e->button() == Qt::MiddleButton ){
        if( this->isContained(e->pos())){    //以下操作只有在画布内部才有效
        }    //以上操作只有在画布内部才有效
        switch (this->operationType) {
            case config::VectorField:
                Util::vectorFieldCurves.push_back(this->vectorFieldCurve);
                Util::drawVectorFieldBackgroundBackup = this->layerManager->getDisplayLayerItem()->image;
                emit this->calcVectorFieldSignal();
                break;
            default:
                break;
        }
    }
}


/**
 * @brief Canvas::mouseMoveEvent
 * @brief 鼠标移动响应函数
 * @param e
 * @return 没有返回值
 */
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
    }else if( e->buttons() & Qt::MiddleButton ){
        if(this->isContained(e->pos())){    //以下操作只有在画布内部才有效
            switch (this->operationType) {
                case config::VectorField:
                    this->vectorFieldCurve.push_back(this->mapToPixmap(e->pos()));
                    this->paint(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                    break;
                default:
                    break;
            }
        }    //以上操作只有在画布内部才有效
    }else{
        switch (this->operationType) {
            case config::BrokenLine:
                this->brokenLineMove(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                break;
            case config::VectorField:
                this->vectorFieldCurve.push_back(this->mapToPixmap(e->pos()));
                this->brokenLineMove(this->mapToPixmap(e->pos()),this->pencilRadius,this->color);
                break;
            default:
                break;
        }
    }
}


QColor Canvas::getColor() const{
    return this->color;
}

/**
 * @brief Canvas::isContained
 * @brief 判断当前点是否在画布内部
 * @param testPoint 当前点的坐标
 * @return bool型判断结果
 */
bool Canvas::isContained(const QPoint testPoint) const{
    assert(this->surfaceImage.isNull() == false);
    QSize canvasSize = QSize(this->surfaceImage.width()*this->scaleFactor, this->surfaceImage.height()*this->scaleFactor);
    QRect canvasRect = QRect(this->topLeftPoint, canvasSize);
    return canvasRect.contains(testPoint);
}

/**
 * @brief Canvas::setOperationType
 * @brief 设置当前canvas的操作模式
 * @param inputOperationType
 * @return 没有返回值
 */
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

void Canvas::paint(const QPoint center, const int radius,const QColor color,QImage& efffectiveImage){
    for(int x = center.x()-radius; x < center.x()+radius; x++){
        for(int y = center.y()-radius; y < center.y()+radius; y++){
            if(Util::calcL2Distance(center, QPoint(x,y)) < radius){
                efffectiveImage.setPixel(x, y,color.rgb());
            }
        }
    }
}

void Canvas::brokenLine(const QPoint center){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    if(this->brokenLineStarted == false){
        //初始化
        this->brokenLinePointVector.clear();
        this->undoStack->push(currentDisplayLayerItem->image);  //压入撤销栈
        this->brokenLineStarted = true;
        this->setMouseTracking(true);   //保持鼠标追踪
    }

    this->beforeBrokenLineBackup = currentDisplayLayerItem->image;  //保存在本次鼠标点击前的状态
    this->brokenLineStartPoint = center;
}

void Canvas::brokenLineMove(const QPoint center, const int radius, const QColor color){  //折线ing，鼠标移动工具
    QImage beforeBrokenLineBackupCopy = this->beforeBrokenLineBackup;

    QPoint startPoint = this->brokenLineStartPoint;
    QPoint endPoint = center;

    float distance = Util::calcL2Distance(startPoint, endPoint);
    float unitDensity = 1;

    float deltaX = (endPoint.x() - startPoint.x())/unitDensity/distance;
    float deltaY = (endPoint.y() - startPoint.y())/unitDensity/distance;

    for(int pointCount = 0; pointCount < unitDensity*distance; pointCount++){
        this->paint(QPoint(startPoint.x()+deltaX*pointCount, startPoint.y()+deltaY*pointCount),brokenLineRadius,color,beforeBrokenLineBackupCopy);
    }

    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    currentDisplayLayerItem->image = beforeBrokenLineBackupCopy;
    this->update();
}

void Canvas::brokenLineEnd(){  //折线结束工具
    this->brokenLineStarted = false;
    this->setMouseTracking(false);   //结束鼠标追踪
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();
    currentDisplayLayerItem->image = this->beforeBrokenLineBackup;
    this->update();
    return;
}

/**
 * @brief Canvas::polygon
 * @brief 多边形绘制工具，按照顺序调用该函数，当最后一个点靠近起始点时，判定为结束点，然后绘制opencv多边形
 * @param center 鼠标的点击中心
 * @param color 多边形的颜色
 * @return 没有返回值
 */
void Canvas::polygon(const QPoint center, const QColor color){  //多边形工具
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    if(this->polygonStarted == false){
        //初始化
        this->polygonPointVector.clear();
        this->undoStack->push(currentDisplayLayerItem->image);
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
 * @brief Canvas::magicErase
 * @brief 魔术橡皮擦工具，输入鼠标位置，然后就可以就用DFS将整块的色块消除
 * @param center 鼠标中心点
 * @return 没有返回值
 */
void Canvas::magicErase(const QPoint center){
    LayerItem* currentDisplayLayerItem = this->layerManager->getDisplayLayerItem();

    cv::Mat_<cv::Vec3b> cvMat;
    Util::convertQImageToMat(currentDisplayLayerItem->image,cvMat);
    Util::replaceColorBlockDFS(cvMat,cv::Point(center.x(),center.y()),cv::Vec3b(255,255,255));
    Util::convertMattoQImage(cvMat,currentDisplayLayerItem->image);

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


/**
 * @brief Canvas::setPencilRadius
 * @brief 设置铅笔的半径
 * @param inputRadius 新的半径
 * @return 没有返回值
 */
void Canvas::setPencilRadius(int inputRadius){
    this->pencilRadius = inputRadius;
}


/**
 * @brief Canvas::setEraserRadius
 * @brief 设置橡皮半径
 * @param inputRadius 新的半径
 * @return 没有返回值
 */
void Canvas::setEraserRadius(int inputRadius){
    this->eraserRadius = inputRadius;
}

void Canvas::setBrokenLineRadius(int inputRadius){
    this->brokenLineRadius = inputRadius;
}

/**
 * @brief Canvas::setColor
 * @brief 设置主调色板颜色
 * @param inputColor 新的主调色板颜色
 * @return 没有返回值
 */
void Canvas::setColor(QColor inputColor){
    this->color = inputColor;
}


/**
 * @brief Canvas::setScale
 * @brief 设置缩放参数
 * @param inputScaleFactor 新的缩放参数
 * @return 没有返回值
 */
void Canvas::setScale(float inputScaleFactor){
    this->scaleFactor = inputScaleFactor;
    emit this->scaleFactorChanged(this->scaleFactor);
}


/**
 * @brief Canvas::resetScale
 * @brief 恢复缩放状态至初始化状态
 * @return 没有返回值
 */
void Canvas::resetScale(){
    this->scaleFactor = 1.0;
    emit this->scaleFactorChanged(this->scaleFactor);
    this->setImageToTheCenter();
    this->update();
}


/**
 * @brief Canvas::setImageToTheCenter
 * @brief 设置图像显示在画布中心
 * @return 没有返回值
 */
void Canvas::setImageToTheCenter(){
    QSize imageSize = this->surfacePixmap.size();
    imageSize = imageSize*this->scaleFactor;

    this->topLeftPoint.setX((this->width() - imageSize.width()) / 2);
    this->topLeftPoint.setY((this->height() - imageSize.height()) / 2);
}
