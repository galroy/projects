/*
 * CliCommand.h
 *
 *  Created on: 7 בנוב׳ 2022
 *      Author: USER-PC
 */

#ifndef INC_IRCOMMAND_H_
#define INC_IRCOMMAND_H_

#include "LedGpioTask.h"
#include "RTCClock.h"
#include "DHT.h"
#include "Buzzer.h"

class IRCommand{
private:
//	char m_commandName[MAX_COMMAND_NAME_LEN];
	uint32_t m_commandNum;
public:
	IRCommand(uint32_t commandNum){
		m_commandNum = commandNum;
	}
	virtual ~IRCommand(){}
	virtual void doCommand() = 0;
	const uint32_t getCommandNum() const {return m_commandNum;}
};

class LedOnIR : public IRCommand{
private:
	LedGpioTask* m_pLed;
public:
	LedOnIR(uint32_t commandNum,LedGpioTask* led):IRCommand(commandNum),m_pLed(led){}
	void doCommand(){
		m_pLed->on();
	}

};

class LedOffIR : public IRCommand{
private:
	LedGpioTask* m_pLed;
public:
	LedOffIR(uint32_t commandNum,LedGpioTask* led):IRCommand(commandNum),m_pLed(led){}
	void doCommand(){
		m_pLed->off();
	}

};

class LedBlinkIR : public IRCommand{
private:
	LedGpioTask *m_pLed;
public:
	LedBlinkIR(uint32_t commandNum,LedGpioTask* led):IRCommand(commandNum),m_pLed(led){}
	void doCommand(){
		m_pLed->blink(LED_DELAY);
	}

};

class RTCClockShowDateTimeIR : public IRCommand{
private:
	RTCClock* m_pRtcClock;
public:
	RTCClockShowDateTimeIR(uint32_t commandNum,RTCClock* rtcClock) :
			IRCommand(commandNum), m_pRtcClock(rtcClock) {
	}
	void doCommand() {
		DateTime dt;
		dt = m_pRtcClock->getTime();
		printf("%02d:%02d:%02d %02d.%02d.%02d\r\n",
				dt.hours, dt.min, dt.sec,
				dt.day, dt.month, dt.year);
	}

};

class DHTGetTemperatureIR : public IRCommand{
private:
	DHT* m_pDHT;
public:
	DHTGetTemperatureIR(uint32_t commandNum,DHT* pDHT) :
			IRCommand(commandNum), m_pDHT(pDHT) {
	}
	void doCommand() {
		m_pDHT->setShowTemperatureFlag(true);
	}

};

class showBoundaryTemperatureIR : public IRCommand{
private:
	temperatureBoundary* m_pTemperatureBoundary;
public:
	showBoundaryTemperatureIR(uint32_t commandNum,temperatureBoundary* pTemperatureBoundary) :
			IRCommand(commandNum), m_pTemperatureBoundary(pTemperatureBoundary) {
	}
	void doCommand() {
		printf("Warning Temperature = %.2f\r\nCritical Temperature = %.2f\r\n",
				m_pTemperatureBoundary->m_warning,m_pTemperatureBoundary->m_critical);
	}

};

class stopBuzzerIR:public IRCommand{
private:
	Buzzer* m_pBuzzer;
public:
	stopBuzzerIR(uint32_t commandNum,Buzzer* pBuzzer) :
			IRCommand(commandNum), m_pBuzzer(pBuzzer) {
	}
	void doCommand() {
		if(m_pBuzzer->getState() == BUZZ_ON){
			m_pBuzzer->off();
			m_pBuzzer->setTemperatureAlarmStat(TAS_OFF_BY_USER);
				}
	}

};

#endif /* INC_IRCOMMAND_H_ */
