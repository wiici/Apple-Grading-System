#ifndef _IMAGEPROCESSWORKER_H_
#define _IMAGEPROCESSWORKER_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <QTextEdit>

#include <ImageProcessModule.h>

// Class ImageProcessWorker inherits from QObject
// because it provides the slots and signals.
class ImageProcessWorker : public QObject , public ImageProcessModule
{

    Q_OBJECT

public:
    ImageProcessWorker(QObject *parent = 0);
    ~ImageProcessWorker();
    enum ImageWindowName { SourceImage,
                           GrayScaleImage,
                           BoundaryImage
                         };
    std::map <int, const char*> WindowNameMap;
    std::vector<cv::Mat *> Images;

private:
    bool isConnectedToCamera;


signals:
    void sendFrame(cv::Mat *Image);
    void connectStatusHasChanged(QString);
    void connectionEstablished();
    void lostConnection();
    void cantConnectToCamera();
    void frameHasGrabbed();

public slots:
    void grabImageFromCamera();
    void changeSetup(int CameraIndex);
    void changeThresholdValue(int value);

};




#endif
