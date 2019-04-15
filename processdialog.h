#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H
#include <QDialog>
#include <QLabel>
#include <QSlider>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <QPoint>

using namespace cv;

class processdialog : public QDialog
{
    Q_OBJECT
public:
    processdialog(Mat original_image, QPoint X1, QPoint X2, QPoint X3, QPoint X4, QWidget *parent = 0);
    int bSums(Mat src);
private slots:
    void sl1_change(int value);
    void sl2_change(int value);
    void sl3_change(int value);

private:
    QLabel *lb;
    QLabel *flag1;
    QLabel *flag2;
    QLabel *flag3;
    QSlider *sl1;
    QSlider *sl2;
    QSlider *sl3;

    Mat imgSrc;
    Mat src;

    Mat imgThres;
    Mat dilateImage;
    Mat erodeImage;
    Mat rotated;


    float count_ratio;


    //vector<Point2f> not_a_rect_shape;
    int Height;
    int Weight;

    QPoint X1;
    QPoint X2;
    QPoint X3;
    QPoint X4;
};

#endif // PROCESSDIALOG_H
