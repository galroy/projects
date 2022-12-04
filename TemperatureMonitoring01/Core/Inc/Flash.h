/*
 * Flash.h
 *
 *  Created on: Nov 20, 2022
 *      Author: student
 */

#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#define PAGE_256_ADRESS 0x08080000

#include "main.h"



class Flash {
private:

public:
	Flash(){};
	virtual ~Flash(){};

	HAL_StatusTypeDef erasePage(uint32_t page, uint32_t NbPages);
	HAL_StatusTypeDef writeToPage(uint32_t TypeProgram, uint32_t Address, void* data, int dataSize);



};

#endif /* INC_FLASH_H_ */
