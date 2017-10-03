#ifndef _IMAGEPROCESSMODULE_H_
#define _IMAGEPROCESSMODULE_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <QTextEdit>

// Class ImageProcessModule inherits from QObject
// because it provides the slots and signals.
class ImageProcessModule : public QObject {

    Q_OBJECT

	private:
		// The image receiving from the camera
        cv::Mat *SourceImage;
        cv::Mat *GrayScaleImage;
        cv::Mat *BinaryImage;
		cv::VideoCapture *Camera;

        bool connectStatus;
        unsigned int thresholdValue;

        void preProcess();

	public:
        explicit ImageProcessModule(QObject *parent = 0);
		~ImageProcessModule();
        bool getConnectStatus();

    signals:
        void sendFrame(cv::Mat *Image);
        void connectStatusHasChanged(QString);

    public slots:
        void grabImage();
        void changeSetup(int CameraIndex);
        void changeThresholdValue(int value);

};




#endif
