#include "include/motor.h"
#include <unistd.h>
#include <QTimer>
#include <QPalette>

Motor::Motor(QPushButton *Motor_ON_OFF_BUtton, QObject *parent) :
    QObject(parent),
    Motor_ON_OFF_BUtton(Motor_ON_OFF_BUtton)
{
    // motor off
    this->isEnable = false;
    this->Motor_ON_OFF_BUtton->setDisabled(true);
    connect(this->Motor_ON_OFF_BUtton, SIGNAL(pressed()), this, SLOT(ON_OFF_BUtton_Pressed()) );

}

Motor::~Motor()
{



}


void Motor::setSpeed()
{





}


void Motor::UARTconnected()
{
    this->Motor_ON_OFF_BUtton->setEnabled(true);

    if(this->isEnable)
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: green");
    else
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: red");

}





void Motor::ON_OFF_BUtton_Pressed()
{
    this->Motor_ON_OFF_BUtton->setDisabled(true);

    if(this->isEnable) {
        this->isEnable = false;
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: red");
    } else {
        this->isEnable = true;
        this->Motor_ON_OFF_BUtton->setStyleSheet("background: green");
    }
    usleep(1000);
    this->Motor_ON_OFF_BUtton->setEnabled(true);
}
