#ifndef __ADC_H
#define	__ADC_H
#include "stm32l0xx_hal.h"
#include "main.h"



void ADC_Init(void);
uint16_t ADCDataGet(uint32_t CH);
float VoltageDataADC_get(void);
extern ADC_HandleTypeDef hadc;
#endif
