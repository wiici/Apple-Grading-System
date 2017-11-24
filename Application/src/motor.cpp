#include "include/motor.h"
#include "include/UARTservice.h"

#include <QTimer>
#include <QPalette>
#include <QDebug>

Motor::Motor(QPushButton *Motor_ON_OFF_BUtton, QSlider *MotorSpeed, QObject *parent) :
    QObject(parent),
    Motor_ON_OFF_BUtton(Motor_ON_OFF_BUtton),
    MotorSpeed(MotorSpeed)
{
    // motor off at the beginnig
    this->isEnable = false;
    // disable widgets which need the connection to the serial port
    this->Motor_ON_OFF_BUtton->setDisabled(true);
    this->MotorSpeed->setDisabled(true);

    this->MotorSpeed->setRange(this->minSpeed, this->maxSpeed);
    this->MotorSpeed->setValue(150);

    // when pressed the button change the background and the flag (isEnable)
    connect(this->Motor_ON_OFF_BUtton, SIGNAL(pressed()), this, SLOT(ON_OFF_BUtton_Pressed()) );
    connect(this->MotorSpeed, SIGNAL(sliderReleased()), this, SLOT(setSpeed()));
}

Motor::~Motor()
{



}


void Motor::setSpeed()
{
    qWarning() << "Motor::setSpeed()_Speed:" << this->MotorSpeed->value();

    // emit the signal to the UARTserive which will create the message
    // and send it to the device via serial port
    emit sendMessage(&MotorCommand,
                     &SetSpeedArgument,
                     MotorSpeed->value());
}


void Motor::UARTconnected()
{
    // enable widgets which need the connection to the serial port
    this->Motor_ON_OFF_BUtton->setEnabled(true);
    this->MotorSpeed->setEnabled(true);

    // set property background colour
    if(this->isEnable)
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: green");
    else
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: red");
}





void Motor::ON_OFF_BUtton_Pressed()
{
    // Disable button for a moment
    this->Motor_ON_OFF_BUtton->blockSignals(true);

    // change the flags and set propertyl background colour
    if(this->isEnable) {
        this->isEnable = false;
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: red");
    } else {
        this->isEnable = true;
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: green");
    }

    qWarning() << "Motor::ON_OFF_BUtton_Pressed()_isEnabled:" << this->isEnable;

    // emit the signal to the UARTserive which will create the message
    // and send it to the device via serial port
    emit sendMessage(&MotorCommand,
                     &MotorOnOffArgument,
                     (int)isEnable);

    // enable the button after some time to avoid frequent switching
    QTimer::singleShot(MotorOnOffDelay, this, SLOT(enableMotorOnOffButton()));
}


void Motor::enableMotorOnOffButton()
{
    this->Motor_ON_OFF_BUtton->blockSignals(false);
}
