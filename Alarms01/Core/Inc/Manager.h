/*
 * Manager.h
 *
 *  Created on: Oct 26, 2022
 *      Author: USER-PC
 */

#ifndef INC_MANAGER_H_
#define INC_MANAGER_H_

#include "commonStrcut.h"

void managerInit();
void managerAddAlarm(Alarm* alarm);
int managerGetAlarmIndex(char *alarmName);
void managerStopAlarm();
void managerPrintAlarmsList();
void managerDeleteAlarm(char *alarmName);
void managerClearAll();
void managerEditAlarm(char *param);

#endif /* INC_MANAGER_H_ */

