/*
 * RTC.cpp
 *
 *  Created on: 20 Nov 2022
 *      Author: student
 */

#include "RTCClock.h"

#define RTC_START_STOP      (1 << 7)
#define RTC_DATE_TIME_SIZE  7
#define DEVICE_ADDR         0xD0

const uint32_t DaysInYear    = 365;
const uint32_t SecondsInMin  = 60;
const uint32_t SecondsInHour = 3600;
const uint32_t SecondsInDay  = 86400;

static const int _daysUntilMonth[] = {
		0,
		31,
		59,
		90,
		120,
		151,
		181,
		212,
		243,
		273,
		304,
		334,
		365
};

RTCClock::RTCClock(I2C_HandleTypeDef* hi2c,uint8_t devAddr) {
	// TODO Auto-generated constructor stub
	m_hi2c = hi2c;
	m_devAddr = devAddr;

}

RTCClock::~RTCClock() {
	// TODO Auto-generated destructor stub
}

void RTCClock::Write(uint16_t memAddr, uint8_t * buffer, uint16_t size)
{
	HAL_I2C_Mem_Write(m_hi2c, m_devAddr, memAddr, 1, buffer, size, 0xFF);
}

void RTCClock::Read(uint16_t memAddr, uint8_t * buffer, uint16_t size)
{
	HAL_I2C_Mem_Read(m_hi2c, m_devAddr, memAddr, 1, buffer, size, 0xFF);
}

void RTCClock::Start()
{
	uint8_t sec = 0;
	HAL_I2C_Mem_Read(m_hi2c, m_devAddr, 0, 1, &sec, 1, 0xFF);
	sec &= ~RTC_START_STOP;
	HAL_I2C_Mem_Write(m_hi2c, m_devAddr, 0, 1, &sec, 1, 0xFF);
}

void RTCClock::Stop()
{
	uint8_t sec = 0;
	HAL_I2C_Mem_Read(m_hi2c, m_devAddr, 0, 1, &sec, 1, 0xFF);
	sec |= RTC_START_STOP;
	HAL_I2C_Mem_Write(m_hi2c, m_devAddr, 0, 1, &sec, 1, 0xFF);
}

int RTCClock::IsRunning()
{
	uint8_t sec = 0;
	HAL_I2C_Mem_Read(m_hi2c, m_devAddr, 0, 1, &sec, 1, 0xFF);
	return (sec & RTC_START_STOP) == 0;
}

int RTCClock::bcdToInt(uint8_t bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t RTCClock::intToBcd(int value, int minVal, int maxVal)
{
	if (value < minVal || value > maxVal) {
		return 0;
	}

	return ((value / 10) << 4) | (value % 10);
}

DateTime RTCClock::getTime()
{
	uint8_t buffer[RTC_DATE_TIME_SIZE];
	HAL_I2C_Mem_Read(m_hi2c, m_devAddr, 0, 1, buffer, RTC_DATE_TIME_SIZE, 0xFF);

	// remove stop bit if set
	buffer[0] &= ~RTC_START_STOP;
	m_dt.sec = bcdToInt(buffer[0]);
	m_dt.min = bcdToInt(buffer[1]);
	m_dt.hours = bcdToInt(buffer[2]);
	m_dt.weekDay = buffer[3] & 0x07;
	m_dt.day = bcdToInt(buffer[4]);
	m_dt.month = bcdToInt(buffer[5]);
	m_dt.year = bcdToInt(buffer[6]);
	return m_dt;
}

bool RTCClock::getSeconds(DateTime* pDateTime,uint32_t* pSeconds)
{
	// calculate seconds from 00:00:00 1/1/2020
		if(pDateTime == nullptr){
			return false;
		}

		*pSeconds = pDateTime->sec +
				pDateTime->min * SecondsInMin +
				pDateTime->hours * SecondsInHour +
				pDateTime->day * SecondsInDay +
				_daysUntilMonth[pDateTime->month - 1] * SecondsInDay +
				pDateTime->year * DaysInYear * SecondsInDay;
		if (pDateTime->year % 4 == 0 && pDateTime->month > 2) {
			// if current year is a leap year and month is after February
			// add seconds for February 29
			*pSeconds += SecondsInDay;
		}
		// add seconds for all previous leap years
		*pSeconds += (pDateTime->year / 4) * SecondsInDay;
		return true;
}

HAL_StatusTypeDef RTCClock::setTime(DateTime * dateTime)
{
	HAL_StatusTypeDef status;
	uint8_t buffer[RTC_DATE_TIME_SIZE];

	buffer[0] = intToBcd(dateTime->sec, 0, 59);
	buffer[1] = intToBcd(dateTime->min, 0, 59);
	buffer[2] = intToBcd(dateTime->hours, 0, 59);
	buffer[3] = dateTime->weekDay < 1 || dateTime->weekDay > 7 ? 0 : dateTime->weekDay;
	buffer[4] = intToBcd(dateTime->day, 1, 31);
	buffer[5] = intToBcd(dateTime->month, 1, 12);
	buffer[6] = intToBcd(dateTime->year, 1, 99);

	status = HAL_I2C_Mem_Write(m_hi2c, m_devAddr, 0, 1, buffer, RTC_DATE_TIME_SIZE, 0xFF);
	return status;
}
