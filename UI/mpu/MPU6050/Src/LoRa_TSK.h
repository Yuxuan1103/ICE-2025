#ifndef    __LORA_TSK_H_
#define    __LORA_TSK_H_
#include   "main.h"




#define DataSendCommand           0x01
#define TimecalibrationCommand    0x02
#define HeartBeatCommand          0x03
#define SensorInitCommand         0x04
#define CLOSE          0x00
#define OPEN           0x01

#define cRecLengthMax     256
#define cSendLengthMax    256

#define cLoraCommIdle     0x00
#define cLoraWorking      0x01
#define cLoraReceiving    0x02

#define  GatID     0X01    //终端ID
#define  WayID     0X03    //网关ID

//帧类型表示符
#define cLoraTypeOrder          0x01
#define cLoraTypeAnswer         0x0F

//远程指令集
#define cLoraOrder_BOOTUP        0x00
#define cLoraOrder_TimeCalb      0x01
#define cLoraOrder_HeartBeat     0x02
#define cLoraOrder_Reset         0x03
#define cLoraOrder_Inquiry_1     0x04
#define cLoraOrder_Config_1      0x05
#define cLoraOrder_Sleep         0x06
#define cLoraOrder_TiltData      0x07
#define cLoraOrder_ErrData       0x08
#define cLoraOrder_Inquiry_2     0x09
#define cLoraOrder_Config_2      0x0A
#define cLoraOrder_IAP           0x2A
#define cLoraOrder_NormalRun     0x0C
#define cLoraOrder_TiltCalb      0x0D
#define cLoraOrder_SleepInform   0x0E
#define cLoraOrder_OpenRece      0x0F
#define cLoraOrder_Test          0xFE


#define  Wakeup_On          0x44        // 唤醒

 extern U8 u8_Wakeup_Flag  ;        //唤醒标志

// float   f32thresholdvalue = 0.5f ; // 报警初始阈值
extern float   f32thresholdvalue_Sx1  ; // 报警初始阈值 顺线  正向
extern float   f32thresholdvalue_Sx2  ; // 报警初始阈值 顺线  反向
extern float   f32thresholdvalue_Hx1  ; // 报警初始阈值 横线  正向
extern float   f32thresholdvalue_Hx2  ; // 报警初始阈值 横线  反向


typedef struct tagT_Lora_RXDATA
{
    uint8_t  u8PointIn;                                 //
    uint8_t  u8PointOut;                                //
    uint8_t  u8RxBuff[cRecLengthMax];                   //
    uint8_t  u8Alllength;                               //
    uint8_t  u8Crclength;
}T_Lora_RXDATA;
extern T_Lora_RXDATA stLoraRxData;



extern void MessageSendFunction(uint8_t command);
extern uint8_t SensorDataSend(void);
extern uint8_t SensorTimecalibration(void);
extern uint8_t SensorHeartBeat(void);
extern uint8_t SensorInit(float *firstpitch, float *firstroll, float *firstyaw);
extern void LoraComm_MainTsk(void);
extern void LoraMessageSend_Treat(uint8_t u8Lora_Command);
extern void PowerupTime(void);



#endif

