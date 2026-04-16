
#include "main.h"
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

//u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];
//u8 USART1_TX_BUF[USART1_MAX_SEND_LEN];
u16 USART1_RX_STA;  
uint8_t  LTEUartReceiveData = 0;
uint8_t  LTEUartReceiveBuff[200];    //串口接收缓存
uint32_t LTEUartReceiveBuffIndex = 0;
uint8_t  LTEUartRx_OK_Flag = 0;
uint32_t LTEUartRx_Data_Length = 0;
uint32_t LTEUartRx_Time_Tick = 0;
uint8_t  LTEUartTx_OK_Flag = 0;



UART_HandleTypeDef USART1_Handler;
UART_HandleTypeDef USART4_Handler;


uint8_t UartReceiveBuff[20];
uint8_t UartLTEReceiveBuff[100];

uint8_t SendBuffTest[10];


uint8_t UartSendBuff[100];
uint8_t UartReceiveBuffIndex = 0;
uint8_t UartReceiveLTEBuffIndex = 0;


//对上位机下发参数进行配置
void RecidatatoSet(void)
{
    ubitint U16_temp;
    uint8_t U8_CRCSUM;

    U8_CRCSUM = SUM_Check(&UartReceiveBuff[0],(UartReceiveBuff[2] + 3));
    if(U8_CRCSUM == UartReceiveBuff[UartReceiveBuff[2] + 3])
    {
        g_SysData.u8_GatewayID = UartReceiveBuff[3] & 0xff;
        g_SysData.u8_AlloID    = UartReceiveBuff[4] & 0xff;
        
        U16_temp.bitdata.highchar = UartReceiveBuff[5] & 0xff;
        U16_temp.bitdata.lowchar  = UartReceiveBuff[6] & 0xff;
        g_SysData.u16_Tower_Num = U16_temp.id;

        g_SysData.u8_SN[0] = UartReceiveBuff[7]  & 0xff;
        g_SysData.u8_SN[1] = UartReceiveBuff[8]  & 0xff;
        g_SysData.u8_SN[2] = UartReceiveBuff[9]  & 0xff;
        g_SysData.u8_SN[3] = UartReceiveBuff[10] & 0xff;
        g_SysData.u8_SN[4] = UartReceiveBuff[11] & 0xff;
        g_SysData.u8_SN[5] = UartReceiveBuff[12] & 0xff;
        g_SysData.u8_SN[6] = UartReceiveBuff[13] & 0xff;
        
        // 向EEPROM中写入下发的设定值
        if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
        {
            unEeprom.stData.wRoutingGWID     = g_SysData.u8_GatewayID;
            unEeprom.stData.wRoutingLocalID  = g_SysData.u8_AlloID;
            unEeprom.stData.wTowerNum        = g_SysData.u16_Tower_Num;
            unEeprom.stData.wLocalSNCode[0]  = g_SysData.u8_SN[0];
            unEeprom.stData.wLocalSNCode[1]  = g_SysData.u8_SN[1];
            unEeprom.stData.wLocalSNCode[2]  = g_SysData.u8_SN[2];
            unEeprom.stData.wLocalSNCode[3]  = g_SysData.u8_SN[3];
            unEeprom.stData.wLocalSNCode[4]  = g_SysData.u8_SN[4];
            unEeprom.stData.wLocalSNCode[5]  = g_SysData.u8_SN[5];
            unEeprom.stData.wLocalSNCode[6]  = g_SysData.u8_SN[6];
            bEepromWriteFlag = 1;
        }
        else
        {
            printf("AT24C02_BUSY");
        }
    }

    UartReceiveBuffIndex = 0;
    memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));

}


void RecidatatoSet2(void)
{
    ubitint U16_temp;
    uint8_t U8_CRCSUM;

    U8_CRCSUM = SUM_Check(&UartReceiveBuff[0],(UartReceiveBuff[2] + 3));
    if(U8_CRCSUM == UartReceiveBuff[UartReceiveBuff[2] + 3])
    {

        U16_temp.bitdata.highchar = UartReceiveBuff[4] & 0xff;
        U16_temp.bitdata.lowchar  = UartReceiveBuff[3] & 0xff;
        g_SysData.u16_TiltWarningTreshold = U16_temp.id;

        g_SysData.u8_LowVoltTreshold = UartReceiveBuff[5] & 0xff;

        g_SysData.u8_SleepTreshold = UartReceiveBuff[6] & 0xff;

        U16_temp.bitdata.highchar = UartReceiveBuff[8] & 0xff;
        U16_temp.bitdata.lowchar  = UartReceiveBuff[7] & 0xff;
        g_SysData.u16_SampTime = U16_temp.id;
        
        // 向EEPROM中写入下发的设定值
        if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
        {
            unEeprom.stData.wTiltTresholdSetValue = g_SysData.u16_TiltWarningTreshold;
            unEeprom.stData.wLowVoltTresholdSetValue = g_SysData.u8_LowVoltTreshold;
            unEeprom.stData.wSleepTresholdSetValue = g_SysData.u8_SleepTreshold;
            unEeprom.stData.wSampTimeSetValue = g_SysData.u16_SampTime;
            bEepromWriteFlag = 1;
        }
        else
        {
            printf("AT24C02_BUSY");
        }
    }

    UartReceiveBuffIndex = 0;
    memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));

}


void SenddatatoSWJ(void)
{

    U8    U8_CRCSUM = 0;

    ubitfloat f32temp;

    UartSendBuff[0]  = 0xAA;    //head
    UartSendBuff[1]  = cUartSendDataLength;
    UartSendBuff[2]  = (uint8_t)(g_SysData.u8_GatewayID) & 0xff;
    UartSendBuff[3]  = (uint8_t)(g_SysData.u8_AlloID)    & 0xff;
    UartSendBuff[4]  = (uint8_t)(g_SysData.u8_BatVolt)   & 0xff;
    UartSendBuff[5]  = (uint8_t)(g_SysData.u16_Tower_Num >> 8) & 0xff;
    UartSendBuff[6]  = (uint8_t)(g_SysData.u16_Tower_Num )     & 0xff;
    UartSendBuff[7]  = (uint8_t)(g_SysData.u16_SampTime >> 8)  & 0xff;
    UartSendBuff[8]  = (uint8_t)(g_SysData.u16_SampTime )      & 0xff;
    UartSendBuff[9]  = (uint8_t)(g_SysData.u16_TiltWarningTreshold >> 8) & 0xff;
    UartSendBuff[10] = (uint8_t)(g_SysData.u16_TiltWarningTreshold) & 0xff;
    UartSendBuff[11] = (uint8_t)(g_SysData.u8_LowVoltTreshold) & 0xff;
    UartSendBuff[12] = (uint8_t)(g_SysData.u8_SleepTreshold) & 0xff;
    UartSendBuff[13] = (uint8_t)(SoftWare_Version >> 24) & 0xff;
    UartSendBuff[14] = (uint8_t)(SoftWare_Version >> 16) & 0xff;
    UartSendBuff[15] = (uint8_t)(SoftWare_Version >> 8) & 0xff;
    UartSendBuff[16] = (uint8_t)(SoftWare_Version) & 0xff;

    f32temp.fd = sIncl_Data.f32_shunxian_Angle_arc_o;
    UartSendBuff[17] = (uint8_t)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             // 顺线角度
    UartSendBuff[18] = (uint8_t)(f32temp.intdata[1].bitdata.highchar & 0xff);
    UartSendBuff[19] = (uint8_t)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
    UartSendBuff[20] = (uint8_t)(f32temp.intdata[0].bitdata.highchar & 0xff);

    f32temp.fd = sIncl_Data.f32_hengxian_Angle_arc_o;
    UartSendBuff[21] = (uint8_t)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             //
    UartSendBuff[22] = (uint8_t)(f32temp.intdata[1].bitdata.highchar & 0xff);
    UartSendBuff[23] = (uint8_t)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
    UartSendBuff[24] = (uint8_t)(f32temp.intdata[0].bitdata.highchar & 0xff);

    f32temp.fd = sIncl_Data.f32_zonghe_Angle_arc_o;
    UartSendBuff[25] = (uint8_t)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             //
    UartSendBuff[26] = (uint8_t)(f32temp.intdata[1].bitdata.highchar & 0xff);
    UartSendBuff[27] = (uint8_t)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
    UartSendBuff[28] = (uint8_t)(f32temp.intdata[0].bitdata.highchar & 0xff);

    UartSendBuff[29] = (uint8_t)(g_SysData.u8_SN[0]) & 0xff;
    UartSendBuff[30] = (uint8_t)(g_SysData.u8_SN[1]) & 0xff;
    UartSendBuff[31] = (uint8_t)(g_SysData.u8_SN[2]) & 0xff;
    UartSendBuff[32] = (uint8_t)(g_SysData.u8_SN[3]) & 0xff;
    UartSendBuff[33] = (uint8_t)(g_SysData.u8_SN[4]) & 0xff;
    UartSendBuff[34] = (uint8_t)(g_SysData.u8_SN[5]) & 0xff;
    UartSendBuff[35] = (uint8_t)(g_SysData.u8_SN[6]) & 0xff;


    U8_CRCSUM = SUM_Check(&UartSendBuff[0],(UartSendBuff[1] + 1));

    //Cal_CRC16(&UartSendBuff[0],(UartSendBuff[1] + 2),&U8_CRC16H,&U8_CRC16L);

    UartSendBuff[36] = (uint8_t)(U8_CRCSUM);
    //UartSendBuff[30] = (uint8_t)(U8_CRC16L);
    UartSendBuff[37] = 0xff;


    Uart_Send_Array(UartSendBuff, cUartSendDataLength + 4);

    UartReceiveBuffIndex = 0;
    memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
}

void USART4_5_IRQHandler(void)
{

    HAL_UART_IRQHandler(&USART4_Handler); //函数会清空中断标志，取消中断使能，并间接调用下面的回调函数
    //UartRx_Treat();
    
    //HAL_UART_Receive_IT(&USART4_Handler, &UartReceiveBuff[UartReceiveBuffIndex], 1);//打开中断
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART4)
    {
        UartRx_Treat();
        HAL_UART_Receive_IT(&USART4_Handler, &UartReceiveBuff[UartReceiveBuffIndex], 1);//执行回调后会清空中断标志，取消中断使能 故需要重新打开中断
    }
    else if(huart->Instance == USART1)
    {
        LTEUartRx_Time_Tick = HAL_GetTick();//获取时间戳
        LTEUartReceiveBuff[LTEUartReceiveBuffIndex] = LTEUartReceiveData;
        if((LTEUartReceiveBuff[0] != 0)&&(LTEUartReceiveBuffIndex == 0))
        {
            LTEUartReceiveBuffIndex = 1;
        }
        else if(LTEUartReceiveBuffIndex > 0)
        {
            LTEUartReceiveBuffIndex++;
        }
        
        LTEUartRx_Data_Length = LTEUartReceiveBuffIndex;
        if(LTEUartReceiveBuffIndex >= 200) //缓存满
        {
            LTEUartReceiveBuffIndex = 0;
            LTEUartRx_OK_Flag = 1;//接收完成
        }
        HAL_UART_Receive_IT(&USART1_Handler, &LTEUartReceiveData, 1);//打开中断
    }
}


void UartRx_Treat(void)
{
    ubitint U16_temp;
    if((UartReceiveBuffIndex == 0) && (UartReceiveBuff[0] == 0xAA))
    {
        UartReceiveBuffIndex = 1;
    }
    else if(UartReceiveBuff[0] == 0xAA)
    {
        UartReceiveBuffIndex++;
        if(UartReceiveBuffIndex >= 40)
        {
            UartReceiveBuffIndex = 0;
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
        }
    }
    else 
    {
        UartReceiveBuffIndex = 0;
        //memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
    }

    if((UartReceiveBuffIndex >= 6)&&(UartReceiveBuff[1] == 0xFF))
    {
        if(UartReceiveBuff[5] == 0x55)
        {

            switch(UartReceiveBuff[2])
            {

            case 0x00 :    //专用上位机查询指令


                    SenddatatoSWJ();

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                case 0xFF :    //调试助手查询指令

                    //Uart_Send_Array(ddd,3);
                    printf(" 00 Recieved \n\r");

                    printf("GW_ID           : 0x%x ;\n\r", g_SysData.u8_GatewayID );    //1
                    printf("TM_ID           : 0x%x ;\n\r", g_SysData.u8_AlloID );
                    printf("BAT_Volt        : %d ;\n\r",   g_SysData.u8_BatVolt );
                    printf("Tower_Num       : 0x%x ;\n\r", g_SysData.u16_Tower_Num );
                    printf("SampleTime      : %d min ;\n\r",   g_SysData.u16_SampTime );
                    printf("TiltWarning     : %d (X100);\n\r",   g_SysData.u16_TiltWarningTreshold );
                    printf("LowVoltTreshold : %d ;\n\r",   g_SysData.u8_LowVoltTreshold );
                    printf("SleepTreshold   : %d ;\n\r",   g_SysData.u8_SleepTreshold );    //11

                    printf("SW_VERSION      : %d ;\n\r",  SoftWare_Version);    //15

                    printf("shunxian_Angle  : %4f° ;\n\r", sIncl_Data.f32_shunxian_Angle_arc_o );    //19
                    printf("hengxian_Angle  : %4f°;\n\r", sIncl_Data.f32_hengxian_Angle_arc_o );    //23
                    printf("zonghe_Angle    : %4f°;\n\r", sIncl_Data.f32_zonghe_Angle_arc_o );    //27

                    printf(" 查询结束，欢迎再来 \n\r");


                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;


                
                case 0x01 :    //写ID指令

                    //Uart_Send_Array(ddd,3);
                    printf(" 01 Recieved ");


                    g_SysData.u8_GatewayID = UartReceiveBuff[3] & 0xff;
                    g_SysData.u8_AlloID    = UartReceiveBuff[4] & 0xff;

                    
                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wRoutingGWID     = g_SysData.u8_GatewayID;
                        unEeprom.stData.wRoutingLocalID  = g_SysData.u8_AlloID;
                        bEepromWriteFlag = 1;
                    }
                    
                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                case 0x02 :    //调平指令

                    //Uart_Send_Array(ddd,3);
                    printf(" 02 Recieved ");

                    if(UartReceiveBuff[3] == 0x01)
                    {
                        g_statecheck.bit.bTiltCalbFlag = cFlagYes;
                    }

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                case 0x03 :    //倾斜阈值设定

                    //Uart_Send_Array(ddd,3);
                    printf(" 03 Recieved ");

                    U16_temp.bitdata.highchar = UartReceiveBuff[4] & 0xff;
                    U16_temp.bitdata.lowchar  = UartReceiveBuff[3] & 0xff;
                    g_SysData.u16_TiltWarningTreshold = U16_temp.id;
                    
                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wTiltTresholdSetValue = g_SysData.u16_TiltWarningTreshold;
                        bEepromWriteFlag = 1;
                    }


                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                case 0x04 :    //欠压阈值设定

                    //Uart_Send_Array(ddd,3);
                    printf(" 04 Recieved ");

                    g_SysData.u8_LowVoltTreshold = UartReceiveBuff[3] & 0xff;

                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wLowVoltTresholdSetValue = g_SysData.u8_LowVoltTreshold;     // 欠压阈值;
                        bEepromWriteFlag = 1;
                    }

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                case 0x05 :    //休眠阈值设定

                    //Uart_Send_Array(ddd,3);
                    printf(" 05 Recieved ");

                    g_SysData.u8_SleepTreshold = UartReceiveBuff[3] & 0xff;

                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wSleepTresholdSetValue = g_SysData.u8_SleepTreshold;     // 欠压阈值;
                        bEepromWriteFlag = 1;
                    }

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;


                case 0x06 :    //采样时间设定

                    //Uart_Send_Array(ddd,3);
                    printf(" 06 Recieved ");

                    U16_temp.bitdata.highchar = UartReceiveBuff[4] & 0xff;
                    U16_temp.bitdata.lowchar  = UartReceiveBuff[3] & 0xff;
                    g_SysData.u16_SampTime = U16_temp.id;

                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wSampTimeSetValue = g_SysData.u16_SampTime;     // 欠压阈值;
                        bEepromWriteFlag = 1;
                    }

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;


                
                case 0x07 :    //关机标志位设定

                    //Uart_Send_Array(ddd,3);
                    printf(" 07 Recieved ");

                    if((UartReceiveBuff[3] == 0X01)
                    ||(UartReceiveBuff[3] == 0X00))
                    {
                        printf(" SET SUCCESS ");
                        g_SysData.u16_SysOnOffFlag = UartReceiveBuff[3] & 0xff;
                        
                        // 向EEPROM中写入下发的设定值
                        if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                        {
                            unEeprom.stData.wSysOnOffSetBit = g_SysData.u16_SysOnOffFlag;     
                            bEepromWriteFlag = 1;
                        }

                    }
                    else
                    {
                        printf(" SET ERROR ");
                    }



                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                                
                case 0x08:    //设置杆塔组号序号

                    //Uart_Send_Array(ddd,3);
                    printf(" 08 Recieved ");
                    
                    U16_temp.bitdata.highchar = UartReceiveBuff[4] & 0xff;
                    U16_temp.bitdata.lowchar  = UartReceiveBuff[3] & 0xff;
                    g_SysData.u16_Tower_Num = U16_temp.id;
                    
                    // 向EEPROM中写入下发的设定值
                    if(bEepromWriteFlag != 1)    //写eeprom期间不得修改unEeprom内容
                    {
                        unEeprom.stData.wTowerNum = g_SysData.u16_Tower_Num;     // 欠压阈值;
                        bEepromWriteFlag = 1;
                    }

                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;

                break;

                
                default :
                    UartReceiveBuffIndex = 0;
                    UartReceiveBuff[0]   = 0;
                    UartReceiveBuff[1]   = 0;
                    UartReceiveBuff[2]   = 0;
                    UartReceiveBuff[3]   = 0;
                    UartReceiveBuff[4]   = 0;
                    UartReceiveBuff[5]   = 0;
                break;
            }


        }
        else if(UartReceiveBuffIndex > 6)
        {
            UartReceiveBuffIndex = 0;
            UartReceiveBuff[0]   = 0;
            UartReceiveBuff[1]   = 0;
            UartReceiveBuff[2]   = 0;
            UartReceiveBuff[3]   = 0;
            UartReceiveBuff[4]   = 0;
            UartReceiveBuff[5]   = 0;
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
        }
    }
    else if((UartReceiveBuffIndex >= 16)&&(UartReceiveBuff[1] == 0x01))    //参数配置1
    {
        if(UartReceiveBuff[15] == 0xFF)
        {
            RecidatatoSet();
        }
        else
        {
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
            UartReceiveBuffIndex= 0;
        }
    }
    else if((UartReceiveBuffIndex >= 11)&&(UartReceiveBuff[1] == 0x02))    //参数配置2
    {

        if(UartReceiveBuff[10] == 0xFF)
        {
            RecidatatoSet2();
        }
        else
        {
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
            UartReceiveBuffIndex = 0;
        }

    }
    else if((UartReceiveBuffIndex >= 6)&&(UartReceiveBuff[1] == 0x03))    //调平命令
    {
        if((UartReceiveBuff[3] == 0x55)&&(UartReceiveBuff[5] == 0xff))
        {
            g_statecheck.bit.bTiltCalbFlag = cFlagYes;    //
            
            UartReceiveBuffIndex = 0;
                memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
        }
        else
        {
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
            UartReceiveBuffIndex = 0;
        }

    }
    else if((UartReceiveBuffIndex >= 6)&&(UartReceiveBuff[1] == 0x04))    //lora发送测试
    {
        if((UartReceiveBuff[3] == 0x66)&&(UartReceiveBuff[5] == 0xff))
        {
        
            SendBuffTest[0] = 0xAA;                                      //帧头
            SendBuffTest[1] = cLoraOrder_Test;                         //控制字
            SendBuffTest[2] = cLoraTypeOrder;                            // type
            SendBuffTest[3] = (U8)(g_SysData.u8_SN[0]& 0xff);          //发送设备ID
            SendBuffTest[4] = (U8)(g_SysData.u8_SN[1]& 0xff);       //接收设备ID
            SendBuffTest[5] = (U8)(g_SysData.u8_SN[2]& 0xff);                                      //数据长
            SendBuffTest[6] = (U8)(g_SysData.u8_SN[3]& 0xff);
            SendBuffTest[7] = (U8)(g_SysData.u8_SN[4]& 0xff);
            SendBuffTest[8] = (U8)(g_SysData.u8_SN[5]& 0xff);
            SendBuffTest[9] = (U8)(g_SysData.u8_SN[6]& 0xff);               //帧尾
            
            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuffTest, 10);


            SX1278_SetSleepMode();

            
            UartReceiveBuffIndex = 0;
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
        
        }
        else
        {
            memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));
            UartReceiveBuffIndex = 0;
        }

    }
    else
    {
        //memset(UartReceiveBuff,0, sizeof(UartReceiveBuff));

    }
    
}




void MX_USART1_UART_Init(void)
{
    USART1_Handler.Instance = USART1;
    USART1_Handler.Init.BaudRate = 115200;
    USART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART1_Handler.Init.StopBits = UART_STOPBITS_1;
    USART1_Handler.Init.Parity = UART_PARITY_NONE;
    USART1_Handler.Init.Mode = UART_MODE_TX_RX;
    USART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART1_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    USART1_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    USART1_Handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    USART1_Handler.gState = HAL_UART_STATE_RESET;
    if (HAL_UART_Init(&USART1_Handler) != HAL_OK)
    {
        printf("串口1初始化失败");
    }
}



void MX_USART4_UART_Init(void)
{
    USART4_Handler.Instance = USART4;
    USART4_Handler.Init.BaudRate = 115200;
    USART4_Handler.Init.WordLength = UART_WORDLENGTH_8B;
    USART4_Handler.Init.StopBits = UART_STOPBITS_1;
    USART4_Handler.Init.Parity = UART_PARITY_NONE;
    USART4_Handler.Init.Mode = UART_MODE_TX_RX;
    USART4_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    USART4_Handler.Init.OverSampling = UART_OVERSAMPLING_16;
    USART4_Handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    USART4_Handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    USART4_Handler.gState = HAL_UART_STATE_RESET;
    if (HAL_UART_Init(&USART4_Handler) != HAL_OK)
    {
        printf("串口4初始化失败");
    }
}

void UART_Init(void)
{
    MX_USART1_UART_Init();
    HAL_UART_Receive_IT(&USART1_Handler, &LTEUartReceiveData,1);//打开中断
	
    MX_USART4_UART_Init();
    HAL_UART_Receive_IT(&USART4_Handler, &UartReceiveBuff[UartReceiveBuffIndex], 1);//打开中断
}

//串口初始化
void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(huart->Instance==USART1)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART1_CLK_ENABLE();
        /*PA9     ------> USART1_TX   PA10     ------> USART1_RX */
        GPIO_InitStruct.Pin       = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;//GPIO_MODE_INPUT;//
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART1 interrupt Init */
//        HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
//        HAL_NVIC_EnableIRQ(USART1_IRQn);
		
		/* USART1 DMA Init */
		/* USART1_TX Init */
		hdma_usart1_tx.Instance = DMA1_Channel2;
		hdma_usart1_tx.Init.Request = DMA_REQUEST_3;
		hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
		hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_tx.Init.Mode = DMA_NORMAL ;
		hdma_usart1_tx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
		{
		  Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmatx,hdma_usart1_tx);

		/* USART1_RX Init */
		hdma_usart1_rx.Instance = DMA1_Channel3;
		hdma_usart1_rx.Init.Request = DMA_REQUEST_3;
		hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_usart1_rx.Init.Mode = DMA_NORMAL;
		hdma_usart1_rx.Init.Priority = DMA_NORMAL;
		if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
		{
		  Error_Handler();
		}

		__HAL_LINKDMA(huart,hdmarx,hdma_usart1_rx);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	  /* USER CODE BEGIN USART1_MspInit 1 */

	  /* USER CODE END USART1_MspInit 1 */
    }
    else if(huart->Instance==USART4)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USART4_CLK_ENABLE();
        /*PA0     ------> USART4_TX   PA1     ------> USART4_RX */
        GPIO_InitStruct.Pin       = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = GPIO_AF6_USART4;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        /* USART4 interrupt Init */
        HAL_NVIC_SetPriority(USART4_5_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART4_5_IRQn);
    }

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
    if(huart->Instance==USART1)
    {
        __HAL_RCC_USART1_CLK_DISABLE();
        /*PA9     ------> USART1_TX   PA10     ------> USART1_RX */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
		if(huart->Instance == USART4)
	{
		__HAL_RCC_USART4_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1);
		HAL_NVIC_DisableIRQ(USART4_5_IRQn);
	}
	
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&USART1_Handler);
}


///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
    /* 发送一个字节数据到串口 */
    //USART_SendData(DEBUG_USARTx, (uint8_t) ch);
    uint8_t data = ch;
    HAL_UART_Transmit(&USART4_Handler, &data,1,500);
    /* 等待发送完毕 */
    //while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		

    return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
    /* 等待串口输入数据 */
    //while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

    //return (int)USART_ReceiveData(DEBUG_USARTx);
    return 0;
}

void Uart_Send_Array(uint8_t *pData, uint16_t Size)
{
    HAL_UART_Transmit(&USART4_Handler, pData,Size,20);
}



void User_Data_Copy(uint8_t *Source_Data, uint8_t *Target_Data,uint16_t Data_Length)
{
    uint16_t i;
    for(i = 0; i < Data_Length; i++)
    {
        *Source_Data++ = *Target_Data++;
    }

}



uint32_t TimeTick = 0;

int Dev_Recv(uint8_t *buf, uint16_t timeout)
{
     int len = 0;
    //int TimeTick = 0;
    int iTimes = 0;
    memset(buf,0,200);//清除数组内容
    while(1)
    {
        if(LTEUartReceiveBuffIndex > 0)//有数据
        {
            iTimes = 0;
            TimeTick = HAL_GetTick();//获取时间戳 
            if(((TimeTick >=  LTEUartRx_Time_Tick)&&((TimeTick - LTEUartRx_Time_Tick) >= 5)) 
            || ((TimeTick <  LTEUartRx_Time_Tick)&&(( LTEUartRx_Time_Tick - TimeTick) >= 5)))
            { 
                 LTEUartRx_OK_Flag = 1;
            }
        }
        if(LTEUartRx_OK_Flag == 1)
        {   	
            User_Data_Copy((uint8_t *)buf,LTEUartReceiveBuff , LTEUartRx_Data_Length);
            memset(LTEUartReceiveBuff,0,LTEUartRx_Data_Length);//清除数组内容
            len = LTEUartRx_Data_Length;
            LTEUartRx_OK_Flag = 0;
            LTEUartReceiveBuffIndex = 0;
            UART_Init();
            break;
        }
        else if(timeout == iTimes)
        {
            len = 0;
            break;
        }
            iTimes++;
            HAL_Delay(1); 
    }
    return len;
}




int Dev_Send(uint8_t *buf, uint16_t len)
{
    HAL_UART_Transmit(&USART1_Handler, buf,len,100);
    return  len;
}






