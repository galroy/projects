/*
 * LedGpioTask.h
 *
 *  Created on: Nov 22, 2022
 *      Author: student
 */

#ifndef INC_LEDGPIOTASK_H_
#define INC_LEDGPIOTASK_H_
#include "main.h"

#define LED_DELAY 300
enum LedState
{
	STATE_OFF,
	STATE_ON,
	STATE_BLINK,
};

class LedGpioTask {
	private:
	GPIO_TypeDef *m_gpioPort;
	uint16_t m_gpioPin;
	LedState m_ledState;
	int m_delay;
public:
	LedGpioTask(GPIO_TypeDef *m_gpioPort, uint16_t m_gpioPin);
	virtual ~LedGpioTask();

	GPIO_TypeDef* getGpioPort(){return m_gpioPort;}
	uint16_t getGpioPin(){return m_gpioPin;}
	int getDelay(){return m_delay;}
	void on();
	void off();
	void blink(int delay);
	LedState getLedState(){return m_ledState;}
};

#endif /* INC_LEDGPIOTASK_H_ */
