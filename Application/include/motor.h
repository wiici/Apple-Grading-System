#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QPushButton>
#include <QSlider>

/** This class include all widgets connect with the motor and
 *   implement all needed slots and signals. */
class Motor : public QObject {

    Q_OBJECT

public:
    Motor(QPushButton *Motor_ON_OFF_BUtton, QSlider *MotorSpeed, QObject *parent = 0 );
    ~Motor();
    /**
     * @brief Get the speed value from MotorSpeed slider
     *
     */
    int getSpeedValue();
    /**
     * @brief Get a private class member that indicate if a moto is enable or not.
     * @return
     */
    bool get_isEnable();

    /**
     * @brief Set inital speed read from file.
     *
     */
    void setInitSpeed(int value);

public slots:
    /**
     * @brief Read the speed value from MotorSpeed slider and send
     *        properly arguments to UARTservice object, which send
     *        the message via UART.
     */
    void setSpeed();
    /**
     * @brief Enable widgets which working with Motor.
     */
    void UARTconnected();



signals:
    /**
     * @brief Signal is emitted when the user want to change motor speeed
     *        or enable/disable this motor.
     * @param Command is a string which indicate that the message applies
     *        to the motor
     * @param Argument indicates what should happen with the motor
     *        (change speed or disable the motor). There are two arguments
     *        for motor ("MT") command: "SP" (set speed) or "ON" (enable/disable motor)
     * @param Value means speed for "speed" argument or on/off value for
     *        second argument
     *
     */
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
    QSlider *MotorSpeedSlider;

};


#endif // MOTOR_H
