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
#include <opencv2/video/video.hpp>

/**
 *
 * Using the OpenCV library, the class implements functions which are using in apple grading system.
 *
 *
 */
class ImageProcessModule
{
private:

	cv::Mat *SourceImage;
	cv::Mat *GrayScaleImage;
	cv::Mat *BinaryImage;

	cv::VideoCapture *Camera;

	unsigned int ThresholdValue;

public:
	/**
	 * @brief Constructor
	 *
	 * @param InitThresholdValue set the value of threshold for binarization.
	 */
	ImageProcessModule(unsigned int InitThresholdValue = 127);
	/*
	 * @brief Desctructor
	 */
	~ImageProcessModule();

	/**
	 *
	 * @brief Setter
	 *
	 * Set the threshold value which is used for binarization.
	 *
	 * @param Value to set
	 */
	void setThresholdValue(const unsigned int Value);


	/**
	 * @brief Getter
	 *
	 * @return The actual threshold value.
	 */
	unsigned int getThresholdValue();

};



#endif /* IMAGEPROCESSMODULE_H_ */
