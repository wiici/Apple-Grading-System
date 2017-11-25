#ifndef _UARTSERVICE_H_
#define _UARTSERVICE_H_

#include <QObject>
#include <QListWidget>
#include <QtSerialPort>
#include <QTextEdit>



class UARTservice : public QObject {

    Q_OBJECT

public:
    const int NumberOfDigits = 5;
    const char CommandSign = '+';
    const char ValueSign = '=';

    UARTservice(QTextEdit *testText, QListWidget *ListOfSerialPorts, int InitBaudRate = 115200, QObject *parent = 0);
    ~UARTservice();



public slots:
    void searchSerialPorts();
    void sendMessage(const QString *Command, const QString *Argument, const int Value);
    QString receiveMessage();




signals:
    void cantFindSerialPorts();
    void UARTconnected();
    void UARTdisconnected();

private slots:
    void newPortSelected(QListWidgetItem*);
    void receiveAboutToClosePort();


private:
    QTextEdit *testText;
    QListWidget *ListOfSerialPorts;
    int BaudRate;
    QList<QSerialPortInfo> AvailablePorts;
    QSerialPort *SelectedPort;
    QSerialPortInfo *SelectedSerialPortInfo;


    void configureSerialPort();
    QString createMessage(const QString *Command, const QString *Argument, const int Value);

};


#endif
