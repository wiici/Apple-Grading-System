#include "include/UARTservice.h"

#include <QString>
#include <QMessageBox>
#include <QMainWindow>


UARTservice::UARTservice(QTextEdit *testText, QListWidget *ListOfSerialPorts,int InitBaudRate, QObject *parent) :
    QObject(parent),
    testText(testText),
    ListOfSerialPorts(ListOfSerialPorts),
    BaudRate(InitBaudRate)
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


void UARTservice::sendMessage(const QString *Command, const QString *Argument, const int Value)
{
    // check if the serial port is open
    if( !this->SelectedPort->isOpen()) {
        qWarning() << "sendMessage->Serial port is not open";
        emit UARTdisconnected();
        return;
    }
    QString Message = createMessage(Command, Argument, Value);
    QString msg = this->testText->toPlainText();

    qWarning() << "UARTserivce::sendMessage()_Message:" << Message;

    //QString test(50, ' ');
    //qWarning() << test;

    //QString test2("costam");
    //test.replace(0, msg.size(), msg);
    //qWarning() << "sendMessage():" << test;

    this->SelectedPort->write(msg.toUtf8());

}


QString UARTservice::receiveMessage()
{

    QByteArray receivedMessage = this->SelectedPort->readAll();
    QString tmp;

    tmp.fromUtf8(receivedMessage);

    qWarning() << "REVEIVED MESSAGE: " << tmp;

    this->SelectedPort->waitForReadyRead(5000);

    return this->SelectedPort->readAll();

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
    if( this->SelectedPort->isOpen())
        this->SelectedPort->close();

    emit  UARTdisconnected();
}


void UARTservice::configureSerialPort()
{
    this->SelectedPort->setBaudRate(this->BaudRate);
    this->SelectedPort->setDataBits(QSerialPort::Data8);
    this->SelectedPort->setStopBits(QSerialPort::OneStop);
    this->SelectedPort->setParity(QSerialPort::NoParity);

}

QString UARTservice::createMessage(const QString *Command, const QString *Argument, const int Value)
{
    QString SpeedValue = QString("%1").arg(Value, this->NumberOfDigits, 10, QChar('0'));

    QString Message = *Command + this->CommandSign + *Argument + this->ValueSign + SpeedValue;

    return Message;
}



