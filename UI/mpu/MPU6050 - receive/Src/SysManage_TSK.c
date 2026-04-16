

#include "main.h"

void SysManage_TSK(void);

void LEDRun(void);
void RF_Frenq_Set(U16 * u16TowerNum);
void GetAllMinutes(void);



T_SysState g_statecheck;

T_SysData g_SysData;

U16 wSysState = cWaitState;    //终端上电后给到wait状态

uint8_t RunLED    = 0;
IWDG_HandleTypeDef hiwdg;
uint8_t LPWStatus = 0;








void SysManage_TSK(void)
{
    static U8  u8_LastMinu     = 0;    //用于存储上次调用的分钟变量


    Get_In_Stop(20,g_SysData.u8_SleepTreshold);    //连续100次低压3V 则进入STOP 1小时

    
    //TimeCalibrationTask(CalibrationTime[0], CalibrationTime[1], CalibrationTime[2]);    //设置校时时间
    //HeartBeatTask(HeartBeatTime[0], HeartBeatTime[1], HeartBeatTime[2]);    //设置发射心跳时间
	HAL_IWDG_Refresh(&hiwdg);    //喂狗
	//printf("System feed");
    //每24H进行一次校时
    //if((RTC_Time.Hours == 1)
    //&& (RTC_Time.Minutes == 1)
    //&& (RTC_Time.Seconds <= 5))
    //{
        //u8_LastMinu = 0;
    //}

    //若存在EEPROM数据存储，则不能直接重启进行烧录
    if((g_statecheck.bit.bIAP_Flag == cFlagYes)
    && (bEepromWriteFlag != 1))
    {
        g_statecheck.bit.bIAP_Flag = cFlagNo;
        HAL_NVIC_SystemReset();
    }




#ifdef LORASEND_DEBUG

    static U16 u16_MinuCnt2    = 0;
    static U8  u8_LastMinu2    = 0;    //用于存储上次调用的分钟变量

    //定时上送数据计时
    if(1)//(g_statecheck.bit.bLora_SendingState == cLoraCommIdle)    //LORA处于空闲状态时
    {
        u8_LastMinu = RTC_Time.Seconds;
        RTC_GetTime(&RTC_Data, &RTC_Time);    // 时间获取
        if(u8_LastMinu != RTC_Time.Seconds)
        {
            u16_MinuCnt ++;
        }

        //if(g_statecheck.bit.bSysTiltState == cFlagYes)
        //{
        //    u16_TimeIntervalTemp = 5;
        //}
        //else
        //{
            u16_TimeIntervalTemp = 30;//g_SysData.u16_SampTime;    //获取系统采样时间间隔设定值
        //}
        
        if(u16_MinuCnt >= u16_TimeIntervalTemp)
        {
            u16_MinuCnt = 0;
            g_statecheck.bit.bLoraTimingSendFlag = cFlagYes;
        }
        else
        {
            g_statecheck.bit.bLoraTimingSendFlag = cFlagNo;
        }
    }
#else    
    //正常发送

    
    //定时上送数据计时
    GetAllMinutes();    //RTC时钟

    //发送时间判断
    if(g_statecheck.bit.bSysTiltState == cFlagYes)
    {
        u8_LastMinu = u32CurrentMinutes % (5u);
    }
    else
    {
        u8_LastMinu = u32CurrentMinutes % g_SysData.u16_SampTime;
    }

    //收到计时时间同步标志
    //if(g_statecheck.bit.bTimeSyncFlag == cFlagYes)
    //{
        //g_statecheck.bit.bTimeSyncFlag = cFlagNo;
        //u16_MinuCnt = 0;
    //}
    
    
    if((u8_LastMinu == 0u)
    && (RTC_Time.Seconds <= 5u))//设定时间发送数据
    {
        g_statecheck.bit.bLoraTimingSendFlag = cFlagYes;
    }
    else
    {
        g_statecheck.bit.bLoraTimingSendFlag = cFlagNo;
    }






#endif

#ifdef LORARECE_DEBUG


    g_statecheck.bit.bLoraReceiveState = cFlagYes;


#else

    //窗口时间开放LORA接收
    if(g_SysData.u16_SysOnOffFlag == cSysOn)
    {
        if(g_statecheck.bit.bPreIAP_Flag == cFlagYes)    //存在等待烧录状态
        {
            g_statecheck.bit.bLoraReceiveState = cFlagYes;
        }
        else
        {
            RTC_GetTime(&RTC_Data, &RTC_Time);    // 时间获取
            if((RTC_Time.Minutes >= 13)
            && (RTC_Time.Minutes < 16))
            {
                g_statecheck.bit.bLoraReceiveState = cFlagYes;
            }
            else
            {
                g_statecheck.bit.bLoraReceiveState = cFlagNo;
            }
        }
    }

#endif

}


/*****************************************************************************************
Function Name       :  LEDRun
Description         : 
Input               :  
Output              :  无
                       
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 

void LEDRun(void)
{
    if(RunLED == 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        RunLED = 1;
    }
    else if(RunLED == 1)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        RunLED = 0;
    }
}


//配置频段

void RF_Frenq_Set(U16 * u16TowerNum)
{
    U16    u16_TowerLastNum;
//    u16_TowerLastNum = (*u16TowerNum);
//    u16_TowerLastNum = u16_TowerLastNum & 0x0f;
//    u16_TowerLastNum = u16_TowerLastNum % 10;
	 u16_TowerLastNum = WayID & 0x0f ;
	 u16_TowerLastNum = u16_TowerLastNum % 10;
	
//    switch(u16_TowerLastNum)
//    {

//        case 0:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x00;
//            g_SysData.u8_RF_FRENQ_L = 0x00;
//        break;

//        case 1:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x10;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 2:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x20;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 3:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x30;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 4:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x40;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 5:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x50;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 6:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x60;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 7:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x70;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 8:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x80;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        case 9:
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x90;
//            g_SysData.u8_RF_FRENQ_L = 0x00;

//        break;

//        default :
//            g_SysData.u8_RF_FRENQ_H = 0x6C;
//            g_SysData.u8_RF_FRENQ_M = 0x00;
//            g_SysData.u8_RF_FRENQ_L = 0x00;
//        break;

//    }

//            g_SysData.u8_RF_FRENQ_H = 0x6C;  //433M
//            g_SysData.u8_RF_FRENQ_M = 0x40;
//            g_SysData.u8_RF_FRENQ_L = 0x00;
		
		        g_SysData.u8_RF_FRENQ_H = 0xE4;   //915M
            g_SysData.u8_RF_FRENQ_M = 0xC0;
            g_SysData.u8_RF_FRENQ_L = 0x00;

//		        g_SysData.u8_RF_FRENQ_H = 0xD9; //860M
//            g_SysData.u8_RF_FRENQ_M = 0x00;
//            g_SysData.u8_RF_FRENQ_L = 0x24;
}


void GetAllMinutes(void)
{
    long u32temp1;
    long u32temp2;

    RTC_GetTime(&RTC_Data, &RTC_Time);    // 时间获取

    //获取天数序号
    switch(RTC_Data.Month)
    {
        case 1u:
            u32temp2 = 0u + RTC_Data.Date;
        break;

        case 2u:
            u32temp2 = 31u + RTC_Data.Date;
        break;

        case 3u:
            u32temp2 = 59u + RTC_Data.Date;
        break;

        case 4u:
            u32temp2 = 90u + RTC_Data.Date;
        break;

        case 5u:
            u32temp2 = 120u + RTC_Data.Date;
        break;

        case 6u:
            u32temp2 = 151u + RTC_Data.Date;
        break;

        case 7u:
            u32temp2 = 181u + RTC_Data.Date;
        break;

        case 8u:
            u32temp2 = 212u + RTC_Data.Date;
        break;

        case 9u:
            u32temp2 = 243u + RTC_Data.Date;
        break;

        case 10u:
            u32temp2 = 273u + RTC_Data.Date;
        break;
        
        case 11u:
            u32temp2 = 304u + RTC_Data.Date;
        break;
        
        case 12u:
            u32temp2 = 334u + RTC_Data.Date;
        break;
        
        default :
        break;
    }

    //当前天数的星期
    u32temp1 = RTC_Time.Hours * 60;
    u32temp1 = u32temp1 + RTC_Time.Minutes;

    u32CurrentMinutes = u32temp1 + ((u32temp2 - 1) * 1440u);

}







