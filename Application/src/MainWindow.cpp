#include "include/MainWindow.h"
#include "include/ImageProcessWorker.h"
#include "ui_mainwindow.h"


#include <QString>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QThread>
#include <QTimer>
#include <QMessageBox>
#include <QCheckBox>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

MainWindow::MainWindow(const QString WindowName ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMainWindow::setWindowTitle(WindowName);

    this->ImPrWorker = new ImageProcessWorker(parent);

    ui->ListOfCameras_ComboBox->addItem("Select the camera...");

    ShowConnectedCameras();

    ui->connectStatus_Label->setStyleSheet("background-color: red");

    ui->thresholdValue_Slider->setSliderPosition(5);
}

MainWindow::~MainWindow()
{
    imageProcessingThread->quit();

    while( !imageProcessingThread->isFinished() )
        ;

    delete ui;
    delete imageProcessingThread;
}

void MainWindow::Init()
{
    // Create new thread for image processing work
    // (streaming capture video in the background.
    imageProcessingThread = new QThread();

    QTimer *timer = new QTimer();
    timer->setInterval(40);

    //this->ImPrWorker->ImageProcessModule::setThresholdValue(249);

    // Connect the signals to the property slots.

    connect(timer, SIGNAL(timeout()), ImPrWorker, SLOT(grabImageFromCamera()) );

    connect(ui->ListOfCameras_ComboBox, SIGNAL(activated(int)), this->ImPrWorker, SLOT(changeSetup(int)));

    connect(this->ImPrWorker, SIGNAL(connectStatusHasChanged(QString)), ui->connectStatus_Label, SLOT(setStyleSheet(QString)) );

    connect(ui->refreshList_Button, SIGNAL(pressed()), this, SLOT(ShowConnectedCameras()) );

    connect(this->ImPrWorker, SIGNAL(sendFrame(cv::Mat*)), this, SLOT(DisplaySourceImage(cv::Mat*)));
    // When the thread starts, the 'timer' will be informed and will start working too
    connect(imageProcessingThread, SIGNAL(started()), timer, SLOT(start()));

    connect(this->ImPrWorker, SIGNAL(lostConnection()), this, SLOT(setInitConf()) );

    connect(this, SIGNAL(setDefaultIndex(int)), this->ImPrWorker, SLOT(changeSetup(int)));

    connect(this->ImPrWorker, SIGNAL(cantConnectToCamera()), this, SLOT(informCannotConnectToCamera()) );

    connect(this->ImPrWorker, SIGNAL(frameHasGrabbed()), this, SLOT(displayBinaryImage()));

    connect(ui->thresholdValue_Slider, SIGNAL(valueChanged(int)), this, SLOT(changeThreshold(int)));

    this->ImPrWorker->moveToThread(imageProcessingThread);
    timer->moveToThread(imageProcessingThread);

    imageProcessingThread->start();



}


//*************************************************
//
//              SLOTS
//
//

// show all available video capture devices in the ComboBox
void MainWindow::ShowConnectedCameras() {

    // Each attached camera has own folder (e.g. video0 - the number depends on
    // the order in which camera has been connected to the system)
    QString Path = "/sys/class/video4linux/video";
    unsigned int Counter = 0;

    // Create the oject and pass the entire path of 'name' file which
    // contains the product name of the camera.
    QFile File( Path + QString::number(Counter) + "/name");

    // The list will be filled again so remove all items.
    ui->ListOfCameras_ComboBox->clear();
    ui->ListOfCameras_ComboBox->addItem("Select the camera...");

    // Useful in read lines from file
    QTextStream FileStream(&File);

    while( File.open(QIODevice::ReadOnly) ) {

        // Add to the ComboBox name of device
        ui->ListOfCameras_ComboBox->addItem( FileStream.readLine() );

        File.close();
        Counter++;
        File.setFileName(Path + QString::number(Counter) + "/name");
    }

}


void MainWindow::DisplaySourceImage(cv::Mat *Image)
{

}

void MainWindow::displayBinaryImage()
{
    if(ui->displaySourceImage_checkBox->isChecked())
        cv::imshow("Source Image", *(this->ImPrWorker->getSourceImage()));

    if(ui->displayGrayscaleImage_checkBox->isChecked())
        cv::imshow("Grayscale Image", *(this->ImPrWorker->getGrayscaleImage()));

    if(ui->displayEdgesImage_checkBox->isChecked())
        cv::imshow("binary", *(this->ImPrWorker->getBinaryImage()));
}

void MainWindow::receiveConnectStatusHasChanged(const bool connectStatus)
{
    if( connectStatus )
        ui->connectStatus_Label->setStyleSheet("background-color: green");
}

void MainWindow::setInitConf()
{
    // Set red color of connection status to inform
    ui->connectStatus_Label->setStyleSheet("background-color: red");

    ShowConnectedCameras();

    emit setDefaultIndex(0);
}

void MainWindow::informCannotConnectToCamera()
{
    QMessageBox::warning(this, "Error", "Can't connect to the camera");
}


void MainWindow::changeThreshold(int Value)
{
    this->ImPrWorker->setThresholdValue(Value);
}

//*************************************************
//
//              OTHER FUNCTIONS
//
//



