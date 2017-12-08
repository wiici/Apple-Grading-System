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

/** This class manages the entire application */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param Name of main window
     * @param parent, see QObject@details
     */
    explicit MainWindow(const QString WindowName, QWidget *parent = 0);
    ~MainWindow();

public slots:
    /** @brief Show a list of connected cameras.
     *
     *
     * Each attached camera has own folder (e.g. video0 - the number depends on
     * the order in which camera has been connected to the system). This function
     * checks these folders and if some folder exists it will find the name
     * of camera in this folder and display it.
     *
     *
    */
    void ShowConnectedCameras();
    /**
     * @brief Set a inital configuration for beginning.
     */
    void setInitConf();
    /**
     * @brief Check all QCheckBoxes and display an image if
     *        a checkbox is selected.
     */
    void displayImages();
    void changeReflectsThreshold(int Value);
    /**
     * @brief Create windows where images will be displayed.
     */
    void setWindowsWithImages();
    /**
     * @brief Inform user about details of connected camera (resolution and FPS)
     *        and enable all widgets needed in image processing operations.
     */
    void receive_cameraConnectionEstablished();
    /**
     * @brief Set initial configuration and inform the user about lost connection.
     */
    void receive_lostCameraConnection();
    /**
     * @brief Slot is called when the user wants to display all available images
     *        (or wants to close all available images)
     * @param status indicates that the images should be closed or opened.
     */
    void selectAllCheckBoxes(int status);

    /**
     * @brief Inform user there is no available serial port.
     */
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
    void changeSecondThresholdValue(int value);



};
#endif // MAINWINDOW_H
