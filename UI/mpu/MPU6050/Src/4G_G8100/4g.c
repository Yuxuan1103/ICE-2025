//#include "Wifi_Cmd_Handle.h"
#include "main.h"
float power;
uint8_t powerTemp;

#define EC20_EXTI_EXTI_IRQn EXTI0_1_IRQn
u32 gb_u32_file_Size;
u8 LTE_Rx_Buffer[Max_Len_1];
u8 LTE_Tx_Buffer[Max_Len_1];


u8 Cache_Buffer_ST[Mid_Len];

u16 u16_4G_Len;


Node_Real_Drb_Data sNode_Drb_Data;    //终端数据信息

AT_4G_Object_t    G8100_obj;
AT_4G_Object_t    EC20_obj;
AT_4G_Conn_Info     Conn_Info =
{
    "139.9.234.35",
//    "125.64.98.232",
    "UDP",                  //service_type:????? TCP/UDP/TCP LISTENER/UDP SERVICE
    1,                      //contextID:Integer type. The context ID. The range is 1-16.
    0,                      //connectID:Integer type. The socket service index. The range is 0-11.
    9999,                   //remote_port:only valid when <service_type> is "tcp"or"udp". The range is 0-65535
    8850,                   //local_port:The range is 0-65535
    1,                      //access_mode: 0: Buffer access mode 1:Direct push mode 2:Transparent access mode
};



void EC20_GPIO_Init(void);
void EC20_4G_Init(void);
AT_4G_Status_t EC20_Init(void);
void Boot_SendMeg(void);
void sMy_4G_Emitte(u8 *buffer);
void sMy_4G_Handler(void);
void EC20_Send_Cal(AT_4G_Object_t *obj,u8 *buffer, U16 u16_Send_Len);

void LTE_DataInit(void);

void LTE_Pack_Func(u8 type);
void LTE_ISR(void);
u8 Boot_Handler(void);
u8 Time_Handler(void);
void Set_PassWd_Handle(void);
void Send_Meg_Handler(void);
void sReset_Handler(void);
void sWake_up(void);
void sParameter(void);
u8 sQuer_Time_Hnadler(void);
void sSend_TiData_Meg(void);
void sEqu_Operation(void);
void sTermial_Level(void);
void sTital_Threshold(void);
void sSend_Fault(void);
void TimeCalb(void);
void sMy_Core_Up(void);

void LTE_MainTsk(void);
void TiltDataSend_LTE(void);
void TiltDataSend_LTE2(void);
void LTE_levelCmd_Treat(U8 * Rx_Buffer);
void LTE_TiltWarn_Treat(U8 * Rx_Buffer);
void LTE_Config03H_Treat(U8 * Rx_Buffer);

/*
************************************************************
****************
*Function Name :    void EC20_GPIO_Init(void)
*Description   :    4G????GPIO???,???GPIOD->14,GPIOD->15
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
*/
void EC20_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = EC20_Reset_Pin|EC20_PowerOn_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = EC20_Wake_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	//apready PB2
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
    GPIO_InitStruct.Pin = EC20_APREADY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	

}
void EC20_EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	  /*Configure GPIO pin : EXTI Pin */
	GPIO_InitStruct.Pin = EC20_EXTI_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(EC20_EXTI_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}
/*
************************************************************
****************
*Function Name :    void G8100_4G_Init(void)
*Description   :    4G,UDP
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Long
************************************************************
*/
void EC20_4G_Init(void)
{
    uint8_t reset_4G_num = 0;
    EC20_GPIO_Init();    //4g 
	//EC20_EXTI_Init();

//    LTE_DataInit();    //配置模块ID，密码

    EC20_obj.fops.dev_receive = Dev_Recv;  //  
    EC20_obj.fops.dev_send = Dev_Send;     //  
    EC20_obj.timeout = 200;
		
	/*****************开机*********************/
    HAL_GPIO_WritePin(EC20_PowerOn_PORT, EC20_PowerOn_Pin, GPIO_PIN_SET);
		
	HAL_Delay(500);
		
	HAL_GPIO_WritePin(EC20_PowerOn_PORT,EC20_PowerOn_Pin, GPIO_PIN_RESET);
	/*****************复位*********************/
	HAL_GPIO_WritePin(EC20_Reset_PORT, EC20_Reset_Pin, GPIO_PIN_SET);
		
	HAL_Delay(170);
		
	HAL_GPIO_WritePin(EC20_Reset_PORT,EC20_Reset_Pin, GPIO_PIN_RESET);
		
	UART_Init();//串口初始化 注意;放太前会发生硬件错误
		
	HAL_Delay(100);
			
	HAL_IWDG_Refresh(&hiwdg);  //喂狗

    while(EC20_Init() != AT_4G_STATUS_OK)
    {
		/*****************复位*********************/
		HAL_GPIO_WritePin(EC20_Reset_PORT, EC20_Reset_Pin, GPIO_PIN_SET);
		HAL_Delay(170);
		HAL_GPIO_WritePin(EC20_Reset_PORT,EC20_Reset_Pin, GPIO_PIN_RESET);
		reset_4G_num++;
		if(reset_4G_num > 10)
		{
			HAL_NVIC_SystemReset();//软件复位
		}
    }

    
}

/*
************************************************************
****************
*Function Name :    void EC20_Init(void)
*Description   :    4G,UDP
*Input         :    None
*Output        :    None
*Author        :    ???
*Creation Data :    
*Revisor       :    Rev1.0
*Note          :    u16_Long
************************************************************
****************
*/
AT_4G_Status_t EC20_Init(void)
{

 
	uint8_t Times = 0;
	AT_4G_Status_t ret  = AT_4G_STATUS_OK;
	//等待接受开机信号
	while(AT_4G_Rdy(&EC20_obj) != AT_4G_STATUS_OK)	
	{
		printf("NO RDY");
		Times++;
		if(Times >= 50)
		{
			Times = 0;
			return AT_4G_STATUS_ERR0R;
		}
	}
	HAL_IWDG_Refresh(&hiwdg);  //喂狗

	//延时等待毫秒4G稳定，时间可以短一点，但是短了不保证数据可靠
	HAL_Delay(5000);

	// 4G信号检测	
	ret = AT_4G_Csq(&EC20_obj);
	if(ret != AT_4G_STATUS_OK)
	{
		printf("NO CSQ");
		return ret;
	}
	//读取PIN
	ret = AT_4G_Cpin(&EC20_obj);    
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	//网络注册状态
	ret = AT_4G_Creg(&EC20_obj,2);    
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	//关闭消息回传
	ret = AT_4G_Ate(&EC20_obj,0);    
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
		HAL_IWDG_Refresh(&hiwdg);  //喂狗

	//配置外部中断唤醒UART1
//	ret = AT_4G_QURCCFG(&EC20_obj);
//	if(ret != AT_4G_STATUS_OK)
//	{
//		return ret;
//	}
	//配置APREADY用于主机状态监测
//	ret = AT_4G_QCFG(&EC20_obj);
//	if(ret != AT_4G_STATUS_OK)
//	{
//		return ret;
//	}
	/***查询是否注册网络***/
	while(AT_4G_Cgreg_sech(&EC20_obj) != AT_4G_STATUS_OK)
	{
		if(Times > 5)
		{
			ret = AT_4G_STATUS_ERR0R;
			break;
		}
		Times++;
		HAL_Delay(200);
	}
	//查询网络状态
	ret = AT_4G_Qnwinfo(&EC20_obj);    
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	//激活PDP上下文
	ret = AT_4G_Qiact(&EC20_obj,Conn_Info.contextID);    
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
		HAL_IWDG_Refresh(&hiwdg);  //喂狗

	/***与服务器创建UDP连接***/
	while(AT_4G_Qiopen(&EC20_obj, &Conn_Info) != AT_4G_STATUS_OK)
	{
      AT_4G_Qiclose(&EC20_obj, Conn_Info.connectID);
		
	}
	HAL_Delay(2000);
	
	HAL_IWDG_Refresh(&hiwdg);  //喂狗

	//发送开机联络信息
	Boot_SendMeg();
	EC20_Tcp_Recv(&EC20_obj,LTE_Rx_Buffer,50);
	sMy_4G_Handler();
	HAL_Delay(500);
	
	HAL_IWDG_Refresh(&hiwdg);  //喂狗

	//发送校时指令
//	TimeCalb();
    EC20_Tcp_Recv(&EC20_obj,LTE_Rx_Buffer,50);
    sMy_4G_Handler();
	HAL_Delay(500);
	
	HAL_IWDG_Refresh(&hiwdg);  //喂狗

	return AT_4G_STATUS_OK;

}




/*
************************************************************
****************
*Function Name :    void LTE_DataInit(void)
*Description   :    LTE_数据初始化
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

//void LTE_DataInit(void)
//{

//    //"DZ0001"
//	g_SysData.u8_Gateway_ID[0] = g_SysData.u16_GatewayID[0]; //读EE中的网关ID
//	g_SysData.u8_Gateway_ID[1] = g_SysData.u16_GatewayID[1];
//	g_SysData.u8_Gateway_ID[2] = g_SysData.u16_GatewayID[2];
//	g_SysData.u8_Gateway_ID[3] = g_SysData.u16_GatewayID[3];
//	g_SysData.u8_Gateway_ID[4] = g_SysData.u16_GatewayID[4];
//	g_SysData.u8_Gateway_ID[5] = g_SysData.u16_GatewayID[5];

//    g_SysData.u8_key_Wd[0]  =  0x31;
//    g_SysData.u8_key_Wd[1]  =  0x32;
//    g_SysData.u8_key_Wd[2]  =  0x33;
//    g_SysData.u8_key_Wd[3]  =  0x34;

//}



/*
************************************************************
****************
*Function Name :    void Boot_SendMeg(void)
*Description   :    ????:0x00
*Input         :    None
*Output        :    None
*Author        :    ???
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Long
************************************************************
****************
*/
void Boot_SendMeg(void)
{

//    LTE_Pack_Func(0);

    sMy_4G_Emitte(LTE_Tx_Buffer);
	
}


void TimeCalb(void)
{

//    LTE_Pack_Func(0x01);

    sMy_4G_Emitte(LTE_Tx_Buffer);
}
void SendHeartBeat(void)
{
	
//	LTE_Pack_Func(0x05);
	sMy_4G_Emitte(LTE_Tx_Buffer);
	
}
void DevSleep(void)
{

//	LTE_Pack_Func(0x0C);
	sMy_4G_Emitte(LTE_Tx_Buffer);


}
/*
************************************************************
****************
*Function Name :    void sMy_4G_Emitte(void)
*Description   :    4G????
*Input         :    buffer:????????
*Output        :    None
*Author        :    ???
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    ??????u16_Send_Len,???????????0???
************************************************************
****************
*/
void sMy_4G_Emitte(u8 *buffer)
{
	u16 u16_Send_Len = 0;
	/*获取本次发送数据的长度*/
	u16_Send_Len         = (LTE_Tx_Buffer[8]<<8)&0xff00;
	u16_Send_Len		 = u16_4G_Len|LTE_Tx_Buffer[9];
	u16_Send_Len        += 12;
	/*建立UDP连接*/
	AT_4G_Qiopen(&EC20_obj, &Conn_Info);
	/*数据以HEX码进行发送*/
	EC20_Tcp_Send_Hex(&EC20_obj, buffer, u16_Send_Len);
	
	u16_Send_Len = 0;
}

void Emitte_4G_Data(u8 *buffer)
{
//    u16 u16_Send_Len = 0;
//    u16_Send_Len         = (LTE_Tx_Buffer[8]<<8)&0xff00;
//    u16_Send_Len         = u16_4G_Len|LTE_Tx_Buffer[9];

//	u16_Send_Len = 1080;
//    GPIO_DTR_L();
//    HAL_Delay(200);
    
//	G8100_Tcp_Send_ASK(&G8100_obj, u16_Send_Len);
    //HAL_Delay(100); 
	
//	G8100_Tcp_Send_Hex(&G8100_obj, buffer, u16_Send_Len);
	//HAL_Delay(200);

//    AT_4G_CSCLK(&G8100_obj,1,1);    //开启休眠

//    GPIO_DTR_H();

 //   u16_Send_Len = 0;
}
/*
************************************************************
****************
*Function Name :    void sMy_4G_Emitte(void)
*Description   :    4G????
*Input         :    buffer:????????
*Output        :    None
*Author        :    ???
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    ??????u16_Send_Len,???????????0???
************************************************************
****************
*/
void EC20_Send_Cal(AT_4G_Object_t *obj,u8 *buffer, U16 u16_Send_Len)
{

	/*建立UDP连接*/
	AT_4G_Qiopen(&EC20_obj, &Conn_Info);
	/*数据以HEX码进行发送*/
	EC20_Tcp_Send_Hex(&EC20_obj, buffer, u16_Send_Len);

    u16_Send_Len = 0;
}

void sMy_4G_Handler(void)
{
    if(LTE_Rx_Buffer[0] == 0x68)
    {
        LTE_ISR();	
    }

    memset( LTE_Rx_Buffer,0xff, Max_Len_1 );
}

/*
************************************************************
****************
*Function Name :    void LTE_Pack_Func(void)
*Description   :    对发送数据打包
*Input         :    void
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
*/


//void LTE_Pack_Func(u8 type)
//{
//    LTE_Tx_Buffer[0]   = 0x68;
//    LTE_Tx_Buffer[1]   = g_SysData.u8_Gateway_ID[0];
//    LTE_Tx_Buffer[2]   = g_SysData.u8_Gateway_ID[1];
//    LTE_Tx_Buffer[3]   = g_SysData.u8_Gateway_ID[2];
//    LTE_Tx_Buffer[4]   = g_SysData.u8_Gateway_ID[3];
//    LTE_Tx_Buffer[5]   = g_SysData.u8_Gateway_ID[4];
//    LTE_Tx_Buffer[6]   = g_SysData.u8_Gateway_ID[5];
//    switch(type)
//    {
//        //0X00：开机联络信息
//        case 0x00:    

//            LTE_Tx_Buffer[7]    = 0x00;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x02;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0x03;
//            LTE_Tx_Buffer[11]   = 0x00;
//            //校验码

//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;

//        //校时指令
//        case 0x01:    
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x01;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x06;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]   = RTC_Data.Year;
//            LTE_Tx_Buffer[11]   = RTC_Data.Month;
//            LTE_Tx_Buffer[12]   = RTC_Data.Date;
//            LTE_Tx_Buffer[13]   = RTC_Time.Hours;
//            LTE_Tx_Buffer[14]   = RTC_Time.Minutes;
//            LTE_Tx_Buffer[15]   = RTC_Time.Seconds;
//            //校验码
//            LTE_Tx_Buffer[16]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[17]   = 0x16;
//            u16_4G_Len          = 0;
//        break;

//        case 0x02:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x02;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x02;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//		//03H密码校验失败
//        case 0x03:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x03;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x02;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;


//        
//        /*装置心跳*/
//        case 0x05:    

//            LTE_Tx_Buffer[7]    = 0x05;    //控制字
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x08;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;

//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]    = RTC_Data.Year;
//            LTE_Tx_Buffer[11]    = RTC_Data.Month;
//            LTE_Tx_Buffer[12]    = RTC_Data.Date;
//            LTE_Tx_Buffer[13]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[14]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[15]    = RTC_Time.Seconds;
//            //电池电压
//			power = VoltageDataADC_get();//供电电源采集
//			power = 2 * power;
//			powerTemp = (uint8_t)power;
//			Rssi = 0; 						//信号强度 0~199
//			AT_4G_Csq_HeartBeat(&EC20_obj);//信号强度获取
//			
//            LTE_Tx_Buffer[16]    = Rssi; //信号强度
//            LTE_Tx_Buffer[17]    = powerTemp; //电池电压

//            LTE_Tx_Buffer[18]    = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            LTE_Tx_Buffer[19]    = 0x16;
//            u16_4G_Len           = 0;
//        break;

//        case 0x06:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x06;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x02;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        //再议
//        case 0x07:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x07;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x0C;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//		//执行重启命令时密码错误
//        case 0x08:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x08;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x04;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        case 0x0A:
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x0A;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x0A;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            //LTE_Tx_Buffer[10]   =  sNode_Drb_Data.g_u8_Heartbeat_Interval;

////            LTE_Tx_Buffer[11]   =  sNode_Drb_Data.g_u16_Sample_Interval>>8;
////            LTE_Tx_Buffer[12]   = (sNode_Drb_Data.g_u16_Sample_Interval)&0XFF;
//            //校验码
//            //LTE_Tx_Buffer[13]    = sSensor_Para_Data.u16_ArrMot_Sample_Time >> 8;
//            //LTE_Tx_Buffer[14]    = sSensor_Para_Data.u16_ArrMot_Sample_Time & 0x00FF;

//            LTE_Tx_Buffer[15]    = unEeprom.stData.wAcclTresholdSetValue >> 8 ;
//            LTE_Tx_Buffer[16]    = unEeprom.stData.wAcclTresholdSetValue & 0x00FF;

//            LTE_Tx_Buffer[17]    = unEeprom.stData.wSleepGap >> 8;
//            LTE_Tx_Buffer[18]    = unEeprom.stData.wSleepGap & 0x00FF;
////            LTE_Tx_Buffer[19]    = sNode_Drb_Data.g_u32_Hard_Rst&0x000000FF;
//            LTE_Tx_Buffer[20]    = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[21]    = 0x16;
//            u16_4G_Len           = 0;
//        break;
//        case 0x0C:
//            //休眠
//            LTE_Tx_Buffer[7]    = 0x0C;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x00;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //校验码
//            LTE_Tx_Buffer[10]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[11]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        case 0x0D:    
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x0D;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x06;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]    = RTC_Data.Year;
//            LTE_Tx_Buffer[11]    = RTC_Data.Month;
//            LTE_Tx_Buffer[12]    = RTC_Data.Date;
//            LTE_Tx_Buffer[13]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[14]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[15]    = RTC_Time.Seconds;

//            //校验码
//            LTE_Tx_Buffer[16]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[17]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        case 0x21:    
//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x21;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x06;
//            //传输数据
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]    = RTC_Data.Year;
//            LTE_Tx_Buffer[11]    = RTC_Data.Month;
//            LTE_Tx_Buffer[12]    = RTC_Data.Date;
//            LTE_Tx_Buffer[13]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[14]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[15]    = RTC_Time.Seconds;

//            //校验码
//            LTE_Tx_Buffer[16]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[17]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        case 0x2A:   
//            LTE_Tx_Buffer[7]    = 0x2A;
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x15;
//            //传输数据
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;

//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]  = g_SysData.u8_key_Wd[0];
//            LTE_Tx_Buffer[11]  = g_SysData.u8_key_Wd[1];
//            LTE_Tx_Buffer[12]  = g_SysData.u8_key_Wd[2];
//            LTE_Tx_Buffer[13]  = g_SysData.u8_key_Wd[3];
//            //测试数据包
//            LTE_Tx_Buffer[14]  = 0x02;//gb_u8_Sensor_Rx_Sta[0];
//            LTE_Tx_Buffer[15]  = 0x00;
//            LTE_Tx_Buffer[16]  = 0x02;

//            LTE_Tx_Buffer[17]    = RTC_Data.Year;
//            LTE_Tx_Buffer[18]    = RTC_Data.Month;
//            LTE_Tx_Buffer[19]    = RTC_Data.Date;
//            LTE_Tx_Buffer[20]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[21]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[22]    = RTC_Time.Seconds;

//            //数据

//            LTE_Tx_Buffer[31]  = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            LTE_Tx_Buffer[32]  = 0x16;
//            u16_4G_Len         = 0;
//        break;
//        case 0x30:    

//            LTE_Tx_Buffer[7]    = 0x30;
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x0F;
//            //传输数据
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;

//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]  = g_SysData.u8_key_Wd[0];
//            LTE_Tx_Buffer[11]  = g_SysData.u8_key_Wd[1];
//            LTE_Tx_Buffer[12]  = g_SysData.u8_key_Wd[2];
//            LTE_Tx_Buffer[13]  = g_SysData.u8_key_Wd[3];
//            //测试数据包
//            LTE_Tx_Buffer[14]  = 0x00;
//            LTE_Tx_Buffer[15]  = 0x00;
//            LTE_Tx_Buffer[16]  = 0x00;


//            LTE_Tx_Buffer[17]    = RTC_Data.Year;
//            LTE_Tx_Buffer[18]    = RTC_Data.Month;
//            LTE_Tx_Buffer[19]    = RTC_Data.Date;
//            LTE_Tx_Buffer[20]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[21]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[22]    = RTC_Time.Seconds;


//            //LTE_Tx_Buffer[23]    = 0x00;
//            //LTE_Tx_Buffer[24]    = 0x00;

//            LTE_Tx_Buffer[25]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            LTE_Tx_Buffer[26]   = 0x16;
//        break;
//        case 0x48:
//            LTE_Tx_Buffer[7]    = 0x48;
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x25;

//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;

//            sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]  = g_SysData.u8_key_Wd[0];
//            LTE_Tx_Buffer[11]  = g_SysData.u8_key_Wd[1];
//            LTE_Tx_Buffer[12]  = g_SysData.u8_key_Wd[2];
//            LTE_Tx_Buffer[13]  = g_SysData.u8_key_Wd[3];

//            LTE_Tx_Buffer[14]    = RTC_Data.Year;
//            LTE_Tx_Buffer[15]    = RTC_Data.Month;
//            LTE_Tx_Buffer[16]    = RTC_Data.Date;
//            LTE_Tx_Buffer[17]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[18]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[19]    = RTC_Time.Seconds;

//            LTE_Tx_Buffer[20]  = 0X41;
//            LTE_Tx_Buffer[29]  = 0X42;
//            LTE_Tx_Buffer[38]  = 0X43;

//            LTE_Tx_Buffer[47]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            LTE_Tx_Buffer[48]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//        case 0xFF:    

//            //控制字写在外面
//            LTE_Tx_Buffer[7]    = 0x00;
//            //数据域长度
//            LTE_Tx_Buffer[8]    = 0x00;
//            LTE_Tx_Buffer[9]    = 0x02;
//            u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//            u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//            u16_4G_Len         += 10;
//            //数据域
//            LTE_Tx_Buffer[10]   = 0xFF;
//            LTE_Tx_Buffer[11]   = 0xFF;
//            //校验码
//            LTE_Tx_Buffer[12]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            //结束码
//            LTE_Tx_Buffer[13]   = 0x16;
//            u16_4G_Len          = 0;
//        break;
//		
//		//上传地震数据
//		case 0x47:
//			//控制字
//			LTE_Tx_Buffer[7]    = 0x47;
//			//数据域长度
//			LTE_Tx_Buffer[8]    = 0x00;
//			LTE_Tx_Buffer[9]    = 0x18;   //24个byte
//			u16_4G_Len          = (LTE_Tx_Buffer[8]<<8)&0xff00;
//			u16_4G_Len          = u16_4G_Len|LTE_Tx_Buffer[9];
//			u16_4G_Len         += 10;
//		
//			sMy_RTC_Get_Time();
//            LTE_Tx_Buffer[10]  = g_SysData.u8_key_Wd[0];
//            LTE_Tx_Buffer[11]  = g_SysData.u8_key_Wd[1];
//            LTE_Tx_Buffer[12]  = g_SysData.u8_key_Wd[2];
//            LTE_Tx_Buffer[13]  = g_SysData.u8_key_Wd[3];
//		
//		    LTE_Tx_Buffer[14]  = 0x01;                    //帧标识
//			LTE_Tx_Buffer[15]  = 0x01;					//包数
//		
//		    LTE_Tx_Buffer[16]    = RTC_Data.Year;
//            LTE_Tx_Buffer[17]    = RTC_Data.Month;
//            LTE_Tx_Buffer[18]    = RTC_Data.Date;
//            LTE_Tx_Buffer[19]    = RTC_Time.Hours;
//            LTE_Tx_Buffer[20]    = RTC_Time.Minutes;
//            LTE_Tx_Buffer[21]    = RTC_Time.Seconds;
//			
//			//数据
//			
//			LTE_Tx_Buffer[34]   = SUM_Check(LTE_Tx_Buffer,u16_4G_Len);
//            LTE_Tx_Buffer[35]   = 0x16;
//			u16_4G_Len = 0;
//		break;
//		
//        default :
//        break;

//    }
//}

void sMy_Core_Up(void)
{
	if(LTE_Rx_Buffer [14] ==0x00 )
	{
		HAL_NVIC_SystemReset();//软件复位
	}
//	else
//	{
//		if(LTE_Rx_Buffer [14] == 0x2A )
//		{
//			gb_u32_file_Size = TerminalProgramReceiveSaveTask(LTE_Rx_Buffer[14],LTE_Rx_Buffer[15]);
//			if(gb_u32_file_Size == 0)
//			{
//				memset( g_u8_Instruct,0x00, CACHE_IST_NUM );
//				printf("传输失败");
//			}
//		}
//	}

	
}



//接收命令处理函数

void LTE_ISR(void)
{

    switch(LTE_Rx_Buffer[7])    //命令位
    {
        //验证
        //0X00：开机联络信息主动上传
        case 0x00:    

            while(Boot_Handler())
            {
                Boot_SendMeg();
                EC20_Tcp_Recv(&G8100_obj,LTE_Rx_Buffer,50);
            }
        break;
        
        //0X01：收到服务器校时指令进行校时
        case 0x01:    
         //   Time_Handler();
        break;
        
        //0X02：设置装置密码下发
        case 0x02:    
         //   Set_PassWd_Handle();
        break;

        //0X00：主站下发参数配置下发
        case 0x03: 
            g_u8_Instruct[0]++;
      //      LTE_Config03H_Treat(LTE_Rx_Buffer); //地震项目进行休眠时间和采样阈值的配置
            //Send_Meg_Handler();
        break;
        
        //0X05：设置心跳信息主动上传
        case 0x05:
            //sHeart_SendMeg();
        break;
		//0x08：模块重启
        case 0x08:
            g_u8_Instruct[0]++;
            u8_cmd_num = g_u8_Instruct[0];
          //  sReset_Handler();
        break;
        
        //0X09：短信唤醒下发
        case 0x09:    
          //  sWake_up();
        break;
        
        //0X0A：查询装置配置参数下发
        case 0x0A:    
           // sParameter(); //地震项目查询当前休眠时间与采样触发阈值
        break;
        
        //0X0C：设置休眠上传
        case 0x0C:    
            //sDormancy_SendMeg();
        break;
        
        //0X0D：查询装置设备信息下发
        case 0x0D:    
          //  sQuer_Time_Hnadler();
        break;
        
        //0X21：主站请求装置数据
        case 0x21:  
	//		TiltDataSend_LTE2();  //数据实时采集
         //   sSend_TiData_Meg();
        break;
        
        //0X2A：上传加速度数据
        case 0x2A: 
			//Send_Accl_Mesg();
        break;
        
        //0X30：上传设备故障信息
        case 0x30:    
           // sSend_Fault();
        break;
		/*0X47：地震数据上报*/
		case 0x47: 
			
			//Send_Accl_Mesg();
	
		break;
        case 0xB2:
           // sEqu_Operation();
        break;
        
        case 0xB3:
            g_u8_Instruct[0]++;
            u8_cmd_num  = g_u8_Instruct[0];
            g_u8_Instruct[u8_cmd_num] = 0x0D;
       //     LTE_levelCmd_Treat(LTE_Rx_Buffer);
           // sTermial_Level();
        break;
        
        case 0xB4:
        
            //LTE_TiltWarn_Treat(LTE_Rx_Buffer);
           // sTital_Threshold();
        break;
		
		
        default:
        break;
        
    }
}




/*
************************************************************
****************
*Function Name :    u8 Boot_Handler(void)
*Description   :    4G
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Long
************************************************************
****************
*/
u8 Boot_Handler(void)
{
    u8 temp;
    if((LTE_Rx_Buffer[0] == 0x68)
    && (LTE_Rx_Buffer[13] == 0x16)
    && (LTE_Rx_Buffer[7] == 0x00))
    {
        temp = 0;
    }
    else
    {
        temp = 1;
    }
    return temp;
}



/*
************************************************************
****************
*Function Name :    void Time_Handler(void)
*Description   :    响应校时指令：0x01
*Input         :    None
*Output        :    1：校验未通过
***************		0：程序执行成功，校时完成
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    局部变量u8_dec
************************************************************
****************
*/
u8 Time_Handler(void)
{
	u8 u8_dec = 0;
	u16 u16_Check_Time = 0;
	u16_Check_Time     = LTE_Rx_Buffer[8];
	u16_Check_Time	   = (u16_Check_Time << 8) & 0xff00;
	u16_Check_Time	   = u16_Check_Time | LTE_Rx_Buffer[9];
	u16_Check_Time    += 10;
	if((LTE_Rx_Buffer[0] == 0x68) && (LTE_Rx_Buffer[17] == 0x16) 
		&&(LTE_Rx_Buffer[16] == SUM_Check(LTE_Rx_Buffer,u16_Check_Time)))
	{
		//判断时间是否超过20s
		if(RTC_Time.Seconds > LTE_Rx_Buffer[15])
		{
			u8_dec = RTC_Time.Seconds - LTE_Rx_Buffer[15];
		}
		else
		{
			u8_dec = LTE_Rx_Buffer[15] - RTC_Time.Seconds;
		}
		if(u8_dec < 40)
		{
            if(LTE_Rx_Buffer[13] > 12)
            {
                RTC_Data.Year    = LTE_Rx_Buffer[10];
                RTC_Data.Month   = LTE_Rx_Buffer[11];
                RTC_Data.Date    = LTE_Rx_Buffer[12];
                RTC_Data.WeekDay = 1;
                RTC_Time.Hours   = LTE_Rx_Buffer[13];
                RTC_Time.Minutes = LTE_Rx_Buffer[14];
                RTC_Time.Seconds = LTE_Rx_Buffer[15];

                sMy_RTC_Set_Time(&RTC_Data,&RTC_Time,RTC_HOURFORMAT12_PM);
            }
            else
            {
                RTC_Data.Year    = LTE_Rx_Buffer[10];
                RTC_Data.Month   = LTE_Rx_Buffer[11];
                RTC_Data.Date    = LTE_Rx_Buffer[12];
                RTC_Data.WeekDay = 1;
                RTC_Time.Hours   = LTE_Rx_Buffer[13];
                RTC_Time.Minutes = LTE_Rx_Buffer[14];
                RTC_Time.Seconds = LTE_Rx_Buffer[15];
                sMy_RTC_Set_Time(&RTC_Data,&RTC_Time,RTC_HOURFORMAT12_AM);

            }
            sMy_RTC_Get_Time();

        }
        return 0;
    }
    return 1;
}



/*
************************************************************
****************
*Function Name :    void Send_Meg_Handler(void)
*Description   :    响应主站下发参数0x02:设置密码
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    密码验证通过以后更改设备密码，并将数据包原包返回
***************:	密码验证不成功则数据位返回0xff 0xff
************************************************************
****************
*/
//void Set_PassWd_Handle(void)
//{
//    //计算复制的字长
//    //u16_Long:用于存储数据字长
//    u16 u16_Len_Pass = 0;
//    u16_Len_Pass     = LTE_Rx_Buffer[8];
//    u16_Len_Pass	 = (u16_Len_Pass << 8) & 0xff00;
//    u16_Len_Pass	 = u16_Len_Pass|LTE_Rx_Buffer[9];
//    u16_Len_Pass    += 12;
//    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Pass);
//    //设置密码
//    if((LTE_Rx_Buffer[0] == 0x68)
//    &&(LTE_Rx_Buffer[10] == g_SysData.u8_key_Wd[0])
//    &&(LTE_Rx_Buffer[11] == g_SysData.u8_key_Wd[1])
//    &&(LTE_Rx_Buffer[12] == g_SysData.u8_key_Wd[2])
//    &&(LTE_Rx_Buffer[13] == g_SysData.u8_key_Wd[3])
//    &&(LTE_Rx_Buffer[19] == 0x16))
//    {
//        g_SysData.u8_key_Wd[0] = LTE_Rx_Buffer[14];
//        g_SysData.u8_key_Wd[1] = LTE_Rx_Buffer[15];
//        g_SysData.u8_key_Wd[2] = LTE_Rx_Buffer[16];
//        g_SysData.u8_key_Wd[3] = LTE_Rx_Buffer[17];
//        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Pass);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//    }
//    else
//    {
//        //密码校验未通过
//        LTE_Pack_Func(2);
//        sMy_4G_Emitte(LTE_Tx_Buffer);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//    }
//}





/*
************************************************************
****************
*Function Name :    void Send_Meg_Handler(void)
*Description   :    响应主站下发参数0x03：主站下发参数配置
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    主站下发参数为为30字节,完成验证
************************************************************
****************
*/
//void Send_Meg_Handler(void)
//{
//    //计算复制的字长
//    //u16_Long:用于存储数据字长

//    u16 u16_Len_SMeg = 0;
//    u16_Len_SMeg     = LTE_Rx_Buffer[8];
//    u16_Len_SMeg	 = (u16_Len_SMeg << 8) & 0xff00;
//    u16_Len_SMeg	 = u16_Len_SMeg|LTE_Rx_Buffer[9];
//    u16_Len_SMeg    += 12;
//    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_SMeg);	
//    //设置密码
//    if((LTE_Rx_Buffer[0] == 0x68)
//    && (LTE_Rx_Buffer[24] == g_SysData.u8_key_Wd[0])
//    && (LTE_Rx_Buffer[25] == g_SysData.u8_key_Wd[1])
//    && (LTE_Rx_Buffer[26] == g_SysData.u8_key_Wd[2])
//    && (LTE_Rx_Buffer[27] == g_SysData.u8_key_Wd[3])
//    && (LTE_Rx_Buffer[29] == 0x16))
//    {

//        //设备在线时长设置为24H
//        //将心跳时间设置为规定时间
//        sNode_Drb_Data.g_u8_Heartbeat_Interval = LTE_Rx_Buffer[14];
//        //将采样间隔时间设置为更改的间隔时间
//        sNode_Drb_Data.g_u16_Sample_Interval   = LTE_Rx_Buffer[15];	
//        sNode_Drb_Data.g_u16_Sample_Interval   = sNode_Drb_Data.g_u16_Sample_Interval<<8;
//        sNode_Drb_Data.g_u16_Sample_Interval   = sNode_Drb_Data.g_u16_Sample_Interval|LTE_Rx_Buffer[16];
//        //将休眠时间为更改的休眠时间
//        sNode_Drb_Data.g_u16_Sleep_Time  	   = LTE_Rx_Buffer[17];
//        sNode_Drb_Data.g_u16_Sleep_Time   	   = sNode_Drb_Data.g_u16_Sleep_Time<<8;
//        sNode_Drb_Data.g_u16_Sleep_Time   	   = sNode_Drb_Data.g_u16_Sleep_Time|LTE_Rx_Buffer[18];
//        //设备的在线时间
//        sNode_Drb_Data.g_u16_Online_Time  	   = LTE_Rx_Buffer[19];
//        sNode_Drb_Data.g_u16_Online_Time   	   = sNode_Drb_Data.g_u16_Online_Time<<8;
//        sNode_Drb_Data.g_u16_Online_Time   	   = sNode_Drb_Data.g_u16_Online_Time|LTE_Rx_Buffer[20];
//        //硬件重启时间
//        sNode_Drb_Data.g_u32_Hard_Rst 		   = LTE_Rx_Buffer[21];
//        sNode_Drb_Data.g_u32_Hard_Rst   	   = sNode_Drb_Data.g_u32_Hard_Rst<<8;
//        sNode_Drb_Data.g_u32_Hard_Rst   	   = sNode_Drb_Data.g_u32_Hard_Rst|LTE_Rx_Buffer[22];
//        sNode_Drb_Data.g_u32_Hard_Rst   	   = sNode_Drb_Data.g_u32_Hard_Rst<<8;
//        sNode_Drb_Data.g_u32_Hard_Rst   	   = sNode_Drb_Data.g_u32_Hard_Rst|LTE_Rx_Buffer[23];
//        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, 30);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//        //添加远程指令进入指令缓存buffer
//        u8_cmd_num  = g_u8_Instruct[0];
//        g_u8_Instruct[u8_cmd_num] = 0x05;
//        u8_cmd_num = 0;	
//    }
//    else
//    {
//        //密码校验
//        LTE_Pack_Func(3);
//        sMy_4G_Emitte(LTE_Tx_Buffer);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//    }
//}




/*
************************************************************
****************
*Function Name :    void sReset_Handler(void)
*Description   :    响应重启指令：0x08
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_Rst用于计算控制长度
************************************************************
****************
*/
//void sReset_Handler(void)
//{
//    //计算复制的字长
//    //u16_Len_Rst:用于计算下发数据包长度
//    u16 u16_Len_Rst = 0;

//    u16_Len_Rst     = LTE_Rx_Buffer[8];
//    u16_Len_Rst     = (u16_Len_Rst << 8) & 0xff00;
//    u16_Len_Rst     = u16_Len_Rst|LTE_Rx_Buffer[9];
//    u16_Len_Rst    += 12;
//    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Rst);
//    if((LTE_Rx_Buffer[0] == 0x68) 
//    && (LTE_Rx_Buffer[10] == g_SysData.u8_key_Wd[0])
//    && (LTE_Rx_Buffer[11] == g_SysData.u8_key_Wd[1]) 
//    && (LTE_Rx_Buffer[12] == g_SysData.u8_key_Wd[2])
//    && (LTE_Rx_Buffer[13] == g_SysData.u8_key_Wd[3]) 
//    && (LTE_Rx_Buffer[15] == 0x16))
//    {
//        //设备复位->终端重启还是中继网关重启
//        //中继网关收到复位信息后若校验正确则将收到的数据直接返回
//        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Rst);
//		HAL_NVIC_SystemReset(); 

//    }
//    else
//    {
//        //密码校验未成功
//        LTE_Pack_Func(8);
//        sMy_4G_Emitte(LTE_Tx_Buffer);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//    }
//}


/*
************************************************************
****************
*Function Name :    void sReset_Handler(void)
*Description   :    响应短信唤醒指令：0x09
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_Wake用于计算控制长度
************************************************************
****************
*/
void sWake_up(void)
{
    //唤醒
    u16 u16_Len_Wake = 0;
    u16_Len_Wake     = LTE_Rx_Buffer[8];
    u16_Len_Wake     = (u16_Len_Wake << 8) & 0xff00;
    u16_Len_Wake     = u16_Len_Wake | LTE_Rx_Buffer[9];
    u16_Len_Wake    += 12;
    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Wake);
    //设置密码
    if((LTE_Rx_Buffer[0] == 0x68)&&(LTE_Rx_Buffer[15] ==0x16))
    {
        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Wake);
        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
    }
}


/*
************************************************************
****************
*Function Name :    void sParameter(void)
*Description   :    查询装置配置参数指令：0x0A
*Input         :    None
*Output        :    None
*Author        :    Lh
*Creation Data :    2021/04/08
*Revisor       :    Rev1.0
*Note          :    u16_Long
************************************************************
****************
*/
void sParameter(void)
{
    //上传参数
 //   LTE_Pack_Func(10);
    sMy_4G_Emitte(LTE_Tx_Buffer);
    memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
}


/*
************************************************************
****************
*Function Name :    void sQuer_Time_Hnadler(void)
*Description   :    查询装置设备时间：0x0D
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_sqTime用于计算控制长度
************************************************************
****************
*/
u8 sQuer_Time_Hnadler(void)
{
    //计算
    //u8 trxx;
    u16 u16_Len_sqTime = 0;
    u16_Len_sqTime     = LTE_Rx_Buffer[8];
    u16_Len_sqTime     = (u16_Len_sqTime << 8) & 0xff00;
    u16_Len_sqTime     = u16_Len_sqTime|LTE_Rx_Buffer[9];
    u16_Len_sqTime    += 10;
    if((LTE_Rx_Buffer[0] == 0x68) 
    && (LTE_Rx_Buffer[11] == 0x16) 
    &&((LTE_Rx_Buffer[10] == SUM_Check(LTE_Rx_Buffer,u16_Len_sqTime))))
    {
        //上报当前时间
//        LTE_Pack_Func(13);
        sMy_4G_Emitte(LTE_Tx_Buffer);
        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
        return 0;
    }
    return 1;
}


/*
************************************************************
****************
*Function Name :    void sSend_TiData_Meg(void)
*Description   :    上送倾斜数据
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

void sSend_TiData_Meg(void)
{
    /*功能识别码：低四位，编号：0～3，高四位表层数：取值，0～3，0是最顶层*/
    LTE_Tx_Buffer[23]  = Cache_Buffer_ST[0];    //顺线
    LTE_Tx_Buffer[24]  = Cache_Buffer_ST[1];    //横线

    LTE_Tx_Buffer[25]  = Cache_Buffer_ST[2];
    LTE_Tx_Buffer[26]  = Cache_Buffer_ST[3];

    LTE_Tx_Buffer[27]  = Cache_Buffer_ST[4];
    LTE_Tx_Buffer[28]  = Cache_Buffer_ST[5];

    LTE_Tx_Buffer[29]  = Cache_Buffer_ST[6];
    LTE_Tx_Buffer[30]  = Cache_Buffer_ST[7];
//    LTE_Pack_Func(0x2A);
    sMy_4G_Emitte(LTE_Tx_Buffer);
}

/*
************************************************************
****************
*Function Name :    void Send_Accl_Mesg(void)
*Description   :    上送倾斜数据
*Input         :    None
*Output        :    None
*Author        :    Lh
*Creation Data :    2021/04/09
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

void Send_Accl_Mesg(void)
{
	//读传感器数据
	XL355_Temp_Calc();
	XL355_Accel_Read();
	//单位 m/s2
	Xdata_Struct.Xdata = sIncl_Data.f32_Accl_Xaxis * Gvalue - sIncl_Data.f32_XAccl_offset;  //float转uint8 数据存放地址高位-低位 大端对齐
	Ydata_Struct.Ydata = sIncl_Data.f32_Accl_Yaxis * Gvalue - sIncl_Data.f32_YAccl_offset;
	Zdata_Struct.Zdata = sIncl_Data.f32_Accl_Zaxis * Gvalue - sIncl_Data.f32_ZAccl_offset;
	
	//X轴加速度
    LTE_Tx_Buffer[22]  = Xdata_Struct.u8_Xdata[3];    
    LTE_Tx_Buffer[23]  = Xdata_Struct.u8_Xdata[2];    
    LTE_Tx_Buffer[24]  = Xdata_Struct.u8_Xdata[1];
    LTE_Tx_Buffer[25]  = Xdata_Struct.u8_Xdata[0];

	//Y轴加速度
    LTE_Tx_Buffer[26]  = Ydata_Struct.u8_Ydata[3];
    LTE_Tx_Buffer[27]  = Ydata_Struct.u8_Ydata[2];
    LTE_Tx_Buffer[28]  = Ydata_Struct.u8_Ydata[1];
    LTE_Tx_Buffer[29]  = Ydata_Struct.u8_Ydata[0];
	
	//Z轴加速度
	LTE_Tx_Buffer[30]  = Zdata_Struct.u8_Zdata[3];
	LTE_Tx_Buffer[31]  = Zdata_Struct.u8_Zdata[2];
	LTE_Tx_Buffer[32]  = Zdata_Struct.u8_Zdata[1];
	LTE_Tx_Buffer[33]  = Zdata_Struct.u8_Zdata[0];
	
//    LTE_Pack_Func(0x47);
    sMy_4G_Emitte(LTE_Tx_Buffer);
}


/*
************************************************************
****************
*Function Name :    void sSend_Fault(void)
*Description   :    上送故障数据
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

void sSend_Fault(void)
{
//    LTE_Pack_Func(48);
    EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, 27);
    memset(LTE_Tx_Buffer,0xff, Max_Len_1);
}




/*
************************************************************
****************
*Function Name :    void sEqu_Operation(void)
*Description   :    设备运行指令：0xB2
*Input         :    None
*Output        :    None
*Author        :    王海波
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_Opera用于计算控制长度
************************************************************
****************
*/
void sEqu_Operation(void)
{
    u16 u16_Len_Opera = 0;
    u16_Len_Opera     = (LTE_Rx_Buffer[8]<<8) & 0xff00;

    u16_Len_Opera     = u16_Len_Opera | LTE_Rx_Buffer[9];
    u16_Len_Opera    += 12;
    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Opera);
    //设置密码
    if((LTE_Rx_Buffer[0] == 0x68)&&(LTE_Rx_Buffer[12] ==0x16))
    {
        //g_u8_Operation_STA = LTE_Rx_Buffer[10];

        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Opera);
        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
        g_u8_Instruct[0]++;
        u8_cmd_num  = g_u8_Instruct[0];
        g_u8_Instruct[u8_cmd_num] = 0x00;
	}
}


/*
************************************************************
****************
*Function Name :    void sTermial_Level(void)
*Description   :    调平指令
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_Opera用于计算控制长度
************************************************************
****************
*/

void sTermial_Level(void)
{
    u16 u16_Len_Level = 0;
    u16_Len_Level     = LTE_Rx_Buffer[8];
    u16_Len_Level     = (u16_Len_Level << 8) & 0XFF00;
    u16_Len_Level     = u16_Len_Level|LTE_Rx_Buffer[9];
    u16_Len_Level    += 12;
    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Level);
    //设置密码
    if((LTE_Rx_Buffer[0] == 0x68)
    &&(LTE_Rx_Buffer[u16_Len_Level - 1] ==0x16))
    {
        EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Level);
        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
    }

}


/*
************************************************************
****************
*Function Name :    void sTital_Threshold(void)
*Description   :   
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    u16_Len_Opera用于计算控制长度
************************************************************
****************
*/

void sTital_Threshold(void)
{
    u16 u16_Len_Threshold = 0;
    u16_Len_Threshold     = LTE_Rx_Buffer[8];
    u16_Len_Threshold	  = (u16_Len_Threshold << 8) & 0XFF00;
    u16_Len_Threshold	  = u16_Len_Threshold|LTE_Rx_Buffer[9];
    u16_Len_Threshold    += 12;
    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_Threshold);
    if((LTE_Rx_Buffer[0] == 0x68)
    &&(LTE_Rx_Buffer[u16_Len_Threshold-1] == 0x16))
    {
        if(LTE_Rx_Buffer[u16_Len_Threshold-5] == 0x2A)
        {
            sNode_Drb_Data.u16_Udr_Titl = (LTE_Rx_Buffer[u16_Len_Threshold-4]) << 8;
            sNode_Drb_Data.u16_Udr_Titl = sNode_Drb_Data.u16_Udr_Titl | LTE_Rx_Buffer[u16_Len_Threshold - 3];
            EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_Threshold);
            memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
        }
    }
    //添加远程指令进入指令缓存buffer	
    g_u8_Instruct[0]++;
    u8_cmd_num  = g_u8_Instruct[0];
    g_u8_Instruct[u8_cmd_num] = 0x05;
    u8_cmd_num = 0;
}




/*
************************************************************
****************
*Function Name :    void LTE_MainTsk(void)
*Description   :    LTE主任务
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/



void LTE_MainTsk(void)
{
    static U16 u16HeartSendCnt = 0;

    static U16 u16TiltSendCnt = 0;

    //上传数据
    if(g_statecheck.bit.bLoraTimingSendFlag == cFlagYes)    //定时上传
    {
        if(g_statecheck.bit.bSysTiltState == cFlagYes)
        {
            u16TiltSendCnt++;
            if(u16TiltSendCnt >= 24)    //2Hours
            {
                g_SysData.u16_TiltSend_K += 1;
                u16TiltSendCnt = 0;
            }
        }

        if(g_statecheck.bit.bLoraCallBackFlag != cFlagYes)
        {
//			TiltDataSend_LTE();
        }
        
    }
    else
    {
        g_statecheck.bit.bLoraCallBackFlag = cFlagNo;
    }


    //上传心跳数据
    u16HeartSendCnt ++;
    if(u16HeartSendCnt >= 22)    //xx*2 = xx s上传一次心跳数据 1min50s 45
    {
        u16HeartSendCnt = 0;
//        LTE_Pack_Func(0x05);
        sMy_4G_Emitte(LTE_Tx_Buffer);
		printf("HeartBeat");
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET); 
		HAL_Delay(500);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET); 
    }

}

/*
************************************************************
****************
*Function Name :    void TiltDataSend(void)
*Description   :    LTE主任务中上传角度值
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

//void TiltDataSend_LTE(void)
//{
//    u16 u16_Send_Angle = 0;    //角度值上传变量
//    float f32_ShunxianSend = 0;
//    float f32_HengxianSend = 0;

//    f32_ShunxianSend = g_SysData.f32_ShunxianAngle * 100.0f;
//    f32_HengxianSend = g_SysData.f32_HenxianAngle  * 100.0f;

//    //顺线角度处理
//    u16_Send_Angle  = AngelTrans(f32_ShunxianSend);
//    Cache_Buffer_ST[0] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[1] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;
//    //横线角度处理
//    u16_Send_Angle  = AngelTrans(f32_HengxianSend);
//    Cache_Buffer_ST[2] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[3] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;

//    //为了跟随原有协议，后面两个角度为0
//    u16_Send_Angle  = AngelTrans(0.0f);
//    Cache_Buffer_ST[4] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[5] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;
//    u16_Send_Angle  = AngelTrans(0.0f);
//    Cache_Buffer_ST[6] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[7] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;
//    //进行数据上送

//    sSend_TiData_Meg();

//    HAL_Delay(2);
//    memset( LTE_Tx_Buffer,0xff, Max_Len_1 );


//}


//倾角数据实时上传
//void TiltDataSend_LTE2(void)
//{
//	u16 u16_Send_Angle = 0;    //角度值上传变量
//    float f32_ShunxianSend = 0;
//    float f32_HengxianSend = 0;

//    f32_ShunxianSend = g_SysData.f32_ShunxianAngle * 100.0f;
//    f32_HengxianSend = g_SysData.f32_HenxianAngle  * 100.0f;

//    //顺线角度处理
////    u16_Send_Angle  = AngelTrans(f32_ShunxianSend);
//    Cache_Buffer_ST[0] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[1] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;
//    //横线角度处理
//    u16_Send_Angle  = AngelTrans(f32_HengxianSend);
//    Cache_Buffer_ST[2] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[3] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;

//    //为了跟随原有协议，后面两个角度为0
//    u16_Send_Angle  = AngelTrans(0.0f);
//    Cache_Buffer_ST[4] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[5] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;
//    u16_Send_Angle  = AngelTrans(0.0f);
//    Cache_Buffer_ST[6] =  (u16_Send_Angle&0xff00)>>8;
//    Cache_Buffer_ST[7] =  u16_Send_Angle&0x00ff;
//    u16_Send_Angle  = 0;

//}

/*
************************************************************
****************
*Function Name :    void void LTE_levelCmd_Treat(U8 * Rx_Buffer)
*Description   :    处理LTE调平指令
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

//void LTE_levelCmd_Treat(U8 * Rx_Buffer)
//{
//    if((Rx_Buffer[1] == g_SysData.u8_Gateway_ID[0])
//    && (Rx_Buffer[2] == g_SysData.u8_Gateway_ID[1])
//    && (Rx_Buffer[3] == g_SysData.u8_Gateway_ID[2])
//    && (Rx_Buffer[4] == g_SysData.u8_Gateway_ID[3])
//    && (Rx_Buffer[5] == g_SysData.u8_Gateway_ID[4])
//    && (Rx_Buffer[6] == g_SysData.u8_Gateway_ID[5]))
//    {
//        if(Rx_Buffer[10] == 0x2A)
//        {
//            g_statecheck.bit.bTiltCalbFlag = cFlagYes;
//        }
//        else
//        {
//            g_statecheck.bit.bTiltCalbFlag = cFlagNo;
//        }
//    }
//}





/*
************************************************************
****************
*Function Name :    void void LTE_TiltWarn_Treat(U8 * Rx_Buffer)
*Description   :    处理倾斜阈值
*Input         :    None
*Output        :    None
*Author        :    
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

//void LTE_TiltWarn_Treat(U8 * Rx_Buffer)
//{

//    ubitint U16_temp;


//    if((Rx_Buffer[1] == g_SysData.u8_Gateway_ID[0])
//    && (Rx_Buffer[2] == g_SysData.u8_Gateway_ID[1])
//    && (Rx_Buffer[3] == g_SysData.u8_Gateway_ID[2])
//    && (Rx_Buffer[4] == g_SysData.u8_Gateway_ID[3])
//    && (Rx_Buffer[5] == g_SysData.u8_Gateway_ID[4])
//    && (Rx_Buffer[6] == g_SysData.u8_Gateway_ID[5]))
//    {

//        // 倾斜报警阈值
//        U16_temp.bitdata.highchar = Rx_Buffer[12] & 0xff;
//        U16_temp.bitdata.lowchar  = Rx_Buffer[11] & 0xff;
//        if(U16_temp.id <= 9000)
//        {
//            g_SysData.u16_TiltWarningTreshold = U16_temp.id;

//            // 向EEPROM中写入下发的设定值
//            if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
//            {
//                unEeprom.stData.wTiltTresholdSetValue = g_SysData.u16_TiltWarningTreshold;     //倾斜
//                bEepromWriteFlag = 1;
//            }

//        }
//    }
//}




/*
************************************************************
****************
*Function Name :    void LTE_Config03H_Treat(U8 * Rx_Buffer)
*Description   :    处理03H配置命令
*Input         :    None
*Output        :    None
*Author        :    Lh
*Creation Data :    2021/04/08
*Revisor       :    Rev1.0
*Note          :    
************************************************************
****************
*/

//void LTE_Config03H_Treat(U8 * Rx_Buffer)
//{
//    U16 u16_temp;
//	U16 u16_temp2;
//	u16 u16_Len_SMeg = 0;
//    u16_Len_SMeg     = LTE_Rx_Buffer[8];
//    u16_Len_SMeg	 = (u16_Len_SMeg << 8) & 0xff00;
//    u16_Len_SMeg	 = u16_Len_SMeg|LTE_Rx_Buffer[9];
//    u16_Len_SMeg    += 12;
//    memcpy(LTE_Tx_Buffer,LTE_Rx_Buffer,u16_Len_SMeg);	
//	
//    if((Rx_Buffer[1] == g_SysData.u8_Gateway_ID[0])
//    && (Rx_Buffer[2] == g_SysData.u8_Gateway_ID[1])
//    && (Rx_Buffer[3] == g_SysData.u8_Gateway_ID[2])
//    && (Rx_Buffer[4] == g_SysData.u8_Gateway_ID[3])
//    && (Rx_Buffer[5] == g_SysData.u8_Gateway_ID[4])
//    && (Rx_Buffer[6] == g_SysData.u8_Gateway_ID[5]))
//    {
//		// 加速度报警阈值
//		U16 Rx_Thrsh_Len = 0; 
//		Rx_Thrsh_Len = Rx_Buffer[15];
//        u16_temp =  (Rx_Thrsh_Len <<8) & 0xff00;
//		u16_temp = u16_temp | Rx_Buffer[16];
//        g_SysData.u16_AcclWarningTreshold = u16_temp;

//		// 休眠间隔
//		U16 Rx_Sleep_Len = 0; 
//		Rx_Sleep_Len = Rx_Buffer[17];
//        u16_temp2 =  (Rx_Sleep_Len <<8) & 0xff00;
//		u16_temp2 = u16_temp2 | Rx_Buffer[18];
//		g_SysData.u16_SleepGap = u16_temp2;

//        if(u16_temp <= 60000 && u16_temp2 <= 2880)
//        {

//            // 向EEPROM中写入下发的设定值
//            if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
//            {
//                unEeprom.stData.wAcclTresholdSetValue = g_SysData.u16_AcclWarningTreshold;
//				unEeprom.stData.wSleepGap = g_SysData.u16_SleepGap;
//                bEepromWriteFlag = 1;
//				EC20_Send_Cal(&EC20_obj, LTE_Tx_Buffer, u16_Len_SMeg);//回送说明写成功

//            }

//        }

//    }
//	    else
//    {
//        //密码校验错误
//        LTE_Pack_Func(3);
//        sMy_4G_Emitte(LTE_Tx_Buffer);
//        memset( LTE_Tx_Buffer,0xff, Max_Len_1 );
//    }
//}

