#ifndef VIEWPATCHDISTRIBUTEDIALOG_H
#define VIEWPATCHDISTRIBUTEDIALOG_H

#include <QDialog>
#include "viewpatchdistributecavvas.h"
#include <opencv2/opencv.hpp>



class ViewPatchDistributeDialog : public QDialog
{
    Q_OBJECT
public:
    ViewPatchDistributeDialog(QWidget* parent);

private:
    cv::Mat sourceImage;
    const int patchSize;
    std::vector<PatchInfoNode> patchInfo;

    void initDialogLayout();
    void initData();

    void mapReturn2OriginSize();    //2015年12月23日添加，把坐标的灰度映射回原图大小

    ViewPatchDistributeCavvas* viewPatchDistributeCavvas;
};

#endif // VIEWPATCHDISTRIBUTEDIALOG_H
