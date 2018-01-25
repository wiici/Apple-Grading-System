/*
 * motor.c
 *
 *  Created on: Nov 23, 2017
 *      Author: root
 */

#include "motor.h"
#include "string.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_hal_tim.h"

void Motor_Init(Motor *mt, int InitSpeed)
{
	// the motor is off
	mt->isEnable = 0;
	// set init speed
	mt->speed = InitSpeed;
}

void MotorCommandHandler(TIM_HandleTypeDef *htim1, Motor *mt, char *arg, int value)
{
	// If the argument is "ON" string then enable or disable motor
	// (depends on value argument)
	if( !strcmp(arg, "ON") ) {
		// disable driver (low on the MOTOR_DRIVER_ON_OFF_Pin)
		if(value == 0) {
			mt->isEnable = 0;
			HAL_GPIO_WritePin(MOTOR_DRIVER_ON_OFF_GPIO_Port,
					          MOTOR_DRIVER_ON_OFF_Pin,
							  GPIO_PIN_RESET);
		}
		// enable driver (high on the MOTOR_DRIVER_ON_OFF_Pin)
		else if(value == 1) {
			mt->isEnable = 1;
			HAL_GPIO_WritePin(MOTOR_DRIVER_ON_OFF_GPIO_Port,
					          MOTOR_DRIVER_ON_OFF_Pin,
							  GPIO_PIN_SET);
		}
	}

	if( !strcmp(arg, "SP")) {
		if(value < 256 && value > 50) {
			mt->speed = value;
			__HAL_TIM_SET_COMPARE(htim1, TIM_CHANNEL_1, value);
		}
	}
}



