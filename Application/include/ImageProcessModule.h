#ifndef _IMAGEPROCESSMODULE_H_
#define _IMAGEPROCESSMODULE_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QTextEdit>

// Class ImageProcessModule inherits from QObject
// because it provides the slots and signals.
class ImageProcessModule : public QObject {

    Q_OBJECT

	private:
		// The image receiving from the camera
		cv::Mat SourceImage;
		cv::VideoCapture *Camera;
        bool connectStatus;


	public:
        explicit ImageProcessModule(QObject *parent = 0);
        explicit ImageProcessModule(const int DeviceIndex, QObject *parent = 0);
		~ImageProcessModule();
        bool getConnectStatus();

    signals:
        void sendFrame(cv::Mat Image);
        void connectStatusHasChanged(QString);

    public slots:
        void grabImage();
        void changeSetup(const int CameraIndex);

};




#endif
