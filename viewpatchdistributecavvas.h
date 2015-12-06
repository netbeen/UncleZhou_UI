#ifndef VIEWPATCHDISTRIBUTECAVVAS_H
#define VIEWPATCHDISTRIBUTECAVVAS_H

#include <QWidget>
#include <opencv2/opencv.hpp>

class PatchInfoNode{
public:
    int id;
    float x;
    float y;
    cv::Mat image;
};


class ViewPatchDistributeCavvas :public QWidget
{
    Q_OBJECT
public:
    ViewPatchDistributeCavvas(QWidget* parent, const std::vector<PatchInfoNode>& patchInfo);

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;

private:
    std::vector<PatchInfoNode> patchInfo;
    std::vector<int> drawList;
};

#endif // VIEWPATCHDISTRIBUTECAVVAS_H
