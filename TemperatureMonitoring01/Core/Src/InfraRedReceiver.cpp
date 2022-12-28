/*
 * InfraRedReceiver.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: USER-PC
 */

#include <InfraRedReceiver.h>
#include <stdio.h>
#include "cmsis_os.h"

InfraRedReceiver::InfraRedReceiver(GPIO_TypeDef *gpioPort, uint16_t gpioPin,
		TIM_HandleTypeDef *timer) {
	// TODO Auto-generated constructor stub
	m_gpioPort = gpioPort;
	m_gpioPin = gpioPin;
	m_timer = timer;
	m_tempCode = 0;
	m_bitIndex = 0;
	m_state = IR_STATE_NO_DATA;
//m_i=0;
	HAL_TIM_Base_Start(m_timer);
	__HAL_TIM_SET_COUNTER(m_timer, 0);

}

InfraRedReceiver::~InfraRedReceiver() {
	// TODO Auto-generated destructor stub
}



void InfraRedReceiver::readAsync() {
	// printf("x=%lu m_bitIndex=%lu\r\n",__HAL_TIM_GET_COUNTER(m_timer),m_bitIndex);
	uint32_t timeElaps = __HAL_TIM_GET_COUNTER(m_timer);
	//m_arr[m_i] = x;
	//m_i++;
	//printf("in if x=%lu m_bitIndex=%lu\r\n",x,m_bitIndex);
	if (timeElaps > 10000) {
		//printf("in if x=%lu m_bitIndex=%lu\r\n",__HAL_TIM_GET_COUNTER(m_timer),m_bitIndex);
		m_tempCode = 0;
		m_bitIndex = 0;
	} else if (timeElaps > 2000) {
		m_tempCode |= (1UL << (31 - m_bitIndex));   // write 1
		m_bitIndex++;
	} else if (timeElaps > 1000) {
		m_tempCode &= ~(1UL << (31 - m_bitIndex));  // write 0
		m_bitIndex++;
	}
	if (m_bitIndex >= 32) {
		uint8_t cmdli = ~m_tempCode; // Logical inverted last 8 bits
		uint8_t cmd = m_tempCode >> 8; // Second last 8 bits
	//	printf("m_tempCode=%lu\r\n",m_tempCode);
		if (cmdli == cmd){ // Check for errors
			m_code = m_tempCode; // If no bit errors
			m_state = IR_STATE_READY;
		}
		m_bitIndex = 0;
	}
	/*
	if(m_i == 32)
	{
		for(int i = 0;i<32;i++){
			printf("m_arr[%d]=%lu\r\n",i,m_arr[i]);
		}
		m_i=0;
	}*/
	__HAL_TIM_SET_COUNTER(m_timer, 0);

}

bool InfraRedReceiver::hasData() {
	int hasData = m_state == IR_STATE_READY;

	if (hasData) {
		// reset state to avoid multiple reads
		m_state = IR_STATE_NO_DATA;
	}

	return hasData;
}

