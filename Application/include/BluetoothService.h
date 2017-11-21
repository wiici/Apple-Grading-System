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
#include <QMenu>
#include <QLatin1String>

class BluetoothService : public QObject{

    Q_OBJECT

public:
    BluetoothService(QListWidget *ListWidget, QPushButton *ScanButton, QObject *parent = 0);
    ~BluetoothService();

    int startCommunication();
    void stopCommunication();

public slots:
    void scanStart();
    void scanStop();
    void addDeviceToListWidget(QBluetoothDeviceInfo);
    void connectToBTdevice(QListWidgetItem*);
    void displayMenu(const QPoint &Position);

    void sendMessage(QString* Message);
    void communicationEstablished();
    void readDataFromSocket();

signals:
    void scanStopped();

    void receivedMessage(QString *Message);


private:
    QBluetoothDeviceDiscoveryAgent *discoveryBTdevicesAgent;
    QBluetoothAddress *BTdevice;
    QBluetoothLocalDevice *LocalDevice;
    QListWidget *ListOfDevices;
    QPushButton *ScanButton;

    QBluetoothSocket *Socket;
    QBluetoothServer *Server;
    QBluetoothServiceInfo *ServiceInfo;

    static QLatin1String Uuid;

    void setServiceInfo();

};

#endif // BLUETOOTHSERVICE_H
