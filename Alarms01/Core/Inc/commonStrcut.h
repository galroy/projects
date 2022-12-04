/*
 * commonStrcut.h
 *
 *  Created on: 18 באוג׳ 2022
 *      Author: USER-PC
 */

#ifndef INC_COMMONSTRCUT_H_
#define INC_COMMONSTRCUT_H_
#include <stdint.h>
#define ALARM_NAME_LENGTH 20
#define ALARM_IN_USED 1

typedef enum alarmState
{
	OFF = -1,
	ON,
}AlarmState;

#define FALSE	0
#define TRUE	1

typedef struct DateTime_ {

	uint8_t sec;
	uint8_t min;
	uint8_t hours;
	uint8_t weekDay;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t padding;
} DateTime;

typedef struct Alarm_
{
	uint16_t magicNumber;
	uint8_t active;
	uint8_t snoozeRepeat;
	uint8_t snoozeLeft;
	uint8_t melodey;
	char alarmName[ALARM_NAME_LENGTH];
	DateTime alarmTime;
	//AlarmState alarmSnooze;
	//AlarmState alarmActive;

	//int alarmStartTime; // start time in second
}Alarm;

/*
typedef struct AlarmsInfo_
{
	uint16_t MagicNumber;//(two bytes) – 0x5A5A
	uint32_t Version;//Major.Minor.Release.Build
	uint8_t ApplicationType;//1 for alarms project
	uint32_t Crc; // for future use
	uint8_t Flags;//Flags (one byte) – currently 0
}AlarmsInfo;

typedef struct Version_
{
	uint8_t Major;
	uint8_t Minor;
	uint8_t Release;
	uint8_t Build;
}Version;
*/

#endif /* INC_COMMONSTRCUT_H_ */
