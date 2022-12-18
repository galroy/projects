/*
 * Flash.cpp
 *
 *  Created on: Nov 20, 2022
 *      Author: student
 */

#include "Flash.h"
#define PAGE_0_ADRESS	0x08000000
#define PAGE_SIZE		0x800
#define PAGE_256		256



HAL_StatusTypeDef Flash::erasePage(uint32_t page, uint32_t NbPages)
{
	HAL_StatusTypeDef ret;
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef flashErase;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	flashErase.Page = (page - PAGE_0_ADRESS) / PAGE_SIZE;
	flashErase.Banks = (flashErase.Page >= PAGE_256) + 1;
	flashErase.NbPages = NbPages;

	uint32_t pageError;
	ret = HAL_FLASHEx_Erase(&flashErase, &pageError);
	HAL_FLASH_Lock();
	return ret;
}


HAL_StatusTypeDef Flash::writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize)
{
	HAL_StatusTypeDef ret;
	HAL_FLASH_Unlock();
	int index = 0;
	while (index < dataSize)
	{
		uint64_t Data =*(uint64_t*)((uint8_t*)data+index);
		ret = HAL_FLASH_Program(TypeProgram, Address+index, Data);
		if(ret != HAL_OK)
		{
			break;
		}
		index += sizeof(uint64_t);
	}
	HAL_FLASH_Lock();

	return ret;
}


