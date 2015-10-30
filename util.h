#ifndef UTIL_H
#define UTIL_H
#include <QObject>
#include <QString>
#include <vector>
#include <QPoint>
#include <opencv2/opencv.hpp>

namespace config{
    enum editPosition{sourceImage, sourceGuidance,targetImage, targetGuidance};
    enum editLevel{readOnly, editable};
    enum operationType{None,Move, Pencil,  Eraser, Bucket, ZoomIn, ZoomOut};
}


typedef struct  tagMyClusteringTestPoint
{
    int nID, nNNHD, label;
    double density; //rho
    double dis2NNHD; // delta

    tagMyClusteringTestPoint()
    {
        nID = nNNHD = label = -1;
        density = 0.0;
        dis2NNHD = 999999.0;
    }
} ClusteringPoints;

class Util
{
public:

    static void init();

    static std::vector<QString> guidanceChannel;

    static float calcL2Distance(const QPoint point1, const QPoint point2);
    static void generateGaborFeatureFromImage(const cv::Mat& sourceImage, cv::Mat& features);
    static void calcDistanceAndDelta(const cv::Mat features, std::vector<ClusteringPoints>& v_points, std::vector<std::pair<int, double> >& v_density_Descend);
    static void labelOtherPoints(std::vector<ClusteringPoints>& v_points, std::vector<std::pair<int, double> >& v_density_Descend);
    static void imgUndoScale(const cv::Mat& scaled, cv::Mat& withoutScale, int scaleFactor);

    static cv::Mat getMyGabor(int width, int height, int U, int V, double Kmax, double f,double sigma, int ktype, const std::string kernel_name);
    static void construct_gabor_bank();
    static cv::Mat gabor_filter(cv::Mat& img, int type);
    static cv::Mat generateFeature(const cv::Mat& gaborImg);
    static int cmp(const std::pair<int, double> &x, const std::pair<int, double> &y );
    static double GetSearchRadius(cv::flann::Index &myKdTree, const cv::Mat &features, int nMaxSearch, float percent);

    static double PI;

};

#endif // UTIL_H
