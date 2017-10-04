#include "include/MainWindow.h"
#include "ui_mainwindow.h"
#include "include/ImageProcessModule.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QImage>
#include <QThread>
#include <QTimer>

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


    ui->ListOfCameras_ComboBox->addItem("Select the camera...");

    ShowConnectedCameras();

    ui->sourceImages_Label->setScaledContents(true);

    ui->connectStatus_Label->setStyleSheet("background-color: red");

    ui->thresholdValue_Slider->setSliderPosition(150);

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
    ImageProcessModule *IPworker = new ImageProcessModule();
    //
    QTimer *timer = new QTimer();
    timer->setInterval(1);

    // Connect the signals to the property slots.
    //
    connect(timer, SIGNAL(timeout()), IPworker, SLOT(grabImage()) );
    //
    connect(ui->ListOfCameras_ComboBox, SIGNAL(activated(int)), IPworker, SLOT(changeSetup(int)));
    //
    connect(IPworker, SIGNAL(connectStatusHasChanged(QString)), ui->connectStatus_Label, SLOT(setStyleSheet(QString)) );
    //
    connect(ui->refreshList_Button, SIGNAL(pressed()), this, SLOT(ShowConnectedCameras()) );
    //
    connect(IPworker, SIGNAL(sendFrame(cv::Mat*)), this, SLOT(DisplaySourceImage(cv::Mat*)));
    // When the thread starts, the 'timer' will be informed and will start working too
    connect(imageProcessingThread, SIGNAL(started()), timer, SLOT(start()));
    //
    connect(ui->thresholdValue_Slider, SIGNAL(valueChanged(int)), IPworker, SLOT(changeThresholdValue(int)));
    //
    connect(IPworker, SIGNAL(lostConnection()), this, SLOT(setInitConf()) );
    //
    connect(this, SIGNAL(setDefaultIndex(int)), IPworker, SLOT(changeSetup(int)));

    IPworker->moveToThread(imageProcessingThread);
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
    // Converting openCV image to Qt Image
    QImage *converted2qimage = new QImage((unsigned char*)Image->data, Image->rows, Image->cols, QImage::Format_Grayscale8);

    ui->sourceImages_Label->setPixmap(QPixmap::fromImage(*converted2qimage));

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

//*************************************************
//
//              OTHER FUNCTIONS
//
//



