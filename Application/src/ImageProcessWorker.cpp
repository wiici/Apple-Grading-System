#include "include/ImageProcessWorker.h"
#include <QMessageBox>
#include <map>
//***************************************************
//
//      CONSTRUCTORS / DESTRUCTOR
//
//

ImageProcessWorker::ImageProcessWorker(QObject *parent) :
    QObject(parent),
    isConnectedToCamera(false)
{
    ImageProcessModule();

    this->WindowNameMap[SourceImage] = "Source image";
    this->WindowNameMap[GrayScaleImage] = "Grayscale image";
    this->WindowNameMap[BoundaryImage] = "Boundary image";

    this->Images.resize(this->WindowNameMap.size());

    Images[SourceImage] = getSourceImage();
    Images[GrayScaleImage] = getGrayscaleImage();
    Images[BoundaryImage] = getBinaryImage();

}


ImageProcessWorker::~ImageProcessWorker()
{
    this->~ImageProcessModule();
    WindowNameMap.clear();
    Images.clear();
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
    else {
        if(this->isConnectedToCamera) {
            emit lostConnection();
            this->isConnectedToCamera = false;
        }
        }
}


void ImageProcessWorker::changeSetup(int CameraIndex)
{
    CameraIndex--;

    if(CameraIndex < 0)
        return;

    if( this->connectToCamera(CameraIndex) ) {
        this->isConnectedToCamera = true;
        emit connectStatusHasChanged("background-color: green");
        emit connectionEstablished();
    }
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

