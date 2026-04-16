/*
*********************************************************************
**
**    Project:  杆塔倾斜3.0版本
**
**    Componet: STM32L071CBT6
**
**    Filename: main.c
**
**    Abstract: 主函数
**
**    Compiler: KEIL 5.0
**           
**    Function List：  
**
**    History：
**
**        <auther>    <date>    <version>   <desc>
**
**
**    Copyright @cdstarriver All Rights Reserved.
**
*********************************************************************
*/


#include "main.h"

U8 g_u8HwVerison = 0x01;
U8 g_u8SwVerison = 0x02;
long SoftWare_Version = SW_VERSION;
long    u32CurrentMinutes = 0;
float SunPower = 0;
uint8_t EnterStopFlag = 1;
//设备指令，半小时内最多接收10条，第一个字节为收到的命令
u8 g_u8_Instruct[CACHE_IST_NUM];
uint8_t RecvCount;   //接收数据计数

U8 u8_ReciveFailCnt = 0;
u8 u8_cmd_num;

U8 u8_Fault_Cnt_Sx1 = 0;  
U8 u8_Fault_Cnt_Sx2 = 0;  
U8 u8_Fault_Cnt_Hx1 = 0;
U8 u8_Fault_Cnt_Hx2 = 0;

void SystemClock_Config(void);
void MX_IWDG_Init(void);
void MX_GPIO_Init(void);
void MX_TIM2_Init(void);
void SensorTypeInit(void);
void DataInit(void);
void ADXL355_EXTI_GPIO(void);
//static void MX_TIM3_Init(void);

TIM_HandleTypeDef htim3;


/*****************************************************************************************
Function Name       :  void delay(uint32_t time)
Description         :  延时函数
Input               :  uint32_t time
Output              :  无
                       
Author              :  
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 
void delay(uint32_t time)
{
    uint16_t j,i;
    for(i = time; i > 0; i--)
    {
        for(j = 0;j < 65535; j++)
        {
            ;
        }
    }
}



/*****************************************************************************************
Function Name       :  int main(void)
Description         :  主函数
Input               :  void
Output              :  无
                       
Author              :  YXD
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 
int main(void)
{
    delay(10);
    INT16U wReadTemp;
  	float power;
	  INT16U  i;
    SystemClock_Config();	
    HAL_Init();
    MX_GPIO_Init();
    VDD1_ON();	
    VDD2_ON();
    VDD3_ON();
	
//    MX_IWDG_Init();    // 打开看门狗 最长28s不喂狗则重启

    RTC_Init();
    ADC_Init();
    sEepromGPIOInit();

//    wReadTemp = sReadEeprom(unEeprom.wArray, unEeprom.wArray + cEepromLength - 1);
//    if(wReadTemp == 0)
//    {
//        wReadTemp = sReadEeprom(unEeprom.wArray, unEeprom.wArray + cEepromLength - 1);
//    }
//    if(wReadTemp == 0)
//    {
//        wReadTemp = sReadEeprom(unEeprom.wArray, unEeprom.wArray + cEepromLength - 1);
//    }

//    if((wReadTemp == 1)
//    && (unEeprom.stData.wEepromDataVer == cEepromDataVer))    //数据结构相同
//    {
//        sCheckSavedCfg();   //判断数据是否合理，不合理则重新配置EE内的数据并在下面重写
//        wEEpromDataStatus = 1;
//    }
//    else
//    {
//        //sCheckSavedCfg();
//        if((unEeprom.wArray[cEepromLength - 1] == 0xffff) //和校验错误且数据全为0xff会进来
//        && (unEeprom.wArray[cEepromLength - 2] == 0xffff)
//        && (unEeprom.wArray[cEepromLength - 3] == 0xffff))    //全为0xff表明eeprom芯片内容未初始化
//        {
//            sDefaultCfg();  	//有错误则手动配置EEPROM内的数据并在下面写数据
//            wEEpromDataStatus = 1;
//        }
//        else    //和校验错误但是数据不全为0xff/和校验正确但是数据版本不匹配---这两种情况会进来
//        {
//            sDefaultCfg();
//            wEEpromDataStatus = 1;
//        }
//    }
//    sEepromFirstWrite();   //判断是否需要写EEPROM
		
    DataInit();    		//数据初始化包含读取EEPROM数据
  	UART_Init();

		Get_WakeUp(); //唤醒  初始化外设
		g_SysData.u8_SensorType = SensorType_MPU6050;  //机种
		g_statecheck.bit.bTiltCalbFlag = cFlagYes;     //开机调平指令
		while(1)
		{
			HAL_Delay(1000);
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_12);
			power = VoltageDataADC_get( );//供电电源采集
			g_SysData.u8_BatVolt = power * 10.0;
			for (i=0;i<10;i++)
			{
				DataCommunication_TSK();  //
			}
				LoraMessageSend_Treat(cLoraOrder_TiltData);    //直接上传角度值
			}
		
}

/*****************************************************************************************
Function Name       :  DataInit
Description         :  数据初始化
Input               :  void
Output              :  void
                       
Author              :  Lh
Creation Date       :  2020/04/26
Revisor             :  2.0
Revision Date       :  2021/04/09
Note                :  None
******************************************************************************************/ 
void DataInit(void)
{
    ubitfloat f32temp;

    LoraCommDelayTime        = 0;       // 发送延时配置 
    LoraCommCallBackWaitTime = 1500;    // 等待回传时间配置 
    LoraCommWaitTime         = 4000;    // 通信周期配置 
    LimitValue = 0.1;
    CalibrationTime[0] = 23;   //校时时间
    CalibrationTime[1] = 59;
    CalibrationTime[2] = 5;



    g_statecheck.bit.bLora_SendingState = cLoraCommIdle;
    g_statecheck.byte.SensorState       = 0x0000;

    g_SysData.u16_TiltSend_K = 0x01;

    g_statecheck.bit.bPreIAP_Flag  = cFlagNo;
    g_statecheck.bit.bTimeSyncFlag = cFlagNo;
    //g_SysData.u16_SampTime = 30;                             //十分钟发送时间



    //读EEPROM数据

    g_SysData.u8_AlloID               = (U8)(unEeprom.stData.wRoutingLocalID & 0x00ff);
    g_SysData.u8_GatewayID            = (U8)(unEeprom.stData.wRoutingGWID & 0x00ff);

    g_SysData.u16_Tower_Num           = unEeprom.stData.wTowerNum;          //读取塔组号和序号

    g_SysData.u16_SysOnOffFlag        = unEeprom.stData.wSysOnOffSetBit;      // 优先读取是否有开机状态位
    g_SysData.u16_SampTime            = unEeprom.stData.wSampTimeSetValue;    // 
    g_SysData.u8_LowVoltTreshold      = (U8)(unEeprom.stData.wLowVoltTresholdSetValue & 0x00ff);
    g_SysData.u16_TiltWarningTreshold = unEeprom.stData.wTiltTresholdSetValue;
    g_SysData.u8_SleepTreshold        = (U8)(unEeprom.stData.wSleepTresholdSetValue & 0x00ff);

    f32temp.intdata[1].id = unEeprom.stData.wShunxianOffset_H;
    f32temp.intdata[0].id = unEeprom.stData.wShunxianOffset_L;
    sIncl_Data.f32_shunxian_Angle_offset = f32temp.fd;

    f32temp.intdata[1].id = unEeprom.stData.wHengxianOffset_H;
    f32temp.intdata[0].id = unEeprom.stData.wHengxianOffset_L;
    sIncl_Data.f32_hengxian_Angle_offset = f32temp.fd;

    f32temp.intdata[1].id = unEeprom.stData.wZongheOffset_H;
    f32temp.intdata[0].id = unEeprom.stData.wZongheOffset_L;
    sIncl_Data.f32_zonghe_Angle_offset = f32temp.fd;

    g_SysData.u8_SN[0] = unEeprom.stData.wLocalSNCode[0];
    g_SysData.u8_SN[1] = unEeprom.stData.wLocalSNCode[1];
    g_SysData.u8_SN[2] = unEeprom.stData.wLocalSNCode[2];
    g_SysData.u8_SN[3] = unEeprom.stData.wLocalSNCode[3];
    g_SysData.u8_SN[4] = unEeprom.stData.wLocalSNCode[4];
    g_SysData.u8_SN[5] = unEeprom.stData.wLocalSNCode[5];
    g_SysData.u8_SN[6] = unEeprom.stData.wLocalSNCode[6];
	
//	g_SysData.u16_AcclWarningTreshold =	unEeprom.stData.wAcclTresholdSetValue; //读EEprom的加速度阈值
//	g_SysData.u16_SleepGap = unEeprom.stData.wSleepGap;  //读Eeprom中模块睡眠唤醒间隔时间
	
//	g_SysData.u16_GatewayID[0] = unEeprom.stData.GatewayID[0]; //读EEPROM中的网关ID
//	g_SysData.u16_GatewayID[1] = unEeprom.stData.GatewayID[1];
//	g_SysData.u16_GatewayID[2] = unEeprom.stData.GatewayID[2];
//	g_SysData.u16_GatewayID[3] = unEeprom.stData.GatewayID[3];
//	g_SysData.u16_GatewayID[4] = unEeprom.stData.GatewayID[4];
//	g_SysData.u16_GatewayID[5] = unEeprom.stData.GatewayID[5];

}



/*****************************************************************************************
Function Name       :  SensorTypeInit
Description         :  机种识别
Input               :  void
Output              :  void
                       
Author              :  YXD
Creation Date       :  2020/08/24
Revisor             :  1.0
Revision Date       :  2020/08/24
Note                :  None
******************************************************************************************/ 

void SensorTypeInit(void)
{
    U8 temp = 0;
    temp = g_SysData.u8_SN[6] % 2;

    if(temp == 0)
    {
        g_SysData.u8_SensorType = SensorType_MPU6050;
    }
    else if(temp == 1)
    {
        g_SysData.u8_SensorType = SensorType_ADXL355;
    }
    else
    {
        g_SysData.u8_SensorType = SensorType_MPU6050;
    }

    temp = g_SysData.u8_SN[3];    //判定是否有LTE模块

//    if(temp == 0x03)
//    {
//        g_SysData.u8_LteModel = TypeWithLTE;
//    }
//    else
//    {
//        g_SysData.u8_LteModel = TypeWithOutLTE;
//    }
}

/*****************************************************************************************
Function Name       :  SystemClock_Config
Description         :  系统时钟配置
Input               :  void
Output              :  void
                       
Author              :  YXD
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct   = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage 
    */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
      /** Configure LSE Drive Capability 
    */
    //HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
    RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                 |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_RTC;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
    PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}


/*****************************************************************************************
Function Name       :  MX_TIM2_Init
Description         :  timer2配置
Input               :  void
Output              :  void
                       
Author              :  YXD
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 

TIM_HandleTypeDef htim2;
void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END   TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 31;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 9999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  
    //HAL_TIM_Base_Start(&htim2);
    HAL_TIM_Base_Start_IT(&htim2);
}


/*****************************************************************************************
Function Name       :  MX_IWDG_Init
Description         :  看门狗初始化
Input               :  void
Output              :  void
                       
Author              :  YXD
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 

void MX_IWDG_Init(void)
{

    /* USER CODE BEGIN IWDG_Init 0 */

    /* USER CODE END IWDG_Init 0   */

    /* USER CODE BEGIN IWDG_Init 1 */

    /* USER CODE END IWDG_Init 1   */
    hiwdg.Instance = IWDG;
    hiwdg.Init.Prescaler = IWDG_PRESCALER_256;    //IWDG_PRESCALER_64;
    hiwdg.Init.Window = 4095; //默认最大值
    hiwdg.Init.Reload = 4095;    //最长28秒  //time = 256*4095/37k
    if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN IWDG_Init 2 */

    /* USER CODE END IWDG_Init 2 */

}

/*****************************************************************************************
Function Name       :  MX_GPIO_Init
Description         :  GPIO配置
Input               :  void
Output              :  void
                       
Author              :  YXD
Creation Date       :  2020/04/26
Revisor             :  1.0
Revision Date       :  2020/04/26
Note                :  None
******************************************************************************************/ 

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    //PB4 PA3  power_EN_1
    GPIO_InitStruct.Pin   = GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = GPIO_PIN_4;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = GPIO_PIN_15;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
		
		//LED RUN
		GPIO_InitStruct.Pin   = GPIO_PIN_12;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
 //   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
		

}

void ADXL355_EXTI_GPIO()
{
	//355外部中断配置
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin : ADXL_355INT1_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN; //上升沿触发 下拉
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

}

void MPU6050_EXTI_GPIO(void)
{
	//6050外部中断配置
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOB_CLK_ENABLE();
 
	/*Configure GPIO pin :  */
//	GPIO_InitStruct.Pin = GPIO_PIN_9;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//	GPIO_InitStruct.Pull = GPIO_PULLDOWN; //上升沿触发 下拉
//	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
		GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;      //上升沿中断触发模式
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;//GPIO_PULLDOWN;              //下拉
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);	//电平拉低

	
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

}


static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */
  //200*400/16M = 5ms 进一次TIMER3中断
  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 199;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 399;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
* @brief  This function is executed in case of error occurrence.
* @retval None
*/
void Error_Handler(void)
{
/* USER CODE BEGIN Error_Handler_Debug */
/* User can add his own implementation to report the HAL error return state */

/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
