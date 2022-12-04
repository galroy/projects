/*
 * Manager.c
 *
 *  Created on: Oct 26, 2022
 *      Author: USER-PC
 */
#include "cmsis_os.h"

#include "Manager.h"
#include "Rtc.h"
#include "Cli.h"
#include "Led.h"
#include "buzzer.h"
#include "button.h"
#include "Flash.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define MAX_ALARMS 10
#define MAX_ALARM_TIME 30 // 30 seconds alarm until stop alarm
#define SNOOZE_WAITING_TIME_SEC 10



extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim3;
extern osMutexId_t rtcMutexHandle;
extern osMutexId_t flashMutexHandle;

Rtc rtc;
Led ledRed;
Led ledBlue;
Buzzer buzzer;
Button rightBtn;
Button leftBtn;

int activeAlarm = -1;
int waitingToSnooze = 0;
int alarmOnCounter = -1;
int doAlarmFlag = FALSE;
int snooze = -1;

Alarm alarmsArr[MAX_ALARMS];
static uint8_t countAlarms = 0;


static void readFromFlash()
{
	Alarm* data = (Alarm *)(PAGE_256);
	while(data->magicNumber==0X5A5A)
	{
		memcpy(&alarmsArr[countAlarms], data, sizeof(Alarm));
		countAlarms++;
		data++;
	}
}

static void writeToFlash()
{

	HAL_StatusTypeDef status;
	osMutexAcquire(flashMutexHandle, osWaitForever);
	status = erasePage(BANK_IN_USED, PAGE_256, 1);
	osMutexRelease(flashMutexHandle);
	if(status != HAL_OK)
	{
		printf("error in erase page in line %d", __LINE__);
		return;
	}
	osMutexAcquire(flashMutexHandle, osWaitForever);
	status = writeToPage(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE_256, &alarmsArr, (sizeof(Alarm)) * MAX_ALARMS);
	osMutexRelease(flashMutexHandle);
	if(status != HAL_OK)
	{
		printf("error in write to page in line %d", __LINE__);

	}
	else
	{
		printf("saved in flash \r\n");
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == rightBtn.gpioPin || GPIO_Pin == leftBtn.gpioPin) // the user click on the btn for turn off the alarm
			{
		if (activeAlarm != OFF) {
			managerStopAlarm();
		}
	}
}

void managerPrintAlarmsList() {
	printf("\r\n--------------------LIST--------------------\r\n");
	if (countAlarms == 0) {
		printf("Alarms list is empty \r\n");
		printf("--------------------------------------------\r\n");
		return;
	}
	for (int i = 0; i < countAlarms; i++) {
		printf("%d.%s: %02d:%02d:%02d ", i + 1, alarmsArr[i].alarmName,
				alarmsArr[i].alarmTime.hours, alarmsArr[i].alarmTime.min,
				alarmsArr[i].alarmTime.sec);
		alarmsArr[i].active == TRUE ?
				printf("alarm Active\r\n") : printf("alarm Off\r\n");
	}
	printf("--------------------------------------------\r\n");
}

void managerDeleteAlarm(char *alarmName) {

	int i = -1;
	i = managerGetAlarmIndex(alarmName);
	if (i != -1) {
		alarmsArr[i] = alarmsArr[countAlarms - 1];
		memset(&alarmsArr[countAlarms - 1], 0, sizeof(Alarm));
		countAlarms--;
		printf("deleted alarm successfully \r\n");
		writeToFlash();

	} else {
		printf("Name not exists \r\n");
	}
}

void managerInit() {
	rtcInit(&rtc, &hi2c1, 0XD0);
	cliInit();
	ledInit(&ledBlue, LEDBLUE_GPIO_Port, LEDBLUE_Pin, BLINK_DELAY_DEFUALT,
			STATE_OFF);
	ledInit(&ledRed, LEDRED_GPIO_Port, LEDRED_Pin, BLINK_DELAY_DEFUALT,
			STATE_OFF);
	buzzerInit(&buzzer, &htim3, TIM_CHANNEL_1);
	buttonInit(&rightBtn, RIGHT_BTN, rightBtn_GPIO_Port, rightBtn_Pin);
	buttonInit(&leftBtn, LEFT_BTN, leftBtn_GPIO_Port, leftBtn_Pin);
	/*HAL_ADC_Start_IT(&hadc2);
		 HAL_NVIC_EnableIRQ(ADC1_2_IRQn);
		 HAL_ADC_Start_IT(&hadc2);*/
	readFromFlash();
}
void managerClearAll()
{
		for(int i = 0; i<countAlarms;i++){
			memset(&alarmsArr[i], 0, sizeof(Alarm));
		}
		countAlarms=0;
		writeToFlash();
}

static void editAlarmName(char *newName, int alarmIndex) {
	int newNamelen = strlen(newName);
	if (newNamelen == 0 || newNamelen > ALARM_NAME_LENGTH) {
		printf("alarm name must small then %d and big then 0 chars\r\n",
				ALARM_NAME_LENGTH);
	} else {
		strcpy(alarmsArr[alarmIndex].alarmName, newName);
		printf("Alarm name update\r\n");
	}
}
static void editAlarmDate(int alarmIndex, int d, int m, int y) {
	if ((d > 0 && d <= 31) && (m > 0 && m <= 12) && (y > 0 && y <= 99)) {
		alarmsArr[alarmIndex].alarmTime.day = d;
		alarmsArr[alarmIndex].alarmTime.month = m;
		alarmsArr[alarmIndex].alarmTime.year = y;
		printf("Alarm date update\r\n");
	} else {
		printf("Alarm date update failed\r\n");
	}
}
static void editAlarmTime(int alarmIndex, int h, int min) {
	if (h > 0 && h < 24 && min > 0 && min < 60) {
		alarmsArr[alarmIndex].alarmTime.min = min;
		alarmsArr[alarmIndex].alarmTime.hours = h;
		printf("Alarm time update\r\n");
	} else {
		printf("Alarm time update failed\r\n");
	}
}
static void editAlarmMelody(int alarmIndex, int melodey) {
	if (melodey >= 0 && melodey <= 2) {
		alarmsArr[alarmIndex].melodey = melodey;
		printf("Alarm melody update\r\n");
	} else {
		printf("Alarm melodey update failed\r\n");
	}
}

void managerEditAlarm(char *param)
{
	uint8_t edit = TRUE;
	const char s[4] = " .:";
		char *token;

		int alarmIndex = -1;

		token = strtok(param, s);
		alarmIndex = managerGetAlarmIndex(param);
		if(alarmIndex == -1){
			printf("can't find alarm name %s\r\n",token);
			return;
		}
		token = strtok(NULL, s);
		if(strcmp(token,"name") == 0){
			token = strtok(NULL, s);
			editAlarmName(token,alarmIndex);


		}else if(strcmp(token,"date") == 0){
			token = strtok(NULL, s);
			int d = atoi(token);

			token = strtok(NULL, s);
			int m = atoi(token);

			token = strtok(NULL, s);
			int y = atoi(token);
			editAlarmDate(alarmIndex,d,m,y);

		}else if(strcmp(token,"time") == 0){
			token = strtok(NULL, s);
			int h = atoi(token);

			token = strtok(NULL, s);
			int min = atoi(token);
			editAlarmTime(alarmIndex,h,min);

		}else if(strcmp(token,"melodey") == 0){
			token = strtok(NULL, s);
			int melodey = atoi(token);
			editAlarmMelody(alarmIndex,melodey);

		}else{
			printf("Unrecognized command\r\n");
			edit = FALSE;
		}
		if(edit == TRUE){
			writeToFlash();
		}
}

void managerAddAlarm(Alarm *alarm) {
	if (countAlarms >= MAX_ALARMS) {
		printf("list is full, please remove any alarm before adding \r\n");
		return;
	}
	memcpy(&alarmsArr[countAlarms], alarm, sizeof(Alarm));
	countAlarms++;
	printf("added alarm successfully \r\n");
	writeToFlash();
}

int managerGetAlarmIndex(char *alarmName) {
	int ret = -1;
	for (int i = 0; i < countAlarms; i++) {
		if (strcmp(alarmsArr[i].alarmName, alarmName) == 0) {
			ret = i;
			break;
		}
	}
	return ret;

}
static int findActiveAlarm(DateTime *dt) {
	for (int i = 0; i < countAlarms; i++) {
		if (alarmsArr[i].active == TRUE
				&& alarmsArr[i].alarmTime.year == dt->year
				&& alarmsArr[i].alarmTime.month == dt->month
				&& alarmsArr[i].alarmTime.day == dt->day
				&& alarmsArr[i].alarmTime.hours == dt->hours
				&& alarmsArr[i].alarmTime.min == dt->min) {
			return i;
		}

	}
	return -1;
}
void managerStopAlarm() {
	/*if(alarmsArr[activeAlarm].snoozeLeft > 0){
	 alarmsArr[activeAlarm].snoozeLeft--;
	 printf("Alarm %s snooze left %d\r\n",alarmsArr[activeAlarm].alarmName,alarmsArr[activeAlarm].snoozeLeft);
	 waitingToSnooze = SNOOZE_WAITING_TIME_SEC;
	 }else{
	 alarmsArr[activeAlarm].active = FALSE;
	 alarmsArr[activeAlarm].snoozeLeft = alarmsArr[activeAlarm].snoozeRepeat;
	 printf("Alarm %s Stop\r\n",alarmsArr[activeAlarm].alarmName);
	 activeAlarm = -1;
	 waitingToSnooze = -1;
	 }*/
	printf("Alarm %s Stop\r\n", alarmsArr[activeAlarm].alarmName);
	ledSetState(&ledRed, STATE_OFF);
	ledSetState(&ledBlue, STATE_OFF);
	buzzerOff(&buzzer);
}
static void StartAlarm() {
	//waitingToSnooze = SNOOZE_WAITING_TIME_SEC;
	printf("Alarm %s active\r\n", alarmsArr[activeAlarm].alarmName);
	ledSetState(&ledRed, STATE_BLINK);
	ledSetState(&ledBlue, STATE_BLINK);
	buzzerOn(&buzzer);
}

void startManagerTask(void *argument) {
	DateTime currTime;

	for (;;) {
		osMutexAcquire(rtcMutexHandle, osWaitForever);
		rtcGetTime(&rtc, &currTime);
		osMutexRelease(rtcMutexHandle);
		if (activeAlarm == -1) {
			activeAlarm = findActiveAlarm(&currTime);
		}
		if (activeAlarm != -1) {
			if (doAlarmFlag == FALSE) {
				StartAlarm();
				doAlarmFlag = TRUE;
				alarmOnCounter = MAX_ALARM_TIME;
			}
			if (alarmOnCounter > 0) {
				alarmOnCounter--;
				if (alarmOnCounter == 0) {
					managerStopAlarm();
					if (alarmsArr[activeAlarm].snoozeRepeat == 0) {
						alarmsArr[activeAlarm].active = FALSE;
						activeAlarm = -1;
					} else {
						if (snooze == -1) {
							snooze = alarmsArr[activeAlarm].snoozeRepeat;
						} else {
							snooze--;
						}

						waitingToSnooze = SNOOZE_WAITING_TIME_SEC;
					}

				}
			}
			if (snooze > 0) {
				if (waitingToSnooze > 0) {
					waitingToSnooze--;
					if (waitingToSnooze == 0) {
						printf("Alarm %s snooze left %d\r\n",
								alarmsArr[activeAlarm].alarmName, snooze);
						doAlarmFlag = FALSE;
					}

				}
			} else if (snooze == 0) {
				alarmsArr[activeAlarm].active = FALSE;
				managerStopAlarm();
				activeAlarm = -1;
			}
			//alarmsArr[activeAlarm].snoozeRepeat;
		}

		/*
		 if (activeAlarm != -1 && alarmOnCounter <= MAX_ALARM_TIME && waitingToSnooze >= -1) {
		 if(alarmOnCounter == 0){

		 StartAlarm();
		 }
		 alarmOnCounter++;
		 if(waitingToSnooze > 0){
		 waitingToSnooze--;
		 }

		 }
		 if (activeAlarm != -1 && alarmOnCounter >= MAX_ALARM_TIME) {
		 alarmOnCounter = 0;
		 managerStopAlarm();
		 }
		 */

		osDelay(1000);
	}

}
