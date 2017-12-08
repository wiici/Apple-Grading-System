#include "ImageProcessModule.h"
#include <string>
#include <vector>



/*
 *
 * 		CONSTRUCTORS / DESTRUCTOR
 *
 */

ImageProcessModule::ImageProcessModule(int InitThresholdValue, int InitRadius) :
	ReflectsThresholdValue(InitThresholdValue),
	InpaintRadius(InitRadius)
{

	this->SecondThresholdValue = InitThresholdValue;

	for(int i=0; i < 3; i++) {
		this->minRGBvalues[i] = 30;
		this->maxRGBvalues[i] = 60;
	}

	this->SourceImage = new cv::Mat();
	this->GrayScaleImage = new cv::Mat();
	this->BinaryImage = new cv::Mat();
	this->LightReflectionsImage = new cv::Mat();

	this->Camera = new cv::VideoCapture();
}

ImageProcessModule::~ImageProcessModule()
{

	this->SourceImage->~Mat();
	this->GrayScaleImage->~Mat();
	this->BinaryImage->~Mat();
	this->LightReflectionsImage->~Mat();

	this->Camera->release();



}

/*
 *
 * 			SETTERS
 *
 */
void ImageProcessModule::setReflectsThresholdValue(int Value)
{
	if(Value > 255) {
		this->ReflectsThresholdValue = 255;
		return;
	}
	else if(Value < 0) {
		this->ReflectsThresholdValue = 0;
		return;
	}

	this->ReflectsThresholdValue = Value;
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

void ImageProcessModule::set_minRGBvalue(int colour, int Value)
{
	// colour means R, G or B channel
	if(colour > 2)
		return;

	if(Value > 255 )
		this->minRGBvalues[colour] = 255;
	else if(Value < 0)
		this->minRGBvalues[colour] = 0;

	this->minRGBvalues[colour] = Value;
}

void ImageProcessModule::set_maxRGBvalue(int colour, int Value)
{
	if(colour > 2)
		return;

	if(Value > 255 )
		this->maxRGBvalues[colour] = 255;
	else if(Value < 0)
		this->maxRGBvalues[colour] = 0;

	this->maxRGBvalues[colour] = Value;
}


void ImageProcessModule::setInpaintRadius(int Value)
{
	if(Value > 15 || Value < 0)
		return;

	this->InpaintRadius = Value;
}

/*
 *
 * 			GETTERS
 *
 */

int ImageProcessModule::getReflectsThresholdValue()
{
	return this->ReflectsThresholdValue;
}

int ImageProcessModule::getInpaintRadius()
{
	return this->InpaintRadius;
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

cv::Mat* ImageProcessModule::getLightReflectionsImage() {
	return LightReflectionsImage;
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

int ImageProcessModule::getMinRGBvalue(int Colour)
{
	if(Colour > 2)
		return -1;

	return this->minRGBvalues[Colour];
}

int ImageProcessModule::getMaxRGBvalue(int Colour)
{
	if(Colour > 2)
		return -1;

	return this->maxRGBvalues[Colour];
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
	cv::cvtColor(*SourceImage, *GrayScaleImage, CV_BGR2GRAY);

	// reduce noise before thresholding
	cv::medianBlur(*(GrayScaleImage), *(GrayScaleImage), 3);

	// thresholding gray scale image to get an area with light reflects
	cv::threshold(*GrayScaleImage, *LightReflectionsImage,
				  this->ReflectsThresholdValue, 255, cv::THRESH_BINARY);

	cv::Mat tmp = *(SourceImage);

	// remove light reflects
	cv::inpaint(tmp, *LightReflectionsImage, *SourceImage, this->InpaintRadius, cv::INPAINT_NS);

	//cv::threshold(*(this->GrayScaleImage), *(this->HSV_Image), 10, 255, cv::THRESH_OTSU);

	// blur an image to reduce a noise (a mask 3x3)
	//cv::blur( *(this->GrayScaleImage), *(this->GrayScaleImage), cv::Size(3,3) );

	//cv::threshold(*(this->GrayScaleImage), *(this->BinaryImage),
		//	  this->ThresholdValue, 255, cv::THRESH_BINARY);
}


bool ImageProcessModule::grabImage() {
	if(this->Camera->isOpened())
		*(this->Camera) >> *(this->SourceImage);
	else
		return false;

	return !this->SourceImage->empty();
}

/*
void ImageProcessModule::displayImages() {


	cv::imshow("SourceImage", *BinaryImage);
	cv::waitKey(25);

}
*/

void ImageProcessModule::imageSegmentation() {

	cv::inRange(*(this->SourceImage),
			cv::Scalar(minRGBvalues[Blue], minRGBvalues[Green], minRGBvalues[Red]),
			cv::Scalar(maxRGBvalues[Blue], maxRGBvalues[Green], maxRGBvalues[Red]),
			*(this->BinaryImage) );
	cv::Mat element = cv::getStructuringElement( 2, cv::Size( 5,5 ), cv::Point( 0,0) );
	cv::Mat tmp = *(this->BinaryImage);
	//cv::morphologyEx(tmp, *(this->BinaryImage),
		//		     cv::MORPH_OPEN, element);

	drawSegmentationArea();

}


void ImageProcessModule::drawSegmentationArea()
{
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	/// Find contours
	cv::findContours( *(this->BinaryImage), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_NONE, cv::Point(0, 0) );

	for( unsigned int i = 0; i < contours.size(); i++ )
	{
		// sip small aread (propably some noise)
		if(cv::contourArea(contours[i]) > 200) {
			cv::Rect BoundingRect = cv::boundingRect(contours[i]);

			cv::rectangle(*(this->SourceImage), BoundingRect,
					  	  cv::Scalar(0,0,255), 4);
		}
	}
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
			imagePreProcessing();
			imageSegmentation();
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
