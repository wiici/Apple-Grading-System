#ifndef _UARTSERVICE_H_
#define _UARTSERVICE_H_

#include <QObject>
#include <QListWidget>
#include <QtSerialPort>

class UARTservice : public QObject {

    Q_OBJECT

public:
    UARTservice(QListWidget *ListOfSerialPorts, QObject *parent = 0);
    ~UARTservice();

public slots:
    void searchSerialPorts();

signals:


private:
    QListWidget *ListOfSerialPorts;
    QList<QSerialPortInfo> AvailablePorts;

};


#endif
