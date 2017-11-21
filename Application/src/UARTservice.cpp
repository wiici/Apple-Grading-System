#include "include/UARTservice.h"


UARTservice::UARTservice(QListWidget *ListOfSerialPorts, QObject *parent    ) :
    QObject(parent),
    ListOfSerialPorts(ListOfSerialPorts)
{


}


UARTservice::~UARTservice()
{


}

void UARTservice::searchSerialPorts()
{
    // Search avaliable serial ports
    this->AvailablePorts = QSerialPortInfo::availablePorts();
    // This item will be added to the list of serial ports
    QListWidgetItem *AddedItem;

    for(int i=0; i < this->AvailablePorts.size(); i++) {
        //qWarning() << this->AvailablePorts[i].description();
        //qWarning() << this->AvailablePorts[i].portName();

        // Concatenate two strings with information about the serial port
        QString DisplayedInformation = QString("%1 %2")
                                       .arg(this->AvailablePorts[i].description())
                                       .arg(this->AvailablePorts[i].systemLocation());

        // Create an item with the right one label
        AddedItem = new QListWidgetItem(DisplayedInformation);
        // Add the crated item to the list
        this->ListOfSerialPorts->addItem(AddedItem);

        // Free memory
        delete AddedItem;
    }
}
