
#include "ImageProcessModule.h"

/*
 *
 * 		CONSTRUCTORS / DESTRUCTOR
 *
 */

ImageProcessModule::ImageProcessModule(unsigned int InitThresholdValue) :
	ThresholdValue(InitThresholdValue)
{
	SourceImage = new cv::Mat();
	GrayScaleImage = new cv::Mat();
	BinaryImage = new cv::Mat();

	Camera = new cv::VideoCapture();
}

ImageProcessModule::~ImageProcessModule()
{
	delete SourceImage;
	delete GrayScaleImage;
	delete BinaryImage;
	delete Camera;
}

/*
 *
 * 			SETTERS
 *
 */
void ImageProcessModule::setThresholdValue(const unsigned int Value)
{
	if(Value > 255) {
		this->ThresholdValue = 255;
		return;
	}

	this->ThresholdValue = Value;
}

/*
 *
 * 			GETTERS
 *
 */

unsigned int ImageProcessModule::getThresholdValue()
{
	return this->ThresholdValue;
}
