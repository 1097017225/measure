#include "widget.h"

#include "processdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    this->setH = 1.5;
    this->setD = 1.0;
    open = new QPushButton("Open image",this);
    VanishingPoint = new QPushButton("VanishingPoint",this);
    chooseMark = new QPushButton("Choose Mark",this);
    chooseArea = new QPushButton("Choose Area",this);
    measure = new QPushButton("Measure",this);
    clear = new QPushButton("Close",this);

    VanishingPoint->setEnabled(false);
    chooseMark->setEnabled(false);
    chooseArea->setEnabled(false);
    measure->setEnabled(false);
    //clear->setEnabled(false);


    open->setMinimumSize(120, 30);
    VanishingPoint->setMinimumSize(120, 30);
    chooseMark->setMinimumSize(120, 30);
    chooseArea->setMinimumSize(120, 30);
    measure->setMinimumSize(120, 30);
    clear->setMinimumSize(120, 30);

    mylabel = new imagelabel(this);
    //mylabel->setMinimumSize(240,360);
    mylabel->setText(QString::fromLocal8Bit("使用说明\n1.点击Open imgage 打开图片，注意图片路径中不能有英文。\n2.选取图片上在实际场景中的两条平行线获得消失点，先得到水平消失点，再是竖直消失点。\n点击VanishingPoint进行计算。\n3.点击Choose Mark,选择标记物体。目前只能是矩形。\n4.点击Choose Area，选择测量区域，目前只是边长平行标记物体的矩形区域。\n5.点击Measure进行测量。\n6.点击Close退出程序。\n7.选取线段时，鼠标在起始点进行点击左键，按住左键拖动，在结束点松开点击能够完成一次选取。 "));
    connect(open,SIGNAL(clicked()),mylabel,SLOT(OpenButton_click()));
    connect(clear,SIGNAL(clicked()),this,SLOT(ClearButton_click()));
    connect(VanishingPoint,SIGNAL(clicked()),this,SLOT(VPButton_click()));
    connect(chooseMark,SIGNAL(clicked()),this,SLOT(MarkButton_click()));
    connect(chooseArea,SIGNAL(clicked(bool)),this,SLOT(AreaButton_click()));
    connect(measure,SIGNAL(clicked(bool)),this,SLOT(MeasureButton_click()));

    QVBoxLayout *vlayout = new QVBoxLayout();
    QHBoxLayout *hlayout = new QHBoxLayout();

    vlayout->addStretch();
    vlayout->addWidget(open,0,Qt::AlignCenter);
    vlayout->addSpacing(40);
    vlayout->addWidget(VanishingPoint,0,Qt::AlignCenter);
    vlayout->addSpacing(40);
    vlayout->addWidget(chooseMark,0,Qt::AlignCenter);
    vlayout->addSpacing(40);
    vlayout->addWidget(chooseArea,0,Qt::AlignCenter);
    vlayout->addSpacing(40);
    vlayout->addWidget(measure,0,Qt::AlignCenter);
    vlayout->addSpacing(40);
    vlayout->addWidget(clear,0,Qt::AlignCenter);
    vlayout->addStretch();

    hlayout->addWidget(mylabel);
    hlayout->addLayout(vlayout);

    hlayout->setStretchFactor(mylabel, 3);
    hlayout->setStretchFactor(vlayout, 1);

    hlayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setLayout(hlayout);

    mylabel->setFixedSize(500,400);


}

void Widget::VPButton_click()
{
    if( mylabel->detectEndPoint.size() == 2 && mylabel->detectStartPoint.size() == 2 )
    {
        vector<float> line_A=calculateLine(mylabel->detectStartPoint[0],mylabel->detectEndPoint[0]);
        vector<float> line_B=calculateLine(mylabel->detectStartPoint[1],mylabel->detectEndPoint[1]);

        qDebug("point1: %d %d",mylabel->detectStartPoint[0].x(),mylabel->detectStartPoint[0].y());
        qDebug("point2: %d %d",mylabel->detectEndPoint[0].x(),mylabel->detectEndPoint[0].y());
        qDebug("point3: %d %d",mylabel->detectStartPoint[1].x(),mylabel->detectStartPoint[1].y());
        qDebug("point4: %d %d",mylabel->detectEndPoint[1].x(),mylabel->detectEndPoint[1].y());

        crossPoint = calculateCrossPoint(line_A,line_B);
        qDebug("crosspoint: %d %d",crossPoint.x(),crossPoint.y());
        Vanishing_Point.push_back(crossPoint);
        mylabel->clear_detectPoint();

        if( Vanishing_Point.size() == 2 )
        {
            chooseMark->setEnabled(true);
            VanishingPoint->setEnabled(false);

            mylabel->isVPDetect = false;
            mylabel->isChooseArea = false;
            mylabel->isChooseMark = false;

            QMessageBox msg(this);
            msg.setWhatsThis("Success");
            msg.setText(QString::fromLocal8Bit("校对成功，请点击Mask按钮选取Mask"));
            msg.setIcon(QMessageBox::Information);
            msg.setStandardButtons(QMessageBox::Ok);
            msg.exec();
        }
    }
    else
    {
        QMessageBox msg(this);
        msg.setWhatsThis("Error");
        msg.setText(QString::fromLocal8Bit("选取的点数量不对,请重新选取点"));
        msg.setIcon(QMessageBox::Information);
        msg.setStandardButtons(QMessageBox::Ok);
        msg.exec();
        mylabel->clear_detectPoint();
    }
}

void Widget::AreaButton_click()
{
    mylabel->isVPDetect = false;
    mylabel->isChooseMark = false;
    mylabel->isChooseArea = true;

    mylabel->chooseStartPoint.clear();
    mylabel->chooseEndPoint.clear();

    mylabel->original_image.copyTo(mylabel->dst);
    Mat src2;
    cvtColor(mylabel->original_image,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    //this->resize(src2.cols,src2.rows);
    mylabel->setFixedSize(src2.cols,src2.rows);
    mylabel->setPixmap(QPixmap::fromImage(image));

}

void Widget::MarkButton_click()
{
    mylabel->isVPDetect = false;
    mylabel->isChooseMark = true;
    mylabel->isChooseArea = false;

    mylabel->markStartPoint.clear();
    mylabel->markEndPoint.clear();

    mylabel->original_image.copyTo(mylabel->dst);
    Mat src2;
    cvtColor(mylabel->original_image,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    //this->resize(src2.cols,src2.rows);
    mylabel->setFixedSize(src2.cols,src2.rows);
    mylabel->setPixmap(QPixmap::fromImage(image));
}

void Widget::MeasureButton_click()
{
    QPoint X3 = mylabel->chooseStartPoint[0];
    QPoint X4 = mylabel->chooseEndPoint[0];
    QPoint X5 = mylabel->chooseStartPoint[1];
    QPoint X6 = mylabel->chooseEndPoint[1];

    QPoint X1 = mylabel->markStartPoint[0];
    QPoint X2 = mylabel->markEndPoint[0];
    QPoint H1 = mylabel->markStartPoint[1];
    QPoint H2 = mylabel->markEndPoint[1];

    QPoint V1 = Vanishing_Point[0];
    QPoint V2 = Vanishing_Point[1];

    float d1 = measure_func(X3,X4,X1,X2,H1,H2,V1,V2,setH,setD);
    float d2 = measure_func(X3,X5,X1,X2,H1,H2,V1,V2,setH,setD);
    float area = d1 * d2;

    //qDebug("The measure area is:%f",area);
    //imageProcessing(mylabel->fileName.toStdString(),X3,X4,X5,X6);
    //qDebug("The bad area is:%f",ratio );

    processdialog image_process(mylabel->original_image,X1,X2,X3,X4,this);
    image_process.exec();

//processdialog(Mat original_image, QPoint X1, QPoint X2, QPoint X3, QPoint X4, QWidget *parent = 0);

}

void Widget::ClearButton_click()
{
    VanishingPoint->setEnabled(false);
    chooseMark->setEnabled(false);
    chooseArea->setEnabled(false);
    measure->setEnabled(false);
    open->setEnabled(true);
    Vanishing_Point.clear();

    mylabel->detectStartPoint.clear();
    mylabel->detectEndPoint.clear();
    mylabel->markStartPoint.clear();
    mylabel->markEndPoint.clear();
    mylabel->chooseStartPoint.clear();
    mylabel->chooseEndPoint.clear();
    mylabel->fileName = "";
    if( mylabel->src.data != 0)
        mylabel->src.release();
     if( mylabel->dst.data != 0)
        mylabel->dst.release();
     if( mylabel->original_image.data != 0)
        mylabel->original_image.release();
    mylabel->isVPDetect = false;
    mylabel->isChooseArea = false;
    mylabel->isChooseMark = false;
    mylabel->clear();
    mylabel->setText(QString::fromLocal8Bit("使用说明\n1.点击Open imgage 打开图片，注意图片路径中不能有英文。\n2.选取图片上在实际场景中的两条平行线获得消失点，先得到水平消失点，再是竖直消失点。\n点击VanishingPoint进行计算。\n3.点击Choose Mark,选择标记物体。目前只能是矩形。\n4.点击Choose Area，选择测量区域，目前只是边长平行标记物体的矩形区域。\n5.点击Measure进行测量。\n6.点击Close退出程序。\n7.选取线段时，鼠标在起始点进行点击左键，按住左键拖动，在结束点松开点击能够完成一次选取。 "));
    mylabel->setFixedSize(500,400);

}

vector<float> Widget::calculateLine(QPoint Xa, QPoint Xb)
{
    float A,B,C;
    vector<float> X;
    A = Xb.y() - Xa.y();
    B = Xa.x() - Xb.x();
    C = Xb.x()*Xa.y() - Xa.x()*Xb.y();
    X.push_back(A);
    X.push_back(B);
    X.push_back(C);
    return X;

}

QPoint Widget::calculateCrossPoint(vector<float> lineA,vector<float> lineB)
{
    QPoint P;
    float m = lineA[0] * lineB[1] - lineB[0] * lineA[1];
    if(0 == m)
    {
        qDebug() << ("NO POINT");
    }
    else
    {
        P.setX((lineB[2]*lineA[1] - lineA[2] * lineB[1])/m);
        P.setY((lineA[2]*lineB[0] - lineB[2] * lineA[0])/m);
    }
    return P;
}

float Widget::getLineDistance(QPoint X1,QPoint X2,QPoint X3,QPoint X4,QPoint VP,float setD)
{
    float cr1,cr2;
    float distanceMeasure;
    cr1 = crossRatio(X1,X2,X3,VP);
    cr2 = crossRatio(X3,X1,X4,VP);
    distanceMeasure = (setD/cr1)/cr2;
    return distanceMeasure;
}

float Widget::crossRatio(QPoint X1,QPoint X2,QPoint X3,QPoint X4)
{
    float cr;
    cr = sqrt( pow((X1.x()-X2.x()),2) + pow((X1.y()-X2.y()),2) ) * sqrt( pow((X3.x()-X4.x()),2) + pow((X3.y()-X4.y()),2) )
            /sqrt( pow((X1.x()-X3.x()),2) + pow((X1.y()-X3.y()),2) )
            /sqrt( pow((X2.x()-X4.x()),2) + pow((X2.y()-X4.y()),2) );
    return cr;
}

float Widget::measure_func(QPoint X3,QPoint X4,QPoint X1,QPoint X2,QPoint H1,QPoint H2,QPoint V1,QPoint V2,float setH,float setD)
{
    float d_m=0;
    QPoint vt,vx;
    double radius = 0.5;
    vector<float> A = calculateLine(X1,X2);
    vector<float> B = calculateLine(H1,X1);
    vector<float> C = calculateLine(X3,X4);

    float line1K = A[0]/A[1];
    float line1B = A[2]/A[1];
    float line2K = B[0]/B[1];
    float line2B = B[2]/B[1];
    float line3K = C[0]/C[1];
    float line3B = C[2]/C[1];

    if(abs(line1K - line3K) > 2*radius)
    {
        if((abs(line2K - line3K) < 20*radius) && (abs(line2B - line3B) < 20000*radius))
        {
            d_m = getLineDistance(X1,H1,X3,X4,V2,setH);
        }
        else
        {
            vt = calculateCrossPoint(calculateLine(H1,X4),calculateLine(V2,V1));
            vx = calculateCrossPoint(calculateLine(X1,vt),calculateLine(V2,X4));
            d_m = getLineDistance(X4,vx,vx,X3,V2,setH) + setH;
        }
    }
    else
    {
        if((abs(line1K - line3K) < 20*radius) && (abs(line1B - line3B) < 100*radius))
        {
            d_m = getLineDistance(X1,X2,X3,X4,V1,setD);
        }
        else
        {
            vt = calculateCrossPoint(calculateLine(X1,X3),calculateLine(V1,V2));
            vx = calculateCrossPoint(calculateLine(X2,vt),calculateLine(V1,X3));
            d_m = getLineDistance(X3,vx,vx,X4,V1,setD) + setD;
        }
    }
    return d_m;
}

void Widget::setVP()
{
    VanishingPoint->setEnabled(true);
}

void Widget::setOpen()
{
    open->setEnabled(false);
}

void Widget::setArea()
{
    chooseArea->setEnabled(true);
    chooseMark->setEnabled(false);

    mylabel->isVPDetect = false;
    mylabel->isChooseMark = false;
    mylabel->isChooseArea = false;
}

void Widget::setMeasure()
{
    chooseArea->setEnabled(false);
    measure->setEnabled(true);

    mylabel->isVPDetect = false;
    mylabel->isChooseMark = false;
    mylabel->isChooseArea = false;
}


Widget::~Widget()
{

}
