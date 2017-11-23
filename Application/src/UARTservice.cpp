#include "include/UARTservice.h"

#include <QString>
#include <QMessageBox>
#include <QMainWindow>


UARTservice::UARTservice(QTextEdit *testText, QListWidget *ListOfSerialPorts,int InitBaudRate, QObject *parent) :
    QObject(parent),
    ListOfSerialPorts(ListOfSerialPorts),
    BaudRate(InitBaudRate),
    testText(testText)

{
    this->SelectedPort = new QSerialPort;

    connect(this->ListOfSerialPorts, SIGNAL(itemDoubleClicked(QListWidgetItem*))
            ,this, SLOT(newPortSelected(QListWidgetItem*)) );

    connect(this->SelectedPort, SIGNAL(readyRead()), this, SLOT(receiveMessage()));


}


UARTservice::~UARTservice()
{


}

void UARTservice::searchSerialPorts()
{

    // This item will be added to the list of serial ports
    QListWidgetItem *AddedItem;

    // Clear the list widget
    this->ListOfSerialPorts->clear();
    this->AvailablePorts.clear();
    // Search avaliable serial ports
    this->AvailablePorts = QSerialPortInfo::availablePorts();

    // Inform the user if there is no available serial port
    if(this->AvailablePorts.isEmpty()) {
        emit cantFindSerialPorts();
        return;
    }


    for(int i=0; i < this->AvailablePorts.size(); i++) {
        //qWarning() << this->AvailablePorts[i].description();
        //qWarning() << this->AvailablePorts[i].portName();

        // Concatenate two strings with information about the serial port
        QString DisplayedInformation = QString("%1 %2")
                                       .arg(this->AvailablePorts[i].systemLocation())
                                       .arg(this->AvailablePorts[i].description());

        // Create an item with the right one label
        AddedItem = new QListWidgetItem(DisplayedInformation);
        // Add the crated item to the list
        this->ListOfSerialPorts->addItem(AddedItem);

    }
}


void UARTservice::sendMessage()
{

    QString msg = this->testText->toPlainText();
//QString msg = "LD+ON=5000";
    QString test(50, ' ');
    qWarning() << test;

    QString test2("costam");
    test.replace(0, msg.size(), msg);
    qWarning() << "test-> " << test;
    if( !this->SelectedPort->isOpen() ) {
        qWarning() << "port not open";
        return;
    }

    this->SelectedPort->write(msg.toUtf8());

    qWarning() << "TEST send message";
}


QString UARTservice::receiveMessage()
{

    QByteArray receivedMessage = this->SelectedPort->readAll();
    QString tmp;

    tmp.fromUtf8(receivedMessage);

    qWarning() << "REVEIVED MESSAGE: " << tmp;

    this->SelectedPort->waitForReadyRead(5000);

}


void UARTservice::newPortSelected(QListWidgetItem* SelectedItem)
{

    QString ItemText = SelectedItem->text();

    int index = ItemText.indexOf(' ');

    if(index > 0) {
        QString ItemSystemLocation = ItemText.left(index);
        qWarning() << "ItemSystemLocation: " << ItemSystemLocation;

        for(int i=0; i < this->AvailablePorts.size(); i++) {
            if(this->AvailablePorts[i].systemLocation() == ItemSystemLocation) {

                this->SelectedSerialPortInfo = &(this->AvailablePorts[i]);
                if( this->SelectedPort->isOpen() )
                    this->SelectedPort->close();
                this->SelectedPort->setPort(*(this->SelectedSerialPortInfo));
                // configure baud rate, parity, stop bits etc.
                configureSerialPort();
                // if successfully open then inform about that
                if( this->SelectedPort->open(QSerialPort::ReadWrite) )
                    emit UARTconnected();

            }
        }
    }
}

void UARTservice::receiveAboutToClosePort()
{
    emit  UARTdisconnected();


}


int UARTservice::configureSerialPort()
{
    this->SelectedPort->setBaudRate(this->BaudRate);
    this->SelectedPort->setDataBits(QSerialPort::Data8);
    this->SelectedPort->setStopBits(QSerialPort::OneStop);
    this->SelectedPort->setParity(QSerialPort::NoParity);

}




