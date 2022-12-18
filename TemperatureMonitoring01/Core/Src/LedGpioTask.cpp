/*
 * LedGpioTask.cpp
 *
 *  Created on: Nov 22, 2022
 *      Author: student
 */

#include "LedGpioTask.h"
#include "Manager.h"

extern Manager *m;

LedGpioTask::LedGpioTask(GPIO_TypeDef *gpioPort,uint16_t gpioPin) {
	m_gpioPort = gpioPort;
	m_gpioPin = gpioPin;
	m_ledState = STATE_OFF;
	m_delay = LED_DELAY;

}

LedGpioTask::~LedGpioTask() {
	// TODO Auto-generated destructor stub
}

void LedGpioTask::on(){
	HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_SET);
	m_ledState = STATE_ON;
}

void LedGpioTask::off(){
	HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_RESET);
	m_ledState = STATE_OFF;


}

void LedGpioTask::blink(int delay)
{
	m_ledState = STATE_BLINK;
	m_delay = delay;
}

extern "C" void startBlinkLed(void* argo)
{

	for(;;)
	{
		switch(m->getRedLed()->getLedState()){
		case STATE_BLINK:
			HAL_GPIO_TogglePin(m->getRedLed()->getGpioPort(), m->getRedLed()->getGpioPin());
			break;
		case STATE_ON:
		case STATE_OFF:
		default:
			osThreadYield();
			break;

		}

		osDelay(m->getRedLed()->getDelay());
	}
}

