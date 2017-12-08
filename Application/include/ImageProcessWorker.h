#ifndef _IMAGEPROCESSWORKER_H_
#define _IMAGEPROCESSWORKER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <QTextEdit>
#include <QSlider>
#include <ImageProcessModule.h>

// Class ImageProcessWorker inherits from QObject
// because it provides the slots and signals.
/*! This class implements all slots provides signal for image processing
 * operations in this application.
 */
class ImageProcessWorker : public QObject , public ImageProcessModule
{

    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param BrightnessSlider is a QSlider to change the camera brightness.
     * @param SaturationSlider is a QSlider to change the camera saturation.
     * @param ContrastSlider is a QSlider to change the camera contrast.
     * @param parent is needed to create object hierarchies (default Qt doc about it QObject@details)
     */
    ImageProcessWorker(QSlider *BrightnessSlider, QSlider *SaturationSlider, QSlider *ContrastSlider, QObject *parent = 0);
    ~ImageProcessWorker();


    enum ImageWindowName { SourceImage,
                           GrayScaleImage,
                           BoundaryImage,
                           LightReflectsImage
                         };
    std::map <int, const char*> WindowNameMap; /**<Keep image number(from enum ImageWindowName) and the names of these images  */
    std::vector<cv::Mat *> Images;      /**<Keep all images*/

    static const int GrabImageInterval = 40; /**< This is the time of the image capture frequency */

private:
    bool isConnectedToCamera;
    QSlider *BrightnessSlider;
    QSlider *SaturationSlider;
    QSlider *ContrastSlider;
    const int SlidersRange = 1000.0;

    void setSlidersRange();
    void setSlidersInitValue();


signals:
    /**
     * @brief Signal is emmited when the attempt to connect to the camera
     *        ends successfully.
     */
    void connectionEstablished();
    /**
     * @brief Signal is emitted when the image cannot be captured
     */
    void lostConnection();
    /**
     * @brief Signal is emitted when the user tries to connect to the camera but
     *        for some reason it can't be done
     */
    void cantConnectToCamera();
    /**
     * @brief Signal is emitted when the attempt to capture the image is successful
     */
    void frameHasGrabbed();

public slots:
    /**
     * @brief Grab image from the camera and do image processing operations
     *
     *
    */
    void grabImageFromCamera();
    /**
     * @brief Change the camera connection.
     *
     * @param SelectedItemText is an item (camera) which user chose
     *
     *
    */
    void changeSetup(QString SelectedItemText);
    /**
     * @brief Change a parameter for image processing algorithm (get rid of the light reflects)
     *
     * @param value is a threshold value from 0 to 255
     *
     *
    */
    void changeReflectsThresholdValue(int value);
    /**
     * @brief Change parameter for defect segmentation (double thresholding).
     *
     *  This function recognizes the object from which it received the signal
     *  (slider for Rmin or Gmin or Bmin values) and save the value in
     *  the properly variable.
     *
     *  @param value is a threshold value from 0 to 255.
    */
    void changeMinRGBvalue(int value);
    /**
     * @brief Change parameter for defect segmentation (double thresholding).
     *
     *This function recognizes the object from which it received the signal
     *  (slider for Rmax or Gmax or Bmax values) and save the value in
     *  the properly variable.
     *
     * @param value is a threshold value from 0 to 255.
    */
    void changeMaxRGBvalue(int value);

private slots:
    // eneable widgets when there is connection to the camera
    void enableWidgets();
    // disable widgets when there isn't connection to the camera
    void disableWidgets();
    void changeCameraBrightness();
    void changeCameraSaturation();
    void changeCameraContrast();


};




#endif
