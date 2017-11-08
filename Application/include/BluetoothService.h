#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include <QObject>
#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothServer>
#include <QBluetoothServiceInfo>
#include <QBluetoothSocket>
#include <QListWidget>
#include <QPushButton>
#include <QBluetoothLocalDevice>

class BluetoothService : public QObject{

    Q_OBJECT

public:
    BluetoothService(QListWidget *ListWidget, QPushButton *ScanButton, QObject *parent = 0);
    ~BluetoothService();

public slots:
    void scanStart();
    void scanStop();
    void addDeviceToListWidget(QBluetoothDeviceInfo);
    void connectToBTdevice(QListWidgetItem*);

signals:
    void scanStopped();

private:
    QBluetoothDeviceDiscoveryAgent *discoveryBTdevicesAgent;
    QBluetoothAddress *BTdevice;
    QBluetoothLocalDevice *LocalDevice;
    QListWidget *ListOfDevices;
    QPushButton *ScanButton;

    QBluetoothServer *Server;
    QBluetoothServiceInfo *ServiceInfo;

};

#endif // BLUETOOTHSERVICE_H
