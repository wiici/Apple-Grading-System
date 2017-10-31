#include "include/ImageProcessWorker.h"
#include <QMessageBox>

//***************************************************
//
//      CONSTRUCTORS / DESTRUCTOR
//
//

ImageProcessWorker::ImageProcessWorker(QObject *parent) :
    QObject(parent)
{
    ImageProcessModule();
}


ImageProcessWorker::~ImageProcessWorker()
{



}

//*************************************************
//
//          SLOTS
//
//

void ImageProcessWorker::grabImageFromCamera()
{

    if( this->grabImage() )
    {
        this->imagePreProcessing();

        this->imageSegmentation();

        emit frameHasGrabbed();

    }
    else
        emit cantGrabImage();
}


void ImageProcessWorker::changeSetup(int CameraIndex)
{
    CameraIndex--;

    if(CameraIndex < 0)
        return;

    if( this->connectToCamera(CameraIndex) )
        emit connectStatusHasChanged("background-color: green");
    else
        emit cantConnectToCamera();
}


void ImageProcessWorker::changeThresholdValue(int value)
{
    ImageProcessModule::setThresholdValue(value);
}

//*************************************************
//
//          OTHER FUNCTIONS
//
//

