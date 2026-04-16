#include "rtc.h"
RTC_HandleTypeDef hrtc;
RTC_TimeTypeDef  RTC_Time;
RTC_DateTypeDef  RTC_Data;


//Real_Time_Clock_Obj sRTCTime;    //RTC时间全局结构体



void MX_RTC_Init(void)
{
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    //hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        //Error_Handler();
    }

}

void RTC_Init(void)
{
    MX_RTC_Init();

    RTC_Time.Hours = 0;
    RTC_Time.Minutes = 0;
    RTC_Time.Seconds = 0;
    RTC_Time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
    RTC_Time.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&hrtc, &RTC_Time,RTC_FORMAT_BIN);

    RTC_Data.Year = 20;
    RTC_Data.Month = 9;
    RTC_Data.Date = 13;
    HAL_RTC_SetDate(&hrtc, &RTC_Data,RTC_FORMAT_BIN);
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
//  if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 2396, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
//  {
//      //  _Error_Handler(__FILE__, __LINE__);
//  }
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}


void RTC_GetTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{

    HAL_RTC_GetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, sDate, RTC_FORMAT_BIN);

}

void RTC_SetTime(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime)
{

    HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN);
}


void  sMy_RTC_Set_Time(RTC_DateTypeDef *sDate, RTC_TimeTypeDef *sTime, unsigned char format)
{


    HAL_RTC_SetTime(&hrtc, &RTC_Time,RTC_FORMAT_BIN);

    HAL_RTC_SetDate(&hrtc, &RTC_Data,RTC_FORMAT_BIN);

    //HAL_RTC_SetTime(&hrtc, sTime, format);
    //HAL_RTC_SetDate(&hrtc, sDate, format);

	
}

void sMy_RTC_Get_Time(void)
{
	HAL_RTC_GetTime(&hrtc,&RTC_Time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&RTC_Data,RTC_FORMAT_BIN);
}

