/*
 * Structs.h
 *
 *  Created on: Nov 22, 2022
 *      Author: USER-PC
 */

#ifndef INC_STRUCTS_H_
#define INC_STRUCTS_H_

#define TEMPERATURE_BOUNDARY_MAGIC_NUM	0X5A5A
enum temperatureStatus
{
	NORMAL,
	WARNING,
	CRITICAL
};
struct DateTime
{
	int sec;
	int min;
	int hours;
	int weekDay;
	int day;
	int month;
	int year;
};

struct temperatureBoundary{
	double m_warning;
	double m_critical;
	uint16_t m_magicNumber;
};

struct eventLog{
	//DateTime dtStamp;
	char chDateTimeStamp[17]; //dd.mm.yy HH:MM:SS
	temperatureStatus ts;
	double temperature;
};


#endif /* INC_STRUCTS_H_ */
