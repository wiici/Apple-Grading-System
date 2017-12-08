/*
 * ImageProcessModule.h
 *
 *  Created on: Oct 17, 2017
 *      Author: witek
 */

#ifndef IMAGEPROCESSMODULE_H_
#define IMAGEPROCESSMODULE_H_

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/photo.hpp>

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
	cv::Mat* LightReflectionsImage;

	cv::VideoCapture* Camera;

	// image processing parameters
	int ReflectsThresholdValue;
	int InpaintRadius;
	int SecondThresholdValue;

	// threshold values for apple defects segmentation
	int minRGBvalues[3];
	int maxRGBvalues[3];

	enum FruitQualityCategories{ExtraClass = 1,
								ClassI,
								ClassII,
								Rejected};

	// draw red rectangle around the apple defects after segmentation
	void drawSegmentationArea();


public:

	/**
	 * @brief Constructor
	 *
	 * @param InitThresholdValue set the value of threshold for binariazation.
	 */
	ImageProcessModule(int InitThresholdValue = 127, int InitRadius = 3);

	/*
	 * @brief Desctructor
	 */
	~ImageProcessModule();

	enum Colour{Red, Green, Blue};

	/**
	 *
	 * @brief Setter
	 *
	 * Set the threshold value which is used for binarization.
	 *
	 * @param Value to set
	 */
	void setReflectsThresholdValue(int Value);

	/**
	 * @brief Set camera brightness (it depends on the camera)
	 *
	 * @param Double Value to set from 0.0 to 1.0
	 *
	 * @return true if succeed
	 */
	bool setCameraBrightness(double Value);

	/**
	 * @brief Set camera saturation (it depends on the camera)
	 *
	 * @param Double Value to set from 0.0 to 1.0
	 *
	 * @return true if succeed
	 */
	bool setCameraSaturation(double Value);

	/**
	 * @brief Set camera contrast (it depends on the camera)
	 *
	 * @param Double Value to set from 0.0 to 1.0
	 *
	 * @return true if succeed
	 */
	bool setCameraContrast(double Value);

	/**
	 * @brief Set minimal value for R,G or B channel
	 *
	 * @param colour is a colour number (see Colour) indicates which channel should be set.
	 *
	 * @param value from 0 to 255
	 */
	void set_minRGBvalue(int colour, int value);

	/**
	 * @brief Set maximal value for R,G or B channel
	 *
	 * @param colour is a colour number (see Colour) indicates which channel should be set.
	 *
	 * @param value from 0 to 255
	 */
	void set_maxRGBvalue(int colour, int Value);

	/** @brief set number of neighbours that will be taken into account in the cv::inpaint
	 *  function to get rid of area with light reflects.
	 *
	 *  @param Value depends on the effects you want to get (usually it is set to 3 or 4)
	 */
	void setInpaintRadius(int Value);

	/**
	 * @brief Get threshold value to get rid of areas with light reflects.
	*/
	int getReflectsThresholdValue();

	/**
	 * @brief Get inpaint radius (see cv::inpaint)
	 */
	int getInpaintRadius();

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

	/*
	 * @brief Get the pointer to the image with an area of light reflects.
	 */
	cv::Mat* getLightReflectionsImage();

	//cv::Mat* getHSV_Image();

	/**
	 * @brief Getter
	 *
	 * @return Get the pointer to the source image from the camera.
	 */
	cv::VideoCapture* getCamera();

	/**
	 * @brief Get information about width of camera frame.
	 */
	int getCameraFrameWidth();

	/**
	 * @brief Get information about height of camera frame.
	 */
	int getCameraFrameHeight();

	/**
	 * @brief Get information about camera FPS parameter.
	 */
	int getCameraFrameRate();

	double getCameraBrightness();

	double getCameraSaturation();

	double getCameraContrast();

	/**
	 * @brief Get the minimal value of R,G or B channel.
	 *
	 * @param Colour indicates which colour you want to get (see Colour)
	 */
	int getMinRGBvalue(int Colour);

	/**
	 * @brief Get the maximal value of R,G or B channel.
	 *
	 * @param Colour indicates which colour you want to get (see Colour)
	 */
	int getMaxRGBvalue(int Colour);


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

	//void displayImages();

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
	 *  - reduce noise from image (see cv::medianBlur)
	 *  - get binary image with light reflects (cv::threshold) and inpaint this region (cV::inpaint)
	 *
	 *
	 */
	void imagePreProcessing();

	/**
	 * @brief Get area with apple defects (use cv::inRange function).
	 *
	 *
	 */
	void imageSegmentation();


	/**
	 * @brief Create k-N classifier based on training images
	 *
	 * This function creates k-NN classifier based on training images and create .xml file.
	 * In a loop, images are loaded from folder TrainingData. The image files should have names
	 * Data0, Data1, Data2, Data3...
	 */
	void create_kNN_Classifier();

};



#endif /* IMAGEPROCESSMODULE_H_ */
