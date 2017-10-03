#include "include/ImageProcessModule.h"

//***************************************************
//
//      CONSTRUCTORS / DESTRUCTOR
//
//

ImageProcessModule::ImageProcessModule(QObject *parent) :
    QObject(parent),
    connectStatus(false)
{
    Camera = new cv::VideoCapture();
}


ImageProcessModule::ImageProcessModule(const int DeviceIndex, QObject *parent) :
    ImageProcessModule(parent)

{
	Camera = new cv::VideoCapture(DeviceIndex);

	if( !Camera->isOpened() )
			;
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
    (*Camera) >> (this->SourceImage);

    emit sendFrame(this->SourceImage);
}


void ImageProcessModule::changeSetup(const int CameraIndex)
{

    if( Camera->isOpened() )
        Camera->release();

    emit connectStatusHasChanged("background-color: red");

    Camera->open(CameraIndex);

    if( Camera->isOpened() )
        emit connectStatusHasChanged("background-color: green");

}


//*************************************************
//
//          OTHER FUNCTIONS
//
//




