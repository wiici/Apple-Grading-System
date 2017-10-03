#include "include/ImageProcessModule.h"

//***************************************************
//
//      CONSTRUCTORS / DESTRUCTOR
//
//

ImageProcessModule::ImageProcessModule(QObject *parent) :
    QObject(parent),
    connectStatus(false),
    thresholdValue(150)
{
    SourceImage = new cv::Mat();
    GrayScaleImage = new cv::Mat();
    BinaryImage = new cv::Mat();

    Camera = new cv::VideoCapture();
}


ImageProcessModule::~ImageProcessModule()
{

    if( Camera->isOpened() )
		Camera->release();

    delete Camera;

}

//*************************************************
//
//          SLOTS
//
//

void ImageProcessModule::grabImage()
{
    (*Camera) >> (*SourceImage);

    if( SourceImage->empty() )
        return;

    preProcess();

    cv::imshow("source", *SourceImage);
    cv::imshow("grayscale", *GrayScaleImage);
    cv::imshow("binary", *BinaryImage);

    emit sendFrame(SourceImage);
}


void ImageProcessModule::changeSetup(int CameraIndex)
{

    CameraIndex--;

    if( Camera->isOpened() )
        Camera->release();

    emit connectStatusHasChanged("background-color: red");

    Camera->open(CameraIndex);

    if( Camera->isOpened() )
        emit connectStatusHasChanged("background-color: green");

}


void ImageProcessModule::changeThresholdValue(int value)
{
    if(value <= 256 && value >= 0)
        this->thresholdValue = value;
}

//*************************************************
//
//          OTHER FUNCTIONS
//
//


void ImageProcessModule::preProcess()
{

    cv::cvtColor(*SourceImage, *GrayScaleImage, CV_BGR2GRAY);

    cv::threshold(*GrayScaleImage, *BinaryImage, thresholdValue, 250, cv::THRESH_BINARY);

}

