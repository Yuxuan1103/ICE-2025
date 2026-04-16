#include "sx1278.h"
#include "spi.h"
#include "string.h"
#include "main.h"

uint8_t RFBuffer[128];
SPI_HandleTypeDef  Hspi;
uint8_t sta1 = 0;




void SX1278_GpioInit(void);
void SX1278_SPI_Init(void);
void SX1278_WriteReg(uint8_t RegAddr, uint8_t Value);
uint8_t SX1278_ReadReg(uint8_t Addr);
void SX1278_RegInitConfig(void);
void SX1278_LoraInit(void);
void SX1278_ReadBuf(uint8_t RegAddr, uint8_t *ReadData, uint8_t len);
void SX1278_WriteBuf(uint8_t RegAddr, uint8_t *pBuf, uint8_t len);
void SX1278_ConfigTxMode(void);
void SPI_SetSpeed(uint8_t SpeedSet);
void SX1278_ConfigRxMode(void);
void SX1278_SendPacket(uint8_t *SendBuff, uint8_t len);
uint8_t SX1278_ReceivePacket(uint8_t *RFBuffer,uint8_t * lengthRX ,uint32_t waittime);
uint8_t SX1278_ReceivePacket1(uint8_t *RFBuffer, uint8_t* length);
uint8_t SX1278_HandleRxOvertime(void);
void SX1278_SetSleepMode(void);



/***********************************************
函数名：SX1278_GpioInit
功  能：初始化与SX1278相关的IO口
参  数：无
返回值：无
************************************************/

void SX1278_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*配置与SX1278的CS、RESET相连接的管脚*/


    HAL_GPIO_WritePin(GPIOB, SX1278_CS, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = SX1278_CS;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      //输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;              //上拉下拉都不激活
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);     

    HAL_GPIO_WritePin(GPIOA, SX1278_RESET, GPIO_PIN_RESET);
    GPIO_InitStruct.Pin = SX1278_RESET;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      //输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;              //上拉下拉都不激活
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	//SX1278 IO口在浮空输入STOP模式功耗最低！
    GPIO_InitStruct.Pin =  SX1278_DIO0|SX1278_DIO1|SX1278_DIO2|SX1278_DIO4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;           //输入
    GPIO_InitStruct.Pull = GPIO_NOPULL;              //上拉下拉都不激活
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);  


    GPIO_InitStruct.Pin = SX1278_DIO3|SX1278_DIO5;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;      //上升沿中断触发模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;              //上拉下拉都不激活
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    // HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

    SX1278_RTSET_H; //拉高RESET脚，使能SX1278
    //  SX1278_POWER_H;
}

/***********************************************
函数名：SPI_RegisterMode_Init
功  能：寄存器方式配置SPI1口
参  数：无
返回值：无
************************************************/
void SX1278_SPI_Init(void)
{
    /*配置SPI1*/
    Hspi.Instance = SPI2;
    Hspi.Init.Mode = SPI_MODE_MASTER;
    Hspi.Init.Direction = SPI_DIRECTION_2LINES;
    Hspi.Init.DataSize = SPI_DATASIZE_8BIT;
    Hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
    Hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
    Hspi.Init.NSS = SPI_NSS_SOFT;
    Hspi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    Hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    Hspi.Init.TIMode = SPI_TIMODE_DISABLE;
    Hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    Hspi.Init.CRCPolynomial = 7;
    
    Hspi.State = HAL_SPI_STATE_RESET;

    //hspi1.State = HAL_SPI_STATE_RESET;
    
    if (HAL_SPI_Init(&Hspi) != HAL_OK)
    {
        //Error_Handler(__FILE__, __LINE__);
    }     
}

/***********************************************
函数名：SX1278_WriteReg
功  能：配置SX1278寄存器函数
参  数：RegAddr；要配置的寄存器地址
        value：要配置的数据
返回值：无
************************************************/
void SX1278_WriteReg(uint8_t RegAddr, uint8_t Value)
{
//    uint8_t rxData;
    SX1278_CS_L;
    RegAddr |= 0x80;  //最高位为1，为写模式
    //HAL_SPI_TransmitReceive( &Hspi,  &RegAddr,  &rxData, 1, HAL_MAX_DELAY);
    //HAL_SPI_TransmitReceive( &Hspi,  &value,  &rxData, 1, HAL_MAX_DELAY);
    
    HAL_SPI_Transmit(&Hspi, &RegAddr, 0x01, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&Hspi, &Value, 0x01, HAL_MAX_DELAY);
    SX1278_CS_H;
}


/***********************************************
函数名：SX1278_ReadWriteByte
功  能：寄存器方式读取指定地址里的一个字节
参  数：Addr：为指定要读的地址
返回值：返回读到的一个字节数
************************************************/
uint8_t SX1278_ReadReg(uint8_t Addr)
{  
    uint8_t ReadData;
    SX1278_CS_L;
    HAL_SPI_Transmit(&Hspi, &Addr, 0x01, HAL_MAX_DELAY);
    HAL_SPI_Receive(&Hspi, &ReadData, 0x01, HAL_MAX_DELAY);
    SX1278_CS_H;
    return ReadData;
}

/***********************************************
函数名：SX1278_RegInitConfig
功  能：初始化SX1278的内部寄存器
参  数：无
返回值：无
************************************************/
void SX1278_RegInitConfig(void)
{
    SX1278_WriteReg(REG_LR_OPMODE, 0x88);     //设置lora的sleep模式

    //SX1278_WriteReg(REG_LR_FRFMSB, LR_SET_FRFMSB);    //设置lora的RF，491MHZ  //频率/信道   6c4000  433
    //SX1278_WriteReg(REG_LR_FRFMID, LR_SET_FRFMID);
    //SX1278_WriteReg(REG_LR_FRFLSB, LR_SET_FRFLSB);
    
    SX1278_WriteReg(REG_LR_FRFMSB, g_SysData.u8_RF_FRENQ_H);    //设置lora的RF
    SX1278_WriteReg(REG_LR_FRFMID, g_SysData.u8_RF_FRENQ_M);
    SX1278_WriteReg(REG_LR_FRFLSB, g_SysData.u8_RF_FRENQ_L);

    SX1278_WriteReg(REG_LR_LORADETECTOPTIMIZE, 0xc3);
    SX1278_WriteReg(REG_LR_MODEMCONFIG2, 0x94);    //设置lora的spreadingfactor为12，normal mode即每次发送一个包，crc使能，序头9最大检测时间symbtimeout为74MS
    SX1278_WriteReg(REG_LR_SYMBTIMEOUTLSB, 0x09);  //关闭前导码
    SX1278_WriteReg(REG_LR_MODEMCONFIG1, 0x78);    //设置lora的codingrate为4，带宽为500khz，explicit head mode
    SX1278_WriteReg(REG_LR_PACONFIG, 0xff);        //设置lora的输出功率为17dbm，可以达到,有疑问，不确定
    //SX1278_WriteReg(REG_LR_PADAC,0x87);//20dbm
    //SX1278_WriteReg(REG_LR_OCP,0x37);//200mA
    SX1278_WriteReg(REG_LR_MODEMCONFIG3, 0x0c);
    SX1278_WriteReg(REG_LR_OPMODE, 0x89);//设置成stand_by模式
}

/***********************************************
函数名：SX1278_LoraInit
功  能：初始化SX1278
参  数：无
返回值：无
************************************************/
void SX1278_LoraInit(void)
{
    SX1278_GpioInit();
    SX1278_SPI_Init();
    //SPI_SimulateMode_Init();
    /*检测SPI口是否能通信*/   
    while( SX1278_ReadReg(0x42) != 0x12)
    {

    }

    RF_Frenq_Set(&g_SysData.u16_Tower_Num);
    SX1278_RegInitConfig();
}


/***********************************************
函数名：SX1278_ReadBuf
功  能：把接收到的数据从fifo寄存器中读出来
参  数：RegAddr：寄存器地址
        ReadData：读出的数据存储的位置
        len：数据包长度
返回值：wu
************************************************/
void SX1278_ReadBuf(uint8_t RegAddr, uint8_t *ReadData, uint8_t len)
{
    SX1278_CS_L;  
    HAL_SPI_Transmit(&Hspi, &RegAddr, 1, HAL_MAX_DELAY);  
    HAL_SPI_Receive(&Hspi, ReadData, len, HAL_MAX_DELAY);
    SX1278_CS_H;                 
}

/***********************************************
函数名：SX1278_WriteBuf
功  能：把要发送的数据包写到fifo
参  数：RegAddr：寄存器地址
        pBuf：数据包的首地址
        len：数据包长度
返回值：发送成功返回0
************************************************/
void SX1278_WriteBuf(uint8_t RegAddr, uint8_t *pBuf, uint8_t len)
{
    RegAddr |= 0x80;    
    SX1278_CS_L;  
    HAL_SPI_Transmit(&Hspi, &RegAddr, 1, HAL_MAX_DELAY);  
    HAL_SPI_Transmit(&Hspi, pBuf, len, HAL_MAX_DELAY);	  //发送数据
    SX1278_CS_H;                 
}

/***********************************************
函数名：SX1278_ConfigTxMode
功  能：配置SX1278为发送模式
参  数：无
返回值：无
************************************************/
void SX1278_ConfigTxMode(void)
{
    SX1278_WriteReg(REG_LR_OPMODE, 0x89);            //设置成stand_by模式
    SX1278_WriteReg(REG_LR_IRQFLAGSMASK, 0xf7);      //打开txdown中断
    SX1278_WriteReg(REG_LR_FIFOTXBASEADDR, 0X00);    //设置发射初始地址为0X00;
    SX1278_WriteReg(REG_LR_FIFOADDRPTR, 0X00);//设置fifo寻址指针初始地址为发射初始地址,与资料设置不同，需考虑
    //配置diomaping1,2对应管脚，表示中断，DIO口txdown，DIO1口rxtimeout，DIO2口fhsschangechannel，
    //DIO3口validheader，DIO4口plllock，DIO5口modeready
    SX1278_WriteReg(REG_LR_DIOMAPPING1, 0X41);
    SX1278_WriteReg(REG_LR_DIOMAPPING2, 0X40);
    //  SX1278_TX_L;
    //  SX1278_RX_H;
}

/***********************************************
函数名：SPI_SetSpeed
功  能：设置SPI1口的通信速率
参  数：SpeedSet：为指定要设置的速率
返回值：无
************************************************/
void SPI_SetSpeed(uint8_t SpeedSet)
{
    SpeedSet &= 0X07;           //限制范围
    SPI1->CR1 &= 0XFFC7; 
    SPI1->CR1 |= SpeedSet<<3;   //设置SPI1速度  
    SPI1->CR1 |= 1<<6;          //SPI设备使能 
}


/***********************************************
函数名：SX1278_ConfigRxMode
功  能：配置SX1278为接收模式
参  数：无
返回值：无
************************************************/
void SX1278_ConfigRxMode(void)
{
    SX1278_WriteReg(REG_LR_OPMODE,0x89);    //设置成stand_by模式
    SX1278_WriteReg(REG_LR_IRQFLAGSMASK,0x1f);//打开rxtimeout,rxdown,payloadcrcerror中断   
    //配置diomaping1,2对应管脚，表示中断，DIO口rxdown，DIO1口rxtimeout，DIO2口fhsschangechannel，DIO3口caddown，DIO4口caddetected，DIO5口modeready
    SX1278_WriteReg(REG_LR_DIOMAPPING1,0X00);
    SX1278_WriteReg(REG_LR_DIOMAPPING2,0X00);
    //sx1278_Write_Reg(REG_LR_HOPPERIOD,255);
    //sta=sx1278_Read_Reg(REG_LR_FIFORXBASEADDR);
    //LCD_ShowNum(90,110,sta,3,16);
    //sx1278_Write_Reg(REG_LR_FIFOADDRPTR,0X00);    //寻址指针指向rxbaseaddr即0x00
    // SX1278_TX_H;
    // SX1278_RX_L;
    SX1278_WriteReg(REG_LR_OPMODE,0x8D);    //连续接收模式，启动接收    //0x8E单次接收模式
    SPI_SetSpeed(SPI_SPEED_32);    //spi速度为2.25Mhz（24L01的最大SPI时钟为10Mhz）
}

/***********************************************
函数名：SX1278_SendPacket
功  能：SX1278无线发送一个数据包
参  数：SendBuff：要发送的数据包首地址
        len：要发送的数据包长度
返回值：发送成功返回0
************************************************/
void SX1278_SendPacket(uint8_t *SendBuff, uint8_t len)
{
    uint8_t txpacketsize = len;
    SPI_SetSpeed(SPI_SPEED_32);    //spi速度为2.25Mhz（24L01的最大SPI时钟为10Mhz）
    SX1278_WriteReg(REG_LR_PAYLOADLENGTH,txpacketsize);//设置负载的长度，在implict模式中需要设置
    SX1278_WriteBuf(0X00,SendBuff,txpacketsize);//将要发送的数据写入fifo中
    SX1278_WriteReg(REG_LR_OPMODE,0x8B);//启动发射模式

    //g_statecheck.bit.bLora_SendingState = cLoraWorking;

    while(SX1278_READ_DIO0!=1)//发射成功，产生txdown中断
    {
        ;
    }
    //HAL_Delay(100);
    
    SX1278_ReadReg(REG_LR_IRQFLAGS);  //读取状态寄存器的值
    SX1278_WriteReg(REG_LR_IRQFLAGS,0x08);//清除中断
    SX1278_WriteReg(REG_LR_OPMODE,0x88); //清除fifo中数据，设置lora的sleep模式	
}

/***********************************************
函数名：SX1278_SendPacket
功  能：SX1278无线接收一个数据包
参  数：SendBuff：存接收到的数据
返回值：发送成功返回0
************************************************/
U16 WATCH1;
U16 WATCH2;
uint8_t SX1278_ReceivePacket(uint8_t *RFBuffer,uint8_t * lengthRX ,uint32_t waittime)
{
    //uint8_t currentaddr;
    uint8_t sta = 0,timeoutflag = 0;
    uint8_t length;
    SPI_SetSpeed(SPI_SPEED_32);    //spi速度为2.25Mhz(24L01的最大SPI时钟为10Mhz)
    memset( RFBuffer, 0, 128 );    //将接收数组rfbuffer清空

    LoraCommCallBackTick = waittime;  //保持接收waittime  ms
    WATCH1 = SX1278_READ_DIO0;
    WATCH2 = SX1278_READ_DIO1;
    while(((!SX1278_READ_DIO0) && (!SX1278_READ_DIO1)) != 0)
    {
        if(LoraCommCallBackTick == 0) 
        {
            timeoutflag = 1;
            break;
        }
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);//等待指示灯
    }
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);//退出等待
    if(timeoutflag == 1) 
    {
        SX1278_WriteReg(REG_LR_OPMODE, 0x88);    //清除fifo中数据即设置lora的sleep模式	
        return 0;
    }

    sta = SX1278_ReadReg(REG_LR_IRQFLAGS);
    sta1 = sta;
    if(sta & 0x80)
    {
        SX1278_WriteReg(REG_LR_IRQFLAGS,0x80);    //清除rxtimeout中断标志?
        SX1278_WriteReg(REG_LR_OPMODE,0x89);      //返回standby模式
        return 0;
    }
    else
    {
        sta = SX1278_ReadReg(REG_LR_IRQFLAGS);
        while(SX1278_READ_DIO0 != 1);//等待产生中断
        
        sta = SX1278_ReadReg(REG_LR_IRQFLAGS);

        //判断中断类型 
        SX1278_WriteReg(REG_LR_IRQFLAGS,0x40);//清除rxdown中断标志
        if(sta & 0x20)//payloadcrcerror
        {
            //连续包接收，接收到错误包就返回sleep模式
            SX1278_WriteReg(REG_LR_IRQFLAGS, 0x20);//清除error中断标志
            SX1278_WriteReg(REG_LR_OPMODE, 0x88); //清除fifo中数据即设置lora的sleep模式
            return 0;
        }
        else
        {
            //sta = SX1278_ReadReg(REG_LR_IRQFLAGS);
            SX1278_WriteReg(REG_LR_FIFOADDRPTR, 0X00);    //寻址指针指向rxbaseaddr即0x00
            length = SX1278_ReadReg(REG_LR_NBRXBYTES);    //读取当前rx数据指针的所指地址
            SX1278_ReadBuf(0X00, RFBuffer, length);       //读出fifo中数据到RFBuffer中，读取数据从ptr指针所指地址开始
            SX1278_WriteReg(REG_LR_OPMODE,0x88);          //清除fifo中数据即设置lora的sleep模式
            //length = SX1278_ReadReg(REG_LR_NBRXBYTES);  //读取当前rx数据指针的所指地址

            * lengthRX = length;
            return 1;
        }
    }
}

uint8_t SX1278_ReceivePacket1(uint8_t *RFBuffer, uint8_t * length)
{
    uint8_t sta = 0;   
    sta = SX1278_ReadReg(REG_LR_IRQFLAGS);
    if(sta & 0x20)            //连续包接收，接收到错误包就返回sleep模式
    {
        SX1278_WriteReg(REG_LR_IRQFLAGS, 0x20);    //清除error中断标志
        SX1278_WriteReg(REG_LR_OPMODE, 0x88);      //清除fifo中数据即设置lora的sleep模式	
        return 0x03;
    }
    if(sta & 0x40)           //成功就收到数据包中断
    {
        SX1278_WriteReg(REG_LR_IRQFLAGS,0x40);//清除rxdown中断标志
        // sta = SX1278_ReadReg(REG_LR_IRQFLAGS);
        memset( RFBuffer, 0, 128 );               //将接收数组rfbuffer清空
        SX1278_WriteReg(REG_LR_FIFOADDRPTR, 0X00);//寻址指针指向rxbaseaddr即0x00
        *length = SX1278_ReadReg(REG_LR_NBRXBYTES);//读取当前rx数据指针的所指地址
        SX1278_ReadBuf(0X00, RFBuffer, *length);//读出fifo中数据到RFBuffer中，读取数据从ptr指针所指地址开始
        SX1278_WriteReg(REG_LR_OPMODE,0x88); //清除fifo中数据即设置lora的sleep模式
        //length = SX1278_ReadReg(REG_LR_NBRXBYTES);//读取当前rx数据指针的所指地址
        return 0x02;
    }  
    if(sta & 0x08)  //发送成功中断
    {
        SX1278_WriteReg(REG_LR_IRQFLAGS,0x08);    //清除中断
        SX1278_WriteReg(REG_LR_OPMODE,0x88);      //清除fifo中数据，设置lora的sleep模式
        return 0x01; 
    }
    
    return 0;
}

uint8_t SX1278_HandleRxOvertime(void)
{
    uint8_t vesion = 0;
    vesion = SX1278_ReadReg(REG_LR_IRQFLAGS);
    if(vesion & 0x80)
    {
        SX1278_WriteReg(REG_LR_IRQFLAGS,0x80);    //清除rxtimeout中断标志
        SX1278_WriteReg(REG_LR_OPMODE,0x89);      //返回standby模式
        return 0x04;
    }
    return 0x00;
}

///***************************************************************************************************************/
///***********************************************
//函数名：SPI_SimulateMode_Init
//功  能：以模拟的方式配置SPI口
//参  数：无
//返回值：无
//************************************************/
//void SPI_SimulateMode_Init(void)
//{
//    GPIO_InitTypeDef GPIO_InitStruct;
//    GPIO_InitStruct.Pin = SX1278_SCK|SX1278_MOSI|SX1278_CS|SX1278_RESET;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//    GPIO_InitStruct.Pull = GPIO_PULLUP;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//	
//    GPIO_InitStruct.Pin = SX1278_MISO;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//    
//    
//        /*检测SPI口是否能通信*/
// 	while( SPI_SimulateMode_ReadWriteByte(0x42) != 0x12)
// 	{
//        _Error_Handler(__FILE__, __LINE__);
// 	}
//}

///***********************************************
//函数名：SPI_SimulateMode_Init
//功  能：以SPI模拟的方式写一个字节的数
//参  数：data：为要发送的数
//返回值：无
//************************************************/
//void SPI_SimulateMode_WriteByte(uint8_t data)
//{
//    uint8_t i;
//    for(i = 0; i < 8; i++)
//    {
//        SPI_Delay_us(1);
//        SX1278_SCK_L;
//        SPI_Delay_us(1);
//        if((data<<i) & 0x80)
//            SX1278_MOSI_H;
//        else
//            SX1278_MOSI_L;
//        SPI_Delay_us(1);
//        SX1278_SCK_H;
//    }
//    SPI_Delay_us(1);
//    SX1278_SCK_L;
//    SX1278_MOSI_L;
//}

///***********************************************
//函数名：SPI_SimulateMode_ReadByte
//功  能：以SPI模拟的方式读一个字节
//参  数：无
//返回值：返回读到的一个字节
//************************************************/
//uint8_t SPI_SimulateMode_ReadByte(void)
//{
//    uint8_t i = 0;
//    uint8_t ReadData = 0;
//    for(i = 0; i < 8; i++)
//    {
//        SX1278_SCK_L;
//        SPI_Delay_us(1);
//          ReadData = (ReadData<<1);
//        SX1278_SCK_H;
//        SPI_Delay_us(1);
//      if( SX1278_READ_MISO == GPIO_PIN_SET)
//      {
//            ReadData |= 0x01;
//      }
//        SPI_Delay_us(1);
//    }
//    SX1278_SCK_L;
//    SPI_Delay_us(1);
//    return ReadData;
//}

///***********************************************
//函数名：SPI_SimulateMode_ReadWriteByte
//功  能：模拟spI口、从指定的地址读一个字节的数
//参  数：Addr：用于指定要读的地址
//返回值：返回读到的一个字节
//************************************************/
//uint8_t SPI_SimulateMode_ReadWriteByte(uint8_t Addr)
//{
//    uint8_t ReadData = 0;
//    SX1278_CS_L;
//    SPI_SimulateMode_WriteByte(Addr & 0x7F);
//    ReadData = SPI_SimulateMode_ReadByte();
//    SX1278_CS_H;
//    return ReadData;
//}

///***********************************************
//函数名：SPI_Delay_us
//功  能：模拟spI口时做演示用
//参  数：DelayTime：延时的时长
//返回值：无
//************************************************/
//void SPI_Delay_us(uint16_t DelayTime)
//{
//    uint16_t i,j;
//  for(i = 0; i < DelayTime; i++)
//  {
//      for(j = 0; j < DelayTime; j++);
//  }
//}

//void SPI_SimulateMode_WriteReg(uint8_t RegAddr, uint8_t value)
//{
//    RegAddr |= 0x80;  //最高位为1，为写模式
//    SPI_SimulateMode_WriteByte(RegAddr);
//    SPI_SimulateMode_WriteByte(value);
//    //status = SPI_ReadWriteByte(addr|0x80);
//    //SPI_ReadWriteByte(value);
//      SX1278_CS_H;

//}


void SX1278_SetSleepMode(void)
{
    //uint8_t   value = SX1278_ReadReg(REG_LR_OPMODE);
    //value &= 0xF8;
    //value |= RFLR_OPMODE_SLEEP;
    //SX1278_WriteReg(REG_LR_OPMODE, value);
//    SX1278_WriteReg(REG_LR_OPMODE,0x89);    //设置成stand_by模式
    
    SX1278_WriteReg(REG_LR_OPMODE,0x88);
    //HAL_Delay(33);    //需要一段
}




