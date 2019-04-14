#ifndef IMAGELABEL_H
#define IMAGELABEL_H
#include <QLabel>
#include <QMouseEvent>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

using namespace cv;
using namespace std;

class Widget;

class imagelabel : public QLabel
{
    Q_OBJECT

public:
    imagelabel(Widget *parent = 0);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent *e);

    void clear_detectPoint();
    void clear_markPoint();
    void clear_choosePoint();

    ~imagelabel();
private slots:
    void OpenButton_click();

public:
    Widget *my_parent;
    Mat src;
    Mat dst;
    Mat original_image;
    QPoint lineStartPoint;
    QPoint lineEndPoint;

    vector<QPoint> detectStartPoint;
    vector<QPoint> detectEndPoint;
    vector<QPoint> markStartPoint;
    vector<QPoint> markEndPoint;
    vector<QPoint> chooseStartPoint;
    vector<QPoint> chooseEndPoint;
    bool isVPDetect;
    bool isChooseArea;
    bool isChooseMark;
    QString fileName;

};

#endif // IMAGELABEL_H
