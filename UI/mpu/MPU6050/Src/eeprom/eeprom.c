#include "eeprom.h"
FLASH_EraseInitTypeDef EEPROM_EraseInit;
float LimitValue;
uint8_t SensorID, GateWayID;
uint8_t CalibrationTime[3],HeartBeatTime[3];
unsigned long LoraCommDelayTime, LoraCommDelayTimeTick, LoraCommWaitTime, LoraCommWaitTimetick, LoraCommCallBackWaitTime, LoraCommCallBackTick;
uint8_t EEPROMBuff[25] ;//= {0x00,0x11,0x00,0x01,0x00,0x00,0x03,0xe8,0x00,0x00,0x03,0xe8,0x00,0x00,0x13,0x88};
/************************************************/
/*                   擦除整页                   */
/********************************************** */
HAL_StatusTypeDef EEPROM_Erase(FLASH_EraseInitTypeDef *pEraseInit)
{
    HAL_StatusTypeDef status;
    uint32_t PageError;
    pEraseInit->TypeErase = FLASH_TYPEERASE_PAGES;/* 擦除方式，这里选择按页擦除 */
    pEraseInit->NbPages = 1;/* 页数量，模块中只对单页进行操作 */
    pEraseInit->PageAddress = EEPROM_FIRST_ADDESS;/* 被擦除页的起始地址，默认为PAGE0的地址，使用中需要对此成员进行更新 */
    HAL_FLASH_Unlock();
    status = HAL_FLASHEx_Erase(pEraseInit, &PageError);
    HAL_FLASH_Lock();
    return status;
}

/************************************************/
/*              指定地址写一个字节                */
/*WriteAddress:地址                             */
/*Data:        数据                             */
/********************************************** */ 
HAL_StatusTypeDef EEPROM_Write_FourBytes(uint32_t WriteAddress, uint32_t Data)
{
    HAL_StatusTypeDef status;
    uint32_t Address;
    //Data = (FourData[0] << 24) + (FourData[1] << 16) + (FourData[2] << 8) + FourData[3];
    Address = EEPROM_FIRST_ADDESS + WriteAddress;
    HAL_FLASH_Unlock();
    status = HAL_FLASH_Program(TYPEPROGRAM_WORD, Address, Data);//TYPEPROGRAM_WORD
    HAL_FLASH_Lock();
    return status;
}

/************************************************/
/*            从指定地址开始写入一段数据          */
/*WriteAddress: 开始地址  (4的倍数)                        */
/*       pData: 存放数据Buff                      */
/*        Size: 数据长度     (4的倍数)                      */
void EEPROM_Write_Array(uint32_t WriteAddress, uint8_t *pData, uint16_t Size)
{
    uint16_t i;
    uint16_t BuffSize;
    uint32_t Data;
    BuffSize = PAGE_SIZE - WriteAddress;//计算内存可读最大区域
    WriteAddress = EEPROM_FIRST_ADDESS + WriteAddress; //换算真实物理地址
    HAL_FLASH_Unlock();
    for(i = 0;i < Size;i=i+4)
    {
        if(i < BuffSize)
        {
            Data = (pData[i+3] << 24) + (pData[i+2] << 16) + (pData[i+1] << 8) + pData[i];

            HAL_FLASH_Program(TYPEPROGRAM_WORD, WriteAddress+i, Data);
            HAL_Delay(1);
        }
    }
    HAL_FLASH_Lock();
}

/************************************************/
/*              指定地址读一个字节               */
/*WriteAddress:地址                             */
/********************************************** */ 
uint8_t  EEPROM_Read_Byte(uint32_t WriteAddress)
{
    uint8_t data;
    data = *(__IO uint8_t*)(WriteAddress);
    return data;
}
/************************************************/
/*            从指定地址开始读取一段数据          */
/*WriteAddress:开始地址                          */
/*       pData:数据存放Buff                      */
/*        Size:数据长度                          */
/********************************************** */ 
void EEPROM_Read_Array(uint32_t ReadAddress,uint8_t *pData, uint16_t Size)
{
    uint16_t i;
    uint16_t BuffSize;
    BuffSize = PAGE_SIZE - ReadAddress;//计算内存可读最大区域
    ReadAddress = EEPROM_FIRST_ADDESS + ReadAddress; //换算真实物理地址
    for(i = 0;i < Size;i++)
    {
        if(i < BuffSize)
        {
            pData[i] = EEPROM_Read_Byte( ReadAddress + i);
        }
    }
}
/************************************************/
/*                 传感器配置                    */
/********************************************** */
void SensorConfig(void)
{
    EEPROM_Read_Array(0,EEPROMBuff, 23);
    SensorID  = EEPROMBuff[1]; //传感器ID
    GateWayID = EEPROMBuff[3];//网关ID
    LoraCommDelayTime = (EEPROMBuff[4] << 24) + (EEPROMBuff[5] << 16) + (EEPROMBuff[6] << 8) + EEPROMBuff[7];//发送延时配置
    LoraCommCallBackWaitTime = (EEPROMBuff[8] << 24) + (EEPROMBuff[9] << 16) + (EEPROMBuff[10] << 8) + EEPROMBuff[11];//等待回传时间配置
    LoraCommWaitTime  = (EEPROMBuff[12] << 24) + (EEPROMBuff[13] << 16) + (EEPROMBuff[14] << 8) + EEPROMBuff[15];//通信周期配置
    LimitValue = (EEPROMBuff[16] >> 4) + (float)(EEPROMBuff[16] & 0x0F)/10;
    CalibrationTime[0] = EEPROMBuff[17];//校时时间
    CalibrationTime[1] = EEPROMBuff[18];
    CalibrationTime[2] = EEPROMBuff[19];

    HeartBeatTime[0] = EEPROMBuff[20];//心跳时间
    HeartBeatTime[1] = EEPROMBuff[21];
    HeartBeatTime[2] = EEPROMBuff[22];


    //EEPROM_Read_Array(0,EEPROMBuff, 23);
    SensorID = 0x11; //传感器ID
    GateWayID = 0x10;//网关ID
    LoraCommDelayTime = 0;//发送延时配置
    LoraCommCallBackWaitTime = 2000;//等待回传时间配置
    LoraCommWaitTime  = 4000;//通信周期配置
    LimitValue = 0.1;
    CalibrationTime[0] = 23;//校时时间
    CalibrationTime[1] = 59;
    CalibrationTime[2] = 5;

    HeartBeatTime[0] = 16;//心跳时间
    HeartBeatTime[1] = 7;
    HeartBeatTime[2] = 0;

}


