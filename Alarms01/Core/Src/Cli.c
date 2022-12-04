/*
 * Cli.c
 *
 *  Created on: 23 Oct 2022
 *      Author: student
 */

#include "cmsis_os.h"

#include "Cli.h"
#include "CommTask.h"
#include "main.h"
#include "Rtc.h"
#include "commonStrcut.h"
#include "Manager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern Rtc rtc;
extern osMutexId_t rtcMutexHandle;
//
static int setDateTime(DateTime *dateTime, char *param) {
	// if string is empty
	if (*param == '\0') {
		return 1;
	}
	if(strlen(param) != 17){
		printf("date string length = %d\r\n",strlen(param));
		return 1;
	}
	int ret = 0;
	const char s[4] = ". :";
	char *token;

	/* get the first token */
	token = strtok(param, s);

	/* walk through other tokens */
	do {
		dateTime->day = atoi(token);
		if (dateTime->day < 0 || dateTime->day > 31) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->month = atoi(token);
		if (dateTime->month < 0 || dateTime->month > 12) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->year = atoi(token);
		if (dateTime->year < 0 || dateTime->year > 99) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->hours = atoi(token);
		if (dateTime->hours < 0 || dateTime->hours > 23) {
			ret = 1;
			break;
		}

		token = strtok(NULL, s);
		dateTime->min = atoi(token);
		if (dateTime->min < 0 || dateTime->min > 59) {
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		dateTime->sec = atoi(token);
		if (dateTime->sec < 0 || dateTime->sec > 59) {
			ret = 1;
			break;
		}

	} while (0);

	return ret;

}
static void setDateTimeCmd(void * obj, char * param)
{

	Rtc *rtc = (Rtc*)obj;
		DateTime dateTime;
		HAL_StatusTypeDef status;
		char strHelp[] = "-h";
		if(strcmp(param,strHelp) == 0)
		{
			printf("date format\r\n");
			printf("dd.mm.yy HH:MM:SS\r\n");
			return;
		}
		if(setDateTime(&dateTime,param) == 0)
		{
			osMutexAcquire(rtcMutexHandle, osWaitForever);
			status = rtcSetTime(rtc,&dateTime);
			osMutexRelease(rtcMutexHandle);
			if(status == HAL_OK)
			{
				printf("date time set Successfully\r\n");
			}
			else
			{
				printf("Error set date time status code = %d\r\n",status);
			}
		}
		else
		{
			printf("Invalid date time, Try again \r\n");
		}

}

static void showDateCmd(void * obj, char * param)
{
	Rtc *rtc = (Rtc*)obj;
	(void)param;
	DateTime dateTime;
	rtcGetTime(rtc, &dateTime);
	printf("%02d:%02d:%02d %02d.%02d.%02d\r\n",
					dateTime.hours, dateTime.min, dateTime.sec,
					dateTime.day, dateTime.month, dateTime.year);

}

static int setAlarm(Alarm* alarm, char* param)
{
	int ret = 0;
	const char s[4] = ". :";
	char *token;

	/* get the first token */
	token = strtok(param, s);

	/* walk through other tokens */
	do {
		// CHECK LEN!!!!
		if (strlen(token) > ALARM_NAME_LENGTH){
			printf("Alarm name to long max long is 10 chars!!\r\n");
			ret = 1;
			break;
		}
		if(managerGetAlarmIndex(token) != -1){
			printf("Alarm name must be unique!!\r\n");
			ret = 1;
			break;
		}
		strcpy(alarm->alarmName,token);
		token = strtok(NULL, s);

		alarm->alarmTime.day = atoi(token);
		if (alarm->alarmTime.day < 0 || alarm->alarmTime.day > 32) {
			printf("Day must between 1-31 !!\r\n");
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.month = atoi(token);
		if (alarm->alarmTime.month< 0 || alarm->alarmTime.month > 12) {
			printf("Month must between 1-12 !!\r\n");
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.year = atoi(token);
		if (alarm->alarmTime.year < 0 || alarm->alarmTime.year > 99) {
			printf("year must between 1-99 !!\r\n");
			ret = 1;
			break;
		}

		token = strtok(NULL, s);
		alarm->alarmTime.hours = atoi(token);
		if (alarm->alarmTime.hours < 0 || alarm->alarmTime.hours > 23) {
			printf("Hours must between 1-23 !!\r\n");
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->alarmTime.min = atoi(token);
		alarm->alarmTime.sec = 0;
		if (alarm->alarmTime.min < 0 || alarm->alarmTime.min > 59) {
			printf("Minute must between 1-31 !!\r\n");
			ret = 1;
			break;
		}
		token = strtok(NULL, s);
		alarm->snoozeLeft = alarm->snoozeRepeat = atoi(token);
		if (alarm->snoozeRepeat < 0 || alarm->snoozeRepeat > 5) {
			printf("snoozRepeat must between 0-5 !!\r\n");
			ret = 1;
			break;
		}

		token = strtok(NULL, s);
		alarm->melodey = atoi(token);
		if (alarm->melodey < 0 || alarm->melodey > 3) {
			printf("Melody must between 0-3 !!\r\n");
			ret = 1;
			break;
		}
		alarm->active = 1;
		if(alarm->snoozeLeft == 0 || alarm->snoozeRepeat == 0){
			alarm->snoozeLeft = alarm->snoozeRepeat = 1;
		}

		alarm->magicNumber = 0X5A5A;
	} while (0);


	return ret;
}

static void addAlarmCmd (void * obj, char* param)
{
	Alarm alarm;
	(void)obj;
	// if string is empty
	if (*param == '\0') {
		return;
	}
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0){
		printf("date format\r\n");
		printf("[AlarmName] dd.mm.yy HH:MM [snoozRepeat 0-5] [melodey 0-2]\r\n");
		return;
	}
	int status;
	status = setAlarm(&alarm, param);
	if(status == 0)
	{
		managerAddAlarm(&alarm);
	}

}

//
//
//
//
//
//
static void stopCmd(void * obj, char * param)
{
	(void)obj;
	(void)param;
	managerStopAlarm();
}
static void clearAllCmd(void * obj, char * param)
{
	(void)obj;
	(void)param;

	managerClearAll();
}

static void printEditHelp()
{
	printf("Edit alarm name\r\n");
	printf("edit [Alarm Name] name [new alarm name]\r\n");
	printf("Edit alarm date\r\n");
	printf("edit [Alarm Name] date [date format dd.mm.yy]\r\n");
	printf("Edit alarm time\r\n");
	printf("edit [Alarm Name] time [time format HH:MM]\r\n");
	printf("Edit alarm melody\r\n");
	printf("edit [Alarm Name] melodey [melodey 0-2]\r\n");
}

static void editCmd(void * obj, char * param)
{

	(void)obj;
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0){
		printEditHelp();
	}else{
		managerEditAlarm(param);
	}


}


static void helpCmd(void * obj, char * param)
{
	(void)obj;
	(void)param;
	commTaskprintCommands();
}
//static void melodyCmd(void * obj, char * param)
//{
//	(void)obj;
//	(void)param;
////	alarmsManager_printMelodyList();
//}
//
static void listCmd (void * obj, char* param)
{
	(void)obj;
	(void)param;
	managerPrintAlarmsList();

}
//
static void delCmd (void* obj, char* param)
{
	(void)obj;
	char strHelp[] = "-h";
	if(strcmp(param,strHelp) == 0){
		printf("delete [Alarm name]\r\n");
		return;
	}
	if (strlen(param) > ALARM_NAME_LENGTH){
		printf("alarm name to long\r\n");
		return;
	}
	managerDeleteAlarm(param);
}

void cliInit()
{
	registerCommand("setdatetime",setDateTimeCmd,&rtc);
	registerCommand("showdate",showDateCmd,&rtc);
	registerCommand("addAlarm",addAlarmCmd,NULL);
	registerCommand("add",     addAlarmCmd,NULL);
	registerCommand("stop",stopCmd,NULL);
	registerCommand("list",listCmd,NULL);
	registerCommand("del",delCmd,NULL);

	registerCommand("clearall",clearAllCmd,NULL);
	registerCommand("edit",editCmd,NULL);
	//registerCommand("melody",melodyCmd,NULL);
	registerCommand("help", helpCmd, NULL);
}

