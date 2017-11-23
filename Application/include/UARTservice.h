#ifndef _UARTSERVICE_H_
#define _UARTSERVICE_H_

#include <QObject>
#include <QListWidget>
#include <QtSerialPort>
#include <QTextEdit>



class UARTservice : public QObject {

    Q_OBJECT

public:
    UARTservice(QTextEdit *testText, QListWidget *ListOfSerialPorts, int InitBaudRate = 115200, QObject *parent = 0);
    ~UARTservice();

public slots:
    void searchSerialPorts();
    void sendMessage();
    QString receiveMessage();




signals:
    void cantFindSerialPorts();
    void UARTconnected();
    void UARTdisconnected();

private slots:
    void newPortSelected(QListWidgetItem*);
    void receiveAboutToClosePort();


private:
    QListWidget *ListOfSerialPorts;
    QTextEdit *testText;
    QList<QSerialPortInfo> AvailablePorts;
    QSerialPort *SelectedPort;
    QSerialPortInfo *SelectedSerialPortInfo;

    int BaudRate;

    int configureSerialPort();

};


#endif
