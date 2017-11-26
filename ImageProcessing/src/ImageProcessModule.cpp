#include "ImageProcessModule.h"
#include <string>
#include <vector>



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



void ImageProcessModule::create_kNN_Classifier()
{
	int ClassificationGroup = 3;
	int isGoodNumber = 0; // 0 means false
	int PressedKey;
	int FileCounter = 1;

	// User rate apple quality (0,1,2 or 3) and his choose is
	// remembered in a vector
	std::vector<int> AppleClassifications;
	// xml to store apple quality rates (this file format will be
	// needed in opencv kNN algorithm)
	cv::FileStorage AppleQualityClassifications("../TrainingData/AppleQualityClassifications.xml", cv::FileStorage::WRITE);

	// Based file name
	std::string FIleName = "../TrainingData/Data";
	// Add file number and "jpg"
	std::string FIleNameWithNumber = FIleName + std::to_string(FileCounter) + ".jpg";

	std::cout << "1 -> ExtraClass (without any defects)\n" <<
			"2 -> ClassI (small skin defents)\n" <<
			"3 -> ClassII (more skin defects)\n" <<
			"4 -> Reject\n";

	//std::cout << "number:" << FIleNameWithNumber << std::endl;

	// read the first image
	*(this->SourceImage) = cv::imread(FIleNameWithNumber, CV_LOAD_IMAGE_COLOR);

	while( this->SourceImage->data ) {

		isGoodNumber = 0;

		//std::cout << "number:" << FIleNameWithNumber << std::endl;

		// image operations
		//
		//
		//
		//

		// extract features vector


		// show images
		imshow("Source image", *(this->SourceImage));

		// a keyboard key was preesed ; read the char sign
		PressedKey = cv::waitKey(0);

		while( !isGoodNumber ) {

			switch(PressedKey) {
			case '1':
				ClassificationGroup = this->ExtraClass;
				AppleClassifications.push_back(this->ExtraClass);
				isGoodNumber = 1;
				std::cout << "Selected quality group: Extra" << std::endl;
				break;
			case '2':
				ClassificationGroup = this->ClassI;
				AppleClassifications.push_back(this->ClassI);
				isGoodNumber = 1;
				std::cout << "Selected quality group: classI" << std::endl;
				break;
			case '3':
				ClassificationGroup = this->ClassII;
				AppleClassifications.push_back(this->ClassII);
				isGoodNumber = 1;
				std::cout << "Selected quality group: classII" << std::endl;
				break;
			case '4':
				ClassificationGroup = this->Rejected;
				AppleClassifications.push_back(this->Rejected);
				isGoodNumber = 1;
				std::cout << "Selected quality group: rejected" << std::endl;
				break;
			// if ESC pressed, nothing will be created (27 is ESC in ASCII)
			case 27:
				return;
			// if not valid key was pressed then wait again for key press
			default:
				PressedKey = cv::waitKey(0);
				isGoodNumber = 0;
				break;
			}
			isGoodNumber = 1;
		}

		// Read the next image
		FileCounter++;
		// Concatenate string
		FIleNameWithNumber = FIleName + std::to_string(FileCounter) + ".jpg";
		// Read image from the file
		*(this->SourceImage) = cv::imread(FIleNameWithNumber, CV_LOAD_IMAGE_COLOR);
	}

	AppleQualityClassifications << "Apple_classifications";
	// save apple quality rates in the .xml file
	AppleQualityClassifications << AppleClassifications;

	if( AppleQualityClassifications.isOpened() )
		std::cout << "Created AppleQualityClassifications.xml file" << std::endl;
}
