#include "imagelabel.h"
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include "widget.h"

imagelabel::imagelabel(Widget *parent) : QLabel(parent)
{
    my_parent = parent;


    isVPDetect = false;
    isChooseArea = false;
    isChooseMark = false;
}

void imagelabel::OpenButton_click()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open Image"),"./",tr("Images (*.png *.xpm *.jpg)"));

    if(fileName != "" )
    {
        Mat src2;
        src = imread(fileName.toStdString(),cv::IMREAD_COLOR);
        if ( src.data )
         {
            src.copyTo(dst);
            src.copyTo(original_image);
            cvtColor(src,src2,CV_BGR2RGB);
            QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
            //this->resize(src2.cols,src2.rows);
            this->setFixedSize(src2.cols,src2.rows);
            this->setPixmap(QPixmap::fromImage(image));
         }

        my_parent->setVP();
        my_parent->setOpen();
        isVPDetect = true;
        isChooseArea = false;
        isChooseMark = false;
    }
}

void imagelabel::mousePressEvent(QMouseEvent* e)
{
    if( (e->button() == Qt::LeftButton) && src.data  )
    {
        lineStartPoint = QPoint(e->x(), e->y());
        if(isVPDetect)
        {
            Mat src2;
            dst.copyTo(src);
            detectStartPoint.push_back(lineStartPoint);
            circle(src, Point(lineStartPoint.x(),lineStartPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);
            cvtColor(src,src2,CV_BGR2RGB);
            QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
            //this->resize(src2.cols,src2.rows);
            this->setFixedSize(src2.cols,src2.rows);
            this->setPixmap(QPixmap::fromImage(image));
            src.copyTo(dst);
        }
        else if(isChooseMark)
        {
            markStartPoint.push_back(lineStartPoint);
            Mat src2;
            dst.copyTo(src);
            circle(src, Point(lineStartPoint.x(),lineStartPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);

            if( markStartPoint.size() == 2 )
                line(src, Point(markStartPoint[0].x(),markStartPoint[0].y()), Point(lineStartPoint.x(),lineStartPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);
            cvtColor(src,src2,CV_BGR2RGB);
            QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
            //this->resize(src2.cols,src2.rows);
            this->setFixedSize(src2.cols,src2.rows);
            this->setPixmap(QPixmap::fromImage(image));
            src.copyTo(dst);
        }
        else if(isChooseArea)
        {
            chooseStartPoint.push_back(lineStartPoint);
            Mat src2;
            dst.copyTo(src);
            circle(src, Point(lineStartPoint.x(),lineStartPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);

            if( chooseStartPoint.size() == 2 )
                line(src, Point(chooseStartPoint[0].x(),chooseStartPoint[0].y()), Point(lineStartPoint.x(),lineStartPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);
            cvtColor(src,src2,CV_BGR2RGB);
            QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
            //this->resize(src2.cols,src2.rows);
            this->setFixedSize(src2.cols,src2.rows);
            this->setPixmap(QPixmap::fromImage(image));
            src.copyTo(dst);
        }
    }
}

void imagelabel::mouseMoveEvent(QMouseEvent* e)
{
    if(  src.data )
    {
        if( isVPDetect || isChooseMark|| isChooseArea)
        {
            Mat src2;
            dst.copyTo(src);
            lineEndPoint = QPoint(e->x(), e->y());
            line(src, Point(lineStartPoint.x(),lineStartPoint.y()), Point(lineEndPoint.x(),lineEndPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);

            cvtColor(src,src2,CV_BGR2RGB);
            QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
            //this->resize(src2.cols,src2.rows);
            this->setFixedSize(src2.cols,src2.rows);
            this->setPixmap(QPixmap::fromImage(image));
        }
    }
}

void imagelabel::mouseReleaseEvent(QMouseEvent *e)
{
    if( (e->button() == Qt::LeftButton) && src.data  )
    {
        lineEndPoint = QPoint(e->x(), e->y());
        if(isVPDetect)
        {
            if( lineStartPoint.x() == lineEndPoint.x() && lineStartPoint.y() == lineEndPoint.y() )
            {
                QMessageBox msg(this);
                msg.setWhatsThis("Error");
                msg.setText(QString::fromLocal8Bit("起始点和结束点相同,请重新开始选择"));
                msg.setIcon(QMessageBox::Information);
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                this->clear_detectPoint();
            }
            else
            {
                Mat src2;
                dst.copyTo(src);
                detectEndPoint.push_back(lineEndPoint);
                circle(src, Point(lineEndPoint.x(),lineEndPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);
                line(src, Point(lineStartPoint.x(),lineStartPoint.y()), Point(lineEndPoint.x(),lineEndPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);

                cvtColor(src,src2,CV_BGR2RGB);
                QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
                //this->resize(src2.cols,src2.rows);
                this->setFixedSize(src2.cols,src2.rows);
                this->setPixmap(QPixmap::fromImage(image));
                src.copyTo(dst);
             }
        }
        else if(isChooseMark)
        {
            if( lineStartPoint.x() == lineEndPoint.x() && lineStartPoint.y() == lineEndPoint.y() )
            {
                QMessageBox msg(this);
                msg.setWhatsThis("Error");
                msg.setText(QString::fromLocal8Bit("起始点和结束点相同,请重新开始选择"));
                msg.setIcon(QMessageBox::Information);
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                this->clear_markPoint();
            }
            else
            {
                Mat src2;
                dst.copyTo(src);
                markEndPoint.push_back(lineEndPoint);
                circle(src, Point(lineEndPoint.x(),lineEndPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);
                line(src, Point(lineStartPoint.x(),lineStartPoint.y()), Point(lineEndPoint.x(),lineEndPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);
                if( markEndPoint.size() == 2 )
                    line(src, Point(lineEndPoint.x(),lineEndPoint.y()), Point(markEndPoint[0].x(),markEndPoint[0].y()), cvScalar(0, 255, 0), 1, CV_AA, 0);

                cvtColor(src,src2,CV_BGR2RGB);
                QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
                //this->resize(src2.cols,src2.rows);
                this->setFixedSize(src2.cols,src2.rows);
                this->setPixmap(QPixmap::fromImage(image));
                src.copyTo(dst);
                //clear_markPoint();
                if( markEndPoint.size() == 2 )
                {
                    my_parent->setArea();
                    QMessageBox msg(this);
                    msg.setWhatsThis("Success");
                    msg.setText(QString::fromLocal8Bit("mark区域选取成功,请点击Area按钮选取Area区域"));
                    msg.setIcon(QMessageBox::Information);
                    msg.setStandardButtons(QMessageBox::Ok);
                    msg.exec();
                }
            }
        }
        else if(isChooseArea)
        {
            if( lineStartPoint.x() == lineEndPoint.x() && lineStartPoint.y() == lineEndPoint.y() )
            {
                QMessageBox msg(this);
                msg.setWhatsThis("Error");
                msg.setText(QString::fromLocal8Bit("起始点和结束点相同,请重新开始选择"));
                msg.setIcon(QMessageBox::Information);
                msg.setStandardButtons(QMessageBox::Ok);
                msg.exec();
                this->clear_choosePoint();
            }
            else
            {
                Mat src2;
                dst.copyTo(src);
                chooseEndPoint.push_back(lineEndPoint);
                circle(src, Point(lineEndPoint.x(),lineEndPoint.y()), 2, cvScalar(255, 0, 0), CV_FILLED, CV_AA, 0);
                line(src, Point(lineStartPoint.x(),lineStartPoint.y()), Point(lineEndPoint.x(),lineEndPoint.y()), cvScalar(0, 255, 0), 1, CV_AA, 0);
                if( chooseEndPoint.size() == 2 )
                    line(src, Point(lineEndPoint.x(),lineEndPoint.y()), Point(chooseEndPoint[0].x(),chooseEndPoint[0].y()), cvScalar(0, 255, 0), 1, CV_AA, 0);

                cvtColor(src,src2,CV_BGR2RGB);
                QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
                //this->resize(src2.cols,src2.rows);
                this->setFixedSize(src2.cols,src2.rows);
                this->setPixmap(QPixmap::fromImage(image));
                src.copyTo(dst);
                //clear_markPoint();
                if( chooseEndPoint.size() == 2 )
                {
                    my_parent->setMeasure();
                    QMessageBox msg(this);
                    msg.setWhatsThis("Success");
                    msg.setText(QString::fromLocal8Bit("Area区域选取成功,请点击Measure进行计算"));
                    msg.setIcon(QMessageBox::Information);
                    msg.setStandardButtons(QMessageBox::Ok);
                    msg.exec();
                }
            }
        }
    }
}

void imagelabel::clear_detectPoint()
{
    lineStartPoint = QPoint(0,0);
    lineEndPoint = QPoint(0,0);
    detectStartPoint.clear();
    detectEndPoint.clear();
    original_image.copyTo(dst);
    Mat src2;
    cvtColor(original_image,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    //this->resize(src2.cols,src2.rows);
    this->setFixedSize(src2.cols,src2.rows);
    this->setPixmap(QPixmap::fromImage(image));

}

void imagelabel::clear_markPoint()
{
    lineStartPoint = QPoint(0,0);
    lineEndPoint = QPoint(0,0);
    markStartPoint.clear();
    markEndPoint.clear();
    original_image.copyTo(dst);
    Mat src2;
    cvtColor(original_image,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    //this->resize(src2.cols,src2.rows);
    this->setFixedSize(src2.cols,src2.rows);
    this->setPixmap(QPixmap::fromImage(image));
}

void imagelabel::clear_choosePoint()
{
    lineStartPoint = QPoint(0,0);
    lineEndPoint = QPoint(0,0);
    chooseStartPoint.clear();
    chooseEndPoint.clear();

    original_image.copyTo(dst);
    Mat src2;
    cvtColor(original_image,src2,CV_BGR2RGB);
    QImage image((const uchar*)src2.data,src2.cols,src2.rows,src2.cols*src2.channels(),QImage::Format_RGB888);
    this->resize(src2.cols,src2.rows);
    this->setFixedSize(src2.cols,src2.rows);
    this->setPixmap(QPixmap::fromImage(image));
}

imagelabel::~imagelabel()
{

}
