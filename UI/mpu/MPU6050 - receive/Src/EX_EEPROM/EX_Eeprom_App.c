/***************************************************************************************************
* Copyright (C)2017, En-Plus.
* File Name                             :     EepromVar.c 
* Description                           :     eeprom相关变量定义,及eeprom数据的初始化和范围校验,以及写eeprom操作
* Version                               :     V1
* Author                                :     
* Creat Date                            :     2017-07-19
* notice                                :     
****************************************************************************************************/
#include "main.h"


union unMasterData unEeprom;
//eeprom写标志----任何修改unEeprom结构体内容的逻辑,都应该在bEepromWriteFlag=0时才能执行,避免将错误的和校验写入eeprom
INT8U bEepromWriteFlag = 0;

//eeprom芯片状态:0---存储区/备份区均不ok,1---存储区ok,备份区未读取,2---存储区不ok,备份区ok,
INT16U wEEpromChipStatus = 0;

//eeprom数据读取结果:1---数据可靠(校验和正确),0---数据不可靠(校验和错误)
INT16U wEEpromDataStatus = 0;


//for sEepromWriteTask
INT16U wEepromWriteStep = 0;   //eeprom写过程的阶段
INT16U *wpRamStartAddr;        //写操作源数据起始地址
INT16U *wpRamStopAddr;         //写操作源数据终止地址
INT16U *wpRamAddrTemp;         //源数据地址中间值
INT16U wEepromAddr;            //写操作目的地址---eeprom中的地址
INT16U wEepromWriteOK;  
INT16U wEepromWriteNumber;
INT32U dwEepromWriteCheckSum;

INT16U wFaultEEPCNT = 0;
INT16U wFaulFlag    = 0;

void sEepromWriteTask(void);
void sEepromFirstWrite(void);
void sEepromWrite(void);
void sEraseEerpom(void);
void sCheckSavedCfg(void);
void sDefaultCfg(void);



/**********************************************************************************************
* Description       :     eeprom写任务
* Author            :     
* modified Date     :     
* notice            :     这个函数是根据软件无操作系统调度多任务能力的特点定制的写操作过程,为避免
*                         任务长时间独占dsp资源,每次sPageWrite()之后退出任务,等待下一个任务周期来
*                         执行下一个sPageWrite()操作
***********************************************************************************************/
void sEepromWriteTask(void)
{
    switch(wEepromWriteStep)
    {
    case 0://检测有没有写操作
        if(bEepromWriteFlag == 1)
        {
            wEepromWriteStep = 1;
            hoWP_L();//write enable
        }
        break;
    case 1://第一阶段,为第二阶段初始化数据
        wEepromWriteOK = 1;
        wpRamStartAddr = &unEeprom.wArray[0];
        wpRamStopAddr  = &unEeprom.wArray[cEepromLength - 1];
        wpRamAddrTemp  = wpRamStartAddr;
        
        wEepromAddr = 0;
        wEepromWriteNumber = wpRamStopAddr - wpRamStartAddr + 1;
        if(wEepromWriteNumber > cEepromLength)
        {
            wEepromWriteOK = 0;
            wEepromWriteStep = 5;//超过正确的数据长度直接跳到最后阶段,结束写操作
            
        }
        else//否则,更新校验和并转入第二阶段
        {
            dwEepromWriteCheckSum = sCrc32(wpRamStartAddr,(wpRamStopAddr - wpRamStartAddr)-1);
            *(wpRamStopAddr - 1)  = (INT16U)(dwEepromWriteCheckSum >> 16);
            *(wpRamStopAddr)      = (INT16U)(dwEepromWriteCheckSum & 0xffff);
            
            wEepromWriteStep = 2;
        }
        break;
    case 2://第二阶段,写主存储区
        while((wpRamAddrTemp < wpRamStopAddr) && (wEepromWriteOK == 1))
        {
            wEepromWriteOK = sPageWrite(wEepromAddr,wpRamAddrTemp) & wEepromWriteOK;
            wEepromAddr    = wEepromAddr + 8;      // 8 bytes per page
            wpRamAddrTemp  = wpRamAddrTemp + 4;    // 4 data per page
            if(wEepromWriteOK != 1)
            {
                wEepromWriteStep = 5;//异常直接跳到最后阶段,结束写操作
            }
            return;
        }
        wEepromWriteStep = 3;
        break;
    case 3://第三阶段,为第四阶段初始化数据
        wpRamStartAddr = &unEeprom.wArray[0];
        wpRamStopAddr  = &unEeprom.wArray[cEepromLength - 1];
        wpRamAddrTemp  = wpRamStartAddr;
        
        wEepromAddr = 128;
        wEepromWriteNumber = wpRamStopAddr - wpRamStartAddr + 1;//结果应该等于cPfcEepromLength
        if(wEepromWriteNumber > cEepromLength)
        {
            wEepromWriteOK = 0;
            wEepromWriteStep = 5;//超过正确的数据长度直接跳到最后阶段,结束写操作
        }
        else
        {
            wEepromWriteStep = 4;//否则,转入第四阶段
        }
        break;
    case 4: //对备份区进行存储，此步骤可在系统读写EEPROM完全可靠的条件下取消
        while((wpRamAddrTemp < wpRamStopAddr) && (wEepromWriteOK == 1))
        {
            wEepromWriteOK = sPageWrite(wEepromAddr,wpRamAddrTemp) & wEepromWriteOK;
            wEepromAddr    = wEepromAddr + 8;
            wpRamAddrTemp  = wpRamAddrTemp + 4;
            if(wEepromWriteOK != 1)
            {
                wEepromWriteStep = 5;//异常直接跳到最后阶段,结束写操作
            }
            return;
        }
        wEepromWriteStep = 5;
        break;
    case 5:
        wEepromWriteStep = 0;
        wFaultEEPCNT++;
        if(wEepromWriteOK == 1)
        {
            bEepromWriteFlag = 0;//写入成功清除写标志
            wFaultEEPCNT = 0;
            g_statecheck.bit.bExEepromFault = 0;
            printf("AT24C02_WR_COMPLETE");
        }
        if(wFaultEEPCNT >= 3)
        {
            g_statecheck.bit.bExEepromFault = 1;
            printf("AT24C02_WR_FAILURE");

            wSysState = cFaultState;
        }
        hoWP_H();    //write disable
        break;
    default:
        wEepromWriteStep = 0;
        break;
    }
}


//上电后第一次写---操作系统建立之前
void sEepromFirstWrite(void)
{
    INT16U  i;
    INT16U wTemp;

    if((bEepromWriteFlag == 1) && (wEEpromDataStatus == 1))    //自身读取的数据不可靠时不能写入
    {
        hoWP_L();//write enable
        for(i = 0;i < 3;i ++)
        {
            wTemp = sWriteEeprom(unEeprom.wArray, unEeprom.wArray + cEepromLength - 1);

            if(wTemp == true)
            {
                bEepromWriteFlag = 0;
                hoWP_H();//write disable
                return;
            }
        }
        
    }
    bEepromWriteFlag = 0;
}


//擦除整个EEPROM空间并改为默认值
void sReWriteEerpom(void)
{
    INT16U  i;
    INT16U wTemp;
    
    sDefaultCfg();
    if(bEepromWriteFlag == 1)    //自身读取的数据不可靠时不能写入
    {
        
        hoWP_L();    //write enable
        for(i = 0;i < 3;i ++)
        {
            wTemp = sWriteEeprom(unEeprom.wArray,unEeprom.wArray + cEepromLength - 1);

            if(wTemp == true)
            {
                bEepromWriteFlag = 0;
                hoWP_H();//write disable
                return;
            }
        }
        
    }
    bEepromWriteFlag = 0;
}




//正常状态下的写操作
void sEepromWrite(void)
{
    INT16U wTemp;
    //eeprom write loop
    
    if((bEepromWriteFlag == 1) && (wEEpromDataStatus == 1))    //master自身读取的数据不可靠时不能写入
    {
        hoWP_L();//write enable

        wTemp = sWriteEeprom(unEeprom.wArray,unEeprom.wArray + cEepromLength - 1);
        
        if(wTemp == false)
        {
            if(++ wFaultEEPCNT >= 3)
            {
                if(wFaulFlag == 0)
                {
                    //g_StateCheck.bit.bLlcEepromFault = 1;
                    wSysState = cFaultState;
                }
                bEepromWriteFlag = 0;
                wFaultEEPCNT     = 0;
                wFaulFlag        = 1;
                hoWP_H();    //write disable
            }
        }
        else
        {
            //g_StateCheck.bit.bLlcEepromFault = 0;
            bEepromWriteFlag = 0;
            wFaultEEPCNT     = 0;
            hoWP_H();    //write disable
        }
    }
}




void sCheckSavedCfg(void)
{

    if((unEeprom.stData.wSleepGap < 2160)    // 最小不小于12*60min
    || (unEeprom.stData.wSleepGap > 4321) 	 //最大不大于24*60min
	|| (unEeprom.stData.wAcclTresholdSetValue  <  10000) //阈值
	|| (unEeprom.stData.wAcclTresholdSetValue  >  60000)
	|| (unEeprom.stData.GatewayID[0] != 0x44) 
	|| (unEeprom.stData.GatewayID[1] != 0x5A) ) 
    {
        unEeprom.stData.wSleepGap = 4320;
		unEeprom.stData.wAcclTresholdSetValue = 32768;  //0x80
		unEeprom.stData.GatewayID[0] = 0x44;
		unEeprom.stData.GatewayID[1] = 0x5A;
		unEeprom.stData.GatewayID[2] = 0x00;
		unEeprom.stData.GatewayID[3] = 0x00;
		unEeprom.stData.GatewayID[4] = 0x00;
		unEeprom.stData.GatewayID[5] = 0x01;
        bEepromWriteFlag = 1;
    }

}



void sDefaultCfg(void)
{
    unEeprom.stData.wEepromDataVer         = cEepromDataVer;

    unEeprom.stData.wLocalSNCode[0]        = 0x0001;
    unEeprom.stData.wLocalSNCode[1]        = 0x0014;
    unEeprom.stData.wLocalSNCode[2]        = 0x001C;
    unEeprom.stData.wLocalSNCode[3]        = 0x0002;
    unEeprom.stData.wLocalSNCode[4]        = 0x0003;
    unEeprom.stData.wLocalSNCode[5]        = 0x0000;
    unEeprom.stData.wLocalSNCode[6]        = 0x0000;
    

    unEeprom.stData.wLoraRoutingFlag       = 0x0001;    // 终端是否已完成LORA组网标志9

    unEeprom.stData.wRoutingLocalID        = 0x002B;    // 组网后终端ID10
    unEeprom.stData.wRoutingGWID           = 0x002B;    // 组网后网关ID11
    unEeprom.stData.wSWversion             = 0x0001;    // 本机软件版本号12
    unEeprom.stData.wHWversion             = 0x0002;    // 本机硬件版本号13
    unEeprom.stData.wSysOnOffSetBit        = 0x0001;    // 开关机位

    unEeprom.stData.wShunxianOffset_H      = 0x0000;    // 角度偏移值
    unEeprom.stData.wShunxianOffset_L      = 0x0000;
    unEeprom.stData.wHengxianOffset_H      = 0x0000;
    unEeprom.stData.wHengxianOffset_L      = 0x0000;
    unEeprom.stData.wZongheOffset_H        = 0x0000;
    unEeprom.stData.wZongheOffset_L        = 0x0000;    // 20

    unEeprom.stData.wTowerNum              = 0x0101;    // 杆塔组号与序号

    unEeprom.stData.wSampTimeSetValue      = 0x003C;    // 30采样时间设定值---60min

    unEeprom.stData.wHBIntervalTimeSetValue     = 0x001E;    // 心跳间隔时间设定值31
    unEeprom.stData.wLowVoltTresholdSetValue    = 0x001E;    // 欠压阈值设定值32
    unEeprom.stData.wSleepTresholdSetValue      = 0x001E;    // 休眠阈值设定值33

    unEeprom.stData.wTiltTresholdSetValue       = 0x0005;    // 倾斜报警阈值设定值34
    
	unEeprom.stData.wAcclTresholdSetValue = 0x8000;       	//加速度报警阈值-----地震项目
	unEeprom.stData.wSleepGap = 0x10E0;   					//睡4320个20s，24小时醒一次发一次数据-------地震项目
    
	unEeprom.stData.GatewayID[0] = 0x44;         		//地震设备网关ID-----地震项目
	unEeprom.stData.GatewayID[1] = 0x5A;
	unEeprom.stData.GatewayID[2] = 0x00;
	unEeprom.stData.GatewayID[3] = 0x00;
	unEeprom.stData.GatewayID[4] = 0x00;
	unEeprom.stData.GatewayID[5] = 0x01;
		
		
	unEeprom.wArray[1]  = 0;	
    
    unEeprom.wArray[22] = 0;
    unEeprom.wArray[23] = 0;
    unEeprom.wArray[24] = 0;
    unEeprom.wArray[25] = 0;
    unEeprom.wArray[26] = 0;
    unEeprom.wArray[27] = 0;
    unEeprom.wArray[28] = 0;
    unEeprom.wArray[29] = 0;


    unEeprom.wArray[43] = 0;
    unEeprom.wArray[44] = 0;
    unEeprom.wArray[45] = 0;
    unEeprom.wArray[46] = 0;
    unEeprom.wArray[47] = 0;
    unEeprom.wArray[48] = 0;
    unEeprom.wArray[49] = 0;
    unEeprom.wArray[50] = 0;
    unEeprom.wArray[51] = 0;
    unEeprom.wArray[52] = 0;
    unEeprom.wArray[53] = 0;
    unEeprom.wArray[54] = 0;
    unEeprom.wArray[55] = 0;
    unEeprom.wArray[56] = 0;
    unEeprom.wArray[57] = 0;
    unEeprom.wArray[58] = 0;
    unEeprom.wArray[59] = 0;
    unEeprom.wArray[60] = 0;
    unEeprom.wArray[61] = 0;


    bEepromWriteFlag = 1;
}



