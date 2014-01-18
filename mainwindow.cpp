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

/// Global Variables
Mat img; Mat templ; Mat result; Mat img_display;

char* image_window = "Source Image";
char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;
float sensivityRange;

/// Function Headers
//void MatchingMethod( int, void* );


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
    matchingWithMethod(0);}

void MainWindow::on_findButton1_clicked()
{
    matchingWithMethod(1);
}

void MainWindow::on_findButton2_clicked()
{
    matchingWithMethod(2);
}

void MainWindow::on_findButton3_clicked()
{
    matchingWithMethod(3);}

void MainWindow::on_findButton4_clicked()
{
    matchingWithMethod(4);
}

void MainWindow::on_findButton5_clicked()
{
    matchingWithMethod(5);
}

void MainWindow::matchingWithMethod(int method){

    match_method = method;
    int tabnumber;
    tabnumber = ui->tabWidget->currentIndex();

    /// Load image and template
    if (tabnumber == 0)
    {
        img = imread(this->imagePath.toStdString());
    }
    else
    {
        img = imread(this->imagePath_2.toStdString());
    }

    templ = imread(this->patternPath.toStdString());

    /// Source image to display
    img.copyTo( img_display );

    /// Create the result matrix
    int result_cols =  img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create( result_cols, result_rows, CV_32FC1 );

    /// Do the Matching and Normalize
    //matchTemplate( img, templ, result, match_method );
    result = match();
    normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

    for( int i=0; i<50; i++ ){

        /// Localizing the best match with minMaxLoc
        double minVal; double maxVal; Point minLoc; Point maxLoc;
        Point matchLoc;

        //minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
        minMax(result, &minLoc, &maxLoc, &minVal, &maxVal);

        /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
        if( match_method  == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED )
          { matchLoc = minLoc; }
        else
          { matchLoc = maxLoc; }

        /// Show me what you got
        rectangle( img_display, matchLoc, Point( matchLoc.x + templ.cols , matchLoc.y + templ.rows ), Scalar::all(0), 2, 8, 0 );

        if(method<=1)
        {
            float test = result.at<float>(minLoc.y, minLoc.x);
             result.at<float>(minLoc.y, minLoc.x)=1;
            test = result.at<float>(minLoc.y, minLoc.x);
           if(minVal>=1-sensivityRange)
            {
                i=50;
            }
        }
        else
        {
            result.at<float>(maxLoc.y, maxLoc.x)=0;
            if(maxVal<=sensivityRange)
            {
                i=50;
            }
        }
    }

    //matchedImage = img_display;
    this->displayImageInImageLabel(img_display);

    //waitKey(0);                                 //nötig?
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


void MainWindow::createGreyImage(Mat colorImage)
{
    int w = colorImage.cols;
    int h = colorImage.rows;

    greyImage->create(h, w, CV_32FC1);
    greyToScreen->create(h, w, CV_32FC3);

    for( int y=0; y<h; y++ )
    {
        for( int x=0; x<w; x++ )
        {
            greyImage->at<float>(y,x) = 0.299*colorImage.at<Vec3b>(y,x)[0]+0.587*colorImage.at<Vec3b>(y,x)[1]+0.114*colorImage.at<Vec3b>(y,x)[2];
            greyToScreen->at<Vec3b>(y,x)[0] = (unsigned char)greyImage->at<float>(y,x);             //greyToScreen only for Testing
            greyToScreen->at<Vec3b>(y,x)[1] = (unsigned char)greyImage->at<float>(y,x);             //
            greyToScreen->at<Vec3b>(y,x)[2] = (unsigned char)greyImage->at<float>(y,x);             //
        }
    }
}

void MainWindow::createGreyPattern(Mat colorPattern)
{
    int w = colorPattern.cols;
    int h = colorPattern.rows;
    greyPatternPixelSum = 0;

    greyPattern->create(h, w, CV_32FC1);
    greyToScreen->create(h, w, CV_32FC3);

    for( int y=0; y<h; y++ )
    {
        for( int x=0; x<w; x++ )
        {
            greyPattern->at<float>(y,x) = 0.299*colorPattern.at<Vec3b>(y,x)[0]+0.587*colorPattern.at<Vec3b>(y,x)[1]+0.114*colorPattern.at<Vec3b>(y,x)[2];
            greyPatternPixelSum += greyPattern->at<float>(y,x);
            greyToScreen->at<Vec3b>(y,x)[0] = (unsigned char)greyPattern->at<float>(y,x);             //greyToScreen only for Testing
            greyToScreen->at<Vec3b>(y,x)[1] = (unsigned char)greyPattern->at<float>(y,x);             //
            greyToScreen->at<Vec3b>(y,x)[2] = (unsigned char)greyPattern->at<float>(y,x);             //
        }
    }
}

Mat MainWindow::match()
{
    int result_cols =  greyImage->cols - greyPattern->cols + 1;
    int result_rows = greyImage->rows - greyPattern->rows + 1;

    Mat localResult;

    localResult.create( result_cols, result_rows, CV_32FC1 );

    for( int y=0; y<localResult.rows; y++ )
    {
        for( int x=0; x<localResult.cols; x++ )
        {
            localResult.at<float>(y,x) = matchingAlgorithm(x,y);
        }
    }
    return localResult;
}

float MainWindow::matchingAlgorithm(int x, int y)
{
    float pixelResult = 0;
    double greyImagePixelSum = 0;
    for( int y2=0; y2<greyPattern->rows; y2++ )
    {
        for( int x2=0; x2<greyPattern->cols; x2++ )
        {
            greyImagePixelSum += greyImage->at<float>(y+y2,x+x2);

        }
    }
    for( int y2=0; y2<greyPattern->rows; y2++ )
    {
        for( int x2=0; x2<greyPattern->cols; x2++ )
        {
            pixelResult += pow( tmpFunction(x2,y2) - imgFunction(x+x2,y+y2, greyImagePixelSum), (float)(2.0) );
        }
    }
    return pixelResult;
}

float MainWindow::tmpFunction( int x, int y )
{
    float result;
    result = greyPattern->at<float>(y,x) - greyPatternPixelSum/(greyPattern->cols * greyPattern->rows);
    return result;
}

float MainWindow::imgFunction( int x, int y, double greyImagePixelSum )
{

    float result;
    result = greyImage->at<float>(y,x) - greyImagePixelSum/(greyPattern->cols * greyPattern->rows);
    return result;
}

void MainWindow::minMax(Mat matResult, Point* min, Point* max, double* minVal, double* maxVal)
{
    *minVal = matResult.at<float>(0,0);
    *maxVal = matResult.at<float>(0,0);

    for( int y=0; y<matResult.rows; y++ )
    {
        for( int x=0; x<matResult.cols; x++ )
        {
            if( matResult.at<float>(y,x) < (float)(*minVal) )
            {
                *minVal = matResult.at<float>(y,x);
                min->x = x;
                min->y = y;
            }
            if( matResult.at<float>(y,x) > (float)(*maxVal) )
            {
                *maxVal = matResult.at<float>(y,x);
                max->x = x;
                max->y = y;
            }
        }
    }
}
