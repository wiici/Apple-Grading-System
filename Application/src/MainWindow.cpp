#include "include/MainWindow.h"
#include "include/ImageProcessWorker.h"
#include "ui_mainwindow.h"




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

    this->ImPrWorker->setThresholdValue(127);
    ui->thresholdValue_Slider->setSliderPosition(127);

    createImageTypesBox();

    ui->groupBox->setDisabled(true);

}

MainWindow::~MainWindow()
{
    imageProcessingThread->quit();

    while( !imageProcessingThread->isFinished() )
        ;

    ListOfCheckBoxes.~QVector();
    ImPrWorker->~ImageProcessWorker();

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

    connect(this->ImPrWorker, SIGNAL(connectionEstablished()), this, SLOT(receive_connectionEstablished()) );

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
    for(unsigned int i=0; i < (this->ImPrWorker->WindowNameMap.size()); i++)
    {
        if(this->ListOfCheckBoxes[i]->isChecked())
        {
            cv::namedWindow(ImPrWorker->WindowNameMap[i], cv::WINDOW_AUTOSIZE);
            cv::imshow(ImPrWorker->WindowNameMap[i], *(this->ImPrWorker->Images[i]));
        }
    }


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

    ui->groupBox->setDisabled(true);

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

void MainWindow::setWindowsWithImages()
{
    for(unsigned int i=0; i < (this->ImPrWorker->WindowNameMap.size()); i++)
    {
        if( !(ListOfCheckBoxes[i]->isChecked()) )
            cv::destroyWindow(this->ImPrWorker->WindowNameMap[i]);
        else
            cv::namedWindow(this->ImPrWorker->WindowNameMap[i], cv::WINDOW_AUTOSIZE);
    }
}

void MainWindow::receive_connectionEstablished()
{

    ui->groupBox->setEnabled(true);

}


void MainWindow::selectAllCheckBoxes(int status)
{

        for(unsigned int i=0; i < this->ImPrWorker->WindowNameMap.size(); i++) {
            this->ListOfCheckBoxes[i]->setChecked(status);

            if(!status)
                cv::destroyWindow(this->ImPrWorker->WindowNameMap[i]);
        }

}


//*************************************************
//
//              OTHER FUNCTIONS
//
//

void MainWindow::createImageTypesBox()
{

    QVBoxLayout *vbox = new QVBoxLayout;

    for(unsigned int i=0; i < (this->ImPrWorker->WindowNameMap.size()); i++)
    {
        // Create QCheckBox widget
        QCheckBox *checkbox = new QCheckBox(this->ImPrWorker->WindowNameMap[i]);
        // Add the created widget to the QVBoxLayout
        vbox->addWidget(checkbox);
        // Remember the pointer to the QCheckBox object
        this->ListOfCheckBoxes.append(checkbox);
        //
        connect(checkbox, SIGNAL(pressed()), this, SLOT(setWindowsWithImages()));
    }

    QCheckBox *checkbox = new QCheckBox("Select all");
    vbox->addWidget(checkbox);
    this->ListOfCheckBoxes.append(checkbox);
    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(selectAllCheckBoxes(int)) );

    vbox->addStretch(1);
    ui->groupBox->setLayout(vbox);
}

