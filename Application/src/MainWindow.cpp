#include "include/MainWindow.h"
#include "include/ImageProcessWorker.h"
#include "ui_mainwindow.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <QLabel>

MainWindow::MainWindow(const QString WindowName ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMainWindow::setWindowTitle(WindowName);

    this->ImPrWorker = new ImageProcessWorker(ui->brightnessValue_Slider,
                                              ui->saturationValue_Slider,
                                              ui->contrastValue_Slider,
                                              parent);

    this->UART = new UARTservice(ui->textEdit, ui->listOfSerialPorts_listWidget, 115200 ,parent);
    this->motor = new Motor(ui->motor_ON_OFF_pushButton, ui->motorSpeed_Slider, parent);

    ui->ListOfCameras_ComboBox->addItem("Select the camera...");

    ShowConnectedCameras();

    this->ImPrWorker->setThresholdValue(127);
    ui->thresholdValue_Slider->setSliderPosition(127);

    this->createImageTypesBox();

    ui->ImageTypes_groupBox->setDisabled(true);
    ui->CameraParameters_groupBox->setDisabled(true);
    ui->ImageProcessingVariables_groupBox->setDisabled(true);
    ui->DeviceVariables_groupBox->setDisabled(true);

    this->connectSignalsToSlots();

    this->createImageProcessingThread();
    this->createCameraParametersBox();
    this->createImageProcessingVarablesBox();



}

MainWindow::~MainWindow()
{
    imageProcessingThread->quit();

    while( !imageProcessingThread->isFinished() )
        ;

    ListOfCheckBoxes.clear();
    ImPrWorker->~ImageProcessWorker();

    delete ui;
    delete imageProcessingThread;
}


//*************************************************
//
//              SLOTS
//
//

// show all available video capture devices in the ComboBox
void MainWindow::ShowConnectedCameras() {

    QFile File;

    // Each attached camera has own folder (e.g. video0 - the number depends on
    // the order in which camera has been connected to the system)
    QString Path = "/sys/class/video4linux/video";

    // Useful in read lines from file
    QTextStream FileStream(&File);

    // The list will be filled again so remove all items.
    ui->ListOfCameras_ComboBox->clear();
    ui->ListOfCameras_ComboBox->addItem("Select the camera...");

    // max 64 devices in the v4l interface
    for(unsigned int Counter = 0; Counter < 64; Counter++) {
        File.setFileName(Path + QString::number(Counter) + "/name");

        if( File.open(QIODevice::ReadOnly) ) {
            QString ItemText = QString("%1 %2").arg(Counter).arg(FileStream.readLine());
            // Add to the ComboBox name of device
            ui->ListOfCameras_ComboBox->addItem( ItemText );

            File.close();
        }
    }
    // Inform user if can't find any camera (in the combo box there is only
    // one item "Select the camera..."
    if(ui->ListOfCameras_ComboBox->count() == 1)
        QMessageBox::warning(this, "Warning", "Can't find any camera");
}

void MainWindow::displayImages()
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

void MainWindow::setInitConf()
{
    ShowConnectedCameras();

    ui->ImageTypes_groupBox->setDisabled(true);
    ui->CameraParameters_groupBox->setDisabled(true);
    ui->ImageProcessingVariables_groupBox->setDisabled(true);

    emit setDefaultIndex("Select camera");
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

void MainWindow::receive_cameraConnectionEstablished()
{
    // Create string with information about camera connection and camera's parameters
    QString CameraParameters;
    CameraParameters  = QString("Connected to the camera (%1x%2 %3FPS)")
            .arg(this->ImPrWorker->getCameraFrameWidth())
            .arg(this->ImPrWorker->getCameraFrameHeight())
            .arg(this->ImPrWorker->getCameraFrameRate());

    QMessageBox::information(this, "Info", CameraParameters);



    // Enable widgets working with camera
    ui->ImageTypes_groupBox->setEnabled(true);
    ui->CameraParameters_groupBox->setEnabled(true);
    ui->ImageProcessingVariables_groupBox->setEnabled(true);
}


void MainWindow::selectAllCheckBoxes(int status)
{
    qWarning() << "TEST dsad sd ads sd a";
    for(unsigned int i=0; i < this->ImPrWorker->WindowNameMap.size(); i++) {
        this->ListOfCheckBoxes[i]->setChecked(status);

        if(!status)
            cv::destroyWindow(this->ImPrWorker->WindowNameMap[i]);
    }

}

void MainWindow::receive_lostCameraConnection()
{
    this->setInitConf();

    QMessageBox::warning(this, "Error", "The conncetion to the camera has been lost");

}

void MainWindow::cantFindSerialPorts()
{
    QMessageBox::warning(this, "Warning", "Can't find any serial port");
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
        // if the button pressed then show/hide the image type
        connect(checkbox, SIGNAL(pressed()), this, SLOT(setWindowsWithImages()));
    }

    QCheckBox *checkbox = new QCheckBox("Select all");
    vbox->addWidget(checkbox);
    this->ListOfCheckBoxes.append(checkbox);
    connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(selectAllCheckBoxes(int)) );

    vbox->addStretch(1);
    ui->ImageTypes_groupBox->setLayout(vbox);
}


void MainWindow::createCameraParametersBox()
{
    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *BrightnessLabel = new QLabel("Brightness");
    vbox->addWidget(BrightnessLabel);
    vbox->addWidget(ui->brightnessValue_Slider);

    QLabel *SaturationLabel = new QLabel("Saturation");
    vbox->addWidget(SaturationLabel);
    vbox->addWidget(ui->saturationValue_Slider);

    QLabel *ContrastLabel = new QLabel("Contrast");
    vbox->addWidget(ContrastLabel);
    vbox->addWidget(ui->contrastValue_Slider);

    vbox->addStretch(1);
    ui->CameraParameters_groupBox->setLayout(vbox);
}

void MainWindow::createImageProcessingVarablesBox()
{
    QVBoxLayout *vbox = new QVBoxLayout;

    QLabel *ThresholdValue = new QLabel("Threshold value");
    vbox->addWidget(ThresholdValue);
    vbox->addWidget(ui->thresholdValue_Slider);

    vbox->addStretch(1);
    ui->ImageProcessingVariables_groupBox->setLayout(vbox);
}

void MainWindow::createDeviceVariablesBox()
{
    QVBoxLayout *vbox = new QVBoxLayout;
vbox->addWidget(ui->motor_ON_OFF_pushButton);

    QLabel *SpeedValue = new QLabel("Motor speed");
    vbox->addWidget(SpeedValue);
vbox->addWidget(ui->motorSpeed_Slider);



    vbox->addStretch(1);
    ui->ImageProcessingVariables_groupBox->setLayout(vbox);
}

void MainWindow::connectSignalsToSlots()
{

    connect(ui->ListOfCameras_ComboBox, SIGNAL(activated(QString)),
            this->ImPrWorker, SLOT(changeSetup(QString)));

    connect(ui->refreshList_Button, SIGNAL(pressed()),
            this, SLOT(ShowConnectedCameras()) );

    connect(this->ImPrWorker, SIGNAL(lostConnection()),
            this, SLOT(setInitConf()) );

    connect(this, SIGNAL(setDefaultIndex(QString)),
            this->ImPrWorker,SLOT(changeSetup(QString)));

    connect(this->ImPrWorker, SIGNAL(cantConnectToCamera()),
            this, SLOT(informCannotConnectToCamera()) );

    connect(this->ImPrWorker, SIGNAL(frameHasGrabbed()),
            this, SLOT(displayImages()));

    connect(ui->thresholdValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(changeThreshold(int)));

    connect(this->ImPrWorker, SIGNAL(connectionEstablished()),
            this, SLOT(receive_cameraConnectionEstablished()) );

    connect(this->ImPrWorker, SIGNAL(lostConnection()),
            this, SLOT(receive_lostCameraConnection()) );

    connect(ui->searchSerialPorts_pushButton, SIGNAL(pressed()),
            this->UART, SLOT(searchSerialPorts()) );

    connect(this->UART, SIGNAL(cantFindSerialPorts()),
            this, SLOT(cantFindSerialPorts()) );

    //connect(ui->searchSerialPorts_pushButton_2, SIGNAL(pressed()), this->UART, SLOT(sendMessage()) );

    connect(this->UART, SIGNAL(UARTconnected()),
            this->motor, SLOT(UARTconnected()));

    connect(this->UART, SIGNAL(UARTdisconnected()),
            this, SLOT(UARTdisconnected()) );

    connect(this->UART, SIGNAL(UARTconnected()), this, SLOT(UARTconnected()));

    connect(this->motor, SIGNAL(sendMessage(const QString*,const QString*,int)),
            this->UART, SLOT(sendMessage(const QString*,const QString*,int)));
}


void MainWindow::createImageProcessingThread()
{
    // Create new thread for image processing work
    // (streaming capture video in the background.
    imageProcessingThread = new QThread();

    QTimer *timer = new QTimer();
    timer->setInterval(ImageProcessWorker::GrabImageInterval);

    connect(timer, SIGNAL(timeout()), ImPrWorker, SLOT(grabImageFromCamera()) );
    // if the
    connect(this->ImPrWorker, SIGNAL(connectionEstablished()),timer, SLOT(start()));
    connect(this->ImPrWorker, SIGNAL(lostConnection()),timer, SLOT(stop()));

    // When the thread starts, the 'timer' will be informed and will start working too
    //connect(imageProcessingThread, SIGNAL(started()), timer, SLOT(start()));

    this->ImPrWorker->moveToThread(imageProcessingThread);
    timer->moveToThread(imageProcessingThread);

    imageProcessingThread->start();
}





void MainWindow::UARTdisconnected()
{
    ui->DeviceVariables_groupBox->setDisabled(true);
    QMessageBox::warning(this, "Error", "Serial port has been closed");;

}

void MainWindow::UARTconnected()
{
    ui->DeviceVariables_groupBox->setEnabled(true);
}
