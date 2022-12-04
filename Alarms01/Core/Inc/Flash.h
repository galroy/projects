/*
 * Flash.h
 *
 *  Created on: 4 Sep 2022
 *      Author: student
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_
#include "main.h"


#define PAGE_256 0x08080000
#define BANK_IN_USED 2

typedef enum
{
	ERASE,
	PROGRAM,
	END_PROGRAM
}FLASH_COMMAND;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t temp;
	uint8_t spare;
}MEASURE;


HAL_StatusTypeDef erasePage(int bank, int page, int NbPages);
HAL_StatusTypeDef writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize);
void erasePage_IT(int bank, int page, int NbPages);
void writeToPage_IT(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize);
void cheakFlash ();
void cheakFlash_IT();
void MyFlashInterruptHandler();
int getFreeIndex(uint32_t Address);
void initFlashAddress();
int indexAt();
void cheak3();

#endif /* INC_FLASH_H_ */
