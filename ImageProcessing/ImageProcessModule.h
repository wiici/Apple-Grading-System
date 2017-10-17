/*
 * ImageProcessModule.h
 *
 *  Created on: Oct 17, 2017
 *      Author: witek
 */

#ifndef IMAGEPROCESSMODULE_H_
#define IMAGEPROCESSMODULE_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

class ImageProcessModule
{
private:

	cv::Mat *SourceImage;
	cv::Mat *GrayScaleImage;
	cv::Mat *BinaryImage;

	cv::VideoCapture *Camera;

	unsigned int ThresholdValue;

public:
	ImageProcessModule(unsigned int InitThresholdValue = 127);
	~ImageProcessModule();

	// SET() FUNCTIONS
	void setThresholdValue(const unsigned int Value);


	// GET() FUNCTIONS
	unsigned int getThresholdValue();




};



#endif /* IMAGEPROCESSMODULE_H_ */
