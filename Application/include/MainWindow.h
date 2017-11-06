#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QVector>

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
    ImageProcessWorker *ImPrWorker;
    QVector<QCheckBox*> ListOfCheckBoxes;
    void createImageTypesBox();

signals:
    void setDefaultIndex(int Index);
public slots:
    void ShowConnectedCameras();
    void DisplaySourceImage(cv::Mat *Image);
    void receiveConnectStatusHasChanged(const bool connectStatus);
    void setInitConf();
    void informCannotConnectToCamera();
    void displayBinaryImage();
    void changeThreshold(int Value);
    void setWindowsWithImages();
    void receive_connectionEstablished();
    void selectAllCheckBoxes(int status);
};
#endif // MAINWINDOW_H
