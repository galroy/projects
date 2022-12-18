/*
 * CliCommand.cpp
 *
 *  Created on: Nov 26, 2022
 *      Author: USER-PC
 */

#include <stdio.h>
#include "Flash.h"
#include "CliCommand.h"
#include "fatfs.h"
#include "Manager.h"
#include "logger.h"

extern osMutexId_t logFileMutexHandle;
extern osMutexId_t alertFileMutexHandle;

#define MAX_LEN 80

void setWarningTemperatureCmd::doCommand(const char *param) {

	if (param == nullptr) {
		printf("setwarning must contain value\r\n");
		return;
	}
	m_pTemperatureBoundary->m_warning = atof(param);
	if (m_pTemperatureBoundary->m_warning
			>= m_pTemperatureBoundary->m_critical) {
		printf("Warning temperature must < critical temperature\r\n");
		return;
	}
	Flash f;
	m_pTemperatureBoundary->m_magicNumber = TEMPERATURE_BOUNDARY_MAGIC_NUM;
	HAL_StatusTypeDef status;

	status = f.erasePage(PAGE_256_ADRESS, 1);
	if (status != HAL_OK) {
		LOG_ERROR("error in erase page in line %d", __LINE__);
		return;
	}
	status = f.writeToPage(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE_256_ADRESS,
			m_pTemperatureBoundary, (sizeof(temperatureBoundary)));
	if (status != HAL_OK) {
		LOG_ERROR("error in write to page in line %d", __LINE__);

	} else {
		LOG_INFO("Warning boundary saved to flash\r\n");
	}
}

void RTCClockSetDateTimeCmd::doCommand(const char *param) {
	DateTime dt;
	char t[50];
	strcpy(t, param);
	if (setDateTime(&dt, t) == true) {
		m_pRtcClock->setTime(&dt);
	} else {
		LOG_INFO("set date time fail\r\n");
	}
}

bool RTCClockSetDateTimeCmd::setDateTime(DateTime *dateTime, char *param) {
	// if string is empty
	if (*param == '\0') {
		return false;
	}
	if (strlen(param) != 17) {
		printf("date string length = %d\r\n", strlen(param));
		return false;
	}
	bool ret = true;
	const char s[4] = ". :";
	char *token;

	/* get the first token */
	token = strtok(param, s);

	/* walk through other tokens */
	do {
		dateTime->day = atoi(token);
		if (dateTime->day < 0 || dateTime->day > 31) {
			ret = false;
			break;
		}
		token = strtok(NULL, s);
		dateTime->month = atoi(token);
		if (dateTime->month < 0 || dateTime->month > 12) {
			ret = false;
			break;
		}
		token = strtok(NULL, s);
		dateTime->year = atoi(token);
		if (dateTime->year < 0 || dateTime->year > 99) {
			ret = false;
			break;
		}
		token = strtok(NULL, s);
		dateTime->hours = atoi(token);
		if (dateTime->hours < 0 || dateTime->hours > 23) {
			ret = false;
			break;
		}

		token = strtok(NULL, s);
		dateTime->min = atoi(token);
		if (dateTime->min < 0 || dateTime->min > 59) {
			ret = false;
			break;
		}
		token = strtok(NULL, s);
		dateTime->sec = atoi(token);
		if (dateTime->sec < 0 || dateTime->sec > 59) {
			ret = false;
			break;
		}

	} while (0);

	return ret;

}

void setCriticalTemperatureCmd::doCommand(const char *param) {

	if (param == nullptr) {
		printf("setcritical must contain value\r\n");
		return;
	}
	m_pTemperatureBoundary->m_critical = atof(param);
	if (m_pTemperatureBoundary->m_critical
			<= m_pTemperatureBoundary->m_warning) {
		printf("Critical temperature must > warning temperature\r\n");
		return;
	}
	Flash f;

	m_pTemperatureBoundary->m_magicNumber = TEMPERATURE_BOUNDARY_MAGIC_NUM;
	HAL_StatusTypeDef status;

	status = f.erasePage(PAGE_256_ADRESS, 1);
	if (status != HAL_OK) {
		LOG_DEBUG("error in erase page in line %d\r\n", __LINE__);
		return;
	}
	status = f.writeToPage(FLASH_TYPEPROGRAM_DOUBLEWORD, PAGE_256_ADRESS,
			m_pTemperatureBoundary, (sizeof(temperatureBoundary)));
	if (status != HAL_OK) {
		LOG_ERROR("error in write to page in line %d\r\n", __LINE__);
	} else {
		printf("saved in flash \r\n");
	}

}

void cleanLogFileCmd::doCommand(const char *param) {
	FRESULT fres;
	FIL fil;
	osMutexAcquire(logFileMutexHandle, osWaitForever);
	fres = f_open(&fil, LOG_FILE_NAME, FA_CREATE_ALWAYS);
	f_close(&fil);
	osMutexRelease(logFileMutexHandle);
	if (fres != FR_OK) {
		printf("fail open log file\r\n");
	} else {
		printf("Delete %s Succeeded\r\n", LOG_FILE_NAME);
	}

}

void readLogFileCmd::doCommand(const char *param) {
	FRESULT fres;
	FIL fil;
	char buffer[MAX_LEN];
	printf("Begin log\r\n");
	osMutexAcquire(logFileMutexHandle, osWaitForever);
	fres = f_open(&fil, LOG_FILE_NAME, FA_READ);
	if (fres == FR_OK) {
		while (!f_eof(&fil)) {
			f_gets(buffer, MAX_LEN, &fil);
			printf("%s\r", buffer);
		}
	}
	f_close(&fil);
	osMutexRelease(logFileMutexHandle);
	printf("End log\r\n");
}

void cleanAlertLogFileCmd::doCommand(const char *param) {
	FRESULT fres;
	FIL fil;
	osMutexAcquire(alertFileMutexHandle, osWaitForever);
	fres = f_open(&fil, ALERT_FILE_NAME, FA_CREATE_ALWAYS);
	f_close(&fil);
	osMutexRelease(alertFileMutexHandle);
	if (fres != FR_OK) {
		printf("fail open log file\r\n");
	} else {
		printf("Delete %s Succeeded\r\n", ALERT_FILE_NAME);
	}

}

void readAlertLogFileCmd::doCommand(const char *param) {
	FRESULT fres;
	FIL fil;
	char buffer[MAX_LEN];
	printf("Begin alert log\r\n");
	osMutexAcquire(alertFileMutexHandle, osWaitForever);
	fres = f_open(&fil, ALERT_FILE_NAME, FA_READ);
	if (fres == FR_OK) {
		while (!f_eof(&fil)) {
			f_gets(buffer, MAX_LEN, &fil);
			printf("%s\r", buffer);
		}
	}
	f_close(&fil);
	osMutexRelease(alertFileMutexHandle);
	printf("End alert log\r\n");
}

