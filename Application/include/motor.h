#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QPushButton>
#include <QSlider>

class Motor : public QObject {

    Q_OBJECT

public:
    Motor(QPushButton *Motor_ON_OFF_BUtton, QSlider *MotorSpeed, QObject *parent = 0 );
    ~Motor();


public slots:
    void setSpeed();
    void UARTconnected();


signals:
    void sendMessage(const QString *Command, const QString *Argument, const int Value);

private slots:
    void ON_OFF_BUtton_Pressed();
    void enableMotorOnOffButton();

private:
    const int minSpeed = 50;
    const int maxSpeed = 256;
    const int MotorOnOffDelay = 2000; // us
    const QString MotorCommand = "MT";
    const QString SetSpeedArgument = "SP";
    const QString MotorOnOffArgument = "ON";
    // on/off flags
    bool isEnable;
    QPushButton *Motor_ON_OFF_BUtton;
    QSlider *MotorSpeed;

};


#endif // MOTOR_H
