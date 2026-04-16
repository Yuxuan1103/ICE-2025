/*
*********************************************************
** Filename: stop_mode.c
** Abstract: 使用STM32L151C8T6MCU，使用RTC唤醒STOP和STANDBY模式下的低功耗,低功耗时长可以人为的进行设置
**          设置低功耗时长时请参考头文件中关于时长的宏定义
** 使用注意事项：使用CubeMX生成函数时，在main()函数后会自动生成SystemClock_Config()函数，此程序中调用了该函数。
**          如果该函数在其他文件中，请将该.h文件加入，以免发生错误；
** Date : 2018-01-04 
** Author:
*********************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "pwr_mode_rtc.h"

RTC_HandleTypeDef RTCHandle;    //RTC结构体变量
//进入STOP模式低功耗，使用RTC功能唤醒，其中stoptime单位为S，如设置1，低功耗1秒后唤醒
void enter_stop_rtc(float stoptime)
{

    //uint32_t i;     //局部变量，用于计算低功耗时长
    //system_power_config();

    /* Disable Wakeup Counter */
    //HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    //i = stoptime * 2396;
    //HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    /* Enter Stop Mode */
    //HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    //SystemClock_Config();//初始化系统时钟


    
        /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();      //必须要加，否则多出1.4uA的电流
    /* Enable Fast WakeUP */
    HAL_PWREx_EnableFastWakeUp();

    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */

    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, stoptime*2396, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    system_power_config();

    /* Enter Stop Mode */
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);


    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);          //必须要加，否则多出1.4uA的电流

    __HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();    //清除标志，否则第二次以后无法进入休眠


    SystemClock_Config();

}

//进入STANDBY模式低功耗，使用RTC功能唤醒，其中standbytime单位为S，如设置1，低功耗1秒后唤醒
void enter_standby_rtc(float standbytime)
{
    uint32_t i;         //局部变量，用于计算低功耗时长
    system_power_config();

    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
    {
        /* Clear Standby flag */
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB); 
    }
  
    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    i = standbytime*2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    /* Clear all related wakeup flags */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

     /* Enter the Standby mode */
    HAL_PWR_EnterSTANDBYMode();
}

//进入SLEEP模式低功耗，使用RTC功能唤醒，其中sleeptime单位为S，如设置1，低功耗1秒后唤醒
void enter_sleep_rtc(float sleeptime)
{
    uint32_t i;     //局部变量，用于计算低功耗时长

    system_power_config();

    /* Disable Wakeup Counter */
    HAL_RTCEx_DeactivateWakeUpTimer(&RTCHandle);

    /*To configure the wake up timer to 4s the WakeUpCounter is set to 0x242B:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
    Wakeup Time Base = 16 /(~37KHz) = ~0,432 ms
    Wakeup Time = ~5s = 0,432ms  * WakeUpCounter
    ==> WakeUpCounter = ~5s/0,432ms = 11562 */
    i = sleeptime*2396;
    HAL_RTCEx_SetWakeUpTimer_IT(&RTCHandle, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    
    /*Suspend Tick increment to prevent wakeup by Systick interrupt. 
    Otherwise the Systick interrupt will wake up the device within 1ms (HAL time base)*/
    HAL_SuspendTick();

    /* Enter Sleep Mode , wake up is done once Key push button is pressed */
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    /* Resume Tick interrupt if disabled prior to sleep mode entry*/
    HAL_ResumeTick();
}

//低功耗关闭项
void system_power_config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();

    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    // __HAL_RCC_GPIOC_CLK_ENABLE();
    // __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    // __HAL_RCC_GPIOE_CLK_ENABLE();

    /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
    /* Note: Debug using ST-Link is not possible during the execution of this         */
    /*       example because communication between ST-link and the device             */
    /*       under test is done through UART. All GPIO pins are disabled (set         */
    /*       to analog input mode) including  UART I/O pins.                          */
    GPIO_InitStructure.Pin = GPIO_PIN_All;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    //	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure);
    //	HAL_GPIO_Init(GPIOE, &GPIO_InitStructure);

    /* Disable GPIOs clock */
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    // __HAL_RCC_GPIOC_CLK_DISABLE();
    // __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();
    // __HAL_RCC_GPIOE_CLK_DISABLE();

    /* Configure RTC */
    RTCHandle.Instance = RTC;
    /* Configure RTC prescaler and RTC data registers as follow:
    - Hour Format = Format 24
    - Asynch Prediv = Value according to source clock
    - Synch Prediv = Value according to source clock
    - OutPut = Output Disable
    - OutPutPolarity = High Polarity
    - OutPutType = Open Drain */
    RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv   = 0x7C;
    RTCHandle.Init.SynchPrediv    = 0x0127;
    RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
    {
        /* Initialization Error */
        // Error_Handler(); 
    }
}

/****************************************************************************
* 名    称：
* 功    能： 进入STOP模式
* 入口参数：
* 出口参数：
* 说    明：
****************************************************************************/

uint32_t dogtimes = 0;
void LowPower_enter_stop(uint32_t stoptime)
{
    uint32_t i;
    SX1278_SetSleepMode();    // LORA模块休眠

	VDD1_OFF();   //PB4
	//VDD2_OFF(); //ADXL355 PA3
	//VDD3_OFF(); //EC20   PA15
//	SPI_ADXL355_S_WRITE(XL355_RESET,0x52);   //重启ADXL355
//	SPI_ADXL355_S_WRITE(XL355_POWER_CTL, 0x01);   //配置寄存器为StandBy模式 待机 

		//读1278每个IO口状态
//		printf("SX1278:  %d",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_12)); //1 CS
//		printf("%d",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_13)); //0 SCLK
//		printf("%d",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14));//0 MISO
//		printf("%d",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15));//1 MOSI
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12));//1   RST
//	
//		//1278的DIO0-5 浮空输入功耗更低！此处不需要配置
//		printf("%d",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_7)); //0
//		printf("%d",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_8));//0
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_11));//0
//	
//		printf("%d",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_6));//0
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8));//0
//		printf("%d\r\n",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_9));//0
//		
//		//读355每个IO口状态
//		printf("ADXL：%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7));//0
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6));//0
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5));//0
//		printf("%d",HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4));//1
//		printf("%d",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2));//0 INT2
//		printf("%d\r\n",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8));//0或1 DataReady单独判断 已断开

//		//读EEPROM IO口 原理图为上拉
//		printf("EEPROM： %d",HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_11)); //
//		printf("%d",HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2));  //
//		printf("%d\r\n",HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3));//
		
		while(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)  //清标志位再开中断，否则会多次触发影响开关效果
		{ 
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
			
		}
		//开ADXL355中断
	//	ADXL355_EXTI_GPIO();
	//	ADXL355_Init(); //重新初始化355，否则有可能在下次触发中断时不能进中断
		I2C_Bus_Init();
    MPU6050_Init();
//		MPU6050_EXTI_GPIO();
		printf("MPU6050 INTR OPENED\n");

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();
		 SysTick->CTRL = 0x00;//
     SysTick->VAL = 0x00;//
    /* Enable Ultra low power mode */
    HAL_PWREx_EnableUltraLowPower();

    /* Enable the fast wake up from Ultra low power mode */
    HAL_PWREx_EnableFastWakeUp();
	GPIO_InitTypeDef GPIO_InitStructure = {0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();

	//EEPROM
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	GPIO_InitStructure.Pin = GPIO_PIN_2;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure); 

	GPIO_InitStructure.Pin = GPIO_PIN_3;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//SX1278 PB12,15
	GPIO_InitStructure.Pin = GPIO_PIN_12 | GPIO_PIN_15;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure); 
	//SX1278 PB13 14
	GPIO_InitStructure.Pin = GPIO_PIN_13 |GPIO_PIN_14;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	//SX1278 RESET
	GPIO_InitStructure.Pin = GPIO_PIN_4 | GPIO_PIN_12;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//sx1278 IO口 DIO0-5
	GPIO_InitStructure.Pin = GPIO_PIN_2  ;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	// A7 6 5  A8 11
	GPIO_InitStructure.Pin =   GPIO_PIN_7 |GPIO_PIN_6 | GPIO_PIN_5 ;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 

	//对剩余所有IO口进行配置 留PA3 PA15 PB4通断开关  PA12 sx1278
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | 
								 GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_13 | GPIO_PIN_14;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	//
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | 
								GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8  | GPIO_PIN_10 | 
									GPIO_PIN_11;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure); 

	
	GPIO_InitStructure.Pin = GPIO_PIN_All;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStructure); 

	//留PC3外部中断
    GPIO_InitStructure.Pin = GPIO_PIN_0 |GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 |
								 GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_15;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |
								GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |
								GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 ;
	GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	__HAL_RCC_GPIOA_CLK_DISABLE();
	__HAL_RCC_GPIOB_CLK_DISABLE();
	__HAL_RCC_GPIOC_CLK_DISABLE();
	__HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOH_CLK_DISABLE();

    HAL_I2C_DeInit(&hi2c1);
//    HAL_I2C_MspDeInit(&hi2c1);
	//关闭所有外设 功耗降低30uA
  HAL_UART_MspDeInit(&USART4_Handler);
	HAL_UART_MspDeInit(&USART1_Handler);
	HAL_ADC_Stop(&hadc); //关闭ADC
    HAL_ADC_MspDeInit(&hadc);
	HAL_I2C_MspDeInit(&hi2c1);//MPU6050
	HAL_RTC_MspDeInit(&RTCHandle);
    HAL_SPI_MspDeInit(&hspi1); //ADXL355
	HAL_SPI_MspDeInit(&hspi2); //
	HAL_TIM_Base_MspDeInit(&htim3);//

    // 禁用/启用调试端口，先打开时钟，下面2句函数功能一样！A13 A14打开
//	 __HAL_RCC_DBGMCU_CLK_ENABLE();
//     HAL_DBGMCU_EnableDBGStopMode();
//     //HAL_DBGMCU_DBG_DisableLowPowerConfig(DBGMCU_STOP);
//	 
//	 __HAL_RCC_DBGMCU_CLK_DISABLE();

	//RTCHandle.Instance = RTC;
//	MX_RTC_Init();

    // 若需要在低功耗模式下调试程序，可以打开此函数
  //  HAL_DBGMCU_DBG_EnableLowPowerConfig(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY);

    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);    // 清除唤醒标志
    //__HAL_RTC_WAKEUPTIMER_EXTI_CLEAR_FLAG();//清除标志，否则第二次以后无法进入休眠

    /*## Setting the RTC Wake up time ########################################*/
    /*  RTC Wakeup Interrupt Generation:
    Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
    Wakeup Time = Wakeup Time Base * WakeUpCounter
    = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
    ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

    To configure the wake up timer to 4s the WakeUpCounter is:
    RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16
    Wakeup Time Base = 16 /(~32768Hz) = ~488.3us
    Wakeup Time = ~4s = 0,410ms  * WakeUpCounter
    ==> WakeUpCounter = 4s/488.3us = 8192       WakeUpCounter:0-65535     */

    i = stoptime * 2048;    //
//    // rtc 唤醒采用LSE时钟，第一参数，计数溢出唤醒；第二参数：进行16分频
    HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, i, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
//    /* Enter Stop Mode */
  	MPU6050_EXTI_GPIO();
   HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2); //wake_up引脚使能
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
     dogtimes++;   //唤醒次数计数
//    HAL_IWDG_Refresh(&hiwdg);  //喂狗
//    dogtimes2++;

}

void LowPower_WakeUp(void)
{
    // 禁止唤醒时钟计数，禁用后，需要重新执行 HAL_RTCEx_SetWakeUpTimer_IT 打开计数
    HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);

    // 如果使用非MSI时钟，请恢复默认时钟
    //SystemClock_Config();
    SystemClock_Config();//初始化系统时钟
    // 清除唤醒标记 减少重复唤醒电流
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_WU) != RESET)
    {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    }

    // 清除待机标志
    if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET)
    {
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    }

    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

}

uint32_t LowPowerTimes;

void Get_In_Stop(uint32_t limit_times, uint8_t limit_data)    //20 30
{
    static U16    LowVoltCnt = 0;
    //static U16    PU_Cnt = 0;
    float power;
    //uint32_t i = 0;
    power = VoltageDataADC_get( );//供电电源采集
//	printf("Volt:%f",power);
    g_SysData.u8_BatVolt = power;
    if(power < limit_data)//供电小于3V
    {
        LowPowerTimes ++;
    }
    else
    {
        LowPowerTimes = 0;
    }

    //判定电池电压是否欠压
    if(power <= g_SysData.u8_LowVoltTreshold)
    {
        LowVoltCnt++;
        if(LowVoltCnt >= 10)
        {
            LowVoltCnt = 10;
            g_statecheck.bit.bLowBatFault = cFlagYes;
        }
    }
    else
    {
        LowVoltCnt--;
        if(LowVoltCnt <= 0)
        {
            g_statecheck.bit.bLowBatFault = cFlagNo;
            LowVoltCnt = 0;
        }
    }

    if((g_SysData.u16_SysOnOffFlag == cSysOff) && (bEepromWriteFlag != 1))
    {
        HAL_Delay(4000 * g_SysData.u8_AlloID);
        //SX1278_LoraInit();
        PowerupTime();
    }

    if((LowPowerTimes > limit_times)
		||((g_SysData.u16_SysOnOffFlag == cSysOff) && (bEepromWriteFlag != 1)))
    {
        //MessageSendFunction(cLoraOrder_SleepInform);    // 紧急上报休眠通知
			
//		LTE_Pack_Func(12);    //上报休眠通知
        LowPowerTimes = 0;
        dogtimes = 0;               // 清零喂狗计数
        //SX1278_SetSleepMode();      // LORA模块休眠
        LPWStatus = 0;
        printf(" ENTER LPM ");
        LowPower_enter_stop(10);    // 停机10s
    }

}


void Get_WakeUp(void)
{
	
	 HAL_Init();
   LowPower_WakeUp();

	MX_GPIO_Init();
	
 HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);  //唤醒后禁止中断
	
	VDD1_ON();
	VDD2_ON();
	VDD3_ON();
	//RTC_Init();
    ADC_Init();
	UART_Init();
//    EC20_GPIO_Init();    //4g 

	sEepromGPIOInit();

//  ADXL355_Init();        // ADXL355SPI配置初始化
    //ADXL355_SPI_Init();
	//ADXL355_EXTI_GPIO();
	//SPI_ADXL355_S_READ(XL355_STATUS);
        I2C_Bus_Init();
        MPU6050_Init();   

    SX1278_LoraInit();
//    printf(" GET WAKE UP ");
}

/******************END OF FILE****************/

