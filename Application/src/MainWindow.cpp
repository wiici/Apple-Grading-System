#include "include/MainWindow.h"
#include "include/ImageProcessWorker.h"
#include "ui_mainwindow.h"

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <QLabel>
#include <QCloseEvent>

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

    this->ImPrWorker->setReflectsThresholdValue(200);
    ui->reflectsThresholdValue_Slider->setSliderPosition(200);
    ui->secondThresholdValue_Slider->setSliderPosition(this->ImPrWorker->getSecondThresholdValue());

    // dont show test widgets
    ui->textEdit->setVisible(false);
    ui->searchSerialPorts_pushButton_2->setVisible(false);
    ui->secondThresholdValue_Slider->setVisible(false);

    // disable some widgets at the beginning
    ui->ImageTypes_groupBox->setDisabled(true);
    ui->CameraParameters_groupBox->setDisabled(true);
    ui->ImageProcessingVariables_groupBox->setDisabled(true);
    ui->DeviceVariables_groupBox->setDisabled(true);
    ui->minRGBvalues_groupBox->setDisabled(true);
    ui->maxRGBvalues_groupBox->setDisabled(true);

    this->connectSignalsToSlots();

    this->readParametersFromFile();

    this->createImageProcessingThread();

    // add items to the proper group box
    this->createImageTypesBox();
    this->createCameraParametersBox();
    this->createImageProcessingVarablesBox();
    this->createMinRGBvaluesBox();
    this->createMaxRGBvaluesBox();
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




//*********************************************************************************
//
//              PUBLIC SLOTS
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



void MainWindow::setInitConf()
{
    ShowConnectedCameras();

    ui->ImageTypes_groupBox->setDisabled(true);
    ui->CameraParameters_groupBox->setDisabled(true);
    ui->ImageProcessingVariables_groupBox->setDisabled(true);
    ui->minRGBvalues_groupBox->setDisabled(true);
    ui->maxRGBvalues_groupBox->setDisabled(true);

    emit setDefaultIndex("Select camera");
}



void MainWindow::displayImages()
{
    for(unsigned int i=0; i < (this->ImPrWorker->WindowNameMap.size()); i++)
    {
        // Check if an image should be displayed and check if the image has some data
        if(this->ListOfCheckBoxes[i]->isChecked() && !(this->ImPrWorker->Images[i]->empty()) )
        {
            cv::namedWindow(ImPrWorker->WindowNameMap[i], cv::WINDOW_AUTOSIZE);
            cv::imshow(ImPrWorker->WindowNameMap[i], *(this->ImPrWorker->Images[i]));
        }
    }
}



void MainWindow::changeReflectsThreshold(int Value)
{
    this->ImPrWorker->setReflectsThresholdValue(Value);
    ui->reflectsThresholdValue_lineEdit->setText(QString::number(Value));
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
    ui->minRGBvalues_groupBox->setEnabled(true);
    ui->maxRGBvalues_groupBox->setEnabled(true);
}



void MainWindow::receive_lostCameraConnection()
{
    this->setInitConf();

    QMessageBox::warning(this, "Error", "The conncetion to the camera has been lost");
}


// do when user will check the "Select all" check box
void MainWindow::selectAllCheckBoxes(int status)
{
    for(unsigned int i=0; i < this->ImPrWorker->WindowNameMap.size(); i++) {
        this->ListOfCheckBoxes[i]->setChecked(status);

        if(!status)
            cv::destroyWindow(this->ImPrWorker->WindowNameMap[i]);
    }

}



void MainWindow::changeSecondThresholdValue(int value)
{
    this->ImPrWorker->setSecondThresholdValue(value);
}



void MainWindow::cantFindSerialPorts()
{
    QMessageBox::warning(this, "Warning", "Can't find any serial port");
}



void MainWindow::informCannotConnectToCamera()
{
    QMessageBox::warning(this, "Error", "Can't connect to the camera");
}





//*************************************************************************8
//
//              PRIVATE FUNCTIONS
//
//
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

    int ReadValue;

    QLabel *ThresholdValue = new QLabel("Threshold value for light reflects");
    vbox->addWidget(ThresholdValue);
    ReadValue = this->ImPrWorker->getReflectsThresholdValue();
    vbox->addWidget(ui->reflectsThresholdValue_lineEdit);
    ui->reflectsThresholdValue_lineEdit->setText(QString::number(ReadValue));
    vbox->addWidget(ui->reflectsThresholdValue_Slider);
    ui->reflectsThresholdValue_Slider->setValue(ReadValue);

    QLabel *InapintRadius = new QLabel("Inapint radius");
    vbox->addWidget(InapintRadius);
    ReadValue = this->ImPrWorker->getInpaintRadius();
    vbox->addWidget(ui->inapintRadius_lineEdit);
    ui->inapintRadius_lineEdit->setText(QString::number(ReadValue));
    vbox->addWidget(ui->inpaintRadius_Slider);
    ui->inpaintRadius_Slider->setValue(ReadValue);

    vbox->addStretch(1);
    ui->ImageProcessingVariables_groupBox->setLayout(vbox);

    connect(ui->reflectsThresholdValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(changeReflectsThreshold(int)));

    connect(ui->inpaintRadius_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(changeInpaintRadius(int)));
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


void MainWindow::createMinRGBvaluesBox()
{
    QVBoxLayout *vbox = new QVBoxLayout;
    int ReadValue;

    QLabel *R_value = new QLabel("R value");
    vbox->addWidget(R_value);
    vbox->addWidget(ui->RminValue_lineEdit);
    vbox->addWidget(ui->RminValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Red);
    ui->RminValue_Slider->setSliderPosition(ReadValue);
    ui->RminValue_lineEdit->setText(QString::number(ReadValue));

    QLabel *G_value = new QLabel("G value");
    vbox->addWidget(G_value);
    vbox->addWidget(ui->GminValue_lineEdit);
    vbox->addWidget(ui->GminValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Green);
    ui->GminValue_Slider->setSliderPosition(ReadValue);
    ui->GminValue_lineEdit->setText(QString::number(ReadValue));

    QLabel *B_value = new QLabel("B value");
    vbox->addWidget(B_value);
    vbox->addWidget(ui->BminValue_lineEdit);
    vbox->addWidget(ui->BminValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Green);
    ui->BminValue_Slider->setSliderPosition(ReadValue);
    ui->BminValue_lineEdit->setText(QString::number(ReadValue));

    vbox->addStretch(1);
    ui->minRGBvalues_groupBox->setLayout(vbox);

    connect(ui->RminValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMinRGBvalue(int)) );

    connect(ui->GminValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMinRGBvalue(int)) );

    connect(ui->BminValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMinRGBvalue(int)) );

    // connect sliders to edit lines
    connect(ui->RminValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMinRGBvalues(int)));

    connect(ui->GminValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMinRGBvalues(int)));

    connect(ui->BminValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMinRGBvalues(int)));
}


void MainWindow::createMaxRGBvaluesBox()
{
    QVBoxLayout *vbox = new QVBoxLayout;
    int ReadValue;

    QLabel *R_value = new QLabel("R value");
    vbox->addWidget(R_value);
    vbox->addWidget(ui->RmaxValue_lineEdit);
    vbox->addWidget(ui->RmaxValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Red);
    ui->RmaxValue_Slider->setSliderPosition(ReadValue);
    ui->RmaxValue_lineEdit->setText(QString::number(ReadValue));

    QLabel *G_value = new QLabel("G value");
    vbox->addWidget(G_value);
    vbox->addWidget(ui->GmaxValue_lineEdit);
    vbox->addWidget(ui->GmaxValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Green);
    ui->GmaxValue_Slider->setSliderPosition(ReadValue);
    ui->GmaxValue_lineEdit->setText(QString::number(ReadValue));

    QLabel *B_value = new QLabel("B value");
    vbox->addWidget(B_value);
    vbox->addWidget(ui->BmaxValue_lineEdit);
    vbox->addWidget(ui->BmaxValue_Slider);
    // set values in added widgets
    ReadValue = this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Blue);
    ui->BmaxValue_Slider->setSliderPosition(ReadValue);
    ui->BmaxValue_lineEdit->setText(QString::number(ReadValue));

    vbox->addStretch(1);
    ui->maxRGBvalues_groupBox->setLayout(vbox);

    connect(ui->RmaxValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMaxRGBvalue(int)) );

    connect(ui->GmaxValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMaxRGBvalue(int)) );

    connect(ui->BmaxValue_Slider, SIGNAL(valueChanged(int)),
            this->ImPrWorker, SLOT(changeMaxRGBvalue(int)) );

    // connect sliders to edit lines
    connect(ui->RmaxValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMaxRGBvalues(int)));
    connect(ui->GmaxValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMaxRGBvalues(int)));
    connect(ui->BmaxValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(showChangedMaxRGBvalues(int)));
}


void MainWindow::saveParameters(QFile& File)
{
    QTextStream ostream(&File);

    // save min RGB values
    ostream << this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Red) << " "
            << this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Green) << " "
            << this->ImPrWorker->getMinRGBvalue(this->ImPrWorker->Blue) << endl;

    //save max RGB values
    ostream << this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Red) << " "
            << this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Green) << " "
            << this->ImPrWorker->getMaxRGBvalue(this->ImPrWorker->Blue) << endl;

    //save camera parameters
    ostream << this->ImPrWorker->getCameraBrightness() << " "
            << this->ImPrWorker->getCameraSaturation() << " "
            << this->ImPrWorker->getCameraContrast() << endl;

    //save parameters for image processing algorithm
    ostream << this->ImPrWorker->getReflectsThresholdValue() << endl;

    // save motor speed value
    ostream << this->motor->getSpeedValue() << endl;
}

void MainWindow::readParametersFromFile()
{
    QFile file("SavedParameters.txt");
    file.open(QFile::ReadOnly);

    if( !file.isOpen() ) {
        qWarning() << "MainWindow::readParametersFromFile: Cant open the file";
        return;
    }

    QTextStream istream(&file);

    int value;
    double CameraParameters;

    // read the min RGB values
    istream >> value;
    this->ImPrWorker->set_minRGBvalue(this->ImPrWorker->Red, value);
    istream >> value;
    this->ImPrWorker->set_minRGBvalue(this->ImPrWorker->Green, value);
    istream >> value;
    this->ImPrWorker->set_minRGBvalue(this->ImPrWorker->Blue, value);

    // read the max RGB values
    istream >> value;
    this->ImPrWorker->set_maxRGBvalue(this->ImPrWorker->Red, value);
    istream >> value;
    this->ImPrWorker->set_maxRGBvalue(this->ImPrWorker->Green, value);
    istream >> value;
    this->ImPrWorker->set_maxRGBvalue(this->ImPrWorker->Blue, value);

    // read the camera parameters
    istream >> CameraParameters;
    this->ImPrWorker->setCameraBrightness(CameraParameters);
    istream >> CameraParameters;
    this->ImPrWorker->setCameraSaturation(CameraParameters);
    istream >> CameraParameters;
    this->ImPrWorker->setCameraContrast(CameraParameters);

    // read parameters for image processing algorithms
    istream >> value;
    this->ImPrWorker->setReflectsThresholdValue(value);

    // read motor parameters
    istream >> value;
    this->motor->setInitSpeed(value);
}



void MainWindow::connectSignalsToSlots()
{

    connect(ui->ListOfCameras_ComboBox, SIGNAL(activated(QString)),
            this->ImPrWorker, SLOT(changeSetup(QString)));

    connect(ui->refreshList_Button, SIGNAL(pressed()),
            this, SLOT(ShowConnectedCameras()) );

    connect(ui->secondThresholdValue_Slider, SIGNAL(valueChanged(int)),
            this, SLOT(changeSecondThresholdValue(int)));

    connect(ui->searchSerialPorts_pushButton, SIGNAL(pressed()),
            this->UART, SLOT(searchSerialPorts()) );


    // CONNECT ImageProcessWorker SIGNALS
    connect(this->ImPrWorker, SIGNAL(lostConnection()),
            this, SLOT(setInitConf()) );

    connect(this, SIGNAL(setDefaultIndex(QString)),
            this->ImPrWorker,SLOT(changeSetup(QString)));

    connect(this->ImPrWorker, SIGNAL(cantConnectToCamera()),
            this, SLOT(informCannotConnectToCamera()) );

    connect(this->ImPrWorker, SIGNAL(frameHasGrabbed()),
            this, SLOT(displayImages()));

    connect(this->ImPrWorker, SIGNAL(connectionEstablished()),
            this, SLOT(receive_cameraConnectionEstablished()) );

    connect(this->ImPrWorker, SIGNAL(lostConnection()),
            this, SLOT(receive_lostCameraConnection()) );



    //  CONNECT UART SIGNALS
    connect(this->UART, SIGNAL(cantFindSerialPorts()),
            this, SLOT(cantFindSerialPorts()) );

    connect(this->UART, SIGNAL(UARTconnected()),
            this->motor, SLOT(UARTconnected()));

    connect(this->UART, SIGNAL(UARTdisconnected()),
            this, SLOT(UARTdisconnected()) );

    connect(this->UART, SIGNAL(UARTconnected()), this, SLOT(UARTconnected()));


    // CONNECT MOTOR SIGNALS
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




//*************************************************************************8
//
//              PRIVATE SLOTS
//
//

void MainWindow::UARTdisconnected()
{
    ui->DeviceVariables_groupBox->setDisabled(true);
    QMessageBox::warning(this, "Error", "Serial port has been closed");;

}



void MainWindow::UARTconnected()
{
    ui->DeviceVariables_groupBox->setEnabled(true);
}


// ask the user to save parameters
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox saveOrNot;
    saveOrNot.setText("Do you want to save the parameters?");
    saveOrNot.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int UserReply = saveOrNot.exec();

    if(UserReply == QMessageBox::No) {
        event->accept();
        return;
    }

    QFile file("SavedParameters.txt");

    if( !file.open(QFile::ReadWrite) ) {
        qWarning() << "MainWindow::closeEvent_cantOpenFile";
        event->accept();
        return;
    }
    qWarning() << "Save parameters to the file";
    this->saveParameters(file);
    event->accept();
}



void MainWindow::showChangedMinRGBvalues(int Value)
{
    QString SenderName = this->sender()->objectName();

    // Sliders which change these values should have
    // 'R', 'G' or 'B' sign on the first place in the string
    // e.g. RminValue_Slider , GminValue_Slider, BminValue_Slider
    QChar Colour = SenderName[0];

    // change QChar to char and check the value
    switch( Colour.unicode() ) {
    case 'R':
        ui->RminValue_lineEdit->setText(QString::number(Value));
        break;
    case 'G':
        ui->GminValue_lineEdit->setText(QString::number(Value));
        break;
    case 'B':
        ui->BminValue_lineEdit->setText(QString::number(Value));
        break;
    }
    //qWarning() <<  Value;
}



void MainWindow::showChangedMaxRGBvalues(int Value)
{
    QString SenderName = this->sender()->objectName();

    // Sliders which change these values should have
    // 'R', 'G' or 'B' sign on the first place in the string
    // e.g. RminValue_Slider , GminValue_Slider, BminValue_Slider
    QChar Colour = SenderName[0];

    // change QChar to char and check the value
    switch( Colour.unicode() ) {
    case 'R':
        ui->RmaxValue_lineEdit->setText(QString::number(Value));
        break;
    case 'G':
        ui->GmaxValue_lineEdit->setText(QString::number(Value));
        break;
    case 'B':
        ui->BmaxValue_lineEdit->setText(QString::number(Value));
        break;
    }
}


// change image processing parameter
void MainWindow::changeInpaintRadius(int Value)
{
    this->ImPrWorker->setInpaintRadius(Value);
    ui->inapintRadius_lineEdit->setText(QString::number(Value));
}
