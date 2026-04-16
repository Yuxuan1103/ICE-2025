#ifndef __CRC_H
#define	__CRC_H
#include "stdint.h"

extern uint16_t CRC16_1(uint8_t *pchMsg, uint16_t wDataLen, uint8_t *CRC16_H, uint8_t *CRC16_L);

extern uint16_t Cal_CRC16(uint8_t* data, uint32_t size, uint8_t *CRC16_H, uint8_t *CRC16_L);

extern uint8_t  SUM_Check(uint8_t *buf,uint8_t len);

#endif
