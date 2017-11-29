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
#include "include/UARTservice.h"
#include "include/motor.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString WindowName, QWidget *parent = 0);
    ~MainWindow();

public slots:
    /** @brief Show a list of connected cameras.
     *
    */
    void ShowConnectedCameras();
    /**
     * @brief Set a inital configuration.
     */
    void setInitConf();
    /**
     * @brief Display an image if a checkbox is selected.
     */
    void displayImages();
    void changeReflectsThreshold(int Value);
    /**
     * @brief Create windows where images will be displayed.
     */
    void setWindowsWithImages();
    void receive_cameraConnectionEstablished();
    void receive_lostCameraConnection();
    void selectAllCheckBoxes(int status);
    void changeSecondThresholdValue(int value);
    void cantFindSerialPorts();
    /**
     * @brief Notice user about error.
     */
    void informCannotConnectToCamera();

signals:
    void setDefaultIndex(QString InitValue);

private:
    Ui::MainWindow *ui;
    QThread *imageProcessingThread;
    ImageProcessWorker *ImPrWorker;
    QVector<QCheckBox*> ListOfCheckBoxes;
    UARTservice *UART;
    Motor *motor;

    void createImageTypesBox();
    void connectSignalsToSlots();
    void createImageProcessingThread();
    void createCameraParametersBox();
    void createImageProcessingVarablesBox();
    void createDeviceVariablesBox();
    void createMinRGBvaluesBox();
    void createMaxRGBvaluesBox();
    void saveParameters(QFile& File);
    void readParametersFromFile();

private slots:
    void UARTdisconnected();
    void UARTconnected();
    void closeEvent(QCloseEvent *event);
    void showChangedMinRGBvalues(int Value);
    void showChangedMaxRGBvalues(int Value);
    void changeInpaintRadius(int Value);



};
#endif // MAINWINDOW_H
