/*
*********************************************************************
**
**    Project:  杆塔倾斜2.0版本
**
**    Componet: STM32L071CBT6
**
**    Filename: main.h
**
**    Abstract: 主函数header
**
**    Compiler: KEIL 5.0
**           
**    Function List：  
**
**    History：
**
**        <auther>    <date>    <version>   <desc>
**
**            YXD        20/04/26        1.0
**
**    Copyright @cdstarriver All Rights Reserved.
**
*********************************************************************
*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"
#include "stdint.h"

#include "string.h"
#include <math.h>
#include "bsp_i2c.h"
#include "inv_mpu.h"
#include "uart.h"
#include "adc.h"
#include "spi.h"
#include "eeprom.h"
#include "EX_Eeprom.h"
#include "sx1278.h"
#include "rtc.h"
#include "crc.h"
#include "mpu6050.h"
#include "ADXL355.h"
#include "InclinationCalc.h"
#include "pwr_mode_rtc.h"
#include "LoRa_TSK.h"
#include "SysManage_TSK.h"
#include "Tilt_TSK.h"
#include "uart.h"
#include "4g.h"
#include "4g_AT.h"
#include "flash.h"
#include "dma.h"
#define Gvalue (float)9.7830     //云南昆明G值

//#include "common.h"
//#include "ymodem.h"
//#include "terminal_program_transmission.h"
extern uint8_t SendDataFlag;

extern uint32_t TIM3_Count;
extern uint8_t DATABUFF[21];
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;
extern uint8_t Send_4G;

extern uint16_t Time10msNum;

extern uint8_t LPWStatus ;

extern U8 g_u8HwVerison;
extern U8 g_u8SwVerison;
extern u32 gb_u32_file_Size;
extern long u32CurrentMinutes;
extern long SoftWare_Version;
extern TIM_HandleTypeDef htim3;
extern uint8_t EnterStopFlag;
extern uint8_t Rssi;
#define SW_VERSION    20200920


#define SensorType_ADXL355     0x01
#define SensorType_MPU6050     0x02

#define TypeWithLTE            0x01
#define TypeWithOutLTE         0x00



#define  STA_Lora_NotReci        0
#define  STA_Lora_Reci           1

#define  STA_LTE_RX_H            1
#define  STA_LTE_RX_L            0
#define  STA_Data_Rx_H           1
#define  STA_Data_Rx_L           0

#define  GATEWAY_ID_NUM           6
#define  My_ID                    0x01
#define  STA_STROAGE_L           0
#define  STA_STROAGE_H           1
#define  CLORA_FRAME_STA_REPLY   0X0F
#define  CLORA_FRAME_STA_CMD     0X01

extern TIM_HandleTypeDef htim2;

extern u8 u8_cmd_num;
extern void TiltDataSend_LTE(void);
extern void ADXL355_EXTI_GPIO(void);
extern void MPU6050_EXTI_GPIO(void);

extern void delay(uint32_t time);
extern void EC20_GPIO_Init(void);
extern void EC20_EXTI_Init(void);
extern uint8_t Incl_Data_Ready;
extern float power;  //AD采样
extern uint8_t powerTemp;
extern uint8_t RecvCount;   //接收数据计数

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

// extern uint8_t UartReceiveBuff[10];

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

void Error_Handler(void);
void SystemClock_Config(void);
void MX_IWDG_Init(void);
void MX_GPIO_Init(void);
void MX_TIM2_Init(void);
void Boot_SendMeg(void);

#define cFlagNo     0x0
#define cFlagYes    0x1


#define cSysOff     0x0
#define cSysOn      0x1


#define VDD1_ON()     HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define VDD1_OFF()    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)
#define VDD2_ON()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET)
#define VDD2_OFF()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET)
#define VDD3_ON()     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define VDD3_OFF()    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)







typedef union 
{
    U16 id ;
    struct   packed_data
    {
        unsigned char highchar : 8;
        unsigned char lowchar  : 8;
    }bitdata;
}ubitint ;



typedef union 
{
    float fd ;
    ubitint intdata[2];   
    long  lData;  

}ubitfloat ;

typedef union
{
	float Xdata;
	uint8_t u8_Xdata[4];
}Xdata_Initstruct;

typedef union
{
	float Ydata;
	uint8_t u8_Ydata[4];
}Ydata_Initstruct;

typedef union
{
	float Zdata;
	uint8_t u8_Zdata[4];
}Zdata_Initstruct;

extern Xdata_Initstruct Xdata_Struct;
extern Ydata_Initstruct Ydata_Struct;
extern Zdata_Initstruct Zdata_Struct;
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
