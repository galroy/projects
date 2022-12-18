/*
 * Manager.h
 *
 *  Created on: Nov 17, 2022
 *      Author: student
 */

#ifndef INC_MANAGER_H_
#define INC_MANAGER_H_

#include "main.h"
#include "fatfs.h"

#include "CommandTask.h"
#include "CliContainer.h"
#include "RTCClock.h"
#include "DHT.h"
#include "Flash.h"
#include "Buzzer.h"
#include "Structs.h"
#include "LedGpioTask.h"

#define LOG_FILE_NAME "log.txt"
#define ALERT_FILE_NAME "alert.txt"

enum temperatureAlarmStat{
	TAS_OFF,
	TAS_OFF_BY_USER,
	TAS_ON
};

enum systemState
{
	SYS_NORMAL,
	SYS_WARNING,
	SYS_CRITICAL,
	SYS_START
};

class Manager {
private:
	temperatureBoundary m_temperatureBoundary;

	RTCClock* m_pRtcClock;
	CommandTask* m_pCommandTask;

	CliContainer *m_pCliContainer;
	DHT* m_pDHT;
	Flash* m_pFlash;
	LedGpioTask* m_pRedLed;
	LedGpioTask* m_pBlueLed;
	Buzzer* m_pBuzzer;
	temperatureAlarmStat m_temperatureAlarmStat;
	systemState m_sysState;
	uint32_t m_logTimeStampSecend;
	uint32_t m_managerThreadDelay;

public:

	Manager(UART_HandleTypeDef *huart2,I2C_HandleTypeDef* hi2c1);
	virtual ~Manager();
	CommandTask* getCommandTask(){return m_pCommandTask;}
	DHT* getDHT(){return m_pDHT;}
	Buzzer* getBuzzer(){return m_pBuzzer;}

	LedGpioTask* getRedLed(){return m_pRedLed;}
	CliContainer* getCliContainer(){return m_pCliContainer;}

	bool getTemperatureBoundaryFromFlash();

	void setTemperatureAlarmStat(temperatureAlarmStat stat){m_temperatureAlarmStat = stat;}
	//uint32_t getLogTimeStampSecend(){return m_logTimeStampSecend;}
	uint32_t getManagerThreadDelay(){return m_managerThreadDelay;}
	void printLogToSDCARD(DateTime* pDT);
	void GetCurrDateTimeFromFlash(DateTime* pDT);
	void normalizManagerThreadDelay(DateTime* pDT);
	void printTemperature();
	void normalState();
	systemState getSysState(){return m_sysState;}
	void AlertLog(systemState sysState);
private:
	void warningState();
	void criticalState();
	void printAlertLogToSDCard(char* alertType,DateTime *pDT);
	void initCliContainer();
	void setTemperatureBoundary();
	FRESULT initSDCardFileSystem();


};

#endif /* INC_MANAGER_H_ */
