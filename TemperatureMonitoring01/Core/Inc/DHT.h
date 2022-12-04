/*
 * DHT.h
 *
 *  Created on: Nov 15, 2022
 *      Author: student
 */

#ifndef INC_DHT_H_
#define INC_DHT_H_

#include "main.h"
#include "TimerTask.h"


enum DhtState
{
	DHT_STATE_NO_DATA,
	DHT_STATE_READY,
	DHT_STATE_ERROR,
	DHT_STATE_POWER_ON,
	DHT_STATE_POWER_ON_ACK,
	DHT_STATE_INIT_RESPONSE,
	DHT_STATE_RECEIVE_DATA
};

class DHT :public TimerTask{
private:
	GPIO_TypeDef * m_gpioPort;
	uint16_t m_gpioPin;
	TIM_HandleTypeDef * m_timer;
	int m_counter;
	int m_maxCounter;
	int m_state;
	uint8_t m_data[5];
	int m_bit;
	double m_temperature;
	double m_humidity;
	bool m_showTemperatureFlag;
public:
	DHT(GPIO_TypeDef * gpioPort, uint16_t gpioPin, TIM_HandleTypeDef * timer);
	virtual ~DHT();
private:
	void setGpioOutput();
	void setGpioInput();
	void setGpioExti();
	int waitWhileEqual(int value, int expectedTime);
public:
	// sync read
	int read();

	// async read
	void readAsync();
	void onGpioInterrupt(uint16_t pin);
	void timerFunc();
	bool hasData();
	void setShowTemperatureFlag(bool showTemperatureFlag){m_showTemperatureFlag = showTemperatureFlag;}
	bool getShowTemperatureFlag(){return m_showTemperatureFlag;}

	// shared between sync and async
	double getHumidty();
	double getTempperature();
};

#endif /* INC_DHT_H_ */
