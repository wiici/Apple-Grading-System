#include "include/MainWindow.h"
#include "ui_mainwindow.h"

#include <QString>
#include <QFile>
#include <QTextStream>


MainWindow::MainWindow(const QString WindowName ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QMainWindow::setWindowTitle(WindowName);

    ui->ListOfCameras_ComboBox->addItem("Select the camera...");
    ShowConnectedCameras(ui->ListOfCameras_ComboBox);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// show all available video capture devices in the ComboBox
void MainWindow::ShowConnectedCameras(QComboBox* ListOfCameras_ComboBox) {

    // Each attached camera has own folder (e.g. video0 - the number depends on
    // the order in which camera has been connected to the system)
    QString Path = "/sys/class/video4linux/video";
    unsigned int Counter = 0;

    // Create the oject and pass the entire path of 'name' file which
    // contains the product name of the camera.
    QFile File( Path + QString::number(Counter) + "/name");

    // The list will be filled again so remove all items.
    ListOfCameras_ComboBox->clear();
    ListOfCameras_ComboBox->addItem("Select the camera...");

    // Useful in read lines from file
    QTextStream FileStream(&File);

    while( File.open(QIODevice::ReadOnly) ) {

        // Add to the ComboBox name of device
        ListOfCameras_ComboBox->addItem( FileStream.readLine() );

        File.close();
        Counter++;
        File.setFileName(Path + QString::number(Counter) + "/name");
    }

}
