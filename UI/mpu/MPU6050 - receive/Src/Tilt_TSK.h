#ifndef    __TILT_TSK_H_
#define    __TILT_TSK_H_

#include   "main.h"


 extern uint8_t Sendcnt ;
 extern  U8 u8_ReciveFailCnt;
extern U8 u8_Fault_Cnt_Sx1 ;  
extern U8 u8_Fault_Cnt_Sx2 ;  
extern U8 u8_Fault_Cnt_Hx1 ;
extern U8 u8_Fault_Cnt_Hx2 ;
 
extern void DataSamplesTask(U16 *  thresholdvalue);
extern void SensorInitialData(float *pitchinitialdata, float *rollinitialdata, float *yawinitialdata);
extern void DataCommunication_TSK(void);
extern void PowerUpZeroClab(void);

extern uint8_t CRC16H , CRC16L;
extern INT16U  ACnt ;
extern INT16U  Aflag ;

extern float f32datacn_Sx ;  //用于存储上次的倾斜值
extern float f32datacn_Hx ;

#endif

