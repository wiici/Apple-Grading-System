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
class ImageProcessWorker : public QObject , public ImageProcessModule
{

    Q_OBJECT

public:
    ImageProcessWorker(QSlider *BrightnessSlider, QSlider *SaturationSlider, QSlider *ContrastSlider, QObject *parent = 0);
    ~ImageProcessWorker();
    enum ImageWindowName { SourceImage,
                           GrayScaleImage,
                           HSV_Image,
                           BoundaryImage,
                           LightReflectsImage
                         };
    std::map <int, const char*> WindowNameMap;
    std::vector<cv::Mat *> Images;

    static const int GrabImageInterval = 40; // msec

private:
    bool isConnectedToCamera;
    QSlider *BrightnessSlider;
    QSlider *SaturationSlider;
    QSlider *ContrastSlider;
    const int SlidersRange = 1000.0;

    void setSlidersRange();
    void setSlidersInitValue();


signals:
    void sendFrame(cv::Mat *Image);
    void connectStatusHasChanged(QString);
    void connectionEstablished();
    void lostConnection();
    void cantConnectToCamera();
    void frameHasGrabbed();

public slots:
    void grabImageFromCamera();
    void changeSetup(QString SelectedItemText);
    void changeReflectsThresholdValue(int value);
    void changeMinRGBvalue(int value);
    void changeMaxRGBvalue(int value);

private slots:
    void enableWidgets();
    void disableWidgets();
    void changeCameraBrightness();
    void changeCameraSaturation();
    void changeCameraContrast();


};




#endif
