/*
 * CommandTask.cpp
 *
 *  Created on: Nov 17, 2022
 *      Author: student
 */
#include "main.h"
#include "CommandTask.h"
#include "Manager.h"
extern Manager *m;





int CommandTask::commTask()
{
	uint8_t ch;

	//HAL_StatusTypeDef Status = HAL_UART_Receive(&huart2, &ch, 1, 0);
	HAL_StatusTypeDef Status = HAL_UART_Receive(m_huart2, &ch, 1, 0);
	if (Status != HAL_OK)
	{
		if ((m_huart2->Instance->ISR & USART_ISR_ORE) != 0)
		{
			//__HAL_UART_CLEAR_OREFLAG(&huart2);
			__HAL_UART_CLEAR_OREFLAG(m_huart2);
		}

		// here we have a time to print the command
		while (m_cmdprint < m_cmdcount)
		{
			//HAL_UART_Transmit(&huart2, &_cmdbuffer[_cmdprint++], 1, 0xFFFF);
			HAL_UART_Transmit(m_huart2, &m_cmdbuffer[m_cmdprint++], 1, 0xFFFF);
		}

		return 0;
	}

	if (ch == '\r' || ch == '\n')
	{
		// here we have a time to print the command
		while (m_cmdprint < m_cmdcount)
		{
			//HAL_UART_Transmit(&huart2, &_cmdbuffer[_cmdprint++], 1, 0xFFFF);
			HAL_UART_Transmit(m_huart2, &m_cmdbuffer[m_cmdprint++], 1, 0xFFFF);
		}

		//HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 0xFFFF);
		HAL_UART_Transmit(m_huart2, (uint8_t*)"\r\n", 2, 0xFFFF);

		m_cmdbuffer[m_cmdcount] = 0;
		m_cmdcount = 0;
		m_cmdprint = 0;

		// command is ready
		return 1;
	}
	else if (ch == '\b')
	{
		char bs[] = "\b \b";
		m_cmdcount--;
		m_cmdprint--;
		//HAL_UART_Transmit(&huart2, (uint8_t*)bs, strlen(bs), 0xFFFF);
		HAL_UART_Transmit(m_huart2, (uint8_t*)bs, strlen(bs), 0xFFFF);
	}
	else
	{
		if (m_cmdcount >= MAX_BUFFER_LENGTH)
		{
			m_cmdcount = 0;
			m_cmdprint = 0;
		}

		m_cmdbuffer[m_cmdcount++] = ch;
	}

	return 0;
}
char * CommandTask::commLastCommand()
{
	return (char *)m_cmdbuffer;
}

#include "CliContainer.h"

void CommandTask::handleCommand()
{
	char * param = NULL;

	// find space to split a command to a command name and parameters
	char * delimPtr = strchr((const char*)m_cmdbuffer, ' ');
	if (delimPtr != NULL) {
		// put null character to split command name and parameters
		*delimPtr = '\0';
		param = delimPtr + 1;

		// find first non space character - it's parameters beginning
		while (*param && *param == ' ') {
			param++;
		}

		if (*param == '\0') {
			param = NULL;
		}
	}

	if(m->getCliContainer()->callCommand((char *)m_cmdbuffer, param) == false){
		if(strcmp((char *)m_cmdbuffer,"help") == 0){
			for(int i = 0;i<m->getCliContainer()->getCommandCount();i++){
				printf("command name %s\r\n",m->getCliContainer()->getCliCommands(i)->getCommandName());
			}
		}else{
			printf("Invalid command\r\n");
		}

	}

}


extern "C" void StartCommTask(void *argument)
{

  for(;;)
  {

	  if(m->getCommandTask()->commTask()){
		  m->getCommandTask()->handleCommand();
	  }
    osDelay(1);
  }

}


