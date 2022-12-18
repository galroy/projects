/*
 * DHT.cpp
 *
 *  Created on: Nov 15, 2022
 *      Author: student
 */
#include <string.h>
#include <DHT.h>
#include "MainTimer.h"
extern MainTimer mainTimer;

#define MAX_BITS 40

DHT::DHT(GPIO_TypeDef *gpioPort, uint16_t gpioPin, TIM_HandleTypeDef *timer) {
	m_gpioPort = gpioPort;
	m_gpioPin = gpioPin;
	m_timer = timer;

	m_counter = 0;
	m_maxCounter = 0;
	m_state = DHT_STATE_NO_DATA;
	m_temperature = 0.0;
	m_humidity = 0.0;
	m_showTemperatureFlag = false;
}

DHT::~DHT() {
	// TODO Auto-generated destructor stub
}

void DHT::setGpioOutput() {
	GPIO_InitTypeDef gpioStruct = { 0 };

	gpioStruct.Pin = m_gpioPin;
	gpioStruct.Mode = GPIO_MODE_OUTPUT_PP;
	gpioStruct.Pull = GPIO_NOPULL;
	gpioStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(m_gpioPort, &gpioStruct);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void DHT::setGpioInput() {
	GPIO_InitTypeDef gpioStruct = { 0 };

	gpioStruct.Pin = m_gpioPin;
	gpioStruct.Mode = GPIO_MODE_INPUT;
	gpioStruct.Pull = GPIO_NOPULL;
	gpioStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(m_gpioPort, &gpioStruct);
	HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
}

void DHT::setGpioExti() {
	GPIO_InitTypeDef gpioStruct = { 0 };

	gpioStruct.Pin = m_gpioPin;
	gpioStruct.Mode = GPIO_MODE_IT_FALLING;
	gpioStruct.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(m_gpioPort, &gpioStruct);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

int DHT::waitWhileEqual(int value, int expectedTime) {
	// set a timeout 20% bigger than the expected time
	uint32_t timeout = expectedTime + (expectedTime) / 5;

	__HAL_TIM_SET_COUNTER(m_timer, 0);
	while (HAL_GPIO_ReadPin(m_gpioPort, m_gpioPin) == value) {
		if (__HAL_TIM_GET_COUNTER(m_timer) > timeout) {
			// too much time in the state
			return false;
		}
	}

	return true;
}

int DHT::readSync() {
	setGpioOutput();

	HAL_TIM_Base_Start(m_timer);
	__HAL_TIM_SET_COUNTER(m_timer, 0);

	// switch the sensor on by putting the line in '0'
	HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_RESET);
	while (__HAL_TIM_GET_COUNTER(m_timer) < 19000)
		;

	HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_SET);

	// start listening
	setGpioInput();

	// wait for response 20-40us
	if (!waitWhileEqual(1, 40)) {
		return HAL_ERROR;
	}

	// DHT should keep in low for 80us
	if (!waitWhileEqual(0, 80)) {
		return HAL_ERROR;
	}

	// DHT should keep in high for 80us
	if (!waitWhileEqual(1, 80)) {
		return HAL_ERROR;
	}

	// DHT start send data bits

	uint8_t data[5] = { 0 };
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) {

			// DHT keeps in low for 50us before each bit
			if (!waitWhileEqual(0, 60)) {
				return HAL_ERROR;
			}

			// DHT sends bit keeping line in high maximum 70us
			if (!waitWhileEqual(1, 70)) {
				return HAL_ERROR;
			}

			data[i] <<= 1;

			// 26-28us for '0' and 70us for '1', so 50us is a good split time
			if (__HAL_TIM_GET_COUNTER(m_timer) > 50) {
				data[i] |= 1;
			}
		}
	}

	uint8_t checksum = data[0] + data[1] + data[2] + data[3];
	if (checksum != data[4]) {
		return HAL_ERROR;
	}

	m_humidity = (double) data[0] + ((double) data[1]) / 10;
	m_temperature = (double) data[2] + ((double) data[3]) / 10;

	//printf("Humidity: %d.%d, temp: %d.%d\r\n", (int)data[0], (int)data[1], (int)data[2], (int)data[3]);

	// wait while DHT return to high
	if (!waitWhileEqual(0, 100)) {
		return HAL_ERROR;
	}

	HAL_TIM_Base_Stop(m_timer);
	m_state = DHT_STATE_READY;
	return HAL_OK;
}

void DHT::readAsync() {
	setGpioOutput();

	// switch the sensor on by putting the line in '0'
	HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_RESET);

	// should be in '0' for 18-20 ms
	mainTimer.addTimerTask(this);
	m_counter = 0;
	m_maxCounter = 19;

	m_state = DHT_STATE_POWER_ON;
}

void DHT::timerFunc() {

	if (m_state != DHT_STATE_POWER_ON) {
		return;
	}

	m_counter++;
	if (m_counter >= m_maxCounter) {
		m_state = DHT_STATE_POWER_ON_ACK;
		HAL_GPIO_WritePin(m_gpioPort, m_gpioPin, GPIO_PIN_SET);
		HAL_TIM_Base_Start(m_timer);
		__HAL_TIM_SET_COUNTER(m_timer, 0);

		setGpioExti();

		m_counter = 0;
		mainTimer.deleteTimerTask(this);
	}
}

void DHT::onGpioInterrupt(uint16_t pin) {
	if (m_gpioPin != pin) {
		return;
	}

	uint32_t timeMs = __HAL_TIM_GET_COUNTER(m_timer);

	switch (m_state) {
	case DHT_STATE_POWER_ON_ACK:
		if (timeMs > 50) {
			m_state = DHT_STATE_ERROR;
		}
		m_state = DHT_STATE_INIT_RESPONSE;
		break;

	case DHT_STATE_INIT_RESPONSE:
		if (timeMs > 200) {
			m_state = DHT_STATE_ERROR;
		}

		memset(m_data, 0, sizeof(m_data));
		m_bit = 0;
		m_state = DHT_STATE_RECEIVE_DATA;

		break;

	case DHT_STATE_RECEIVE_DATA: {
		if (timeMs > 140) {
			m_state = DHT_STATE_ERROR;
		}

		// 50us in low + 50 us in high (> 30 and < 70)
		int byte = m_bit / 8;
		m_data[byte] <<= 1;

		if (timeMs > 100) {
			// '1' is detected
			m_data[byte] |= 1;
		}

		m_bit++;
		if (m_bit >= MAX_BITS) {

			uint8_t checksum = m_data[0] + m_data[1] + m_data[2] + m_data[3];

			if (checksum == m_data[4]) {
				m_state = DHT_STATE_READY;

				m_humidity = (double) m_data[0] + ((double) m_data[1]) / 10;
				m_temperature = (double) m_data[2] + ((double) m_data[3]) / 10;
			} else {
				m_state = DHT_STATE_ERROR;
			}

			// stop timer and disable GPIO interrupts
			HAL_TIM_Base_Stop(m_timer);
			HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		}

	}
		break;

	default:
		// in all other states ignore the interrupt
		break;
	}

	__HAL_TIM_SET_COUNTER(m_timer, 0);
}

bool DHT::hasData() {

	bool ret = false;
	if(m_state == DHT_STATE_READY){
		ret = true;
		m_state = DHT_STATE_NO_DATA;
	}
	return ret;

}

double DHT::getHumidty() {
	return m_humidity;
}

double DHT::getTempperature() {
	return m_temperature;
}

