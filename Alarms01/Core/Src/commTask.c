
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "commTask.h"
#include "main.h"
#include "cmsis_os.h"

#define MAX_BUFFER_LENGTH   50
#define MAX_COMMANDS      20

extern UART_HandleTypeDef huart2;

typedef struct Command_
{
	const char * name;
	HanlderFunc func;
	void * obj;
} Command;

static Command _commands[MAX_COMMANDS];
static int _commandsCount = 0;
static uint8_t _cmdbuffer[MAX_BUFFER_LENGTH];
static int _cmdcount = 0;
static int _cmdprint = 0;








void commTaskInit()
{
	_commandsCount = 0;
	memset(&_commands, 0, sizeof(_commands));
}

void registerCommand(const char * name, HanlderFunc func, void * obj)
{
	if (_commandsCount >= MAX_COMMANDS) {
		printf("Too many commands, cannot register\r\n");
		return;
	}
	_commands[_commandsCount].name = name;
	_commands[_commandsCount].func = func;
	_commands[_commandsCount].obj = obj;
	_commandsCount++;
}

int _write(int fd, char* ptr, int len) {
    HAL_UART_Transmit(&huart2, (uint8_t *) ptr, len, HAL_MAX_DELAY);
    return len;
}

int commTask()
{
	uint8_t ch;

	HAL_StatusTypeDef Status = HAL_UART_Receive(&huart2, &ch, 1, 0);

	if (Status != HAL_OK)
	{
		if ((huart2.Instance->ISR & USART_ISR_ORE) != 0)
		{
			__HAL_UART_CLEAR_OREFLAG(&huart2);
		}

		// here we have a time to print the command
		while (_cmdprint < _cmdcount)
		{
			HAL_UART_Transmit(&huart2, &_cmdbuffer[_cmdprint++], 1, 0xFFFF);
		}

		return 0;
	}

	if (ch == '\r' || ch == '\n')
	{
		// here we have a time to print the command
		while (_cmdprint < _cmdcount)
		{
			HAL_UART_Transmit(&huart2, &_cmdbuffer[_cmdprint++], 1, 0xFFFF);
		}

		HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, 0xFFFF);

		_cmdbuffer[_cmdcount] = 0;
		_cmdcount = 0;
		_cmdprint = 0;

		// command is ready
		return 1;
	}
	else if (ch == '\b')
	{
		char bs[] = "\b \b";
		_cmdcount--;
		_cmdprint--;
		HAL_UART_Transmit(&huart2, (uint8_t*)bs, strlen(bs), 0xFFFF);
	}
	else
	{
		if (_cmdcount >= MAX_BUFFER_LENGTH)
		{
			_cmdcount = 0;
			_cmdprint = 0;
		}

		_cmdbuffer[_cmdcount++] = ch;
	}

	return 0;
}
char * commLastCommand()
{
	return (char *)_cmdbuffer;
}



void handleCommand()
{
	char * param = NULL;

	// find space to split a command to a command name and parameters
	char * delimPtr = strchr((const char*)_cmdbuffer, ' ');
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

	for (int i = 0; i < _commandsCount; i++) {
		if (strcmp(_commands[i].name, (char *)_cmdbuffer) == 0) {
			_commands[i].func(_commands[i].obj, param);
			return;
		}
	}

	printf("Invalid command\r\n");

}

void commTaskprintCommands()
{
	printf("Available commands:\r\n");
	for (int i = 0; i < _commandsCount; i++) {
		printf("\t%s\r\n", _commands[i].name);
	}
}


void startCommTask(void *argument)
{

  for(;;)
  {
	  if (commTask()) {
	  	  handleCommand();
	  }
    osDelay(1);
  }

}
