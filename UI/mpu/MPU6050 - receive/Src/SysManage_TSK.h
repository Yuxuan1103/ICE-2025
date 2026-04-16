#ifndef    __SYS_MANAGE_TSK_H_
#define    __SYS_MANAGE_TSK_H_

#include   "main.h"

//定义终端系统状态
#define cWaitState           0x00
#define cLoRaRoutingState    0x01
#define cNormalState         0x02
#define cSleepState          0x03
#define cIAPState            0x04
#define cFaultState          0x05

//定义终端休眠状态
//#define c


typedef struct tagT_SysState
{

    struct
    {
        U16  SensorState       :16;    //byte0 
        U8   SensorFault        :8;    //byte1 
    }byte;

    struct
    {
        /*********************************************************/
        //Word0 SensorState
        U16  bLora_SendingState      :1;    // LORA发送状态位
        U16  bLoraCallBackFlag       :1;    // 
        U16  bLoraTimingSendFlag     :1;    // 终端定时发送标志位
        U16  bLoraTiltSendingFlag    :1;    // 用于倾斜报警下发数据
        U16  bSysTiltState           :1;    // 系统存在杆塔倾斜
        U16  bShunxianTiltState      :1;    // 顺线角度超过阈值位
        U16  bHenxianTiltState       :1;    // 横线角度超过阈值位
        U16  bReserved07             :1;    //
        U16  bLoraReceiveState       :1;    // LORA窗口时间接收标志
        U16  bTiltCalbFlag           :1;    // 倾斜终端调平标志
        U16  bIAP_Flag               :1;    // 终端代码更新标志
        U16  bPreIAP_Flag            :1;    // 打开接收等待烧录标志
        U16  bTimeSyncFlag           :1;    // 计时时间同步信号
        U16  bReserved0D             :1;    // 
        U16  bReserved0E             :1;    // 
        U16  bReserved0F             :1;    // 

        /*********************************************************/
        //Word1  SensorFault
        U8  bADXLFault         : 1;     // adxl355采样故障
        U8  bLowBatFault       : 1;     // 电池欠压故障
        U8  bSHIncompFault     : 1;     // 软硬件不兼容故障
        U8  bLORACommFault     : 1;     // LORA通信故障
        U8  bExEepromFault     : 1;     // 外部EEPROM读写故障
        U8  bReserved13        : 1; 
        U8  bReserved14        : 1; 
        U8  bReserved15        : 1; 

    }bit;

}T_SysState;

typedef struct tagT_SysData
{
    U8     u8_SensorType;    //核心传感器类型
    U8     u8_RemoteOrder;
    U8     u8_SN[7];    //出厂ID号
    U8     u8_GatewayID;       //网关ID,自动分配流程结束后存EEPROM
    U8     u8_AlloID;          //终端接收到的网关分配的子ID，自动分配流程结束后存EEPROM

    U16    u16_SampTime;             // 采样时间
    U16    u16_HeartBeatInterval;    // 心跳间隔
    U8     u8_LowVoltTreshold;       // 欠压阈值
    U8     u8_SleepTreshold;         // 休眠阈值
    U16    u16_TiltWarningTreshold;  // 倾斜报警阈值

    U16    u16_TiltSend_K;           // 倾斜阈值系数

    U8       u8_BatVolt;               // 电池电压数据
    float    f32_HenxianAngle ;
    float    f32_ShunxianAngle ;
    float    f32_ZongheAngle;

    float    f32_AdxlTemperature;

    U16    u16_SysOnOffFlag;         // 系统开关机命令位


    U16     u16_Tower_Num;           //杆塔号
		
		U16     u16_AcclWarningTreshold;   //  中断阈值
  
    U8      u8_RF_FRENQ_H;           // lora频率设定值
    U8      u8_RF_FRENQ_M;
    U8      u8_RF_FRENQ_L;

}T_SysData;




extern void SysManage_TSK(void);
extern void LEDRun(void);
extern void RF_Frenq_Set(U16 * u16TowerNum);



extern U16 wSysState;

extern uint8_t RunLED;
extern IWDG_HandleTypeDef hiwdg;
extern uint8_t flag1;
extern uint8_t LPWStatus;
extern uint8_t LPWStatus2;


extern T_SysState g_statecheck;
extern T_SysData g_SysData;



#endif

