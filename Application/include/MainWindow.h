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
#include "include/BluetoothService.h"
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


private:
    Ui::MainWindow *ui;
    QThread *imageProcessingThread;
    ImageProcessWorker *ImPrWorker;
    QVector<QCheckBox*> ListOfCheckBoxes;
    BluetoothService *BTservice;
    UARTservice *UART;
    Motor *motor;

    void createImageTypesBox();
    void connectSignalsToSlots();
    void createImageProcessingThread();
    void createCameraParametersBox();
    void createImageProcessingVarablesBox();
    void createDeviceVariablesBox();

signals:
    void setDefaultIndex(QString InitValue);
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
     * @brief Notice user about error.
     */
    void informCannotConnectToCamera();
    /**
     * @brief Display an image if a checkbox is selected.
     */
    void displayImages();
    void changeThreshold(int Value);
    /**
     * @brief Create windows where images will be displayed.
     */
    void setWindowsWithImages();
    void receive_cameraConnectionEstablished();
    void receive_lostCameraConnection();
    void selectAllCheckBoxes(int status);
    void cantFindSerialPorts();

private slots:
    void UARTdisconnected();
    void UARTconnected();


};
#endif // MAINWINDOW_H
