#ifndef _FLASH_H
#define _FLASH_H
#include "main.h"
#define FLASHPAGE_SIZE             (0x80)        /* 128Bytes */
#define DataAddress     			0x08008000		//数据存放起始地址 page256
HAL_StatusTypeDef FLASH_ErasePage(uint32_t Page_Address, uint32_t Number_Pages);

uint8_t FlashProgramData(uint32_t address, uint8_t* buf_ptr1 , uint32_t size);


uint32_t STMFLASH_ReadWord(uint32_t faddr);






#endif




