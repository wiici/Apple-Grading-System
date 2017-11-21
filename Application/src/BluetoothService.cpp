#include "include/BluetoothService.h"


BluetoothService::BluetoothService(QListWidget *ListWidget,QPushButton *ScanButton, QObject *parent) :
    QObject(parent),
    ListOfDevices(ListWidget),
    ScanButton(ScanButton)

{
    this->discoveryBTdevicesAgent = new QBluetoothDeviceDiscoveryAgent;
    this->Server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol,parent);
    this->LocalDevice = new QBluetoothLocalDevice(parent);
    this->Socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    this->LocalDevice->setHostMode(QBluetoothLocalDevice::HostDiscoverable);

    connect(this->discoveryBTdevicesAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo))
            ,this, SLOT(addDeviceToListWidget(QBluetoothDeviceInfo)) );
    connect(this->discoveryBTdevicesAgent, SIGNAL(finished())
            ,this, SLOT(scanStop()) );
    connect(this->ListOfDevices, SIGNAL(itemDoubleClicked(QListWidgetItem*))
            ,this, SLOT(connectToBTdevice(QListWidgetItem*)) );

    this->ListOfDevices->setContextMenuPolicy(Qt::CustomContextMenu);





}

BluetoothService::~BluetoothService()
{

}


int BluetoothService::startCommunication()
{
    this->Server = new QBluetoothServer(QBluetoothServiceInfo::RfcommProtocol, this);

    connect(this->Server, SIGNAL(newConnection()), this, SLOT(communicationEstablished()) );



    return 0;
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


void BluetoothService::sendMessage(QString* Message)
{

}

void BluetoothService::communicationEstablished()
{



}

void BluetoothService::readDataFromSocket()
{
    qWarning() << "TEST";
    while(this->Socket->canReadLine())
    {
        QByteArray Line = this->Socket->readLine();

        qWarning() << "Line: " << QString::fromUtf8(Line.constData(), Line.length());


    }

}

void BluetoothService::connectToBTdevice(QListWidgetItem* DeviceItem)
{

    for(int i=0; i < (this->discoveryBTdevicesAgent->discoveredDevices().size()); i++)
    {
        if( this->discoveryBTdevicesAgent->discoveredDevices()[i].name() == DeviceItem->text() ) {
            QBluetoothAddress DeviceAdressToPair = this->discoveryBTdevicesAgent->discoveredDevices()[i].address();
            qWarning() << "FOUND: " << DeviceAdressToPair.toString();
            //this->LocalDevice->pair(this->discoveryBTdevicesAgent->discoveredDevices()[i].address(), "1234");

            if(this->LocalDevice->pairingStatus(DeviceAdressToPair) == QBluetoothLocalDevice::Unpaired) {
                qWarning() << "Not paired. Will pair";
                this->LocalDevice->requestPairing(this->discoveryBTdevicesAgent->discoveredDevices()[i].address(), QBluetoothLocalDevice::Paired);
            }
            else
                qWarning() << "Device is already paired";

            static const QLatin1String serviceUUID("00001101-0000-1000-8000-00805f9b34fb");

            this->Socket->connectToService(DeviceAdressToPair, QBluetoothUuid(serviceUUID), QBluetoothSocket::ReadWrite);

            qWarning() << "connectToService";
            connect(this->Socket, SIGNAL(readyRead()), this, SLOT(readDataFromSocket()) );
        }
    }
}

void BluetoothService::displayMenu(const QPoint &Position)
{

}


void BluetoothService::setServiceInfo()
{
    this->ServiceInfo->setAttribute(QBluetoothServiceInfo::ServiceName, "HC-05 BT connection");
    this->ServiceInfo->setAttribute(QBluetoothServiceInfo::ServiceDescription, "Test description");
    this->ServiceInfo->setAttribute(QBluetoothServiceInfo::ServiceProvider, "Test provider" );

}

