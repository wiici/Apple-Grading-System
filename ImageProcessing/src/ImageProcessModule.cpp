#include "ImageProcessModule.h"


/*
 *
 * 		CONSTRUCTORS / DESTRUCTOR
 *
 */

ImageProcessModule::ImageProcessModule(int InitThresholdValue)
{
	this->setThresholdValue(InitThresholdValue);

	SourceImage = new cv::Mat();
	GrayScaleImage = new cv::Mat();
	BinaryImage = new cv::Mat();

	Camera = new cv::VideoCapture();
}

ImageProcessModule::~ImageProcessModule()
{
	SourceImage->~Mat();
	GrayScaleImage->~Mat();
	BinaryImage->~Mat();

	Camera->~VideoCapture();
}

/*
 *
 * 			SETTERS
 *
 */
void ImageProcessModule::setThresholdValue(const int Value)
{
	if(Value > 255) {
		this->ThresholdValue = 255;
		return;
	}
	else if(Value < 0) {
		this->ThresholdValue = 0;
		return;
	}

	this->ThresholdValue = Value;
}

/*
 *
 * 			GETTERS
 *
 */

int ImageProcessModule::getThresholdValue()
{
	return this->ThresholdValue;
}

cv::Mat* ImageProcessModule::getSourceImage() {
	return this->SourceImage;
}

cv::Mat* ImageProcessModule::getGrayscaleImage() {
	return this->GrayScaleImage;
}

cv::Mat* ImageProcessModule::getBinaryImage() {
	return this->BinaryImage;
}


cv::VideoCapture* ImageProcessModule::getCamera() {
	return this->Camera;
}



bool ImageProcessModule::connectToCamera(int CameraID) {

	if(CameraID < 0)
		return false;

	if(this->Camera->isOpened())
		this->Camera->release();

	this->Camera->open(CameraID);

	return this->Camera->isOpened();
}

void ImageProcessModule::imagePreProcessing() {

	// RGB -> Grayscale
	cv::cvtColor(*(this->SourceImage), *(this->GrayScaleImage), cv::COLOR_RGB2GRAY);

	// Grayscale -> Binary
	cv::threshold(*(this->GrayScaleImage), *(this->BinaryImage), this->ThresholdValue, 255, cv::THRESH_OTSU);

}

bool ImageProcessModule::grabImage() {
	if(this->Camera->isOpened())
		*(this->Camera) >> *(this->SourceImage);
	else
		return false;

	return !this->SourceImage->empty();
}

void ImageProcessModule::displayImages() {

	while(this->grabImage())
	{
		cv::imshow("SourceImage", *(this->SourceImage));

	}

}

void ImageProcessModule::imageSegmentation() {


}
