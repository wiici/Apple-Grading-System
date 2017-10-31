#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QComboBox>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "include/ImageProcessWorker.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString WindowName, QWidget *parent = 0);
    ~MainWindow();
    void Init();

private:
    Ui::MainWindow *ui;
    QThread *imageProcessingThread;
    QSlider costam();
    ImageProcessWorker *ImPrWorker;
signals:
    void setDefaultIndex(int Index);

    // show all available video capture devices in the ComboBox
public slots:
    void ShowConnectedCameras();
    void DisplaySourceImage(cv::Mat *Image);
    void receiveConnectStatusHasChanged(const bool connectStatus);
    void setInitConf();
    void informCannotConnectToCamera();
    void displayBinaryImage();
    void changeThreshold(int Value);
};

#endif // MAINWINDOW_H
