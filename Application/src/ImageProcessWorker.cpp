#include "include/ImageProcessWorker.h"
#include <QMessageBox>
#include <QDebug>
#include <QString>
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
    int ReflectsInitValue = 150;
    int InpaintRadius = 3;
    ImageProcessModule(ReflectsInitValue, InpaintRadius);

    // add available images to the map
    this->WindowNameMap[SourceImage] = "Source image";
    this->WindowNameMap[GrayScaleImage] = "Grayscale image";
    this->WindowNameMap[BoundaryImage] = "ROI Image";
    this->WindowNameMap[LightReflectsImage] = "Image with light reflects";

    this->Images.resize(this->WindowNameMap.size());

    // get pointer to images
    Images[SourceImage] = getSourceImage();
    Images[GrayScaleImage] = getGrayscaleImage();
    Images[BoundaryImage] = getBinaryImage();
    Images[LightReflectsImage] = getLightReflectionsImage();

    // set sliders for image processing
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
//         PUBLIC SLOTS
//
//

void ImageProcessWorker::grabImageFromCamera()
{
    // is operations succeed
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


void ImageProcessWorker::changeSetup(QString SelectedItemText)
{
    int CameraIndex;
    int index = SelectedItemText.indexOf(' ');
    QString FirstString = SelectedItemText.left(index);

    if(FirstString == "Select")
        return;

    CameraIndex = FirstString.toInt();

    qWarning() << "ImageProcessWorker::changeSetup()_CameraIndex: " << CameraIndex;

    if(CameraIndex < 0)
        return;

    if( this->connectToCamera(CameraIndex) ) {
        this->isConnectedToCamera = true;
        emit connectionEstablished();
    }
    else
        emit cantConnectToCamera();
}


void ImageProcessWorker::changeReflectsThresholdValue(int value)
{
    ImageProcessModule::setReflectsThresholdValue(value);
}

void ImageProcessWorker::changeMinRGBvalue(int value)
{
    QString SenderName = this->sender()->objectName();

    // Sliders which change these values should have
    // 'R', 'G' or 'B' sign on the first place in the string
    // e.g. RminValue_Slider , GminValue_Slider, BminValue_Slider
    QChar Colour = SenderName[0];

    // change QChar to char and check the value
    switch( Colour.unicode() ) {
    case 'R':
        set_minRGBvalue(Red, value);
        break;
    case 'G':
        set_minRGBvalue(Green, value);
        break;
    case 'B':
        set_minRGBvalue(Blue, value);
        break;
    }
    qWarning() <<  value;
}

void ImageProcessWorker::changeMaxRGBvalue(int value)
{
    QString SenderName = this->sender()->objectName();

    // Sliders which change these values should have
    // 'R', 'G' or 'B' sign on the first place in the string
    // e.g. RminValue_Slider , GminValue_Slider, BminValue_Slider
    QChar Colour = SenderName[0];

    // change QChar to char and check the value
    switch( Colour.unicode() ) {
    case 'R':
        set_maxRGBvalue(Red, value);
        break;
    case 'G':
        set_maxRGBvalue(Green, value);
        break;
    case 'B':
        set_maxRGBvalue(Blue, value);
        break;
    }
    qWarning() <<  value;
}

//*************************************************
//
//         PRIVATE SLOTS
//
//

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
