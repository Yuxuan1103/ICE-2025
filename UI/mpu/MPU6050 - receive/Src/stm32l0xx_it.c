/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l0xx_it.h"
#include "stdio.h"
#include "eeprom.h"
#include "inv_mpu.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t SendDataFlag = 1;
uint8_t DATABUFF[21];
uint8_t cnt1;
unsigned int Task_Delay[10];   //Task_Delay[3] 倍用做Lora通信等待计时

extern void Uart_Send_Array(uint8_t *pData, uint16_t Size);
uint8_t Send_4G;
uint8_t Incl_Data_Ready = 0;
uint32_t TIM3_Count = 0;

Xdata_Initstruct Xdata_Struct;
Ydata_Initstruct Ydata_Struct;
Zdata_Initstruct Zdata_Struct;

//const uint8_t ExitTransparentModeBuff[3]={'+','+','+'};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern IWDG_HandleTypeDef hiwdg;
/* USER CODE BEGIN EV */




/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable Interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
	while (1)
	{
			/* USER CODE BEGIN W1_HardFault_IRQn 0 */


			// PERROR(ERROR,Memory Access Error!);
			//  Panic(r_sp);

			/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    uint8_t i ;
    uint8_t NumOfTask = 10;
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */

    for(i=0;i<NumOfTask;i++)
    {
        if(Task_Delay[i])
        {
            Task_Delay[i]--;
        }
    }

    if(LoraCommDelayTimeTick) LoraCommDelayTimeTick --;
    if(LoraCommWaitTimetick)  LoraCommWaitTimetick --;
    if(LoraCommCallBackTick)  LoraCommCallBackTick --;


  /* USER CODE END SysTick_IRQn 1 */
}


/*
void USART4_5_IRQHandler(void)
{

    //UartRx_Treat();
    
    //HAL_UART_Receive_IT(&huart4, &UartReceiveBuff[UartReceiveBuffIndex], 1);//打开中断
}
*/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
    /* USER CODE BEGIN TIM2_IRQn 0 */

    /* USER CODE END TIM2_IRQn 0 */
    HAL_TIM_IRQHandler(&htim2);

    /* USER CODE BEGIN TIM2_IRQn 1 */ 


    /* USER CODE END TIM2_IRQn 1 */
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == htim2.Instance)
	{
		HAL_TIM_Base_Stop_IT(&htim2);

		HAL_TIM_Base_Start_IT(&htim2);
	}
	if(htim->Instance == htim3.Instance)
	{
		
		XL355_Temp_Calc();
		XL355_Accel_Read();
		//printf("Accl:  X=%f  Y=%f  Z=%f\r\n", sIncl_Data.f32_Accl_Xaxis,sIncl_Data.f32_Accl_Yaxis,sIncl_Data.f32_Accl_Zaxis);
		//TiltDataSend_LTE();
//		Tilt_Angle_Calc(&sIncl_Data);
//		printf("Angle: X=%f  Y=%f  Z=%f\r\n",sIncl_Data.f32_shunxian_Angle_arc_o,sIncl_Data.f32_hengxian_Angle_arc_o,sIncl_Data.f32_zonghe_Angle_arc_o);
//		TIM3_Count++; //采样计数
//		
//		memset(DATABUFF, 0x00, sizeof(DATABUFF)); //清数组内容 先清除！

//		Xdata_Struct.Xdata = sIncl_Data.f32_Accl_Xaxis * Gvalue - sIncl_Data.f32_XAccl_offset;;  //float转uint8存储RAM 数据存放地址高位-低位 大端对齐
//		Ydata_Struct.Ydata = sIncl_Data.f32_Accl_Yaxis * Gvalue - sIncl_Data.f32_YAccl_offset;;
//		Zdata_Struct.Zdata = sIncl_Data.f32_Accl_Zaxis * Gvalue - sIncl_Data.f32_ZAccl_offset;;
//		
//		DATABUFF[0] = 0x68;						//包头
//		DATABUFF[1] = g_SysData.u16_GatewayID[0];
//		DATABUFF[2] = g_SysData.u16_GatewayID[1];
//		DATABUFF[3] = g_SysData.u16_GatewayID[2];
//		DATABUFF[4] = g_SysData.u16_GatewayID[3];
//		DATABUFF[5] = g_SysData.u16_GatewayID[4];
//		DATABUFF[6] = g_SysData.u16_GatewayID[5];
//		DATABUFF[7] = 0x48; 					//控制字
//		
//		DATABUFF[8]  = Xdata_Struct.u8_Xdata[3];
//		DATABUFF[9]  = Xdata_Struct.u8_Xdata[2];
//		DATABUFF[10] = Xdata_Struct.u8_Xdata[1];
//		DATABUFF[11] = Xdata_Struct.u8_Xdata[0];
//		
//		DATABUFF[12] = Ydata_Struct.u8_Ydata[3];
//		DATABUFF[13] = Ydata_Struct.u8_Ydata[2];
//		DATABUFF[14] = Ydata_Struct.u8_Ydata[1];
//		DATABUFF[15] = Ydata_Struct.u8_Ydata[0];
//		
//		DATABUFF[16]  =  Zdata_Struct.u8_Zdata[3];
//		DATABUFF[17]  =  Zdata_Struct.u8_Zdata[2];
//		DATABUFF[18]  =  Zdata_Struct.u8_Zdata[1];
//		DATABUFF[19]  =  Zdata_Struct.u8_Zdata[0];
//		
//		DATABUFF[20]  =  0x16;
		
//		memcpy(DATABUFF, Xdata_Struct.u8_Xdata, 4);
//		memcpy(DATABUFF + 4, Ydata_Struct.u8_Ydata, 4);
//		memcpy(DATABUFF + 8, Zdata_Struct.u8_Zdata, 4);
		
		//SendDataFlag = 1;

		//HAL_UART_Transmit_IT(&USART1_Handler, (uint8_t *)DATABUFF, sizeof(DATABUFF)); //串口中断发送
		HAL_UART_Transmit_DMA(&USART1_Handler, (uint8_t *)DATABUFF, sizeof(DATABUFF));  //串口DMA发送 透传
		//printf("DMA Send Done");

	}
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/*
void USART4_5_IRQHandler(void)
{

    //HAL_USART_IRQHandler(&USART4_Handler);
    UartRx_Treat();

    HAL_USART_Receive_IT(&USART4_Handler, &UartReceiveBuff[UartReceiveBuffIndex], 1);//打开中断
}
*/

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    //uint8_t  n;
    if(huart->Instance == USART4)
    {
        UartRx_Treat();
    }
    HAL_USART_Receive_IT(&USART4_Handler, &UartReceiveBuff[UartReceiveBuffIndex], 1);//????
}
*/


//RTC中断 回调函数
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    if(dogtimes < 6)  //  g_SysData.u16_SleepGap  *  20s  默认4320--24小时
    {
		LPWStatus = 1;
    }
    else
    {
		LPWStatus = 2;   //唤醒 
    }
}

/**
* @brief This function handles RTC global interrupt through EXTI lines 17, 19 and 20 and LSE CSS interrupt through EXTI line 19.
*/
void RTC_IRQHandler(void)
{
    /* USER CODE BEGIN RTC_IRQn 0 */

    /* USER CODE END RTC_IRQn 0 */

    HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);

    /* USER CODE BEGIN RTC_IRQn 1 */

    /* USER CODE END RTC_IRQn 1 */
}

//外部中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{

		if(GPIO_Pin == GPIO_PIN_9) //6050 EXTI  外部中断
		{
		  EnterStopFlag = 1;
		
		}
	if(GPIO_Pin == GPIO_PIN_3) //ADXL355 EXTI  外部中断
	{
		
		Incl_Data_Ready = 1;
		EnterStopFlag = 0;
		LPWStatus = 0;
		HAL_SPI_MspInit(&hspi1); //ADXL355
    
		ADXL355_SPI_Init();
		//ADXL355_EXTI_GPIO();
//		UART_Init();
		printf("enter 355\r\n");
		SPI_ADXL355_S_READ(XL355_STATUS); //清传感器中断标志
		SPI_ADXL355_S_READ(XL355_STATUS); //清传感器中断标志		
		
	}
	if(GPIO_Pin == GPIO_PIN_1)  // EC20 EXTI
	{			
		
		GPIO_DTR_L(); //唤醒4G
		GPIO_APREADY_L(); //主机通知4G已准备好
		//AT_4G_Qsclk(&EC20_obj,0);  //不需要AT指令即可唤醒，加在中断里导致卡死
		printf("Enter 4G Interrupt");
//		Boot_SendMeg();    //发送放在中断外执行！否则卡死在中断里
	}
	
}

//  * @brief This function handles EXTI line 2 to 3 interrupts.
void EXTI2_3_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_3_IRQn 0 */
	
  /* USER CODE END EXTI2_3_IRQn 0 */
//	LowPower_WakeUp();
	UART_Init();
	printf("enter 355\r\n");
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
  /* USER CODE BEGIN EXTI2_3_IRQn 1 */

  /* USER CODE END EXTI2_3_IRQn 1 */
}

void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */

  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
/**
  * @brief This function handles DMA1 channel 2 and channel 3 interrupts.
  */
void DMA1_Channel2_3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 0 */

  /* USER CODE END DMA1_Channel2_3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart1_tx);
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  /* USER CODE BEGIN DMA1_Channel2_3_IRQn 1 */

  /* USER CODE END DMA1_Channel2_3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line 0 and line 1 interrupts.
  */
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}

void EXTI4_15_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
	
//		LowPower_WakeUp();
//	UART_Init();
//	printf("enter 6050\r\n");
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}


