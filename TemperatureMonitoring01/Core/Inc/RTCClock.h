/*
 * RTC.h
 *
 *  Created on: 20 Nov 2022
 *      Author: student
 */

#ifndef INC_RTCClock_H_
#define INC_RTCClock_H_

#include "main.h"
#include "Structs.h"
/*
struct DateTime
{
	int sec;
	int min;
	int hours;
	int weekDay;
	int day;
	int month;
	int year;
};*/

class RTCClock {
private:
	I2C_HandleTypeDef* m_hi2c;
	uint8_t m_devAddr;
	DateTime m_dt;
public:
	RTCClock(I2C_HandleTypeDef* hi2c,uint8_t devAddr);

	virtual ~RTCClock();

	void Write(uint16_t memAddr, uint8_t * buffer, uint16_t size);

	void Read(uint16_t memAddr, uint8_t * buffer, uint16_t size);

	void Start();

	void Stop();

	int IsRunning();

	DateTime getTime();

	bool getSeconds(DateTime* pDateTime,uint32_t* pSeconds);

	HAL_StatusTypeDef setTime(DateTime * dateTime);

private:
	int bcdToInt(uint8_t bcd);
	uint8_t intToBcd(int value, int minVal, int maxVal);
	};

#endif /* INC_RTCClock_H_ */
