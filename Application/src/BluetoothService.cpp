#include "include/BluetoothService.h"


BluetoothService::BluetoothService(QListWidget *ListWidget,QPushButton *ScanButton, QObject *parent) :
    QObject(parent),
    ListOfDevices(ListWidget),
    ScanButton(ScanButton)

{
    this->discoveryBTdevicesAgent = new QBluetoothDeviceDiscoveryAgent;
    this->Server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol,parent);
    this->LocalDevice = new QBluetoothLocalDevice(parent);

    connect(this->discoveryBTdevicesAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo))
            ,this, SLOT(addDeviceToListWidget(QBluetoothDeviceInfo)) );
    connect(this->discoveryBTdevicesAgent, SIGNAL(finished())
            ,this, SLOT(scanStop()) );
    connect(this->ListOfDevices, SIGNAL(itemDoubleClicked(QListWidgetItem*))
            ,this, SLOT(connectToBTdevice(QListWidgetItem*)) );


}

BluetoothService::~BluetoothService()
{

}



void BluetoothService::scanStart()
{
    this->ListOfDevices->clear();
    this->discoveryBTdevicesAgent->start();
    this->ScanButton->setDisabled(true);
}

void BluetoothService::scanStop()
{
    this->ScanButton->setEnabled(true);

}

void BluetoothService::addDeviceToListWidget(QBluetoothDeviceInfo Device)
{

    QListWidgetItem *NewItem = new QListWidgetItem(Device.name());

    this->ListOfDevices->addItem(NewItem);

}


void BluetoothService::connectToBTdevice(QListWidgetItem* DeviceItem)
{

    for(int i=0; i < (this->discoveryBTdevicesAgent->discoveredDevices().size()); i++)
    {
        if( this->discoveryBTdevicesAgent->discoveredDevices()[i].name() == DeviceItem->text() ) {
            qDebug() << "FOUND: " << this->discoveryBTdevicesAgent->discoveredDevices()[i].address().toString();
            //this->LocalDevice->pair(this->discoveryBTdevicesAgent->discoveredDevices()[i].address(), "1234");
        }
    }
}
