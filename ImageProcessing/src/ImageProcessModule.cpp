#include "ImageProcessModule.h"


/*
 *
 * 		CONSTRUCTORS / DESTRUCTOR
 *
 */

ImageProcessModule::ImageProcessModule(int InitThresholdValue) :
ThresholdValue(InitThresholdValue)
{
	this->SourceImage = new cv::Mat();
	this->GrayScaleImage = new cv::Mat();
	this->BinaryImage = new cv::Mat();

	this->Camera = new cv::VideoCapture();
}

ImageProcessModule::~ImageProcessModule()
{

	this->SourceImage->~Mat();
	this->GrayScaleImage->~Mat();
	this->BinaryImage->~Mat();

	this->Camera->release();



}

/*
 *
 * 			SETTERS
 *
 */
void ImageProcessModule::setThresholdValue(int Value)
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

bool ImageProcessModule::setCameraBrightness(double Value)
{
	return this->Camera->set(cv::CAP_PROP_BRIGHTNESS, Value);
}

bool ImageProcessModule::setCameraSaturation(double Value)
{
	return this->Camera->set(cv::CAP_PROP_SATURATION, Value);
}

bool ImageProcessModule::setCameraContrast(double Value)
{
	return this->Camera->set(cv::CAP_PROP_CONTRAST, Value);
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
	return SourceImage;
}

cv::Mat* ImageProcessModule::getGrayscaleImage() {
	return GrayScaleImage;
}

cv::Mat* ImageProcessModule::getBinaryImage() {
	return BinaryImage;
}


cv::VideoCapture* ImageProcessModule::getCamera() {
	return Camera;
}

int ImageProcessModule::getCameraFrameWidth()
{
	return this->Camera->get(cv::CAP_PROP_FRAME_WIDTH);
}

int ImageProcessModule::getCameraFrameHeight()
{
	return this->Camera->get(cv::CAP_PROP_FRAME_HEIGHT);
}

int ImageProcessModule::getCameraFrameRate()
{
	return this->Camera->get(cv::CAP_PROP_FPS);
}

double ImageProcessModule::getCameraBrightness()
{
	return this->Camera->get(cv::CAP_PROP_BRIGHTNESS);
}

double ImageProcessModule::getCameraSaturation()
{
	return this->Camera->get(cv::CAP_PROP_SATURATION);
}

double ImageProcessModule::getCameraContrast()
{
	return this->Camera->get(cv::CAP_PROP_CONTRAST);
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
	cv::cvtColor(*SourceImage, *GrayScaleImage, CV_RGB2GRAY);

	// blur an image to reduce a noise (a mask 3x3)
	cv::blur( *(this->GrayScaleImage), *(this->GrayScaleImage), cv::Size(3,3) );
}

bool ImageProcessModule::grabImage() {
	if(this->Camera->isOpened())
		*(this->Camera) >> *(this->SourceImage);
	else
		return false;

	return !this->SourceImage->empty();
}

void ImageProcessModule::displayImages() {


	cv::imshow("SourceImage", *BinaryImage);
	cv::waitKey(25);

}

void ImageProcessModule::imageSegmentation() {

	thresh_callback(0,0);

}

/** @function thresh_callback */
void ImageProcessModule::thresh_callback(int, void* )
{
	cv::Mat canny_output;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::RNG rng(12345);

	/// Detect edges using canny
	cv::Canny( *(this->GrayScaleImage), *(this->BinaryImage), this->ThresholdValue, this->ThresholdValue*2, 3 );
	/// Find contours
	cv::findContours( *(this->BinaryImage), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );
	/// Get the moments
	std::vector<cv::Moments> mu(contours.size() );
	for( unsigned int i = 0; i < contours.size(); i++ )
	{ mu[i] = moments( contours[i], false ); }

	///  Get the mass centers:
	std::vector<cv::Point2f> mc( contours.size() );
	for( unsigned int i = 0; i < contours.size(); i++ )
	{ mc[i] = cv::Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 ); }
	/// Draw contours
	cv::Mat drawing = cv::Mat::zeros( this->BinaryImage->size(), CV_8UC3 );
	for( unsigned int i = 0; i< contours.size(); i++ )
	{
		// set white color of contours
		cv::Scalar color = cv::Scalar( 255, 255, 255 );
		cv::drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, cv::Point() );
		circle( drawing, mc[i], 4, color, -1, 8, 0 );
	}

	/// Show in a window
	//cv::namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
	//cv::imshow( "Contours", drawing );

}
