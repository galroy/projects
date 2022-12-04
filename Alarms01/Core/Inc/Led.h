/*
 * Led.h
 *
 *  Created on: Sep 13, 2022
 *      Author: student
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "main.h"
#include <stdint.h>

typedef enum LedState_
{
	STATE_OFF,
	STATE_ON,
	STATE_BLINK,
} LedState;


#define BLINK_DELAY_DEFUALT	300

typedef struct Led_
{
	GPIO_TypeDef * gpioPort;
	uint16_t gpioPin;
	int delay;
	LedState state;
} Led;


void ledInit(Led* led, GPIO_TypeDef* gpioPort, uint16_t gpioPin,int delay,LedState state);

void ledSetBlinkDelay(Led* led,int delay);
void ledSetState(Led* led,LedState state);

#endif /* INC_LED_H_ */
