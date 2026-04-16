#ifndef __SPI_H
#define	__SPI_H

#include "stm32l0xx_hal.h"

// SPI总线速度设置  
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		  4
#define SPI_SPEED_64 		  5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7

extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi1;


void SPI1_Init(void);
void SPI2_Init(void);
#endif
