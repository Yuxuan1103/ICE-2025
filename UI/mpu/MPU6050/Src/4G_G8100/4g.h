#ifndef __4G__H
#define __4G__H

#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "4g_AT.h"
/*- Exported typedef -*/


#define  EC20_Reset_PORT         GPIOC   //G8100管脚配置
#define  EC20_PowerOn_PORT       GPIOC
#define  EC20_Wake_PORT          GPIOB
#define  EC20_APREADY_PORT       GPIOB   
#define  EC20_EXTI_GPIO_Port     GPIOB

#define  EC20_Reset_Pin      GPIO_PIN_5    //PC5
#define  EC20_PowerOn_Pin    GPIO_PIN_4	//PC4
#define  EC20_Wake_Pin       GPIO_PIN_0     //PB0
#define  EC20_APREADY_Pin    GPIO_PIN_2  //PB2 apready
#define  EC20_EXTI_Pin       GPIO_PIN_1


#define  CACHE_IST_NUM           11
#define  Max_Len_1               1024
#define  Mid_Len                 512
#define  Key_Wd_Num              4



#define GPIO_DTR_H()  HAL_GPIO_WritePin(EC20_Wake_PORT, EC20_Wake_Pin, GPIO_PIN_RESET);
#define GPIO_DTR_L()  HAL_GPIO_WritePin(EC20_Wake_PORT, EC20_Wake_Pin, GPIO_PIN_SET);

#define GPIO_APREADY_H()   HAL_GPIO_WritePin(EC20_APREADY_PORT, EC20_APREADY_Pin, GPIO_PIN_RESET);
#define GPIO_APREADY_L()   HAL_GPIO_WritePin(EC20_APREADY_PORT, EC20_APREADY_Pin, GPIO_PIN_SET);
extern void Emitte_4G_Data(u8 *buffer);

extern void sMy_4G_Emitte(u8 *buffer);
//extern 	ErrorStatus		Conn_Status_Flag;
extern 	AT_4G_Object_t 	G8100_obj;
extern  AT_4G_Object_t  EC20_obj;
extern	AT_4G_Conn_Info	Conn_Info;




typedef enum DEAL_RETURN_CODES {
    DEAL_SUCCES       = 0,  //无错误
    DEAL_FAIL         = 1,  //无错误
}Deal_err_t;

typedef struct
{
    u16 u16_HBt_time;
    u16 u16_Udr_Titl;
    u8  u8_Udr_Vol;
    u8  u8_Slp_Vol;
    u8  u8_year;
    u8  u8_month;
    u8  u8_day;
    u8  u8_hour;
    u8  u8_min;
    u8  u8_sec;
    u8  g_u8_Heartbeat_Interval;
    u16 g_u16_Sample_Interval;
    u16 g_u16_Sleep_Time;
    u16 g_u16_Online_Time;
    u32 g_u32_Hard_Rst;
}Node_Real_Drb_Data;//下发参数


extern u8 LTE_Rx_Buffer[Max_Len_1];
extern u8 LTE_Tx_Buffer[Max_Len_1];
extern Node_Real_Drb_Data sNode_Drb_Data;    //终端数据信息

extern u8 g_u8_Instruct[CACHE_IST_NUM];
extern u8 Cache_Buffer_ST[Mid_Len];



//FUNCTIONS
extern void EC20_4G_Init(void);
extern AT_4G_Status_t G8100_Init(void);
extern void LTE_Pack_Func(u8 type);
extern void LTE_ISR(void);
extern void G8100_Send_Cal(AT_4G_Object_t *obj,u8 *buffer, U16 u16_Send_Len);

extern void LTE_MainTsk(void);

extern void TimeCalb(void);

extern void sMy_4G_Handler(void);

extern void SendHeartBeat(void);

extern void DevSleep(void);

extern void Send_Accl_Mesg(void);

#endif /* 4G.h*/

