#ifndef BLUETOOTHSERVICE_H
#define BLUETOOTHSERVICE_H

#include <QObject>
#include <QBluetoothAddress>

class BluetoothService : public QObject{

    Q_OBJECT

public:
    BluetoothService(QObject *parent = 0);
    ~BluetoothService();

private:
    QBluetoothAddress *BTadress;
    
public slots:



};

#endif // BLUETOOTHSERVICE_H
