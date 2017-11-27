#ifndef _UARTSERVICE_H_
#define _UARTSERVICE_H_

#include <QObject>
#include <QListWidget>
#include <QtSerialPort>
#include <QTextEdit>


class UARTservice : public QObject {

    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param testText
     * @param ListOfSerialPorts is a qt widget which stores all available serial ports.
     * @param InitBaudRate is a value od baud rate which is used in serial communication.
     *        Default value is set to 115200
     * @param parent . More details gere QObject@details
     *
     *
     */
    UARTservice(QTextEdit *testText, QListWidget *ListOfSerialPorts, int InitBaudRate = 115200, QObject *parent = 0);
    ~UARTservice();

public slots:
    /**
     * @brief It search availables serial ports and add it to the qt widget list.
     *
     * @details Search available serial ports using QSerialPort::availablePorts().
     *          Then to the qt list widget is added an item with information about
     *          serial port system location and description.
     */
    void searchSerialPorts();
    /**
     * @brief Concatenate strings and send a created message via serial port.
     *
     * @details Concatenate strings based on three thing: command, command's argument and
     *          value. These things are separeted by the two signs: '+' and '='.
     *          Example of the created message look like that "MT+ON=00001".
     *          A purpose of this operation is to create
     *
     * @param Command is
     * @param Argument
     * @param Value
     */
    void sendMessage(const QString *Command, const QString *Argument, const int Value);
    QString receiveMessage();

signals:
    /**
     * @brief The signal is emitted when there is no available serial port.
     */
    void cantFindSerialPorts();
    /**
     * @brief The signal is emitted when an attempt to open a serial port is successful.
     */
    void UARTconnected();
    /**
     * @brief The signal is emitted when the connection with the serial port has been lost.
     */
    void UARTdisconnected();

private slots:
    /**
     * @brief Try to open a serial port selected by the user.
     *
     * @param SelectedItem is the item from widget list (QListWidget) which was
     *        double clicked by the user.
     */
    void newPortSelected(QListWidgetItem* SelectedItem);
    /**
     * @brief receiveAboutToClosePort
     */
    void receiveAboutToClosePort();

private:
    QTextEdit *testText;
    QListWidget *ListOfSerialPorts;
    int BaudRate;
    QList<QSerialPortInfo> AvailablePorts;
    QSerialPort *SelectedPort;
    QSerialPortInfo *SelectedSerialPortInfo;

    // variables for message
    const int NumberOfDigits = 5;
    const char CommandSign = '+';
    const char ValueSign = '=';

    // set baud rate, data bits, stop bits, parity option
    void configureSerialPort();
    // concatenate string in one message
    QString createMessage(const QString *Command, const QString *Argument, const int Value);

};


#endif
