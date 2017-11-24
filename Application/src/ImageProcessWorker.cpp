#include "include/ImageProcessWorker.h"
#include <QMessageBox>
#include <QDebug>
#include <map>
//***************************************************
//
//      CONSTRUCTORS / DESTRUCTOR
//
//

ImageProcessWorker::ImageProcessWorker(QSlider *BrightnessSlider, QSlider *SaturationSlider, QSlider *ContrastSlider, QObject *parent) :
    QObject(parent),
    BrightnessSlider(BrightnessSlider),
    SaturationSlider(SaturationSlider),
    ContrastSlider(ContrastSlider),
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


    setSlidersRange();

    this->BrightnessSlider->setDisabled(true);
    this->ContrastSlider->setDisabled(true);
    this->SaturationSlider->setDisabled(true);

    connect(this, SIGNAL(connectionEstablished()), this, SLOT(enableWidgets()));
    connect(this, SIGNAL(cantConnectToCamera()), this, SLOT(disableWidgets()));
    connect(this->BrightnessSlider, SIGNAL(sliderReleased()), this, SLOT(changeCameraBrightness()) );
    connect(this->SaturationSlider, SIGNAL(sliderReleased()), this, SLOT(changeCameraSaturation()));
    connect(this->ContrastSlider, SIGNAL(sliderReleased()), this, SLOT(changeCameraContrast()));
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


void ImageProcessWorker::enableWidgets()
{
    this->BrightnessSlider->setDisabled(false);
    this->ContrastSlider->setDisabled(false);
    this->SaturationSlider->setDisabled(false);

    setSlidersInitValue();
}

void ImageProcessWorker::disableWidgets()
{
    this->BrightnessSlider->setDisabled(true);
    this->ContrastSlider->setDisabled(true);
    this->SaturationSlider->setDisabled(true);
}

 void ImageProcessWorker::changeCameraBrightness()
 {
     this->setCameraBrightness((double)this->BrightnessSlider->value() / this->SlidersRange);
     qWarning() << "ImageProcessWorker::changeCameraBrightness()_Brightness: " <<
                   (double)this->BrightnessSlider->value() / this->SlidersRange;


 }

 void ImageProcessWorker::changeCameraSaturation()
 {
     this->setCameraSaturation((double)this->SaturationSlider->value()/this->SlidersRange);
     qWarning() << "ImageProcessWorker::changeCameraSaturation()_Saturation: " <<
                   (double)this->SaturationSlider->value() / this->SlidersRange;
 }

 void ImageProcessWorker::changeCameraContrast()
 {
     this->setCameraContrast((double)this->ContrastSlider->value()/this->SlidersRange);
     qWarning() << "ImageProcessWorker::changeCameraContrast()Contrast: " <<
                   (double)this->ContrastSlider->value() / this->SlidersRange;
 }

//*************************************************
//
//          OTHER FUNCTIONS
//
//

void ImageProcessWorker::setSlidersRange()
{
    this->BrightnessSlider->setRange(0, this->SlidersRange);
    this->SaturationSlider->setRange(0, this->SlidersRange);
    this->ContrastSlider->setRange(0, this->SlidersRange);
}

void ImageProcessWorker::setSlidersInitValue()
{
    this->BrightnessSlider->setSliderPosition((int)(this->getCameraBrightness()*this->SlidersRange));
    this->SaturationSlider->setSliderPosition((int)(this->getCameraSaturation()*this->SlidersRange));
    this->ContrastSlider->setSliderPosition((int)(this->getCameraContrast()*this->SlidersRange));
}
