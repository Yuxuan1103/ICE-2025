#ifndef __RTC_H
#define	__RTC_H
#include "stm32l0xx_hal.h"
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef  RTC_Time;
extern RTC_DateTypeDef  RTC_Data;

typedef struct
{
    u8 u8_year;
    u8 u8_month;
    u8 u8_day;
    u8 u8_hour;
    u8 u8_min;
    u8 u8_sec;
}Real_Time_Clock_Obj;

extern void RTC_Init(void);
extern void RTC_GetTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
extern void RTC_SetTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime);
extern void MX_RTC_Init(void);
extern void sMy_RTC_Get_Time(void);
extern void  sMy_RTC_Set_Time(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, unsigned char format);


#endif
