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
//#include "CliContainer.h"

#include "main.h"
#include "cmsis_os.h"

#define MAX_BUFFER_LENGTH   50
#define MAX_COMMANDS      20

class CommandTask{
private:
	UART_HandleTypeDef *m_huart2;

	//CliContainer* m_CliContainer;
	uint8_t m_cmdbuffer[MAX_BUFFER_LENGTH];
	int m_cmdcount;// = 0;
	int m_cmdprint;// = 0;
public:
	CommandTask(UART_HandleTypeDef* huart2){
		//m_CliContainer = nullptr;
		m_huart2 = huart2;
		m_cmdcount = 0;
		m_cmdprint = 0;
	}
	//CommandTask(){}
	virtual ~CommandTask(){};
	int commTask();
	void handleCommand();
	//void setCliContainer(CliContainer* CliContainer){m_CliContainer = CliContainer;}
private:

	char * commLastCommand();

};

#endif /* INC_COMMANDTASK_H_ */
