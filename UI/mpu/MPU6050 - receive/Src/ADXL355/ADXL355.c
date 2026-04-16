/***************************************************************************************************
* Copyright (C)2019
* File Name                             :     ADXL355.c 
* Description                           :     ADXL355读写相关函数
* Version                               :     
* Author                                :     YXD
* Creat Date                            :     20191108
* notice                                :     
****************************************************************************************************/
#include "main.h"
#include "ADXL355.h"
uint8_t sADXL355_THRESH_H;
uint8_t sADXL355_THRESH_L;

ADXL355_REGS sXD355_DATA;

u8 W_data[9];
u8 R_data[9];

u16  ADXL355_ReadID(void);
void ADXL355_POWER_ON(void);
void ADXL355_RESET(void);

void SPI_ADXL355_S_WRITE(u8 adress, u8 data);
u8   SPI_ADXL355_S_READ(u8 adress);
void SPI_ADXL355_M_WRITE(u8 adress, u8 TXCONT);
U32 SPI_ADXL355_M_READ(u8 adress, enRegsNum enRegs);
void ADXL355_Check(U16 RW,U16 Order,U16 Data);


void ADXL355_Init(void);


void ADXL355_GpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    /*配置与SX1278的CS、RESET相连接的管脚*/

    
    GPIO_InitStruct.Pin = ADXL355_CS;
    //GPIO_InitStruct.Pin =  SX1278_RESET;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      //输出模式
    GPIO_InitStruct.Pull = GPIO_NOPULL;              //上拉下拉都不激活
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 
    HAL_GPIO_WritePin(GPIOA, ADXL355_CS, GPIO_PIN_SET);
}


/***********************************************
函数名：&hspi2_WriteReg
功  能：配置&hspi2寄存器函数
参  数：RegAddr；要配置的寄存器地址
        value：要配置的数据
返回值：无
************************************************/
void ADXL355_WriteReg(uint8_t RegAddr, uint8_t Value)
{
    uint8_t Write_Adress;
    Write_Adress = RegAddr << 1;//最低位为 0， 写模式
    Write_Adress |= 0x00;
    ADXL355_CS_L;
    //HAL_SPI_TransmitReceive( &Hspi,  &RegAddr,  &rxData, 1, HAL_MAX_DELAY);
    //HAL_SPI_TransmitReceive( &Hspi,  &value,  &rxData, 1, HAL_MAX_DELAY);
      
    HAL_SPI_Transmit(&hspi1, &Write_Adress, 0x01, HAL_MAX_DELAY);
    HAL_SPI_Transmit(&hspi1, &Value, 0x01, HAL_MAX_DELAY);
    ADXL355_CS_H;
}

/***********************************************
函数名：ADXL355_ReadWriteByte
功  能：寄存器方式读取指定地址里的一个字节
参  数：Addr：为指定要读的地址
返回值：返回读到的一个字节数
************************************************/
uint8_t ADXL355_ReadReg(uint8_t Addr)
{  
    uint8_t ReadData;
    uint8_t Write_Adress;
    Write_Adress = Addr << 1;//最低位为 1， 读模式
    Write_Adress |= 0x01;
    ADXL355_CS_L;
    HAL_SPI_Transmit(&hspi1, &Write_Adress, 0x01, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, &ReadData, 0x01, HAL_MAX_DELAY);
    ADXL355_CS_H;
    return ReadData;
}

/***********************************************
函数名：ADXL355_ReadWriteByte
功  能：寄存器方式读取指定地址里的一个字节
参  数：Addr：为指定要读的地址
返回值：返回读到的一个字节数
************************************************/
uint8_t ADXL355_ReadBuff(uint8_t Addr, uint8_t *buff, uint8_t size)
{  
    uint8_t ReadData;
    uint8_t Write_Adress;
    Write_Adress = Addr << 1;//最低位为 1， 读模式
    Write_Adress |= 0x01;
    ADXL355_CS_L;
    HAL_SPI_Transmit(&hspi1, &Write_Adress, 0x01, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, buff, size, HAL_MAX_DELAY);
    ADXL355_CS_H;
    return ReadData;
}

/*******************************************************************************
* Function Name  : ADXL355_ReadID()
* Description    : 读355ID函数
* Input          : None
* Output         : None.
* Return         : None.   ED
*******************************************************************************/

u16 ADXL355_ReadID(void)
{
    u16 Temp = 0;
    Temp = ADXL355_ReadReg(XL355_PARTID);
 //   Temp = SPI1_ReadWriteByte(0xEE);
    return Temp;
}

/*******************************************************************************
* Function Name  : ADXL355_POWER_ON()
* Description    : 355上电函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

//void ADXL355_POWER_ON(void)
//{
//    SPI1_ReadWriteByte(XL355_POWER_CTL);
//    SPI1_ReadWriteByte(0x00);
//}


/*******************************************************************************
* Function Name  : ADXL355_RESET()
* Description    : 355复位函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

//void ADXL355_RESET(void)
//{
//    SPI1_ReadWriteByte(XL355_POWER_CTL);
//    SPI1_ReadWriteByte(0x00);
//}




/*******************************************************************************
* Function Name  : SPI_ADXL355_S_WRITE()
* Description    : 355 single byte 写 函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

void SPI_ADXL355_S_WRITE(u8 adress, u8 data)
{

//    u8 Write_Adress;
//    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
//    //XL355_CS = 0;
//    //delay_us(1);                       //CS to SCLK set up time
//    Write_Adress = adress <<1;
//    SPI1_ReadWriteByte(Write_Adress);    //write adress
//    SPI1_ReadWriteByte(data);            //data to be written
//    //delay_us(1);                       //CS to SCLK set up time
//    GPIO_SetBits(GPIOA,GPIO_Pin_4);
//    //XL355_CS = 1;
//    delay_us(1);                         //Wait Time
	
    ADXL355_WriteReg(adress, data);

}

/*******************************************************************************
* Function Name  : SPI_ADXL355_M_WRITE()
* Description    : 355 multi-byte 写 函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

//void SPI_ADXL355_M_WRITE(u8 adress, u8 TXCONT)
//{
//    unsigned char    TX_BYTE;
//    unsigned char    Write_Adress;
//    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
//    delay_us(1);                                        //CS to SCLK set up time
//    Write_Adress = adress << 1;
//    Write_Adress |= 0x00;
//    SPI1_ReadWriteByte(Write_Adress);                   //write adress
//    for(TX_BYTE=0; TX_BYTE<TXCONT; TX_BYTE++)
//    {
//        SPI1_ReadWriteByte(W_data[TX_BYTE]);            //data to be written
//    }
//    delay_us(1);                                        //CS to SCLK set up time
//    GPIO_SetBits(GPIOA,GPIO_Pin_4);                     //
//    delay_us(1);                                        //Wait Time
//}


/*******************************************************************************
* Function Name  : SPI_ADXL355_S_READ()
* Description    : 355 single-byte 读 函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

u8 SPI_ADXL355_S_READ(u8 adress)  
{

//    u8 result;
//    u8 Read_Adress;
//    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
//    //delay_us(1);                                   //Wait Time      //CS to SCLK set up time
//    Read_Adress = ((adress <<1)|ADXL355_READ);
//    result = SPI1_ReadWriteByte(Read_Adress);      //read adress
//    result = SPI1_ReadWriteByte(0x00);             //Read 8bit data (Send Dummy data 0x00)
//    //delay_us(1);                                   //CS to SCLK set up time
//    GPIO_SetBits(GPIOA,GPIO_Pin_4);
//    //XL355_CS = 1;
//    delay_us(1);                                   //Wait Time

    u8 result;
    result = ADXL355_ReadReg(adress);
    return result;
}

/*******************************************************************************
* Function Name  : SPI_ADXL355_M_READ()
* Description    : 355 multi-byte 读 函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

U32 SPI_ADXL355_M_READ(u8 adress, enRegsNum enRegs)
{

    U32    ui32Result = 0;
    U32    ui32valueL = 0;
    U32    ui32valueM = 0;
    U32    ui32valueH = 0;
    //u8     Read_Adress;
    //u8     ReadData;
    u8     ReadDatabuff[3];

//    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
//    ADXL355_CS_L;
//    delay_us(1);                                        //CS to SCLK set up time
//      Read_Adress = adress<<1;
//      Read_Adress |= ADXL355_READ;
//    SPI1_ReadWriteByte(Read_Adress);                    //read adress
//    HAL_SPI_Transmit(&hspi2, &Read_Adress, 0x01, HAL_MAX_DELAY);
   
    switch(enRegs)
    {
        case SPI_READ_ONE_REG:
        //     ui32Result = SPI1_ReadWriteByte(0xff);      // Set read result

                ADXL355_ReadBuff(adress, ReadDatabuff, 1);
                ui32Result = ReadDatabuff[0] ;
            break;

        case SPI_READ_TWO_REG:
//            ui32valueH = SPI1_ReadWriteByte(0xff);      // Read 8bit data (Send Dummy data 0xff)
//            ui32valueL = SPI1_ReadWriteByte(0xff);      // Read 8bit data (Send Dummy data 0xff)
//            ui32Result = ((ui32valueH << 8) | ui32valueL);      // Set read result

                ADXL355_ReadBuff(adress, ReadDatabuff, 2);
                ui32valueH = ReadDatabuff[0] ;
                ui32valueL = ReadDatabuff[1] ;
                ui32Result =  ((ui32valueH << 8) | ui32valueL);
            break;

        case SPI_READ_THREE_REG:
//            ui32valueH = SPI1_ReadWriteByte(0xff);      // Read 8bit data (Send Dummy data 0xff)
//            ui32valueM = SPI1_ReadWriteByte(0xff);      // Read 8bit data (Send Dummy data 0xff)
//            ui32valueL = SPI1_ReadWriteByte(0xff);      // Read 8bit data (Send Dummy data 0xff)
//            ui32Result = ((ui32valueH << 16) | (ui32valueM << 8) | ui32valueL);;      // Set read result


                ADXL355_ReadBuff(adress, ReadDatabuff, 3);
                ui32valueH = ReadDatabuff[0] ;
                ui32valueM = ReadDatabuff[1] ;
                ui32valueL = ReadDatabuff[2] ;
                ui32Result = ((ui32valueH << 16) | (ui32valueM << 8) | ui32valueL);

            break;

        default:

            break;

    }
//    delay_us(1);                                        //CS to SCLK set up time
//    GPIO_SetBits(GPIOA,GPIO_Pin_4);
//    delay_us(1);                                        //Wait Time
//      ADXL355_CS_H;
    return ui32Result;

}


/*******************************************************************************
* Function Name  : ADXL355_Init()
* Description    : 355 初始化函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

void  ADXL355_SPI_Init(void)
{
    ADXL355_GpioInit();
    SPI1_Init();
}

void ADXL355_Init(void)
{
    u8 ui8temp;
    ADXL355_SPI_Init();
//	ADXL355_EXTI_GPIO();
	
	g_SysData.u16_AcclWarningTreshold = 0x8000;
  sADXL355_THRESH_H = (g_SysData.u16_AcclWarningTreshold >> 8) & 0xFF; 
	sADXL355_THRESH_L = (g_SysData.u16_AcclWarningTreshold) & 0x00FF;
	
//	 printf("加速度值：%d",g_SysData.u16_AcclWarningTreshold); 
	
    SPI_ADXL355_S_WRITE(XL355_RESET,0x52);                        // 重启355 复位
  //  delay_us(200);
    SPI_ADXL355_S_WRITE(XL355_RANGE,XL355_Range_Set);                      // set range :2g 中断在高电平有效
  //  delay_us(200);
    SPI_ADXL355_S_WRITE(XL355_FILTER,0x02);                      // set filter 250Hz  sampling 1000Hz
  //  delay_us(200);
	SPI_ADXL355_S_WRITE(XL355_INT_MAP,0x00);   //配置阈值触发中断失能
	SPI_ADXL355_S_WRITE(XL355_FIFO_SAMPLES,0x60);
	
	SPI_ADXL355_S_WRITE(XL355_ACT_THRESH_H, sADXL355_THRESH_H); // 中断触发阈值
	SPI_ADXL355_S_WRITE(XL355_ACT_THRESH_L, sADXL355_THRESH_L); 
	SPI_ADXL355_S_WRITE(XL355_ACT_COUNT,0x01); //采样n次后仍高于阈值则触发中断
	SPI_ADXL355_S_WRITE(XL355_ACT_EN,0x07);
	SPI_ADXL355_S_WRITE(XL355_INT_MAP,0x08);   //Activity阈值触发中断使能
	
	ui8temp = (u8)SPI_ADXL355_S_READ(XL355_POWER_CTL);       /* Read POWER_CTL register, before modifying it */
    ui8temp = ui8temp & 0x00;                                          /* Set measurement bit in POWER_CTL register */
    SPI_ADXL355_S_WRITE(XL355_POWER_CTL, ui8temp);   //配置寄存器为测量模式  
              
 //   delay_us(200);

    sXD355_DATA.ADXLDATA.u8_POWER_CTL  = SPI_ADXL355_S_READ(XL355_POWER_CTL);

    //delay_us(200);

    sXD355_DATA.ADXLDATA.u8_DEVID_AD  = SPI_ADXL355_S_READ(XL355_DEVID_AD);
 //   delay_us(200);
    sXD355_DATA.ADXLDATA.u8_DEVID_MST = SPI_ADXL355_S_READ(XL355_DEVID_MST);
  //  delay_us(200);
    sXD355_DATA.ADXLDATA.u8_PARTID    = SPI_ADXL355_S_READ(XL355_PARTID);
 //   delay_us(200);
    sXD355_DATA.ADXLDATA.u8_REVID     = SPI_ADXL355_S_READ(XL355_REVID);
//    delay_us(200);

    sXD355_DATA.ADXLDATA.u8_Range     = SPI_ADXL355_S_READ(XL355_RANGE);

}


/*******************************************************************************
* Function Name  : ADXL355_Check()
* Description    : 355 multi-byte 写 函数
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/


void ADXL355_Check(U16 RW,U16 Order,U16 Data)
{

    U16    Order_temp;
    Order_temp = Order;
    switch(Order_temp)
    {
        case ORDER_DEVID_AD :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_DEVID_AD = SPI_ADXL355_S_READ(XL355_DEVID_AD);
               // delay_us(200);
                printf("DEVID_AD= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_DEVID_AD);
            }
            else if(RW == ORDER_WRITE)
            {
                printf("INPUT ERR\n,");
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_DEVID_MST :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_DEVID_MST = SPI_ADXL355_S_READ(XL355_DEVID_MST);
               // delay_us(200);
                printf("DEVID_MST= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_DEVID_MST);
            }
            else if(RW == ORDER_WRITE)
            {
                printf("INPUT ERR\n,");
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_PARTID :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_PARTID = SPI_ADXL355_S_READ(XL355_PARTID);
             //   delay_us(200);
                printf("PARTID= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_PARTID);
            }
            else if(RW == ORDER_WRITE)
            {
                printf("INPUT ERR\n,");
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_REVID :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_REVID = SPI_ADXL355_S_READ(XL355_REVID);
//                delay_us(200);
                printf("REVID= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_REVID);
            }
            else if(RW == ORDER_WRITE)
            {
                printf("INPUT ERR\n,");
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_STATUS :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_Status = SPI_ADXL355_S_READ(XL355_STATUS);
               // delay_us(200);
                printf("STATUS= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_Status);
            }
            else if(RW == ORDER_WRITE)
            {
                printf("INPUT ERR\n,");
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_FILTER :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_Filter = SPI_ADXL355_S_READ(XL355_FILTER);
               // delay_us(200);
                printf("FILTER= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_Filter);
            }
            else if(RW == ORDER_WRITE)
            {
                SPI_ADXL355_S_WRITE(XL355_FILTER, Data);
               // delay_us(200);
                printf("FILTER INPUT = %d\r\r\n,",Data);
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;

        case ORDER_RANGE :
            if(RW == ORDER_READ)
            {
                sXD355_DATA.ADXLDATA.u8_Range = SPI_ADXL355_S_READ(XL355_RANGE);
            //    delay_us(200);
                printf("RANGE= %d\r\r\n,",sXD355_DATA.ADXLDATA.u8_Range);
            }
            else if(RW == ORDER_WRITE)
            {
                SPI_ADXL355_S_WRITE(XL355_RANGE, Data);
             //   delay_us(200);
                printf("RANGE INPUT = %d\r\r\n,",Data);
            }
            else
            {
                printf("INPUT ERR\n,");
            }

        break;
        default :
        break;

        
        
    }
}
