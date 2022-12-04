/*
 * Led.c
 *
 *  Created on: Sep 13, 2022
 *      Author: student
 */
#include "led.h"
#include "cmsis_os.h"


void ledInit(Led* led, GPIO_TypeDef* gpioPort, uint16_t gpioPin,int delay /*= 300*/,LedState state )
{

	led->gpioPort = gpioPort;
	led->gpioPin = gpioPin;
	led->delay = delay ;
	led->state = state;
}



void startBlinkLed(void* argo)
{
	Led* led = (Led*) argo;
	for(;;)
	{
		switch(led->state){
		case STATE_BLINK:
			HAL_GPIO_TogglePin(led->gpioPort, led->gpioPin);
			break;
		case STATE_ON:
			HAL_GPIO_WritePin(led->gpioPort, led->gpioPin, 1);
			break;
		case STATE_OFF:
		default:
			HAL_GPIO_WritePin(led->gpioPort, led->gpioPin, 0);
			osThreadYield();
			break;

		}


		osDelay(led->delay);
	}
}



void ledSetState(Led* led,LedState state)
{
	led->state = state;
}

void ledSetBlinkDelay(Led* led,int delay)
{
	led->delay = delay;

}



