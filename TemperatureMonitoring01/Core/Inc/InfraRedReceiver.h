/*
 * InfraRedReceiver.h
 *
 *  Created on: Dec 21, 2022
 *      Author: USER-PC
 */

#ifndef INC_INFRAREDRECEIVER_H_
#define INC_INFRAREDRECEIVER_H_

#include "main.h"
#include "TimerTask.h"
enum IRState
{
	IR_STATE_NO_DATA,
	IR_STATE_READY,
	IR_STATE_ERROR,
	//IR_STATE_POWER_ON,
	//IR_STATE_POWER_ON_ACK,
	//IR_STATE_INIT_RESPONSE,
	//IR_STATE_RECEIVE_DATA
};

class InfraRedReceiver {
	private:
	GPIO_TypeDef * m_gpioPort;
	uint16_t m_gpioPin;
	TIM_HandleTypeDef * m_timer;
	IRState m_state;
	uint32_t m_tempCode;
	uint32_t m_code;
	uint8_t m_bitIndex;
	//uint32_t m_arr[32];
	//int m_i;
public:
	InfraRedReceiver(GPIO_TypeDef * gpioPort, uint16_t gpioPin, TIM_HandleTypeDef * timer);
	virtual ~InfraRedReceiver();

	void readAsync();
	bool hasData();
	uint32_t getCode(){return m_code;}
};

#endif /* INC_INFRAREDRECEIVER_H_ */
