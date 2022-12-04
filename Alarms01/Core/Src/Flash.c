/*
 * Flash.c
 *
 *  Created on: 4 Sep 2022
 *      Author: student
 */

#include "Flash.h"
#include <stdio.h>
#include <stdint.h>

#define DATA_SIZE 1024
#define PAGE_SIZE 2048
#define PAGE_FULL -1
#define PAGE_256 0x08080000
#define PAGE_257 0x08080800
#define PAGE_257_END 0x08080FFF

HAL_StatusTypeDef status = HAL_OK;
uint32_t* flashData = (uint32_t *)(0x08080000);
FLASH_COMMAND flashState;
static int index_it = 0;
uint32_t TypeProgram_it;
uint32_t Address_it;
void* data_it;
int dataSize_it;


HAL_StatusTypeDef erasePage(int bank, int page, int NbPages)
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef flashErase;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	flashErase.Banks = bank;
	flashErase.Page = page;
	flashErase.NbPages = NbPages;

	uint32_t pageError;
	HAL_FLASHEx_Erase(&flashErase, &pageError);
	if(pageError == 0xFFFFFFFF)
	{
		HAL_FLASH_Lock();
		return HAL_OK;
	}
	else
	{
		HAL_FLASH_Lock();
		return HAL_ERROR;
	}
}

void erasePage_IT(int bank, int page, int NbPages)
{
	flashState = ERASE;
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef flashErase;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	flashErase.Banks = bank;
	flashErase.Page = page;
	flashErase.NbPages = NbPages;
	HAL_FLASHEx_Erase_IT(&flashErase);

}

HAL_StatusTypeDef writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize)
{
	HAL_StatusTypeDef status;
	HAL_FLASH_Unlock();
	int index = 0;
	while (index < dataSize)
	{
		uint64_t Data =*(uint64_t*)(data+index);
		status = HAL_FLASH_Program(TypeProgram, Address+index, Data);
		if(status != HAL_OK)
		{
			return status;
		}
		index += sizeof(uint64_t);
	}
	HAL_FLASH_Lock();
	//printf("%d\r\n", status);
	return status;
}

void writeToPage_IT(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize)
{
	HAL_FLASH_Unlock();
	dataSize_it = dataSize;
	data_it = data;
	Address_it = Address;
	TypeProgram_it = TypeProgram;
	uint64_t Data =*(uint64_t*)(data);
	status = HAL_FLASH_Program_IT(TypeProgram, Address, Data);
}

void MyFlashInterruptHandler()
{
	if(flashState == ERASE)
	{
		flashState = PROGRAM;
	}

	if(flashState == PROGRAM)
	{
		if(index_it >= dataSize_it)
		{
			flashState = END_PROGRAM;
			HAL_FLASH_Lock();
		}
		else
		{
			if(status == HAL_OK)
			{
				index_it += sizeof(uint64_t);
			}
			uint64_t Data =*(uint64_t*)(data_it+index_it);
			status = HAL_FLASH_Program_IT(TypeProgram_it, Address_it+index_it, Data);
		}
	}

}

int getFreeIndex(uint32_t Address)
{
	for(int i = 0; i < PAGE_SIZE/sizeof(uint64_t); i++)
	{
		uint8_t address = *(uint8_t*)(Address + i*8);
		if(address == 0xFF)
		{
			return (Address + i*8);
		}
	}
	return PAGE_FULL;
}

void initFlashAddress()
{
	index_it = getFreeIndex(PAGE_256);
	if(index_it == PAGE_FULL)
	{
		index_it = getFreeIndex(PAGE_257);
	}
	if(index_it == PAGE_FULL)
	{
		erasePage_IT(FLASH_BANK_2, PAGE_256, 1);
		index_it = PAGE_256;
	}
}

void saveMeasure (MEASURE * measure)
{
	uint32_t TypeProgram = FLASH_TYPEPROGRAM_DOUBLEWORD;
	if(index_it >= PAGE_257_END)
	{
		HAL_FLASH_Unlock();
		erasePage_IT(FLASH_BANK_2, PAGE_256, 1);
		index_it = PAGE_256;
		writeToPage_IT(TypeProgram, PAGE_256, (void*)measure, sizeof(MEASURE));
	}
	else if (index_it == PAGE_257)
	{
		HAL_FLASH_Unlock();
		erasePage_IT(FLASH_BANK_2, PAGE_257, 1);
		index_it = PAGE_257;
		writeToPage_IT(TypeProgram, PAGE_257, (void*)measure, sizeof(MEASURE));
	}
	else
	{
		HAL_FLASH_Unlock();
		writeToPage_IT(TypeProgram, index_it, (void*)measure, sizeof(MEASURE));
	}
//	MEASURE * read = (MEASURE*)index_it; // validate saving in flash
//	printf("temp is: %d, hour: %d, min: %d, sec: %d, address = %x\r\n", read->temp, read->hour, read -> min, read ->sec, index_it);

	HAL_FLASH_Lock();
	index_it += sizeof(MEASURE);
}

void cheakFlash()//test for erasepage and writeToPage.
{
	int size = 24;
	char info [24] = "hello World";

	uint32_t Banks = FLASH_BANK_2;
	uint32_t page = 256;
	uint32_t NbPages = 1;

	if(erasePage(Banks, page, NbPages) == HAL_ERROR)
	{
		printf("Erase Error\r\n");
	}

	uint32_t TypeProgram = FLASH_TYPEPROGRAM_DOUBLEWORD;
	uint32_t Address = 0x08080000;
	if(writeToPage(TypeProgram, Address, info, size) != HAL_OK)
	{
		printf("write Error\r\n");
	}
	char * read = (char*)flashData;
	printf("%s\r\n", read);

}

void cheakFlash_IT()//test for writeToPage_IT and erasePage_IT
{
	int size = 2048;
	char info [2048] = "hello World in cheak flash it sasdadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsadsa";

	uint32_t Banks = FLASH_BANK_2;
	uint32_t page = 256;
	uint32_t NbPages = 1;
	erasePage_IT(Banks, page, NbPages);
	uint32_t TypeProgram = FLASH_TYPEPROGRAM_DOUBLEWORD;
	uint32_t Address = 0x08080000;
	writeToPage_IT(TypeProgram, Address, info, size);
	char * read = (char*)flashData;
	printf("%s\r\n", read);
	uint32_t freeAddr = getFreeIndex(0x08080000);
	printf("%d\r\n", freeAddr);

}

void cheak3()// test for read after restart. and for fine nextfreeAddress.
{
	char * read = (char*)flashData;
	printf("%s\r\n", read);
	uint32_t freeAddr = getFreeIndex(0x08080000);
	printf("%x\r\n", freeAddr);
}


int indexAt()// validate the index in the range of page256- 257.
{
	return index_it;
}


