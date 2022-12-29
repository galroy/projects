/*
 * InfraRedReceiver.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: USER-PC
 */

#include <InfraRedReceiver.h>

enum timeDefine {
	TEN_THOUSAND	= 10000,
	TWO_THOUSAND	= 2000,
	THOUSAND		= 1000
};

InfraRedReceiver::InfraRedReceiver(GPIO_TypeDef *gpioPort, uint16_t gpioPin,
		TIM_HandleTypeDef *timer) {
	// TODO Auto-generated constructor stub
	m_gpioPort = gpioPort;
	m_gpioPin = gpioPin;
	m_timer = timer;
	m_tempCode = 0;
	m_bitIndex = 0;
	m_state = IR_STATE_NO_DATA;

	HAL_TIM_Base_Start(m_timer);
	__HAL_TIM_SET_COUNTER(m_timer, 0);

}

InfraRedReceiver::~InfraRedReceiver() {
	// TODO Auto-generated destructor stub
}

void InfraRedReceiver::readAsync() {

	uint32_t timeElaps = __HAL_TIM_GET_COUNTER(m_timer);

	if (timeElaps > TEN_THOUSAND) {
		m_tempCode = 0;
		m_bitIndex = 0;
	} else if (timeElaps > TWO_THOUSAND) {
		m_tempCode |= (1UL << (31 - m_bitIndex));   // write 1
		m_bitIndex++;
	} else if (timeElaps > THOUSAND) {
		m_tempCode &= ~(1UL << (31 - m_bitIndex));  // write 0
		m_bitIndex++;
	}
	if (m_bitIndex >= 32) {
		uint8_t cmdli = ~m_tempCode; // Logical inverted last 8 bits
		uint8_t cmd = m_tempCode >> 8; // Second last 8 bits

		if (cmdli == cmd){ // Check for errors
			m_code = m_tempCode; // If no bit errors
			m_state = IR_STATE_READY;
		}
		m_bitIndex = 0;
	}

	__HAL_TIM_SET_COUNTER(m_timer, 0);

}

bool InfraRedReceiver::hasData() {
	bool hasData = m_state == IR_STATE_READY;

	if (hasData) {
		// reset state to avoid multiple reads
		m_state = IR_STATE_NO_DATA;
	}

	return hasData;
}

