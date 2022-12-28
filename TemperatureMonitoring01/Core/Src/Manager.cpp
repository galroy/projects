/*
 * Manager.cpp
 *
 *  Created on: Nov 17, 2022
 *      Author: student
 */

#include "Manager.h"
#include "cmsis_os.h"
#include "logger.h"
#include "Flash.h"

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;

extern osMutexId_t logFileMutexHandle;
extern osMutexId_t alertFileMutexHandle;
extern osSemaphoreId_t DHT_SEMHandle;
extern Manager *m;

enum timeDefine {
	MILLISECONDS250 = 250,
	CYCLES250 = 250,
	SIXTY_SECONDS = 60,
	FOUR_SECONDS = 4,
	THOUSAND_MILLISECONDS = 1000
};

enum TemperatureBoundary {
	DEFULT_WARNING_TEMPERATURE = 26, DEFULT_CRITICAL_TEMPERATURE = 28
};

#define LOG_LEN		80

#define S_WARNING	(char*)"warning"
#define S_CRITICAL	(char*)"critical"
#define S_NORMAL	(char*)"normal"

Manager::Manager(UART_HandleTypeDef *huart2, I2C_HandleTypeDef *hi2c1) {

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	HAL_TIM_Base_Start_IT(&htim6);

	__HAL_TIM_SET_COUNTER(&htim3, 0);
	HAL_TIM_Base_Start_IT(&htim3);

	if (initSDCardFileSystem() != FR_OK) {
		LOG_INFO(
				"The file system is not initialized\r\nLog files will not be saved\r\nCheck if the SDCARD card is in place\r\n");
	}

	setTemperatureBoundary();

	m_sysState = SYS_START;

	m_pCommandTask = new CommandTask(huart2);
	m_pRtcClock = new RTCClock(hi2c1, 0XD0);
	m_pDHT = new DHT(GPIOB, GPIO_PIN_5, &htim7);
	m_pRedLed = new LedGpioTask(REDLED_GPIO_Port, REDLED_Pin);
	m_pBlueLed = new LedGpioTask(BLUELED_GPIO_Port, BLUELED_Pin);
	m_pBuzzer = new Buzzer(&htim3);
	m_pIrr = new InfraRedReceiver(IR_GPIO_Port, IR_Pin, &htim16);

	initCliContainer();
	initIRContainer();

	DateTime dt;
	dt = m_pRtcClock->getTime();
	if (m_pRtcClock->getSeconds(&dt, &m_logTimeStampSecend) == false) {
		LOG_INFO("Initialization m_logTimeStampSecend failed\r\n");
	}

	m_managerThreadDelay = MILLISECONDS250;

}

Manager::~Manager() {
	// TODO Auto-generated destructor stub
}

void Manager::initIRContainer() {
	m_pIRContainer = new IRContainer();

	LedOnIR *pLedOnIR = new LedOnIR(IR_COMMAND_1, m_pRedLed);
	m_pIRContainer->addCommand(pLedOnIR);

	LedOffIR *pLedOffIR = new LedOffIR(IR_COMMAND_2, m_pRedLed);
	m_pIRContainer->addCommand(pLedOffIR);

	LedBlinkIR *pLedBlinkIR = new LedBlinkIR(IR_COMMAND_3, m_pRedLed);
	m_pIRContainer->addCommand(pLedBlinkIR);

	RTCClockShowDateTimeIR *pRTCClockShowDateTimeIR =
			new RTCClockShowDateTimeIR(IR_COMMAND_4, m_pRtcClock);
	m_pIRContainer->addCommand(pRTCClockShowDateTimeIR);

	DHTGetTemperatureIR *pDHTGetTemperatureIR = new DHTGetTemperatureIR(
			IR_COMMAND_5, m_pDHT);
	m_pIRContainer->addCommand(pDHTGetTemperatureIR);

	showBoundaryTemperatureIR *pShowBoundaryTemperatureIR =
			new showBoundaryTemperatureIR(IR_COMMAND_6, &m_temperatureBoundary);
	m_pIRContainer->addCommand(pShowBoundaryTemperatureIR);

	stopBuzzerIR* pStopBuzzerIR =
				new stopBuzzerIR(IR_COMMAND_7, m_pBuzzer);
		m_pIRContainer->addCommand(pStopBuzzerIR);

}

void Manager::initCliContainer() {
	m_pCliContainer = new CliContainer();

	LedOnCmd *pLedOnCmd = new LedOnCmd((char*) "ledon", m_pRedLed);
	m_pCliContainer->addCommand(pLedOnCmd);

	LedOffCmd *pLedOffCmd = new LedOffCmd((char*) "ledoff", m_pRedLed);
	m_pCliContainer->addCommand(pLedOffCmd);

	RTCClockSetDateTimeCmd *pRTCClockSetDateTimeCmd =
			new RTCClockSetDateTimeCmd((char*) "setdatetime", m_pRtcClock);
	m_pCliContainer->addCommand(pRTCClockSetDateTimeCmd);

	RTCClockShowDateTimeCmd *pRTCClockShowDateTimeCmd =
			new RTCClockShowDateTimeCmd((char*) "showdate", m_pRtcClock);
	m_pCliContainer->addCommand(pRTCClockShowDateTimeCmd);

	DHTGetTemperatureCmd *pDHTGetTemperatureCmd = new DHTGetTemperatureCmd(
			(char*) "showtemp", m_pDHT);
	m_pCliContainer->addCommand(pDHTGetTemperatureCmd);

	setWarningTemperatureCmd *pSetWarningTemperatureCmd =
			new setWarningTemperatureCmd((char*) "setwarning",
					&m_temperatureBoundary);
	m_pCliContainer->addCommand(pSetWarningTemperatureCmd);

	setCriticalTemperatureCmd *pSetCriticalTemperatureCmd =
			new setCriticalTemperatureCmd((char*) "setcritical",
					&m_temperatureBoundary);
	m_pCliContainer->addCommand(pSetCriticalTemperatureCmd);

	showBoundaryTemperatureCmd *pShowBoundaryTemperatureCmd =
			new showBoundaryTemperatureCmd((char*) "showboundary",
					&m_temperatureBoundary);
	m_pCliContainer->addCommand(pShowBoundaryTemperatureCmd);

	cleanLogFileCmd *pCleanLogFileCmd = new cleanLogFileCmd((char*) "cleanlog",
			nullptr);
	m_pCliContainer->addCommand(pCleanLogFileCmd);

	readLogFileCmd *pReadLogFileCmd = new readLogFileCmd((char*) "readlog",
			nullptr);
	m_pCliContainer->addCommand(pReadLogFileCmd);

	cleanAlertLogFileCmd *pCleanAlertLogFileCmd = new cleanAlertLogFileCmd(
			(char*) "cleanalertlog", nullptr);
	m_pCliContainer->addCommand(pCleanAlertLogFileCmd);

	readAlertLogFileCmd *pReadAlertLogFileCmd = new readAlertLogFileCmd(
			(char*) "readalertlog", nullptr);
	m_pCliContainer->addCommand(pReadAlertLogFileCmd);

}

FRESULT Manager::initSDCardFileSystem() {
	FRESULT fres;
	FATFS fatFs;

	HAL_Delay(THOUSAND_MILLISECONDS);
	fres = f_mount(&fatFs, "", 1); //1=mount now
	if (fres != FR_OK) {
		LOG_ERROR("f_mount error (%i)\r\n", fres);
	}
	return fres;

}

void Manager::setTemperatureBoundary() {
	if (getTemperatureBoundaryFromFlash() == false) {
		m_temperatureBoundary.m_warning = DEFULT_WARNING_TEMPERATURE;
		m_temperatureBoundary.m_critical = DEFULT_CRITICAL_TEMPERATURE;
	}
}

bool Manager::getTemperatureBoundaryFromFlash() {

	temperatureBoundary *data = (temperatureBoundary*) (PAGE_256_ADRESS);
	memcpy(&m_temperatureBoundary, data, sizeof(temperatureBoundary));
	if (m_temperatureBoundary.m_magicNumber == TEMPERATURE_BOUNDARY_MAGIC_NUM) {
		return true;
	}
	return false;
}

void Manager::printAlertLogToSDCard(char *alertType, DateTime *pDT) {

	FRESULT fres;
	FIL fil;
	char buff[LOG_LEN] = { 0 };
	osMutexAcquire(alertFileMutexHandle, osWaitForever);
	fres = f_open(&fil, ALERT_FILE_NAME, FA_WRITE | FA_OPEN_APPEND);
	if (fres != FR_OK) {
		osMutexRelease(alertFileMutexHandle);
		LOG_ERROR("f_open error (%i)\r\n",fres);
		return;
	}

	sprintf(buff,
			"Alert state %-8s Temperature: %4.1f date time stamp %02d.%02d.%02d %02d:%02d:%02d\r\n",
			alertType, m_pDHT->getTempperature(), pDT->day, pDT->month,
			pDT->year, pDT->hours, pDT->min, pDT->sec);

	LOG_DEBUG("%s", buff);

	f_sync(&fil);
	if (f_printf(&fil, "%s", buff) == -1) {
		LOG_ERROR("print to log file fail\r\n");
	}
	f_close(&fil);
	osMutexRelease(alertFileMutexHandle);
	if (fres != FR_OK) {
		LOG_ERROR("f_close error (%i)\r\n", fres);
		return;
	}
}
void Manager::printLogToSDCARD(DateTime *pDT) {

	FRESULT fres;
	FIL fil;
	char buff[LOG_LEN] = { 0 };
	char sys[9];
	osMutexAcquire(logFileMutexHandle, osWaitForever);
	fres = f_open(&fil, LOG_FILE_NAME, FA_WRITE | FA_OPEN_APPEND);
	if (fres != FR_OK) {
		osMutexRelease(logFileMutexHandle);
		LOG_ERROR("f_open error (%i)\r\n", fres);
		return;
	}

	do {
		if (m_sysState == SYS_NORMAL) {
			strcpy(sys, S_NORMAL);
			break;
		}
		if (m_sysState == SYS_WARNING) {
			strcpy(sys, S_WARNING);
			break;
		}
		strcpy(sys, S_CRITICAL);
	} while (false);

	sprintf(buff,
			"system state %-8s Temperature: %4.1f date time stamp %02d.%02d.%02d %02d:%02d:%02d\r\n",
			sys, m_pDHT->getTempperature(), pDT->day, pDT->month, pDT->year,
			pDT->hours, pDT->min, pDT->sec);

	LOG_DEBUG("%s", buff);

	f_sync(&fil);
	if (f_printf(&fil, "%s", buff) == -1) {
		LOG_ERROR("print to log file fail\r\n");
	}
	f_close(&fil);
	osMutexRelease(logFileMutexHandle);
	if (fres != FR_OK) {
		LOG_ERROR("f_close error (%i)\r\n", fres);
		return;
	}

}

void Manager::normalizManagerThreadDelay(DateTime *pDT) {
	uint32_t sec;
	int delta;
	if (m_pRtcClock->getSeconds(pDT, &sec) == true) {
		delta = SIXTY_SECONDS - (int(sec - m_logTimeStampSecend));
		m_logTimeStampSecend = sec;
	}

	if (delta > FOUR_SECONDS || delta < -FOUR_SECONDS || delta == 0) {
		m_managerThreadDelay = MILLISECONDS250; //try back to norma
	} else {
		m_managerThreadDelay = m_managerThreadDelay
				+ (((delta * THOUSAND_MILLISECONDS) / SIXTY_SECONDS)
						/ FOUR_SECONDS);
	}

}

void Manager::printTemperature() {
	printf("Temperature: %4.1f\r\n", getDHT()->getTempperature());
	m_pDHT->setShowTemperatureFlag(false);
}

void Manager::warningState() {

	m_pBlueLed->off();
	m_pRedLed->on();
	m_pBuzzer->setTemperatureAlarmStat(TAS_OFF);
	m_pBuzzer->off();
	m_sysState = SYS_WARNING;
}
void Manager::criticalState() {

	m_pBlueLed->off();
	m_pRedLed->blink(300);

	if(m_pBuzzer->getTemperatureAlarmStat() == TAS_OFF){
		m_pBuzzer->setTemperatureAlarmStat(TAS_ON);
		m_pBuzzer->on();
	}
	m_sysState = SYS_CRITICAL;
}
void Manager::normalState() {

	m_pBlueLed->on();
	m_pRedLed->off();
	m_pBuzzer->setTemperatureAlarmStat(TAS_OFF);
	m_pBuzzer->off();
	m_sysState = SYS_NORMAL;
}

void Manager::GetCurrDateTimeFromFlash(DateTime *pDT) {
	*pDT = m_pRtcClock->getTime();
}

void Manager::AlertLog(systemState sysState) {
	DateTime dt;

	if (m_pDHT->getTempperature() >= m_temperatureBoundary.m_critical) {
		sysState = SYS_CRITICAL;
		if (sysState != m_sysState) {
			GetCurrDateTimeFromFlash(&dt);
			printAlertLogToSDCard(S_CRITICAL, &dt);
			criticalState();
		}
	} else {
		if (m_pDHT->getTempperature() >= m_temperatureBoundary.m_warning) {
			sysState = SYS_WARNING;
			if (sysState != m_sysState) {
				warningState();
				GetCurrDateTimeFromFlash(&dt);
				printAlertLogToSDCard(S_WARNING, &dt);
			}
		} else {
			sysState = SYS_NORMAL;
			if (sysState != m_sysState) {
				normalState();
			}
		}
	}

}

extern "C" void StartManagerTask(void *argument) {

	printf("temratur01 app by Gal Alroy\r\ntype help for help\r\n");

	DateTime dt;
	systemState sysState = SYS_NORMAL;
	static uint32_t minCounter = 0;
	for (;;) {
		minCounter++;
		if ((minCounter % CYCLES250) == 0) { //one minute
			m->GetCurrDateTimeFromFlash(&dt);
			m->normalizManagerThreadDelay(&dt);
			m->printLogToSDCARD(&dt);
		}
		if (m->getDHT()->hasData() == true) {
			if (m->getDHT()->getShowTemperatureFlag() == true) {
				m->printTemperature();
			}
			m->AlertLog(sysState);

			sysState = m->getSysState();
		}
		if (m->getIrr()->hasData()) {
			//printf("code = %lu\r\n", m->getIrr()->getCode());
			m->getIRContainer()->callCommand(
					m->getIrr()->getCode());
		}
		osDelay(m->getManagerThreadDelay());
	}

}

extern "C" void startReadDHTTask(void *argument) {
	for (;;) {
		osStatus_t status;
		status = osSemaphoreAcquire(DHT_SEMHandle, osWaitForever);
		if (status == osOK) {
			m->getDHT()->readSync();
		}
		osSemaphoreRelease(DHT_SEMHandle);

		osDelay(THOUSAND_MILLISECONDS);
	}
}
