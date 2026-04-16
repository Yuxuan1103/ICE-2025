/*
*********************************************************************
**
**    Project:  ¸ËËþÇãÐ±2.0°æ±¾
**
**    Componet: STM32L071CBT6
**
**    Filename: main.c
**
**    Abstract: Ö÷º¯Êý
**
**    Compiler: KEIL 5.0
**
**    History£º
**
**        <auther>    <date>    <version>   <desc>
**
**            YXD        20/04/26        1.0
**
**    Copyright @cdstarriver All Rights Reserved.
**
*********************************************************************
*/

#include "main.h"

uint8_t ReceiveBuff[20];

uint8_t u8_ReceiveBuffer[cRecLengthMax];

void MessageSendFunction(uint8_t command);
void LoraMessageSend_Treat(uint8_t u8Lora_Command);
void MessageReceiveFunction(void);
void LoraComm_MainTsk(void);
void RemoteOrder_Reset_Treat(void);
void RemoteOrder_Config1_Treat(void);
void RemoteOrder_Config2_Treat(void);
void RemoteOrder_Sleep_Treat(void);
void RemoteOrder_SysOnOff_Treat(void);
void RemoteOrder_TiltCalb_Treat(void);
void RemoteOrder_IAP_Treat(void);
void RemoteOrder_PreIAP_Treat(void);
void RemoteOrder_TimeCalb_Treat(void);


void PowerupTime(void);



T_Lora_RXDATA stLoraRxData;


/*****************************************************************************************
Function Name       :  MessageSendFunction
Description         :  Í¨ÐÅÊý¾Ý·¢ËÍ¿ØÖÆ 
Input               :  ÎÞ
Output              :  ÎÞ
                       
Author              :  
Creation Date       :  2020/4/26
Revisor             :  1.0
Revision Date       :  2020/4/26
Note                :  None
******************************************************************************************/ 

void MessageSendFunction(uint8_t command)
{

    uint8_t SendBuff[cSendLengthMax];

    memset(&SendBuff[0],0,cSendLengthMax);

    ubitfloat f32temp;
    
    //HAL_Delay(g_SysData.u8_AlloID * 1200);


    switch (command)
    {
			
//			  case Wakeup_On :

//            SendBuff[0] = 0xAA;                                      //??í·
//            SendBuff[1] = Wakeup_On;                                //????×?
//            SendBuff[2] = cLoraTypeOrder;                            // type
//            SendBuff[3] = GatID; //(U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
//            SendBuff[4] = WayID;//(U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
//            SendBuff[5] = 0x00;                                      //êy?Y3¤
//            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
//            SendBuff[6] = CRC16H;
//            SendBuff[7] = CRC16L;
//            SendBuff[8] = 0x55;                                      //???2
//            
//            SX1278_SetSleepMode();
//            SX1278_ConfigTxMode();
//            SX1278_SendPacket(SendBuff, 9);

//        break;
							
        case cLoraOrder_BOOTUP :

            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_BOOTUP;                         //????×?
            SendBuff[2] = cLoraTypeOrder;                            // type
            SendBuff[3] =GatID;// 0X01;  //(U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
            SendBuff[4] = WayID;//(U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
            SendBuff[5] = 0x00;                                      //êy?Y3¤
            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[6] = CRC16H;
            SendBuff[7] = CRC16L;
            SendBuff[8] = 0x55;                                      //???2
            
            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 9);

        break;

        case cLoraOrder_TiltData :
        
            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_TiltData;                       //????×?
            SendBuff[2] = cLoraTypeOrder;
            SendBuff[3] = GatID; //(U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
            SendBuff[4] = WayID;//(U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
            SendBuff[5] = 0x11;                                      //êy?Y3¤?è

            //êy?Yóò
            f32temp.fd = g_SysData.f32_ShunxianAngle;
            SendBuff[6] = (U8)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             //Ë³Ïß
            SendBuff[7] = (U8)(f32temp.intdata[1].bitdata.highchar & 0xff);
            SendBuff[8] = (U8)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
            SendBuff[9] = (U8)(f32temp.intdata[0].bitdata.highchar & 0xff);
			    //	g_SysData.f32_ShunxianAngle = 0;
            
            f32temp.fd = g_SysData.f32_HenxianAngle;
            SendBuff[10] = (U8)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             // ºáÏß
            SendBuff[11] = (U8)(f32temp.intdata[1].bitdata.highchar & 0xff);
            SendBuff[12] = (U8)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
            SendBuff[13] = (U8)(f32temp.intdata[0].bitdata.highchar & 0xff);
					//g_SysData.f32_HenxianAngle = 0;

            f32temp.fd = g_SysData.f32_ZongheAngle;
            SendBuff[14] = (U8)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             // ×ÛºÏ
            SendBuff[16] = (U8)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
            SendBuff[17] = (U8)(f32temp.intdata[0].bitdata.highchar & 0xff);
						//g_SysData.f32_HenxianAngle = 0;

            f32temp.fd = g_SysData.f32_AdxlTemperature;
            SendBuff[18] = (U8)(f32temp.intdata[1].bitdata.lowchar  & 0xff);             // ÎÂ¶È
            SendBuff[19] = (U8)(f32temp.intdata[1].bitdata.highchar & 0xff);
            SendBuff[20] = (U8)(f32temp.intdata[0].bitdata.lowchar  & 0xff);
            SendBuff[21] = (U8)(f32temp.intdata[0].bitdata.highchar & 0xff);
					//g_SysData.f32_AdxlTemperature = 0;

            SendBuff[22] = (U8)(g_SysData.u8_BatVolt & 0xff);             // µ?3?µ??1êy?Y
						
						if ( Aflag  == 1)
						{
						
					  	SendBuff[23] = 0X01;
							
						}
						else
						SendBuff[23] = 0;
						
            
            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[24] = CRC16H;
            SendBuff[25] = CRC16L;
            SendBuff[26] = 0x55;                                          //???2
            
    //      SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 27);
//						printf("Ë³Ïß= %f \n\r", g_SysData.f32_ShunxianAngle );
//						printf("ºáÏß= %f \n\r", g_SysData.f32_HenxianAngle );
//						printf("×ÛºÏ= %f \n\r", g_SysData.f32_ZongheAngle);
//						printf("ÎÂ¶È= %f \n\r", g_SysData.f32_AdxlTemperature);
						Uart_Send_Array(SendBuff, 27);

        break;

        case cLoraOrder_ErrData :

            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_ErrData;                        //????×?
            SendBuff[2] = cLoraTypeOrder;
            SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
            SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
            SendBuff[5] = 0x01;                                      //êy?Y3¤?è
            //êy?Yóò
            SendBuff[6] = (U8)((g_statecheck.byte.SensorFault >> 8) & 0xff);             // 1ê??D???
            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[7] = CRC16H;
            SendBuff[8] = CRC16L;
            SendBuff[9] = 0x55;                                      //???2
            
            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 10);


            
        break;

        case cLoraOrder_TimeCalb :

            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_TimeCalb;                       //????×?
            SendBuff[2] = cLoraTypeOrder;
            SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
            SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
            SendBuff[5] = 0x00;                                      //êy?Y3¤?è
            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[6] = CRC16H;
            SendBuff[7] = CRC16L;
            SendBuff[8] = 0x55;                                      //???2
            
            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 9);

        case cLoraOrder_HeartBeat :
            RTC_GetTime(&RTC_Data, &RTC_Time);// ê±????è?
            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_HeartBeat;                      //????×?
            SendBuff[2] = cLoraTypeOrder;
            SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);          //·??íéè±?ID
            SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       //?óê?éè±?ID
            SendBuff[5] = 0x07;                                      //êy?Y3¤?è
            //êy?Yóò
            SendBuff[6]  = (U8)((RTC_Data.Year) & 0xff);                 // ?ê
            SendBuff[7]  = (U8)((RTC_Data.Month) & 0xff);                // ??
            SendBuff[8]  = (U8)((RTC_Data.Date) & 0xff);                 // è?
            SendBuff[9]  = (U8)((RTC_Time.Hours) & 0xff);                // ê±
            SendBuff[10] = (U8)((RTC_Time.Minutes) & 0xff);              // ·?
            SendBuff[11] = (U8)((RTC_Time.Seconds) & 0xff);              // ??
            SendBuff[12] = (U8)((g_SysData.u8_BatVolt >> 8) & 0xff);     // µ?3?µ??1êy?Y
            
            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[13] = CRC16H;
            SendBuff[14] = CRC16L;
            SendBuff[15] = 0x55;                                      //???2
            
            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 16);

        case cLoraOrder_SleepInform :
            RTC_GetTime(&RTC_Data, &RTC_Time);    // ê±????è?
            SendBuff[0] = 0xAA;                                      //??í·
            SendBuff[1] = cLoraOrder_SleepInform;                      //????×?
            SendBuff[2] = cLoraTypeOrder;
            SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);       //·??íéè±?ID
            SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);    //?óê?éè±?ID
            SendBuff[5] = 0x01;                                      //êy?Y3¤?è
            //êy?Yóò
            SendBuff[6] = 0x01;                 // 

            Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
            SendBuff[7] = CRC16H;
            SendBuff[8] = CRC16L;
            SendBuff[9] = 0x55;                                      //???2

            SX1278_SetSleepMode();
            SX1278_ConfigTxMode();
            SX1278_SendPacket(SendBuff, 10);

        default :
        break;
    }

}

/*****************************************************************************************
Function Name       :  MessageRecieveFunction
Description         :  ½ÓÊÕÖÐ¼ÌÍø¹ØÏÂ·¢µÄÖ¸ÁîÍ¨ÐÅ´¦Àíº¯Êý 
Input               :  ÎÞ
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void MessageReceiveFunction(void)
{

    uint8_t SendBuff[cSendLengthMax];

    memset(&SendBuff[0],0,cSendLengthMax);
    SX1278_SetSleepMode();
    SX1278_ConfigRxMode();
    if(SX1278_ReceivePacket(stLoraRxData.u8RxBuff,&stLoraRxData.u8Alllength,LoraCommCallBackWaitTime) == 1)
    {
        stLoraRxData.u8Crclength = stLoraRxData.u8Alllength - 4;    //³ýÈ¥0xaa,0x55,CRC16
        Cal_CRC16(&stLoraRxData.u8RxBuff[1],stLoraRxData.u8Crclength, &CRC16H, &CRC16L);
        //Ð£ÑéCRC
        if((stLoraRxData.u8RxBuff[stLoraRxData.u8Alllength - 3] == CRC16H)
        && (stLoraRxData.u8RxBuff[stLoraRxData.u8Alllength - 2] == CRC16L
        && (stLoraRxData.u8RxBuff[2] == cLoraTypeOrder))
        && (stLoraRxData.u8RxBuff[3] == g_SysData.u8_GatewayID))
        {
            g_SysData.u8_RemoteOrder = stLoraRxData.u8RxBuff[1];

            HAL_Delay(g_SysData.u8_AlloID * 1200);

            //¸ù¾ÝÐ­ÒéÄÚÈÝ£¬ÒÔÏÂ²Ù×÷ÎªÊÕµ½ÉÏ¼¶·¢ËÍµÄÔ¶³ÌÖ¸ÁîÓÅÏÈ»Ø¸´Ö¡,²¢»ñÈ¡ÏÂ·¢µÄÖ¸Áî
            switch(g_SysData.u8_RemoteOrder)
            {
                case cLoraOrder_Reset :

                    SendBuff[0] = 0xAA;                                      //Ö¡Í·
                    SendBuff[1] = cLoraOrder_Reset;                          //¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);       //·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);    //½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;                                      //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                      //Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_Reset_Treat();
                break;


                case cLoraOrder_TimeCalb :

                    SendBuff[0] = 0xAA;                                      //Ö¡Í·
                    SendBuff[1] = cLoraOrder_TimeCalb;                          //¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);       //·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);    //½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;                                      //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                      //Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_TimeCalb_Treat();
                break;

                case cLoraOrder_Inquiry_1 :

                    SendBuff[0] = 0xAA;                                                   // Ö¡Í·
                    SendBuff[1] = cLoraOrder_Inquiry_1;                                   // ¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);                       // ·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);                    // ½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x0C;                                                      // Êý¾Ý³¤¶È
                    SendBuff[6] = (U8)((g_SysData.u16_SampTime >> 8) & 0xff);             // ²ÉÑùÊ±¼ä
                    SendBuff[7] = (U8)((g_SysData.u16_SampTime >> 0) & 0xff);
                    SendBuff[8] = (U8)((g_SysData.u16_HeartBeatInterval >> 8) & 0xff);    // ÐÄÌø¼ä¸ô
                    SendBuff[9] = (U8)((g_SysData.u16_HeartBeatInterval >> 0) & 0xff);
                    SendBuff[10] = (U8)(g_SysData.u8_LowVoltTreshold & 0xff);             // Ç·Ñ¹ãÐÖµ
                    SendBuff[11] = (U8)(g_SysData.u8_SleepTreshold & 0xff);               // ÐÝÃßãÐÖµ

                    SendBuff[12] = (U8)(RTC_Data.Year    & 0xff);                         // 6 bytes Ê±¼äÐÅÏ¢
                    SendBuff[13] = (U8)(RTC_Data.Month   & 0xff);
                    SendBuff[14] = (U8)(RTC_Data.Date    & 0xff);
                    SendBuff[15] = (U8)(RTC_Time.Hours   & 0xff);
                    SendBuff[16] = (U8)(RTC_Time.Minutes & 0xff);
                    SendBuff[17] = (U8)(RTC_Time.Seconds & 0xff);

                    Cal_CRC16(&SendBuff[1],(SendBuff[4] + 4),&CRC16H,&CRC16L);
                    SendBuff[18] = CRC16H;
                    SendBuff[19] = CRC16L;
                    SendBuff[20] = 0x55;

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 21);

                break;

                case cLoraOrder_Config_1 :

                    SendBuff[0] = 0xAA;                                      // Ö¡Í·
                    SendBuff[1] = cLoraOrder_Config_1;                       // ¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID    & 0xff);       // ·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       // ½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;                                      // Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                      // Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_Config1_Treat();
                break;

                case cLoraOrder_Sleep :

                    SendBuff[0] = 0xAA;                                      // Ö¡Í·
                    SendBuff[1] = cLoraOrder_Sleep;                          // ¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID    & 0xff);       // ·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       // ½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;    //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;    //Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_Sleep_Treat();

                break;

                case cLoraOrder_Inquiry_2 :

                    SendBuff[0] = 0xAA;                                      // Ö¡Í·
                    SendBuff[1] = cLoraOrder_Inquiry_2;                      // ¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID    & 0xff);       // ·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       // ½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x02;    //Êý¾Ý³¤¶È

                    SendBuff[6] = (U8)(g_u8HwVerison & 0xff);
                    SendBuff[7] = (U8)(g_u8SwVerison & 0xff);

                    Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
                    SendBuff[8] = CRC16H;
                    SendBuff[9] = CRC16L;
                    SendBuff[10] = 0x55;                                      // Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff,11);

                break;

                case cLoraOrder_Config_2 :

                    SendBuff[0] = 0xAA;                                      // Ö¡Í·
                    SendBuff[1] = cLoraOrder_Config_2;                       // ¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID    & 0xff);       // ·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       // ½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;    //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],(SendBuff[5] + 5),&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                      // Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);

                    RemoteOrder_Config2_Treat();

                break;

                case cLoraOrder_BOOTUP :

                    SendBuff[0] = 0xAA;                                      //Ö¡Í·
                    SendBuff[1] = cLoraOrder_BOOTUP;                         //¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID    & 0xff);       //·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);       //½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;                                      //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                      //Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_SysOnOff_Treat();
                break;

                case cLoraOrder_TiltCalb :

                    SendBuff[0] = 0xAA;                                       //Ö¡Í·
                    SendBuff[1] = cLoraOrder_TiltCalb;                        //¿ØÖÆ×Ö
                    SendBuff[2] = cLoraTypeAnswer;
                    SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);           //·¢ËÍÉè±¸ID
                    SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);        //½ÓÊÕÉè±¸ID
                    SendBuff[5] = 0x00;                                       //Êý¾Ý³¤¶È
                    Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                    SendBuff[6] = CRC16H;
                    SendBuff[7] = CRC16L;
                    SendBuff[8] = 0x55;                                       //Ö¡Î²

                    SX1278_SetSleepMode();
                    SX1278_ConfigTxMode();
                    SX1278_SendPacket(SendBuff, 9);
                    RemoteOrder_TiltCalb_Treat();

                break;

                case cLoraOrder_IAP :

                    if(stLoraRxData.u8RxBuff[4] == g_SysData.u8_AlloID)
                    {
                        SendBuff[0] = 0xAA;                                       //Ö¡Í·
                        SendBuff[1] = cLoraOrder_IAP;                             //¿ØÖÆ×Ö
                        SendBuff[2] = cLoraTypeAnswer;
                        SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);           //·¢ËÍÉè±¸ID
                        SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);        //½ÓÊÕÉè±¸ID
                        SendBuff[5] = 0x00;                                       //Êý¾Ý³¤¶È
                        Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                        SendBuff[6] = CRC16H;
                        SendBuff[7] = CRC16L;
                        SendBuff[8] = 0x55;                                       //Ö¡Î²

                        SX1278_SetSleepMode();
                        SX1278_ConfigTxMode();
                        SX1278_SendPacket(SendBuff, 9);
                        RemoteOrder_IAP_Treat();
                    }

                break;

                case cLoraOrder_OpenRece:
                        SendBuff[0] = 0xAA;                                       //Ö¡Í·
                        SendBuff[1] = cLoraOrder_OpenRece;                             //¿ØÖÆ×Ö
                        SendBuff[2] = cLoraTypeAnswer;
                        SendBuff[3] = (U8)(g_SysData.u8_AlloID & 0xff);           //·¢ËÍÉè±¸ID
                        SendBuff[4] = (U8)(g_SysData.u8_GatewayID & 0xff);        //½ÓÊÕÉè±¸ID
                        SendBuff[5] = 0x00;                                       //Êý¾Ý³¤¶È
                        Cal_CRC16(&SendBuff[1],SendBuff[5] + 5,&CRC16H,&CRC16L);
                        SendBuff[6] = CRC16H;
                        SendBuff[7] = CRC16L;
                        SendBuff[8] = 0x55;                                       //Ö¡Î²

                        SX1278_SetSleepMode();
                        SX1278_ConfigTxMode();
                        SX1278_SendPacket(SendBuff, 9);
                        RemoteOrder_PreIAP_Treat();

                break;

                default :
                break;
            }
        }
    }
    else
    {
        memset(&stLoraRxData.u8RxBuff[0],0,cRecLengthMax);
        stLoraRxData.u8Alllength = 0;
        stLoraRxData.u8Crclength = 0;
    }
}




/*****************************************************************************************
Function Name       :  LoraComm_MainTsk
Description         :  ÏµÍ³loraÍ¨ÐÅÖ÷ÈÎÎñ 
Input               :  VOID
Output              :  VOID
                       
Author              :  YXD
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 

void LoraComm_MainTsk(void)
{
    static U16 u16SendCnt = 0;
    static U16 u16TiltSendCnt = 0;

    //Æ½ÐÐº¯Êý£¬ÏìÓ¦ÖÐ¼ÌÍø¹ØÖ¸Áî
    if(g_statecheck.bit.bLoraReceiveState == cFlagYes)
    {
        MessageReceiveFunction();    // ½ÓÊÕµ½ÖÐ¼ÌÍø¹ØÏÂ·¢µÄÈÎÎñ
    }
    else
    {
        SX1278_SetSleepMode();    // LORAÄ£¿éÐÝÃß

        if ((unEeprom.stData.wSysOnOffSetBit          != g_SysData.u16_SysOnOffFlag)
          ||(unEeprom.stData.wSampTimeSetValue        != g_SysData.u16_SampTime)
          ||(unEeprom.stData.wLowVoltTresholdSetValue != g_SysData.u8_LowVoltTreshold)
          ||(unEeprom.stData.wSleepTresholdSetValue   != g_SysData.u8_SleepTreshold)
          ||(unEeprom.stData.wTiltTresholdSetValue    != g_SysData.u16_TiltWarningTreshold))
          {
            // ÏòEEPROMÖÐÐ´ÈëÏÂ·¢µÄÉè¶¨Öµ
            if(bEepromWriteFlag != 1)    //Ð´eepromÆÚ¼ä²»µÃÐÞ¸ÄunEepromÄÚÈÝ
            {
                unEeprom.stData.wSysOnOffSetBit          = g_SysData.u16_SysOnOffFlag;
                unEeprom.stData.wSampTimeSetValue        = g_SysData.u16_SampTime;              // ²ÉÑù¼ä¸ô
                unEeprom.stData.wLowVoltTresholdSetValue = g_SysData.u8_LowVoltTreshold;        // Ç·Ñ¹ãÐÖµ
                unEeprom.stData.wSleepTresholdSetValue   = g_SysData.u8_SleepTreshold  ;        // ÐÝÃßãÐÖµ
                unEeprom.stData.wTiltTresholdSetValue    = g_SysData.u16_TiltWarningTreshold;   // ÇãÐ±±¨¾¯ãÐÖµ
                bEepromWriteFlag = 1;
                
            }

          }
        



    }


    if(g_statecheck.bit.bLoraTimingSendFlag == cFlagYes)    //¶¨Ê±ÉÏ´«
    {
        u16SendCnt ++;
        if(u16SendCnt >= g_SysData.u8_AlloID * 20)
        {
            u16SendCnt = 0;

            if(g_statecheck.bit.bSysTiltState == cFlagYes)
            {
                u16TiltSendCnt++;
                if(u16TiltSendCnt >= 24)    //2Hours
                {
                    g_SysData.u16_TiltSend_K += 1;
                    u16TiltSendCnt = 0;
                }
            }

            do
            {
                LoraMessageSend_Treat(cLoraOrder_TiltData);
            }
            while(g_statecheck.bit.bLoraCallBackFlag != cFlagYes);

            if(g_statecheck.byte.SensorFault != 0)
            {
                do
                {
                    LoraMessageSend_Treat(cLoraOrder_ErrData);
                }
                while(g_statecheck.bit.bLoraCallBackFlag != cFlagYes);
            }
        }



    }


}

/*****************************************************************************************
Function Name       :  LoraMessageSend_Treat
Description         :  LORAÉÏ´«Êý¾Ý½Ó¿Úº¯Êý
Input               :  u8Lora_Command
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
float   f32thresholdvalue_Sx1 = 0.5f ; // ±¨¾¯³õÊ¼ãÐÖµ Ë³Ïß  ÕýÏò
float   f32thresholdvalue_Sx2 = 0.5f ; // ±¨¾¯³õÊ¼ãÐÖµ Ë³Ïß  ·´Ïò
float   f32thresholdvalue_Hx1 = 0.5f ; // ±¨¾¯³õÊ¼ãÐÖµ ºáÏß  ÕýÏò
float   f32thresholdvalue_Hx2 = 0.5f ; // ±¨¾¯³õÊ¼ãÐÖµ ºáÏß  ·´Ïò
void LoraMessageSend_Treat(uint8_t u8Lora_Command)
{
 //   static U8 u8_ReciveFailCnt = 0;
    MessageSendFunction(u8Lora_Command);    //  LORA·¢ËÍ
 //   printf(" Lora Send ");
    SX1278_SetSleepMode();
//    HAL_Delay(33);    //ÐèÒªÒ»¶Î
    SX1278_ConfigRxMode();
	  HAL_Delay(50); 
    if(SX1278_ReceivePacket(stLoraRxData.u8RxBuff,&stLoraRxData.u8Alllength,LoraCommCallBackWaitTime) == 1)   //Á¬Ðø½ÓÊÜÄ£Ê½»áÒ»Ö±µÈLoraCommunicationCallBackWaitTime
    {

        //SX1278_LoraInit();
        Cal_CRC16(&stLoraRxData.u8RxBuff[1], stLoraRxData.u8RxBuff[5] + 5, &CRC16H, &CRC16L);

        //ÑéÖ¤»Ø¸´Ö¡
        if((stLoraRxData.u8RxBuff[0]  == 0xAA)
        && (stLoraRxData.u8RxBuff[1]  == u8Lora_Command)
        && (stLoraRxData.u8RxBuff[3]  == WayID)//g_SysData.u8_GatewayID
				&& (stLoraRxData.u8RxBuff[4]  == GatID)
        && (stLoraRxData.u8RxBuff[5]  == 0x07)
        && (stLoraRxData.u8RxBuff[13] == CRC16H)
        && (stLoraRxData.u8RxBuff[14] == CRC16L)
        && (stLoraRxData.u8RxBuff[15] == 0x55))
        {
            //µ±Ç°Ê±¼ä
            RTC_Data.Year    = stLoraRxData.u8RxBuff[6]  & 0xff;
            RTC_Data.Month   = stLoraRxData.u8RxBuff[7]  & 0xff;
            RTC_Data.Date    = stLoraRxData.u8RxBuff[8]  & 0xff;
            RTC_Time.Hours   = stLoraRxData.u8RxBuff[9]  & 0xff;
            RTC_Time.Minutes = stLoraRxData.u8RxBuff[10]  & 0xff;
            RTC_Time.Seconds = stLoraRxData.u8RxBuff[11] & 0xff;
            RTC_SetTime(&RTC_Data, &RTC_Time);//ÉèÖÃÊ±¼ä
					
					  if (stLoraRxData.u8RxBuff[12] == 1)
						{
							g_statecheck.bit.bTiltCalbFlag = cFlagYes;
							printf("FLAG= %d\n\r", g_statecheck.bit.bTiltCalbFlag );
						}
						else g_statecheck.bit.bTiltCalbFlag = cFlagNo;
						
						if  ( (g_statecheck.bit.bSysTiltState == cFlagYes) && ( (g_SysData.f32_ShunxianAngle > 0) && (g_SysData.f32_ShunxianAngle <= (f32thresholdvalue_Sx1 + 0.5))) )
						{
						   u8_Fault_Cnt_Sx1 ++ ;
						if (u8_Fault_Cnt_Sx1 >= 12)    //ÇãÐ±ÔÚ0.5-1.0 ÉÏ±¨12´ÎºóÔö¼Ó±¨¾¯ãÐÖµ
						{
						  u8_Fault_Cnt_Sx1 = 0;
							f32thresholdvalue_Sx1 =  f32thresholdvalue_Sx1 + 0.5;
							f32thresholdvalue_Sx2 = 0.5;
							if (f32thresholdvalue_Sx1 >=4.0)
							{
						  	f32thresholdvalue_Sx1 = 4.0;     //ãÐÖµ×î´óÖµ4.0
							}
						}

						}
						else u8_Fault_Cnt_Sx1 = 0;
						
					if  ( (g_statecheck.bit.bSysTiltState == cFlagYes) && ( (g_SysData.f32_ShunxianAngle < 0) && (g_SysData.f32_ShunxianAngle >= (-f32thresholdvalue_Sx2 - 0.5))) )
						{
						   u8_Fault_Cnt_Sx2 ++ ;
						if (u8_Fault_Cnt_Sx2 >= 12)    //ÇãÐ±ÔÚ0.5-1.0 ÉÏ±¨12´ÎºóÔö¼Ó±¨¾¯ãÐÖµ
						{
						  u8_Fault_Cnt_Sx2 = 0;
							f32thresholdvalue_Sx2 =  f32thresholdvalue_Sx2 + 0.5;
							f32thresholdvalue_Sx1 = 0.5;
							if (f32thresholdvalue_Sx2 >=4.0)
							{
						  	f32thresholdvalue_Sx1 = 4.0;
							}
						}

						}
						else u8_Fault_Cnt_Sx2 = 0;
						
					if  ( (g_statecheck.bit.bSysTiltState == cFlagYes) && ((g_SysData.f32_HenxianAngle > 0) && (g_SysData.f32_HenxianAngle <= (f32thresholdvalue_Hx1 + 0.5) )) )
						{
						   u8_Fault_Cnt_Hx1 ++ ;
						if (u8_Fault_Cnt_Hx1 >= 12)    
						{
						  u8_Fault_Cnt_Hx1 = 0;
							f32thresholdvalue_Hx1 =  f32thresholdvalue_Hx1 + 0.5;
							f32thresholdvalue_Hx2 = 0.5;
							if (f32thresholdvalue_Hx1 >=4.0)
							{
						  	f32thresholdvalue_Hx1 = 4.0;
							}
						}

						}
						else u8_Fault_Cnt_Hx1 = 0;
						
					if  ( (g_statecheck.bit.bSysTiltState == cFlagYes) && ((g_SysData.f32_HenxianAngle < 0) && (g_SysData.f32_HenxianAngle >= (-f32thresholdvalue_Hx2 - 0.5) )) )
						{
						   u8_Fault_Cnt_Hx2 ++ ;
						if (u8_Fault_Cnt_Hx2 >= 12)    //
						{
						  u8_Fault_Cnt_Hx2 = 0;
							f32thresholdvalue_Hx2 =  f32thresholdvalue_Hx2 + 0.5;
							f32thresholdvalue_Hx1 =  0.5;
							if (f32thresholdvalue_Hx2 >=4.0)
							{
						  	f32thresholdvalue_Hx2 = 4.0;
							}
						}

						}
						else u8_Fault_Cnt_Hx2 = 0;


            g_statecheck.bit.bLoraCallBackFlag = cFlagYes;
            u8_ReciveFailCnt = 0;
            g_statecheck.bit.bLora_SendingState = cLoraCommIdle;
            g_statecheck.bit.bLoraTimingSendFlag = cFlagNo;
        }
        else    //ÊÕµ½µÄÄÚÈÝÓë·¢ËÍ²»·û
        {
            g_statecheck.bit.bLoraCallBackFlag = cFlagNo;
            u8_ReciveFailCnt ++;
            if(u8_ReciveFailCnt >= 3)
            {
                u8_ReciveFailCnt = 0;
                g_statecheck.bit.bLORACommFault = cFlagYes;    //Á¬Ðø·¢ËÍ2´ÎÎ´ÊÕ»ØËÍLORAÊý¾ÝÖ¡£¬ÔòÊÓÎªLORAÍ¨Ñ¶¹ÊÕÏ
                g_statecheck.bit.bLora_SendingState = cLoraCommIdle;
                g_statecheck.bit.bLoraCallBackFlag = cFlagYes;    //ÆäÊµÎÞ»ØÖ¡£¬ÓÃÓÚÅÐ¶¨²»·¢
                g_statecheck.bit.bLoraTimingSendFlag = cFlagNo;
            }
        }
    }
    else
    {
        //Î´ÊÕµ½
        g_statecheck.bit.bLoraCallBackFlag = cFlagNo;
        u8_ReciveFailCnt ++;
        if(u8_ReciveFailCnt >= 3)
        {
            u8_ReciveFailCnt = 0;
            g_statecheck.bit.bLORACommFault = cFlagYes;    //Á¬Ðø·¢ËÍ2´ÎÎ´ÊÕ»ØËÍLORAÊý¾ÝÖ¡£¬ÔòÊÓÎªLORAÍ¨Ñ¶¹ÊÕÏ
            g_statecheck.bit.bLora_SendingState = cLoraCommIdle;
            g_statecheck.bit.bLoraCallBackFlag = cFlagYes;    //ÆäÊµÎÞ»ØÖ¡£¬ÓÃÓÚÅÐ¶¨²»·¢
            g_statecheck.bit.bLoraTimingSendFlag = cFlagNo;
        }
    }
}

/*****************************************************************************************
Function Name       :  PowerupTime
Description         :  ÐÝÃßÄ£Ê½ÏÂ£¬ÏòÖÐ¼ÌÍø¹Ø·¢ËÍÇëÇó¿ª»ú
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void PowerupTime(void)
{
    MessageSendFunction(cLoraOrder_BOOTUP);    //²éÑ¯¿ª»úÖ¸Áî
    SX1278_SetSleepMode();
    //HAL_Delay(33);    //ÐèÒªÒ»¶Î
    SX1278_ConfigRxMode();
    if(SX1278_ReceivePacket(stLoraRxData.u8RxBuff,&stLoraRxData.u8Alllength,LoraCommCallBackWaitTime) == 1)   //Á¬Ðø½ÓÊÜÄ£Ê½»áÒ»Ö±µÈLoraCommunicationCallBackWaitTime
    {
        //SX1278_LoraInit();
        Cal_CRC16(&stLoraRxData.u8RxBuff[1], stLoraRxData.u8RxBuff[5] + 5, &CRC16H, &CRC16L);
        
        //ÑéÖ¤»Ø¸´Ö¡
        if((stLoraRxData.u8RxBuff[0]  == 0xAA)
        && (stLoraRxData.u8RxBuff[1]  == cLoraOrder_BOOTUP)
        && (stLoraRxData.u8RxBuff[2]  == cLoraTypeOrder)
        && (stLoraRxData.u8RxBuff[3]  == g_SysData.u8_GatewayID)
        && (stLoraRxData.u8RxBuff[5]  == 0x07)
        && (stLoraRxData.u8RxBuff[13] == CRC16H)
        && (stLoraRxData.u8RxBuff[14] == CRC16L)
        && (stLoraRxData.u8RxBuff[15]  == 0x55))
        {
            g_SysData.u16_SysOnOffFlag = stLoraRxData.u8RxBuff[6] & 0xff;
            //µ±Ç°Ê±¼ä
            RTC_Data.Year    = stLoraRxData.u8RxBuff[7]  & 0xff;
            RTC_Data.Month   = stLoraRxData.u8RxBuff[8]  & 0xff;
            RTC_Data.Date    = stLoraRxData.u8RxBuff[9]  & 0xff;
            RTC_Time.Hours   = stLoraRxData.u8RxBuff[10]  & 0xff;
            RTC_Time.Minutes = stLoraRxData.u8RxBuff[11] & 0xff;
            RTC_Time.Seconds = stLoraRxData.u8RxBuff[12] & 0xff;
            RTC_SetTime(&RTC_Data, &RTC_Time);//ÉèÖÃÊ±¼ä
            
            printf(" LORA ASK POWER ON ");
            
            if(unEeprom.stData.wSysOnOffSetBit != g_SysData.u16_SysOnOffFlag)
            {
                if(g_SysData.u16_SysOnOffFlag == cSysOff)
                {
                    printf(" Remote Remain OFF ");
                }
                else
                {
                    printf(" Remote ON  succcess ");
                }
                if(bEepromWriteFlag != 1)    //Ð´eepromÆÚ¼ä²»µÃÐÞ¸ÄunEepromÄÚÈÝ
                {
                    //Get_WakeUp(); //»½ÐÑ  ³õÊ¼»¯ÍâÉè
                    unEeprom.stData.wSysOnOffSetBit = g_SysData.u16_SysOnOffFlag;
                    bEepromWriteFlag = 1;
                    
                }
            }
        }
        else    //ÊÕµ½µÄÄÚÈÝÓë·¢ËÍ²»·û
        {
            g_SysData.u16_SysOnOffFlag = cSysOff;
        }
    }
    else
    {
        g_SysData.u16_SysOnOffFlag = cSysOff;
    }
}



/*****************************************************************************************
Function Name       :  RemoteOrder_Reset_Treat
Description         :  ÏìÓ¦ÖÐ¼ÌÍø¹ØÏÂ·¢µÄÖØÆôÖ¸Áî
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_Reset_Treat(void)
{

}



/*****************************************************************************************
Function Name       :  RemoteOrder_Config1_Treat
Description         :  ÏìÓ¦ÖÐ¼ÌÍø¹ØÏÂ·¢µÄÅäÖÃÖ¸Áî1
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_Config1_Treat(void)
{
    ubitint U16_temp;
    
    //²ÉÑùÊ±¼ä
    U16_temp.bitdata.highchar = stLoraRxData.u8RxBuff[7] & 0xff;
    U16_temp.bitdata.lowchar  = stLoraRxData.u8RxBuff[6] & 0xff;
    if((U16_temp.id <= 1440)
    && (U16_temp.id >= 30))
    {
        g_SysData.u16_SampTime = U16_temp.id;
        g_statecheck.bit.bTimeSyncFlag = cFlagYes;    //½øÐÐÒ»´Î¼ÆÊ±Í¬²½
        printf(" Sample Time Set ");
    }

    //ÐÄÌø¼ä¸ô
    U16_temp.bitdata.highchar = stLoraRxData.u8RxBuff[9] & 0xff;
    U16_temp.bitdata.lowchar  = stLoraRxData.u8RxBuff[8] & 0xff;
    g_SysData.u16_HeartBeatInterval = U16_temp.id;
    
    //Ç·Ñ¹ãÐÖµ
    g_SysData.u8_LowVoltTreshold = stLoraRxData.u8RxBuff[10] & 0xff;
    
    //ÐÝÃßãÐÖµ
    g_SysData.u8_SleepTreshold = stLoraRxData.u8RxBuff[11] & 0xff;
    
    // ÇãÐ±±¨¾¯ãÐÖµ
    U16_temp.bitdata.highchar = stLoraRxData.u8RxBuff[13] & 0xff;
    U16_temp.bitdata.lowchar  = stLoraRxData.u8RxBuff[12] & 0xff;
    if(U16_temp.id <= 450)
    {
        g_SysData.u16_TiltWarningTreshold = U16_temp.id;
    }

    
    //µ±Ç°Ê±¼ä
    RTC_Data.Year    = stLoraRxData.u8RxBuff[14] & 0xff;
    RTC_Data.Month   = stLoraRxData.u8RxBuff[15] & 0xff;
    RTC_Data.Date    = stLoraRxData.u8RxBuff[16] & 0xff;
    RTC_Time.Hours   = stLoraRxData.u8RxBuff[17] & 0xff;
    RTC_Time.Minutes = stLoraRxData.u8RxBuff[18] & 0xff;
    RTC_Time.Seconds = stLoraRxData.u8RxBuff[19] & 0xff;
    RTC_SetTime(&RTC_Data, &RTC_Time);    //ÉèÖÃÊ±¼ä


}



/*****************************************************************************************
Function Name       :  RemoteOrder_Config2_Treat
Description         :  ÏìÓ¦ÖÐ¼ÌÍø¹ØÏÂ·¢µÄÅäÖÃÖ¸Áî2
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_Config2_Treat(void)
{
    
}



/*****************************************************************************************
Function Name       :  RemoteOrder_Sleep_Treat
Description         :  ÏìÓ¦ÖÐ¼ÌÍø¹ØÏÂ·¢µÄË¯ÃßÖ¸Áî
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_Sleep_Treat(void)
{
    
}



/*****************************************************************************************
Function Name       :  RemoteOrder_SysOnOff_Treat
Description         :  ÏµÍ³¿ª¹Ø»úÃüÁî´¦Àí
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_SysOnOff_Treat(void)
{
    g_SysData.u16_SysOnOffFlag = stLoraRxData.u8RxBuff[6] & 0xff;
    //µ±Ç°Ê±¼ä
    RTC_Data.Year    = stLoraRxData.u8RxBuff[7]  & 0xff;
    RTC_Data.Month   = stLoraRxData.u8RxBuff[8]  & 0xff;
    RTC_Data.Date    = stLoraRxData.u8RxBuff[9]  & 0xff;
    RTC_Time.Hours   = stLoraRxData.u8RxBuff[10] & 0xff;
    RTC_Time.Minutes = stLoraRxData.u8RxBuff[11] & 0xff;
    RTC_Time.Seconds = stLoraRxData.u8RxBuff[12] & 0xff;
    RTC_SetTime(&RTC_Data, &RTC_Time);//ÉèÖÃÊ±¼ä

}


/*****************************************************************************************
Function Name       :  RemoteOrder_TimeCalb_Treat
Description         :  Ê±¼äÍ¬²½´¦Àíº¯Êý
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 

void RemoteOrder_TimeCalb_Treat(void)
{
    g_statecheck.bit.bTimeSyncFlag = stLoraRxData.u8RxBuff[6] & 0xff;
}



/*****************************************************************************************
Function Name       :  RemoteOrder_TiltCalb_Treat
Description         :  µ÷Æ½ÃüÁî´¦Àíº¯Êý
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 

void RemoteOrder_TiltCalb_Treat(void)
{
    g_statecheck.bit.bTiltCalbFlag = stLoraRxData.u8RxBuff[6] & 0xff;
}


/*****************************************************************************************
Function Name       :  RemoteOrder_IAP_Treat
Description         :  ÉÕÂ¼ÃüÁî´¦Àíº¯Êý
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_IAP_Treat(void)
{

    g_statecheck.bit.bIAP_Flag = stLoraRxData.u8RxBuff[6] & 0xff;    //»ñÈ¡ÃüÁî

}


/*****************************************************************************************
Function Name       :  RemoteOrder_IAP_Treat
Description         :  ´ò¿ª½ÓÊÕµÈ´ýÉÕÂ¼ÃüÁîÏÂ·¢
Input               :  void
Output              :  ÎÞ
                       
Author              :  YXD
Creation Date       :  2020/03/10
Revisor             :  1.0
Revision Date       :  2020/03/10
Note                :  None
******************************************************************************************/ 
void RemoteOrder_PreIAP_Treat(void)
{

    g_statecheck.bit.bPreIAP_Flag = stLoraRxData.u8RxBuff[6] & 0xff;    //»ñÈ¡ÃüÁî

}






