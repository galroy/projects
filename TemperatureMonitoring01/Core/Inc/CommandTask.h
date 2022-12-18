/*
 * CommandTask.h
 *
 *  Created on: Nov 17, 2022
 *      Author: student
 */

#ifndef INC_COMMANDTASK_H_
#define INC_COMMANDTASK_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "cmsis_os.h"

enum BuffLength
{
	MAX_BUFFER_LENGTH = 50
};

class CommandTask{
private:
	UART_HandleTypeDef *m_huart2;
	uint8_t m_cmdbuffer[MAX_BUFFER_LENGTH];
	int m_cmdcount;
	int m_cmdprint;
public:
	CommandTask(UART_HandleTypeDef* huart2){
		m_huart2 = huart2;
		m_cmdcount = 0;
		m_cmdprint = 0;
	}

	virtual ~CommandTask(){};
	int commTask();
	void handleCommand();

private:
	char * commLastCommand();
};

#endif /* INC_COMMANDTASK_H_ */
