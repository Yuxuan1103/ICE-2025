/*********************************************************************************************************
**
**    Project:  BootLoader
**
**    Componet: STM32L071
**	
**    Filename: ymodem.h
**	
**    Abstract: common文件的头文件声明
**	                                                        
**    Compiler: Keil 5.15
**           
**    Function List：  
**	
**        
**
**    History：
**	
**        <auther>    <date>    <version>   <desc>
**		
**          凡文    2020/01/19     1.0       规范该文件的排版
**   
*************************************************************************************************************/

/* 防止重定义 *****************************************************************/
#ifndef _YMODEM_H_
#define _YMODEM_H_
#include "stdint.h"
/* 宏 ------------------------------------------------------------------------*/
//#define PACKET_SEQNO_INDEX      (1)   
//#define PACKET_SEQNO_COMP_INDEX (2)

//#define PACKET_HEADER           (3)
//#define PACKET_TRAILER          (2)
//#define PACKET_OVERHEAD         (PACKET_HEADER + PACKET_TRAILER)
#define PACKET_SIZE             (128)
#define PACKET_1K_SIZE          (1024)

#define FILE_NAME_LENGTH        (112)
#define FILE_SIZE_LENGTH        (16)

#define SOH           (0x01)  //128字节数据包开始
#define STX           (0x02)  //1024字节的数据包开始
#define EOT           (0x04)  //结束传输
#define ACK           (0x06)  //认可应答
#define NAK           (0x15)  //不认可应答
#define CaN           (0x18)  //撤销传
#define C             (0x43)  //开始传送请求'C' == 0x43, 需要 16-bit CRC   

extern uint8_t  device_code[6];//装置码 123456
extern uint8_t  ciphertext[4]; //密文验证码 1234
extern uint8_t  start_code;    //起始码
extern uint8_t  end_code;      //结束码
extern uint8_t  control_code;  //控制码
extern uint8_t  function_code; //功能识别码（中继网关）
extern uint8_t  target_code;   //设备标号（中继网关）

//#define ABORT1                  (0x41)  //'A' == 0x41, 用户终止 
//#define ABORT2                  (0x61)  //'a' == 0x61, 用户终止

#define NAK_TIMEOUT             (60)  //25*200
#define MAX_ERRORS              (5)

/* 函数声明 ------------------------------------------------------------------*/

extern uint8_t file_name[FILE_NAME_LENGTH];
extern uint8_t file_size[FILE_SIZE_LENGTH];
//int32_t Ymodem_Receive_file(uint8_t *buf);
int32_t Ymodem_Receive_file(uint8_t *buf, uint8_t *filename, uint8_t *filesize);
//extern void Lora_Send_Ymodem (uint8_t *data,uint16_t length);
extern void Receiver_Return(uint8_t functioncode, uint8_t targetcode,  uint8_t FunctionReturnCode,  uint8_t Responseder, uint16_t ReceivingIndexes);

#endif  /* _YMODEM_H_ */

/*******************************文件结束***************************************/
