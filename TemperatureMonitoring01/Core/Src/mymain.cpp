/*
 * mymain.cpp
 *
 *  Created on: Nov 17, 2022
 *      Author: student
 */

#include <stdio.h>
#include "main.h"
#include "mymain.h"
#include "cmsis_os.h"
#include "MainTimer.h"
#include "CommandTask.h"
#include "CliContainer.h"
#include "CliCommand.h"
#include "Manager.h"
#include "DHT.h"

extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;


MainTimer mainTimer;

Manager *m = nullptr;

void mymain()
{
	m = new Manager(&huart2,&hi2c1);
}

void myPeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim6) {
		mainTimer.callTimerFunc();
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
	if(pin == SW1_Pin){
		if(m->getBuzzer()->getState() == BUZZ_ON){
			m->getBuzzer()->off();
			m->setTemperatureAlarmStat(TAS_OFF_BY_USER);
		}

	}
	if(pin == DHT_Pin){
		m->getDHT()->onGpioInterrupt(pin);
	}

}


extern "C" int _write(int fd, char* ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    return len;
}
