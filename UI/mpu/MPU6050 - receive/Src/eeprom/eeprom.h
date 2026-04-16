#ifndef __EEPROM_H
#define	__EEPROM_H

#include "stm32l0xx_hal.h"

#define PAGE_SIZE  (uint16_t)0x80   //3k
#define EEPROM_FIRST_ADDESS  DATA_EEPROM_BASE 
extern float LimitValue;
extern uint8_t SensorID, GateWayID;
extern uint8_t CalibrationTime[3],HeartBeatTime[3];
extern unsigned long LoraCommDelayTime, LoraCommDelayTimeTick, LoraCommWaitTime, LoraCommWaitTimetick, LoraCommCallBackWaitTime, LoraCommCallBackTick;
extern FLASH_EraseInitTypeDef EEPROM_EraseInit;
HAL_StatusTypeDef EEPROM_Erase(FLASH_EraseInitTypeDef *pEraseInit);
void EEPROM_Write_Array(uint32_t WriteAddress, uint8_t *pData, uint16_t Size);
void EEPROM_Read_Array(uint32_t ReadAddress,uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef EEPROM_Write_FourBytes(uint32_t WriteAddress, uint32_t Data);
uint8_t  EEPROM_Read_Byte(uint32_t WriteAddress);
void SensorConfig(void);
#endif
