/***************************************************************************************************
* File Name                             :     EepromVar.h 
* Description                           :     eeprom相关变量定义,及驱动中几个接口函数申明
* Version                               :     V1
* Author                                :     
* Creat Date                            :     2017-07-19
* notice                                :     
****************************************************************************************************/
#ifndef _EEPROMVAR_H
#define _EEPROMVAR_H

#include "main.h"

#define true          1
#define false         0


/* EEPROM */
#define cEepromLength         64            //64*16bit     128*8
#define cEepromDataVer        0x04             //第3版---修改stMasterData结构体时,应提升此处版本号
#define cCustomerAreaLength   20            //20*16bit
#define cEepromDeviceByteNum  256           // AT2402 字节长度
#define cEepromStrByteNum     1024           // AT2402 主存储区存储长度



struct stMasterData//1~64 共64个16bit参数
{
    U16       wEepromDataVer;            //        0, eeprom数据结构版本
    U16       wBak1[1];                  //        1, 保留

    U16       wLocalSNCode[7];           //        2 ~ 8, 出厂SN码

    U16       wLoraRoutingFlag;          //        9, 终端是否已完成LORA组网标志

    U16       wRoutingLocalID;           //       10, 组网后终端ID
    U16       wRoutingGWID;              //       11, 组网后网关ID
    U16       wSWversion;                //       12, 本机软件版本号
    U16       wHWversion;                //       13, 本机硬件版本号

    U16       wSysOnOffSetBit;           //       14, 系统开机位

    U16       wShunxianOffset_H;         //       15, 角度偏移校准值，三个角度
    U16       wShunxianOffset_L;         //       16, 
    U16       wHengxianOffset_H;         //       17, 
    U16       wHengxianOffset_L;         //       18, 
    U16       wZongheOffset_H;           //       19, 
    U16       wZongheOffset_L;           //       20, 
    
    U16       wTowerNum;                 //       21, 

    U16       wBak2[8];                  //   22~ 29, 保留区2

    U16       wSampTimeSetValue;         //       30, 采样时间设定值

    U16       wHBIntervalTimeSetValue;   //       31, 心跳间隔时间设定值
    U16       wLowVoltTresholdSetValue;  //       32, 欠压阈值设定值
    U16       wSleepTresholdSetValue;    //       33, 休眠阈值设定值
    U16       wTiltTresholdSetValue;     //       34, 倾斜报警阈值设定值
	
    U16       wAcclTresholdSetValue;     //       35, 加速度报警阈值设定值------地震项目
	U16       wSleepGap;				 //       36, 睡眠时间------地震项目
	
	U16       GatewayID[6];              //    	  37~42, 网关ID------地震项目
	
    U16       wBak3[19];                 //       43~61, 保留区3
    
    U16       wCheckSumH;                //       62, 和校验高16位
    U16       wCheckSumL;                //       63, 和校验低16位
};


union unMasterData
{
    U16 wArray[cEepromLength];
    struct stMasterData stData;
};




extern union  unMasterData unEeprom;
extern INT8U  bEepromWriteFlag;
extern INT16U wEEpromChipStatus;
extern INT16U wEEpromDataStatus;



extern void sEepromWriteTask(void);
extern void sCheckSavedCfg(void);
extern void sDefaultCfg(void);
extern INT16U sPageWrite(INT16U bWordAddress,INT16U *bpMCUAddress);



//for eeprom_temp.c
#define cOutputPin 0x01
#define cInputPin  0x00

#define cWriteDisable 0x01
#define cWriteEnable  0x00

#define hoSDA_H()           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
#define hoSDA_L()           HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET)
#define hiSDA()             HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)
#define hoSCL_H()           HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
#define hoSCL_L()           HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)
#define hoWP_H()            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET)
#define hoWP_L()            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET)


extern void sEepromGPIOInit(void);
extern INT32U sCrc32( INT16U * aData, INT16U aSize );
extern INT16U sReadEeprom(INT16U *bpMCUStartAddr,INT16U *bpMCUStopAddr);
extern INT16U sWriteEeprom(INT16U *bpMCUStartAddr,INT16U *bpMCUEndAddr);


extern void sEepromWriteTask(void);
extern void sEepromFirstWrite(void);
extern void sEepromWrite(void);



#endif
