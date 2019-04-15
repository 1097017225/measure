#include "processdialog.h"
#include <QGridLayout>
#include <QVBoxLayout>
#include <QDebug>

processdialog::processdialog(Mat original_image,QPoint X1,QPoint X2,QPoint X3,QPoint X4,QWidget *parent) : QDialog(parent)
{
    QGridLayout* glayout = new QGridLayout();
    QVBoxLayout* vlayout = new QVBoxLayout();
    lb = new QLabel();
    flag1 = new QLabel();
    flag2 = new QLabel();
    flag3 = new QLabel();
    sl1 = new QSlider(Qt::Horizontal);
    sl2 = new QSlider(Qt::Horizontal);
    sl3 = new QSlider(Qt::Horizontal);

    connect(sl1,SIGNAL(valueChanged(int)),this,SLOT(sl1_change(int)));
    connect(sl2,SIGNAL(valueChanged(int)),this,SLOT(sl2_change(int)));
    connect(sl3,SIGNAL(valueChanged(int)),this,SLOT(sl3_change(int)));

    glayout->addWidget(flag1,0,0);
    glayout->addWidget(sl1,0,1);

    glayout->addWidget(flag2,1,0);
    glayout->addWidget(sl2,1,1);

    glayout->addWidget(flag3,2,0);
    glayout->addWidget(sl3,2,1);

    vlayout->addLayout(glayout);
    vlayout->addWidget(lb);

    setLayout(vlayout);

    flag1->setText(QString::fromLocal8Bit("二值化:150"));
    flag2->setText(QString::fromLocal8Bit(" 膨胀 :000"));
    flag3->setText(QString::fromLocal8Bit(" 腐蚀 :003"));

    original_image.copyTo(imgSrc);

    Height = 200;
    Weight = 900;

    this->X1 = X1;
    this->X2 = X2;
    this->X3 = X3;
    this->X4 = X4;

    cv::resize(imgSrc,src,Size(Weight,Height));
    cv::Point2f src_vertices[4];

    src_vertices[0].x = X1.x();
    src_vertices[0].y = X1.y();
    src_vertices[1].x = X2.x();
    src_vertices[1].y = X2.y();
    src_vertices[2].x = X3.x();
    src_vertices[2].y = X3.y();
    src_vertices[3].x = X4.x();
    src_vertices[3].y = X4.y();

    src_vertices[0].x = src_vertices[0].x * Weight / 1200;
    src_vertices[0].y = src_vertices[0].y * Height / 800;
    src_vertices[1].x = src_vertices[1].x * Weight / 1200;
    src_vertices[1].y = src_vertices[1].y * Height / 800;
    src_vertices[2].x = src_vertices[2].x * Weight / 1200;
    src_vertices[2].y = src_vertices[2].y * Height / 800;
    src_vertices[3].x = src_vertices[3].x * Weight / 1200;
    src_vertices[3].y = src_vertices[3].y * Height / 800;

    Point2f dst_vertices[4];
    dst_vertices[0] = Point(0, 0);
    dst_vertices[1] = Point(Weight, 0);
    dst_vertices[2] = Point(0, Height);
    dst_vertices[3] = Point(Weight, Height);

    Mat warpMatrix = getPerspectiveTransform(src_vertices, dst_vertices);

    warpPerspective(src, rotated, warpMatrix, rotated.size(), INTER_LINEAR, BORDER_CONSTANT);

    sl1->setMinimum(0);
    sl1->setMaximum(255);
    sl1->setValue(150);

    sl2->setMinimum(0);
    sl2->setMaximum(30);
    sl2->setValue(1);

    sl3->setMinimum(0);
    sl3->setMaximum(10);
    sl3->setValue(3);

    Mat src2;
    rotated.copyTo(src2);
    cvtColor(src2,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    lb->setFixedSize(src2.cols,src2.rows);
    lb->setPixmap(QPixmap::fromImage(image));

}

void processdialog::sl1_change(int value)
{
    flag1->setText(QString::fromLocal8Bit("二值化:")+ QString("%1").arg(value,3,10,QChar('0')) );

    qDebug() << "sl1_change_rotated" << rotated.channels();
    cv::threshold(rotated, imgThres, value, 255, THRESH_BINARY_INV);
    Mat sl1;
    imgThres.copyTo(sl1);
    cv::cvtColor(sl1,sl1,CV_BGR2RGB);
    QImage image((const uchar*)sl1.data,sl1.cols,sl1.rows,sl1.cols*sl1.channels(),QImage::Format_RGB888);
    lb->setFixedSize(sl1.cols,sl1.rows);
    lb->setPixmap(QPixmap::fromImage(image));

}

void processdialog::sl2_change(int value)
{
    flag2->setText(QString::fromLocal8Bit(" 膨胀 :")+ QString("%1").arg(value,3,10,QChar('0')) );
    qDebug() << "sl2_change_imgThres" << imgThres.channels();

    Mat element = getStructuringElement(MORPH_RECT, Size(value+1,value+1));
    dilate(imgThres, dilateImage, element);
    Mat sl2;
    dilateImage.copyTo(sl2);
    cv::cvtColor(sl2,sl2,CV_BGR2RGB);
    QImage image((const uchar*)sl2.data,sl2.cols,sl2.rows,sl2.cols*sl2.channels(),QImage::Format_RGB888);
    lb->setFixedSize(sl2.cols,sl2.rows);
    lb->setPixmap(QPixmap::fromImage(image));
    //int count = bSums(dilateImage);
    //count_ratio = count / (Weight * Height * 1.0);

}

void processdialog::sl3_change(int value)
{
    flag3->setText(QString::fromLocal8Bit(" 腐蚀 :")+ QString("%1").arg(value,3,10,QChar('0')) );
    qDebug() << "sl3_change_dilateImage" << dilateImage.channels();

    Mat element = getStructuringElement(MORPH_RECT, Size(value+1, value+1));
    erode(dilateImage, erodeImage, element);

    Mat sl3;
    erodeImage.copyTo(sl3);
    cv::cvtColor(sl3,sl3,CV_BGR2RGB);
    QImage image((const uchar*)sl3.data,sl3.cols,sl3.rows,sl3.cols*sl3.channels(),QImage::Format_RGB888);
    lb->setFixedSize(sl3.cols,sl3.rows);
    lb->setPixmap(QPixmap::fromImage(image));

    //int count = bSums(erodeImage);
    //count_ratio = count / (Weight * Height * 1.0);

}

int processdialog::bSums(Mat src)
{
    int counter = 0;
    //迭代器访问像素点
    Mat_<uchar>::iterator it = src.begin<uchar>();
    Mat_<uchar>::iterator itend = src.end<uchar>();
    for (; it != itend; ++it)
    {
        if ((*it) > 0) counter += 1;//二值化后，像素点是0或者255
    }
    return counter;
}
