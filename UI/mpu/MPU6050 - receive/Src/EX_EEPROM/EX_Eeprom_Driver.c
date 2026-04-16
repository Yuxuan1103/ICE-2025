/***************************************************************************************************
* File Name                             :     eeprom_temp.c 
* Description                           :     AT24C16芯片读写驱动程序
* Version                               :     V1
* Author                                :     
* Creat Date                            :     2017-07-19
* notice                                :     
****************************************************************************************************/
#include "main.h"



INT16U wReadCheckBuf[cEepromLength];




#define False 0
#define True  1


void sEepromGPIOInit(void);
INT32U sCrc32( INT16U * aData, INT16U aSize );
void sI2CReset(void);
void sDelay2us(void);
void sDelay10mSec(void);
void sI2CReset(void);
void sI2CStart(void);
void sI2CStop(void);
INT16U sByteIn(void);
INT16U sByteOut(INT16U bOutByteData);
void sSendACK(void);
INT16U sReadACK(void);
INT16U sReadData(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr,INT16U bEepromSatrtAddr);
INT16U sWriteData(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr,INT16U bEepromAddr);
INT16U sPageWrite(INT16U bWordAddress,INT16U *bpMCUAddress);
void sSDA_DIR(INT8U SET);


/******************************************************************************

******************************************************************************/
INT16U sReadEeprom(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr)
{
    INT32U dwCheckSum,dwCheckSumTemp;
    INT16U bEEBlockStatusOK;
    
    wEEpromChipStatus = 0;
    bEEBlockStatusOK = 0;
    sI2CReset();
    
    bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,0);
    if(bEEBlockStatusOK == 0)
    {
        bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,0);
    }
    
    if(bEEBlockStatusOK == 0)
    {
        bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,0);
    }
    
    
    if(bEEBlockStatusOK)
    {
        dwCheckSumTemp = *(bpMCUStopAddr-1) * 0x010000;
        dwCheckSumTemp += *bpMCUStopAddr;
        dwCheckSum = sCrc32(bpMCUStartAddr,(bpMCUStopAddr - bpMCUStartAddr)-1);
        if(dwCheckSum == dwCheckSumTemp)
        {
            wEEpromChipStatus = 1;//存储区ok,就不用读取备份区了
            return (True);
        }
        else
        {
            bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,cEepromStrByteNum);
            if(bEEBlockStatusOK == 0)
            {
                bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,cEepromStrByteNum);
            }
            
            if(bEEBlockStatusOK == 0)
            {
                bEEBlockStatusOK = sReadData(bpMCUStartAddr,bpMCUStopAddr,cEepromStrByteNum);
            }
            
            if(bEEBlockStatusOK)
            {
                dwCheckSumTemp = *(bpMCUStopAddr-1) * 0x010000;
                dwCheckSumTemp += *bpMCUStopAddr;
                dwCheckSum = sCrc32(bpMCUStartAddr,(bpMCUStopAddr - bpMCUStartAddr)-1);
                if(dwCheckSum == dwCheckSumTemp)
                {
                    wEEpromChipStatus = 2;
                    return (True);
                }
            }
        }
    }
    return(False);
}


/**********************************************************************************************
* Description       :     eeprom驱动程序,写eeprom操作
* Author            :     
* modified Date     :     2017-07-19
* notice            :     这个函数相比原始函数修改了写操作逻辑和写入后再读取校验逻辑,未做调试
*                         由于目前的工程不含操作系统,这个函数不太适用,所以没调试,需要额外开发
*                         写eeprom数据的逻辑.
***********************************************************************************************/
INT16U sWriteEeprom(INT16U* bpMCUStartAddr,INT16U* bpMCUStopAddr)
{   
    INT32U dwCheckSum;
    INT32U dwCheckSumTemp;
    INT16U wDataLength;
    INT16U wWriteResult;
    
    wDataLength = bpMCUStopAddr - bpMCUStartAddr;
    if((bpMCUStopAddr >= bpMCUStartAddr) && (wDataLength <= (cEepromLength - 1)))
    {
        sI2CReset();
        dwCheckSum = sCrc32(bpMCUStartAddr,(bpMCUStopAddr - bpMCUStartAddr)-1);
        *(bpMCUStopAddr-1) = (INT16U)(dwCheckSum >> 16);
        *(bpMCUStopAddr)   = (INT16U)(dwCheckSum & 0xffff);

        wWriteResult = 1;
        wWriteResult = wWriteResult & sWriteData(bpMCUStartAddr,bpMCUStopAddr,0);    //向主存储区写数据
        wWriteResult = wWriteResult & sWriteData(bpMCUStartAddr,bpMCUStopAddr,cEepromStrByteNum);    //向备份区写数据

        if(sReadData(&wReadCheckBuf[0],&wReadCheckBuf[0] + wDataLength,0))    //读取刚才写入的主存储区数据
        {
            dwCheckSumTemp = wReadCheckBuf[wDataLength-1] * 0x10000;
            dwCheckSumTemp += wReadCheckBuf[wDataLength];
            if(dwCheckSum == dwCheckSumTemp)    //比对读取的校验位是否与写入的校验位一致
            {
                wWriteResult = wWriteResult & 1;
            }
            else
            {
                wWriteResult = 0;
            }
        }
        
        if(sReadData(&wReadCheckBuf[0],&wReadCheckBuf[0] + wDataLength,cEepromStrByteNum))    //读取刚才写入的主存储区数据
        {
            dwCheckSumTemp = wReadCheckBuf[wDataLength-1] * 0x10000;
            dwCheckSumTemp += wReadCheckBuf[wDataLength];
            if(dwCheckSum == dwCheckSumTemp)
            {
                wWriteResult = wWriteResult & 1;
            }
            else
            {
                wWriteResult = 0;
            }
        }
        return(wWriteResult);
    }
    else 
    {
        return(False);
    } 
}

INT16U sReadData(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr,INT16U bEepromSatrtAddr)
{   
    INT16U bByteOutOK;
    INT16U wReadNumber;
    INT16U *bpMCUStoreAddr;
    
    bByteOutOK = 1;
    wReadNumber = bpMCUStopAddr - bpMCUStartAddr;
    
    if(wReadNumber > (cEepromLength - 1))
    {
        return(False);
    }

    //参考时序
    sI2CStart();
    bByteOutOK = sByteOut(((INT16U)0xA0) | ((bEepromSatrtAddr >> 7) & 0x00));//device address---AT24C16：0xa0/0xa2/0xa4/0xa6/0xa8/0xaa/0xac/0xae
    bByteOutOK = sByteOut((INT16U)(bEepromSatrtAddr & 0xff));//Word address---0~255之间
    sI2CStop();
    
    sI2CStart();
    bByteOutOK = sByteOut(((INT16U)0xA1) | ((bEepromSatrtAddr >> 7) & 0x00));//
    bpMCUStoreAddr = bpMCUStartAddr;   
    while ((bpMCUStoreAddr <= bpMCUStopAddr) && bByteOutOK)
    {
        *bpMCUStoreAddr = (sByteIn() << 8) & 0xff00; 
        sSendACK();
        *bpMCUStoreAddr += sByteIn(); 
        if(bpMCUStoreAddr != bpMCUStopAddr)
        {
            sSendACK();
        }
        bpMCUStoreAddr++;
    }  
    sI2CStop();
    return(bByteOutOK);
}

INT16U sWriteData(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr,INT16U bEepromAddr)
{
    INT16U bWriteOK;
    INT16U *bpWriteAddr;
    INT16U wWriteNumber;
    
    bpWriteAddr = bpMCUStartAddr;
    wWriteNumber = bpMCUStopAddr-bpMCUStartAddr;
    if(wWriteNumber > (cEepromLength - 1))
    {
        return(False);
    }
    
    bWriteOK = 1;
    while((bpWriteAddr <= bpMCUStopAddr) && (bWriteOK == 1))
    {
        bWriteOK = sPageWrite(bEepromAddr,bpWriteAddr) & bWriteOK;
        sDelay10mSec();
        bEepromAddr = bEepromAddr + 8;
        bpWriteAddr = bpWriteAddr + 4;
    }
    return(bWriteOK);
}
 
INT16U sPageWrite(INT16U bWordAddress,INT16U *bpMCUAddress)
{
    INT16U bWriteOK;
    INT16U wTempi;

    bWriteOK = 0;
    if((bWordAddress & 0x07) == 0)      //00000111    // 判定是否越界
    {
        sI2CStart();
        bWriteOK = sByteOut(((INT16U)0xA0) | ((bWordAddress >> 7) & 0x00));    //构建device address正确
        bWriteOK = sByteOut(bWordAddress & 0xff) & bWriteOK;    //Word Address---0~255之间

        for(wTempi = 0;wTempi < 4;wTempi++)    //由于是8bytes per page，故每页只能存4个INU16U的数据
        {
            bWriteOK = sByteOut((*bpMCUAddress >> 8) & 0xff) & bWriteOK;// Data
            bWriteOK = sByteOut(*bpMCUAddress        & 0xff) & bWriteOK;// Data
            bpMCUAddress++;
        }
        sI2CStop();
    }
    return(bWriteOK);
}


void sI2CStart(void)
{  

    sSDA_DIR(cOutputPin); 

    hoSDA_H(); 
    sDelay2us();
    hoSCL_H(); /* clock */
    sDelay2us();
    hoSDA_L(); 
    sDelay2us();
    hoSCL_L(); /* clock */
    sDelay2us();

    sSDA_DIR(cInputPin);

    //hioSDA=1;
}

INT16U sBitIn(void)
{
    INT16U bInBitData;
    // DINT;
    hoSCL_H(); /* clock */
    sDelay2us();

    sSDA_DIR(cInputPin);

    bInBitData = hiSDA();
    sDelay2us();
    hoSCL_L(); /* clock */
    // EINT;
    sDelay2us();
    return(bInBitData);
}

INT16U sByteIn(void)
{   
    INT16U bInByteData;
    bInByteData = 0;
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    bInByteData = bInByteData*2 + sBitIn();
    return(bInByteData);
}

void sBitOut(INT16U bOutBitData)    //  DSP write in EEPROM
{
    sSDA_DIR(cOutputPin);
    if(bOutBitData == 0x1)
    {
        hoSDA_H();
    }
    else
    {
        hoSDA_L();
    }
    sDelay2us();
    // DINT;  deced by yyy @111115 需要测试
    hoSCL_H(); /* clock */
    sDelay2us();
    hoSCL_L(); /* clock */
    sDelay2us();
    hoSDA_H(); 
    sSDA_DIR(cInputPin);
    // EINT;
    sDelay2us();
}

INT16U sByteOut(INT16U bOutByteData)
{
    INT16U bACKOK;
    
    sBitOut((bOutByteData >> 7) & 1);
    sBitOut((bOutByteData >> 6) & 1);
    sBitOut((bOutByteData >> 5) & 1);
    sBitOut((bOutByteData >> 4) & 1);
    sBitOut((bOutByteData >> 3) & 1);
    sBitOut((bOutByteData >> 2) & 1);
    sBitOut((bOutByteData >> 1) & 1);
    sBitOut(bOutByteData & 1);
    bACKOK = sReadACK();
    return(bACKOK);
}

void sI2CStop(void)
{
    sSDA_DIR(cOutputPin); 
    hoSDA_L();
    sDelay2us();
    hoSCL_H(); /* clock */
    sDelay2us();
    hoSDA_H();
    sDelay2us();
}
 
void sSendACK(void)
{
    sBitOut(0);
}

INT16U sReadACK(void)
{
    INT16U bACKOK; 

    sSDA_DIR(cInputPin);

    sDelay2us();
    hoSCL_H();
    sDelay2us();
    if(hiSDA())
    {
        bACKOK = 0;
    }
    else 
    {
        bACKOK = 1;
    }   
    hoSCL_L();
    sDelay2us();
    return(bACKOK);
}

INT16U  sByteWrite(INT16U bWordAddress,INT16U bWriteData)
{
    INT16U bWriteOK;
    bWriteOK = 0;
    sI2CStart();
    bWriteOK = sByteOut(((INT16U)0xA8) | ((bWordAddress >> 7) & 0x06));//modify by hzh for 0~1023 write
    bWriteOK = sByteOut(bWordAddress & 0xff) & bWriteOK;//bWordAddress范围0~255
    bWriteOK = sByteOut(bWriteData) & bWriteOK;      // Data
    sI2CStop();
    return(bWriteOK);
}
//
INT16U sReadByte(INT16U bReadAddr,INT16U*bpInData)
{  
    // INT16U bInData;
    INT16U bACKOK;
    
    *bpInData = 0;
    bACKOK = 0;
    
    sI2CStart();
    bACKOK = sByteOut(((INT16U)0xA8) | ((bReadAddr >> 7) & 0x06));    //
    bACKOK = sByteOut(bReadAddr & 0xff)&bACKOK;    //bReadAddr范围0~255
    sI2CStop();
    sI2CStart();
    bACKOK = sByteOut(((INT16U)0xA9) | ((bReadAddr >> 7) & 0x06)) & bACKOK;    //
    *bpInData = sByteIn();
    sI2CStop(); 
    return(bACKOK);
}


void sDelay10mSec(void)
{
    INT16U i;
    
    for(i=0;i<2000;i++) 
    {
        sDelay2us();
    }
}

/**************************************************************************/
/* Function : sDelay2us                                                   */
/* Object   : For IIC Interface delay                                     */
/**************************************************************************/
void sDelay2us(void)
{
    INT16U xxx;
    for (xxx=0;xxx<2;xxx++);//18
}


/**************************************************************************/
/* Function : Reset EEProm Device                                         */
/* Object   :                                                             */
/**************************************************************************/
void sI2CReset(void)
{
    INT16U i;
    sSDA_DIR(cOutputPin);

    for (i=0; i<9; i++)
    {
        hoSCL_L(); /* clock */
        sDelay2us();
        hoSCL_H(); /* clock */
        sDelay2us();
        if (hiSDA() == 1)
        {
            break;
        }
    }
}



//其实是和校验计算
INT32U sCrc32( INT16U * aData, INT16U aSize )
{
    INT32U dwCrc32 = 0;
    
    while(aSize--)
    {
        dwCrc32 += *aData++;
    }
    return dwCrc32;
}




void sEepromGPIOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    //FOR  EEPROM
    //WP
    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);

    //SCL
    GPIO_InitStruct.Pin   = GPIO_PIN_2;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

    //SDA
    GPIO_InitStruct.Pin   = GPIO_PIN_3;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

}

void sSDA_DIR(INT8U SET)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(SET == cOutputPin)
    {
        GPIO_InitStruct.Pin   = GPIO_PIN_3;
        GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else
    {
        GPIO_InitStruct.Pin   = GPIO_PIN_3;
        GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull  = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}




