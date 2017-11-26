/*
 * ImageProcessModule.h
 *
 *  Created on: Oct 17, 2017
 *      Author: witek
 */

#ifndef IMAGEPROCESSMODULE_H_
#define IMAGEPROCESSMODULE_H_

#include "FeaturesVector.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <cstdlib>
#include <cstdio>
#include <string.h>


/**
 *
 * Using the OpenCV library, the class implements functions which are using in
 * the apple grading system.
 *
 */
class ImageProcessModule
{
private:


	cv::Mat* SourceImage;
	cv::Mat* GrayScaleImage;
	cv::Mat* BinaryImage;

	cv::VideoCapture* Camera;

	// image processing parameters
	int ThresholdValue;

	enum FruitQualityCategories{ExtraClass = 1,
								ClassI,
								ClassII,
								Rejected};


public:

	/**
	 * @brief Constructor
	 *
	 * @param InitThresholdValue set the value of threshold for binariazation.
	 */
	ImageProcessModule(int InitThresholdValue = 127);

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
	void setThresholdValue(int Value);

	bool setCameraBrightness(double Value);

	bool setCameraSaturation(double Value);

	bool setCameraContrast(double Value);


	/**
	 * @brief Getter
	 *
	 * @return The actual threshold value.
	 */
	int getThresholdValue();

	/**
	 * @brief Getter
	 *
	 * @return Get the pointer to the source image from the camera.
	 */
	cv::Mat* getSourceImage();

	/**
	 * @brief Getter
	 *
	 * @return Get the pointer to the grayscale image.
	 */
	cv::Mat* getGrayscaleImage();

	/**
	 * @brief Getter
	 *
	 * @return Get the pointer to the binary image which is create by
	 * @see imagePreProcessing()
	 */
	cv::Mat* getBinaryImage();

	/**
	 * @brief Getter
	 *
	 * @return Get the pointer to the source image from the camera.
	 */
	cv::VideoCapture* getCamera();

	int getCameraFrameWidth();

	int getCameraFrameHeight();

	int getCameraFrameRate();

	double getCameraBrightness();

	double getCameraSaturation();

	double getCameraContrast();


	/**
	 * @brief Connect to the attached camera.
	 *
	 * Connect to the camera using @link cv::VideoCapture::open @endlink. If something was connected earlier,
	 * it will be disconnected and the new connection will be established.
	 *
	 * @param CameraID The identification number of the camera attached	 to the Linux system
	 * 		  (check /dev/videoX file where X is the camera ID). This ID number should be positive.
	 *
	 * @return <B>True</B>, when the connection operation has ended successfully
	 * @return <B>False</B>, when could not connect to the camera (ID doesn't exist or negative number).
	 *
	 */
	bool connectToCamera(int CameraID);

	void displayImages();

	/**
	 * @brief Grab the image from the camera.
	 *
	 * This function grabs the image and save it in the SourceImage.
	 *
	 * @return <B>True</B>, when the operation has ended successfully.
	 * @return <B>False</B>, when there isn't connection with the camera
	 * or the grabbed image is empty.
	 */
	bool grabImage();

	/**
	 * @brief Preprocessing operations.
	 *
	 * Prepare the image for the next operations. Stages:
	 *  - change the image from RGB to grayscale (using @link cv::cvtColor @endlink).
	 *  - change the image from grayscale to binary using Otsu's thresholding
	 *    (using @link cv::threshold @endlink).
	 *
	 *
	 */
	void imagePreProcessing();

	void thresh_callback(int, void* );

	void imageSegmentation();

	void create_kNN_Classifier();

};



#endif /* IMAGEPROCESSMODULE_H_ */
