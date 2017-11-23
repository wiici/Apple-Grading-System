#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QPushButton>
#include <QSlider>

class Motor : public QObject {

    Q_OBJECT

public:
    Motor(QPushButton *Motor_ON_OFF_BUtton, QObject *parent = 0 );
    ~Motor();


public slots:
    void setSpeed();
    void UARTconnected();


signals:
    void sendMessage(QString Message);

private slots:
    void ON_OFF_BUtton_Pressed();

private:
    // on/off flags
    bool isEnable;
    QPushButton *Motor_ON_OFF_BUtton;

};


#endif // MOTOR_H
