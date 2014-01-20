#include <QtGui>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QStringList>
#include <QFile>
#include <QDir>
#include <iostream>
#include <stdio.h>
//#include <math.h>

using namespace std;
using namespace cv;

float sensivityRange = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    image = new QImage;
    image_2 = new QImage;
    greyImage = new Mat;
    pattern = new QImage;
    greyPattern = new Mat;
    greyToScreen = new Mat;

    ui->setupUi(this);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    ui->imageLabel->setPixmap(QPixmap::fromImage(*image));
    ui->imageLabel_2->setPixmap(QPixmap::fromImage(*image_2));
    ui->patternLabel->setPixmap(QPixmap::fromImage(*pattern));
    ui->dial->setRange(0,100);

    ui->findButton0->setDefault(true);
    ui->findButton0->setEnabled(false);
    ui->findButton1->setDefault(true);
    ui->findButton1->setEnabled(false);
    ui->findButton2->setDefault(true);
    ui->findButton2->setEnabled(false);
    ui->findButton3->setDefault(true);
    ui->findButton3->setEnabled(false);
    ui->findButton4->setDefault(true);
    ui->findButton4->setEnabled(false);
    ui->findButton5->setDefault(true);
    ui->findButton5->setEnabled(false);

    //ui->brightnessSlider->setMinimum(0);
    //ui->brightnessSlider->setMinimum(100);
    //ui->brightnessSlider->setValue(0);
    //ui->contrastSlider->setMinimum(1.0);
    //ui->contrastSlider->setMaximum(3.0);


    // Only if images are uploaded, find button is enabled
    connect(ui->loadImage, SIGNAL(clicked()),
            this, SLOT(enableFindButtons()));
    connect(ui->loadPattern, SIGNAL(clicked()),
            this, SLOT(enableFindButtons()));
    connect(ui->LoadSelectedPattern, SIGNAL(clicked()),
            this, SLOT(enableFindButtons()));

    connect(ui->brightnessSlider, SIGNAL(valueChanged(int)),
    this, SLOT(setBrightness(int)));
    connect(ui->contrastSlider, SIGNAL(valueChanged(int)),
    this, SLOT(setContrast(int)));
    connect(ui->dial, SIGNAL(valueChanged(int)),
            this, SLOT(sensivity(int)));
    connect(ui->dial, SIGNAL(valueChanged(int)),
            ui->sensivityValueText, SLOT(setNum(int)));
    connect(ui->filterButton, SIGNAL(clicked()),
            this, SLOT(filterImage()));
}


void MainWindow::enableFindButtons()
{
    if (!image->isNull() && !pattern->isNull())
    {
        ui->findButton0->setEnabled(true);
        ui->findButton1->setEnabled(true);
        ui->findButton2->setEnabled(true);
        ui->findButton3->setEnabled(true);
        ui->findButton4->setEnabled(true);
        ui->findButton5->setEnabled(true);

    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loadPattern_clicked()
{
    QString fileName;
    QFileDialog dialog;
    Mat matPattern;

    fileName = dialog.getOpenFileName(this,
        tr("Open Image"), "/Users/resa/Studium/WiSe2013/Thesis", tr("Image Files (*.png *.jpg *.bmp *.tif)"));
    int w = ui->patternLabel->width();
    int h = ui->patternLabel->height();

    pattern->load(fileName);
    patternPath = fileName;                          //ACHTUNG! Pointer auf Speicher

    matPattern = imread(this->patternPath.toStdString());
    createGreyPattern(matPattern);
    /* <-- nur für Testing*/
    QImage greyQImage = QImage((unsigned char*) greyToScreen->data, greyToScreen->cols, greyToScreen->rows, greyToScreen->step, QImage::Format_RGB888);
    ui->imageLabel_2->setPixmap(QPixmap::fromImage(greyQImage).scaled(w,h,Qt::KeepAspectRatio));
    /*--> nur für Testing */
    ui->patternLabel->setPixmap(QPixmap::fromImage(*pattern).scaled(w,h,Qt::KeepAspectRatio));  //evtl weglassen, wenn standardmäßig image/pattern in label angezeigt wird

}

void MainWindow::on_loadImage_clicked()
{
    int tabnumber;
    tabnumber = ui->tabWidget->currentIndex();

    QString fileName;
    QFileDialog dialog;
    fileName = dialog.getOpenFileName(this,
        tr("Open Image"), "/Users/resa/Studium/WiSe2013/Thesis", tr("Image Files (*.png *.jpg *.bmp *.tif)"));

    ///check active tab
    if(tabnumber == 0 )
    {
        Mat matImage;
        image->load(fileName);
        //*image = image->convertToFormat(QImage::Format_RGB888);
        imagePath = fileName;

        matImage = imread(this->imagePath.toStdString());
        createGreyImage(matImage);

        int w = ui->imageLabel->width();
        int h = ui->imageLabel->height();

        //greyToScreen(greyImage->cols,greyImage->rows,greyImage->type());
       // cv::cvtColor(*greyImage, greyToScreen,CV_BGR2RGB);

        QImage greyQImage = QImage((unsigned char*) greyToScreen->data, greyToScreen->cols, greyToScreen->rows, greyToScreen->step, QImage::Format_RGB888);
        ui->imageLabel_2->setPixmap(QPixmap::fromImage(greyQImage).scaled(w,h,Qt::KeepAspectRatio));

        ui->imageLabel->setPixmap(QPixmap::fromImage(*image).scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        image_2->load(fileName);
        //*image_2 = image_2->convertToFormat(QImage::Format_RGB888);

        imagePath_2 = fileName;
        int w = ui->imageLabel_2->width();
        int h = ui->imageLabel_2->height();
        ui->imageLabel_2->setPixmap(QPixmap::fromImage(*image_2).scaled(w,h,Qt::KeepAspectRatio));
    }
}

void MainWindow::displayImageInImageLabel(Mat mat)
{
    int tabnumber;
    tabnumber = ui->tabWidget->currentIndex();

    QPixmap pixmap;

    ///check active tab
    if(tabnumber == 0)
    {
        int w = ui->imageLabel->width();
        int h = ui->imageLabel->height();
        //QImage::Format imageFormat = this->image->format();

        //mat.depthMat(image->height(),image->width(),CV_8UC4,(uchar*)image->bits(),image->bytesPerLine());
        QImage imageToShow = QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);

        imageToShow.save("/Users/resa/Studium/WiSe2013/Thesis/imageToShow.png", 0, 100);


        pixmap = QPixmap::fromImage(imageToShow);

        ui->imageLabel->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
    else
    {
        int w = ui->imageLabel_2->width();
        int h = ui->imageLabel_2->height();
        QImage::Format imageFormat = this->image->format();

        pixmap = QPixmap::fromImage(QImage((unsigned char*) mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888));

        ui->imageLabel_2->setPixmap(pixmap.scaled(w,h,Qt::KeepAspectRatio));
    }
}


void MainWindow::on_findButton0_clicked()
{
    matchingWithMethod(0, sensivityRange);
}

void MainWindow::on_findButton1_clicked()
{
    matchingWithMethod(1, sensivityRange);
}

void MainWindow::on_findButton2_clicked()
{
    matchingWithMethod(2, sensivityRange);
}

void MainWindow::on_findButton3_clicked()
{
    matchingWithMethod(3, sensivityRange);
}

void MainWindow::on_findButton4_clicked()
{
    matchingWithMethod(4, sensivityRange);
}

void MainWindow::on_findButton5_clicked()
{
    matchingWithMethod(5, sensivityRange);
}

void MainWindow::on_findButton6_clicked()
{
    matchingWithMethod(6, sensivityRange);
}

void MainWindow::on_findButton7_clicked()
{
    matchingWithMethod(7, sensivityRange);
}

void MainWindow::on_findButton8_clicked()
{
    matchingWithMethod(8, sensivityRange);
}

void MainWindow::on_LoadSelectedPattern_clicked()
{
    int w = ui->patternLabel->width();
    int h = ui->patternLabel->height();
    int imagewidth = this->image->width();
    int imageheight = this->image->height();
    int pixmapwidth = ui->imageLabel->pixmap()->width();
    int pixmapheight = ui->imageLabel->pixmap()->height();

    float widthFactor = (float)imagewidth/(float)pixmapwidth;
    float heightFactor = (float)imageheight/(float)pixmapheight;

    QString fileName = "/Users/resa/Studium/WiSe2013/Thesis/pattern01.png";         //Ort muss einstellbar sein, evt. globale Variable

    selectRect = ui->imageLabel->getQImageRect(widthFactor, heightFactor);

    *pattern = this->image->copy(selectRect);

    pattern->save(fileName, 0, 100);        //muss aufgehellt werden
    patternPath = fileName;

    Mat matPattern = imread(this->patternPath.toStdString());
    createGreyPattern(matPattern);
    /* <-- nur für Testing*/
    QImage greyQImage = QImage((unsigned char*) greyToScreen->data, greyToScreen->cols, greyToScreen->rows, greyToScreen->step, QImage::Format_RGB888);
    ui->imageLabel_2->setPixmap(QPixmap::fromImage(greyQImage).scaled(w,h,Qt::KeepAspectRatio));
    /*--> nur für Testing */

    ui->patternLabel->setPixmap(QPixmap::fromImage(*pattern).scaled(w,h,Qt::KeepAspectRatio));

}

void MainWindow::setBrightness(int value)
{
    int w = ui->patternLabel->width();
    int h = ui->patternLabel->height();
    /// Read pattern
    Mat matImage = imread(this->patternPath.toStdString());
    Mat new_image = Mat::zeros( matImage.size(), matImage.type() );

    /// Do the operation new_image(i,j) = alpha*image(i,j) + beta
    for( int y = 0; y < matImage.rows; y++ )
    {
        for( int x = 0; x < matImage.cols; x++ )
        {
            for( int c = 0; c < 3; c++ )
            {
               new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( matImage.at<Vec3b>(y,x)[c] + value );
            }
        }
    }    

   *pattern = QImage((unsigned char*) new_image.data, new_image.cols, new_image.rows, new_image.step, QImage::Format_RGB888);
    pattern->save("/Users/resa/Studium/WiSe2013/Thesis/pattern01.png", 0, 100);
    ui->patternLabel->setPixmap(QPixmap::fromImage(*pattern).scaled(w,h,Qt::KeepAspectRatio));

}

void MainWindow::setContrast(int value)
{
    int w = ui->patternLabel->width();
    int h = ui->patternLabel->height();
    /// Read pattern
    Mat matImage = imread(this->patternPath.toStdString());
    Mat new_image = Mat::zeros( matImage.size(), matImage.type() );

    float contrast = (float)value/100*2+1;


    /// Do the operation new_image(i,j) = alpha*image(i,j) + beta
    for( int y = 0; y < matImage.rows; y++ )
    {
        for( int x = 0; x < matImage.cols; x++ )
        {
            for( int c = 0; c < 3; c++ )
            {
                new_image.at<Vec3b>(y,x)[c] = saturate_cast<uchar>( contrast*( matImage.at<Vec3b>(y,x)[c] ) );
            }
        }
    }

    *pattern = QImage((unsigned char*) new_image.data, new_image.cols, new_image.rows, new_image.step, QImage::Format_RGB888);
    pattern->save("/Users/resa/Studium/WiSe2013/Thesis/pattern01.png", 0, 100);
    ui->patternLabel->setPixmap(QPixmap::fromImage(*pattern).scaled(w,h,Qt::KeepAspectRatio));
}

void MainWindow::sensivity(int value)
{
    sensivityRange = (float)value/100;
}

void MainWindow::createGreyImage(Mat colorImage, int cmyk)
{
    int w = colorImage.cols;
    int h = colorImage.rows;

    float red = 0.299;
    float green = 0.587;
    float blue = 0.114;

    int cmyArray[3];

    greyImage->create(h, w, CV_32FC1);
    greyToScreen->create(h, w, CV_32FC3);

    for( int y=0; y<h; y++ )
    {
        for( int x=0; x<w; x++ )
        {
            bool draw = true;
            int first;
            int second;

            if( cmyk != 4 )
            {
                cmyArray[0] = 255-colorImage.at<Vec3b>(y,x)[0];
                cmyArray[1] = 255-colorImage.at<Vec3b>(y,x)[1];
                cmyArray[2] = 255-colorImage.at<Vec3b>(y,x)[2];

                //evtl. Summe > 50 prüfen
                if (cmyArray[0] > cmyArray[1])
                {
                    first = 0;
                    second = 1;
                }
                else{
                    first = 1;
                    second = 0;
                }
                if(cmyArray[2] > cmyArray[first])
                {
                    second = first;
                    first = 2;
                }
                else{
                    if(cmyArray[2] > cmyArray[second])
                    {
                        second = 2;
                    }
                }
                if(first != cmyk || cmyArray[first] < 1.5*cmyArray[second])   //so that color is definitly identified
                {
                    draw = false;
                }
            }

            if (draw)
            {
                greyImage->at<float>(y,x) = red*colorImage.at<Vec3b>(y,x)[0]+green*colorImage.at<Vec3b>(y,x)[1]+blue*colorImage.at<Vec3b>(y,x)[2];
                greyToScreen->at<Vec3b>(y,x)[0] = (unsigned char)greyImage->at<float>(y,x);             //greyToScreen only for Testing
                greyToScreen->at<Vec3b>(y,x)[1] = (unsigned char)greyImage->at<float>(y,x);             //
                greyToScreen->at<Vec3b>(y,x)[2] = (unsigned char)greyImage->at<float>(y,x);             //
            }
            else{
                greyImage->at<float>(y,x) = 255;
                greyToScreen->at<Vec3b>(y,x)[0] = (unsigned char)greyImage->at<float>(y,x);             //greyToScreen only for Testing
                greyToScreen->at<Vec3b>(y,x)[1] = (unsigned char)greyImage->at<float>(y,x);             //
                greyToScreen->at<Vec3b>(y,x)[2] = (unsigned char)greyImage->at<float>(y,x);             //
            }


        }
    }
}

void MainWindow::createGreyPattern(Mat colorPattern, int cmyk)
{
    int w = colorPattern.cols;
    int h = colorPattern.rows;
    greyPatternPixelSum = 0;
    greyPatternPixelSumPow = 0;
    float red = 0.299;
    float green = 0.587;
    float blue = 0.114;

    greyPattern->create(h, w, CV_32FC1);
    greyToScreen->create(h, w, CV_32FC3);

    for( int y=0; y<h; y++ )
    {
        for( int x=0; x<w; x++ )
        {
            greyPattern->at<float>(y,x) = red*colorPattern.at<Vec3b>(y,x)[0]+green*colorPattern.at<Vec3b>(y,x)[1]+blue*colorPattern.at<Vec3b>(y,x)[2];
            double summand = greyPattern->at<float>(y,x);
            greyPatternPixelSum += summand;
            greyPatternPixelSumPow += pow(summand, 2);
            greyToScreen->at<Vec3b>(y,x)[0] = (unsigned char)greyPattern->at<float>(y,x);             //greyToScreen only for Testing
            greyToScreen->at<Vec3b>(y,x)[1] = (unsigned char)greyPattern->at<float>(y,x);             //
            greyToScreen->at<Vec3b>(y,x)[2] = (unsigned char)greyPattern->at<float>(y,x);             //
        }
    }
}

void MainWindow::filterImage()
{
    if(this->ui->radioC->isChecked())
    {
        this->filter(0);
    }
    if(this->ui->radioM->isChecked())
    {
        this->filter(1);
    }
    if(this->ui->radioY->isChecked())
    {
        this->filter(2);
    }
    if(this->ui->radioK->isChecked())
    {
        this->filter(3);
    }
}

void MainWindow::filter(int cmyk)
{
    Mat matImage = imread(this->imagePath.toStdString());
    Mat matPattern = imread(this->patternPath.toStdString());

    int w = ui->imageLabel_2->width();
    int h = ui->imageLabel_2->height();

    createGreyImage(matImage, cmyk);
    //createGreyPattern(matPattern, cmyk);      //filter not allowed before pattern loaded

    QImage greyQImage = QImage((unsigned char*) greyToScreen->data, greyToScreen->cols, greyToScreen->rows, greyToScreen->step, QImage::Format_RGB888);
    ui->imageLabel_2->setPixmap(QPixmap::fromImage(greyQImage).scaled(w,h,Qt::KeepAspectRatio));
}
