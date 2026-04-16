
#ifndef __stop_mode_rtc_H
#define __stop_mode_rtc_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
extern uint32_t dogtimes;
extern uint32_t dogtimes2;

extern void _Error_Handler(char *, int);


void enter_stop_rtc(float stoptime);
void enter_standby_rtc(float standbytime);
void enter_sleep_rtc(float sleeptime);
void system_power_config(void);
void system_clock_config(void);
void LowPower_enter_stop(uint32_t stoptime);
void LowPower_WakeUp(void);
void Get_In_Stop(uint32_t limit_times, uint8_t limit_data);
void Get_WakeUp(void);
#endif

/************************ Johnking *****END OF FILE****/
