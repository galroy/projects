/*
 * CliCommand.h
 *
 *  Created on: 7 בנוב׳ 2022
 *      Author: USER-PC
 */

#ifndef INC_CLICOMMAND_H_
#define INC_CLICOMMAND_H_

#include <string.h>
#include <stdlib.h>     /* atoi */

#include "LedGpioTask.h"
#include "RTCClock.h"
#include "DHT.h"
#include "Structs.h"

enum{
	MAX_COMMAND_NAME_LEN = 15
};

class CliCommand{
private:
	char m_commandName[MAX_COMMAND_NAME_LEN];
public:
	CliCommand(char *commandName){
		strcpy(m_commandName,commandName);
	}
	virtual ~CliCommand(){}
	virtual void doCommand(const char *param) = 0;
	const char *getCommandName() const {return m_commandName;}
};

class LedOnCmd : public CliCommand{
private:
	LedGpioTask* m_pLed;
public:
	LedOnCmd(char *commandName,LedGpioTask* led):CliCommand(commandName),m_pLed(led){}
	void doCommand(const char *param){
		m_pLed->on();
	}

};

class LedOffCmd : public CliCommand{
private:
	LedGpioTask* m_pLed;
public:
	LedOffCmd(char *commandName,LedGpioTask* led):CliCommand(commandName),m_pLed(led){}
	void doCommand(const char *param){
		m_pLed->off();
	}

};

class LedBlinkCmd : public CliCommand{
private:
	LedGpioTask *m_pLed;
public:
	LedBlinkCmd(char *commandName,LedGpioTask* led):CliCommand(commandName),m_pLed(led){}
	void doCommand(const char *param){
		if(strlen(param) == 0){
			m_pLed->blink(LED_DELAY);
		}else{
			m_pLed->blink(atoi(param));
		}

	}

};

class RTCClockSetDateTimeCmd : public CliCommand{
private:

	RTCClock* m_pRtcClock;
public:
	RTCClockSetDateTimeCmd(char *commandName,RTCClock* rtcClock) :
			CliCommand(commandName), m_pRtcClock(rtcClock) {
	}
	void doCommand(const char *param);
private:
	bool setDateTime(DateTime *dateTime,char *param);

};

class RTCClockShowDateTimeCmd : public CliCommand{
private:

	RTCClock* m_pRtcClock;
public:
	RTCClockShowDateTimeCmd(char *commandName,RTCClock* rtcClock) :
			CliCommand(commandName), m_pRtcClock(rtcClock) {
	}
	void doCommand(const char *param) {
		DateTime dt;
		dt = m_pRtcClock->getTime();
		printf("%02d:%02d:%02d %02d.%02d.%02d\r\n",
				dt.hours, dt.min, dt.sec,
				dt.day, dt.month, dt.year);
	}

};

class DHTGetTemperatureCmd : public CliCommand{
private:

	DHT* m_pDHT;
public:
	DHTGetTemperatureCmd(char *commandName,DHT* pDHT) :
			CliCommand(commandName), m_pDHT(pDHT) {
	}
	void doCommand(const char *param) {
		m_pDHT->setShowTemperatureFlag(true);
	}

};

class setWarningTemperatureCmd : public CliCommand{
private:

	temperatureBoundary* m_pTemperatureBoundary;
public:
	setWarningTemperatureCmd(char *commandName,temperatureBoundary* pTemperatureBoundary) :
			CliCommand(commandName), m_pTemperatureBoundary(pTemperatureBoundary) {
	}
	void doCommand(const char *param);

};

class setCriticalTemperatureCmd : public CliCommand{
private:

	temperatureBoundary* m_pTemperatureBoundary;
public:
	setCriticalTemperatureCmd(char *commandName,temperatureBoundary* pTemperatureBoundary) :
			CliCommand(commandName), m_pTemperatureBoundary(pTemperatureBoundary) {
	}
	void doCommand(const char *param);

};

class showBoundaryTemperatureCmd : public CliCommand{
private:

	temperatureBoundary* m_pTemperatureBoundary;
public:
	showBoundaryTemperatureCmd(char *commandName,temperatureBoundary* pTemperatureBoundary) :
			CliCommand(commandName), m_pTemperatureBoundary(pTemperatureBoundary) {
	}
	void doCommand(const char *param) {
		printf("Warning Temperature = %.2f\r\nCritical Temperature = %.2f\r\n",
				m_pTemperatureBoundary->m_warning,m_pTemperatureBoundary->m_critical);
	}

};

class cleanLogFileCmd : public CliCommand{
private:


public:
	cleanLogFileCmd(char *commandName,void* ) :
			CliCommand(commandName) {
	}
	void doCommand(const char *param);

};

class readLogFileCmd : public CliCommand{
private:


public:
	readLogFileCmd(char *commandName,void* ) :
			CliCommand(commandName) {
	}
	void doCommand(const char *param);

};

class cleanAlertLogFileCmd : public CliCommand{
private:


public:
	cleanAlertLogFileCmd(char *commandName,void* ) :
			CliCommand(commandName) {
	}
	void doCommand(const char *param);

};

class readAlertLogFileCmd : public CliCommand{
private:


public:
	readAlertLogFileCmd(char *commandName,void* ) :
			CliCommand(commandName) {
	}
	void doCommand(const char *param);

};





#endif /* INC_CLICOMMAND_H_ */
