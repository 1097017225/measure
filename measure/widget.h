#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include "imagelabel.h"
#include <vector>
using namespace std;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    vector<float> calculateLine(QPoint Xa, QPoint Xb);
    QPoint calculateCrossPoint(vector<float> lineA,vector<float> lineB);
    float measure_func(QPoint X3,QPoint X4,QPoint X1,QPoint X2,QPoint H1,QPoint H2,QPoint V1,QPoint V2,float setH,float setD);
    float getLineDistance(QPoint X1,QPoint X2,QPoint X3,QPoint X4,QPoint VP,float setD);
    float crossRatio(QPoint X1,QPoint X2,QPoint X3,QPoint X4);
    void setVP();
    void setOpen();
    void setArea();
    void setMeasure();
    ~Widget();

private slots:
    void VPButton_click();
    void AreaButton_click();
    void MarkButton_click();
    void MeasureButton_click();
    void ClearButton_click();

private:
    QPushButton* open;
    QPushButton* VanishingPoint;
    QPushButton* chooseMark;
    QPushButton* chooseArea;
    QPushButton* measure;
    QPushButton* clear;
    imagelabel* mylabel;

    QPoint crossPoint;
    vector<QPoint> Vanishing_Point;

    float setH;
    float setD;
};

#endif // WIDGET_H
