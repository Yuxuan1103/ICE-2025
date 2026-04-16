#ifndef __UART_H
#define	__UART_H
#include "stm32l0xx_hal.h"
#include <stdio.h>

//extern uint8_t UartReceiveBuff[20];
void UART_Init(void);
void Uart_Send_Array(uint8_t *pData, uint16_t Size);
void UartRx_Treat(void);
void SenddatatoSWJ(void);
void RecidatatoSet(void);
void RecidatatoSet2(void);




#define  USART1_MAX_RECV_LEN 1024                   //最大接收缓存字节数
#define  USART1_MAX_SEND_LEN 1024                   //最大发送缓存字节数
#define USART1_RX_EN        1                       //0:不接收；1：接收

//extern u8 USART1_RX_BUF[USART1_MAX_RECV_LEN];       //接收缓存，最大USART1_MAX_RECV_LEN字节
//extern u8 USART1_TX_BUF[USART1_MAX_SEND_LEN];       //发送缓存，最大USART1_MAX_SEND_LEN字节
extern u16 USART1_RX_STA;                          //接收数据状态

extern UART_HandleTypeDef USART1_Handler;
extern UART_HandleTypeDef USART4_Handler;

extern uint8_t  LTEUartReceiveData;
extern uint8_t  LTEUartReceiveBuff[200];    //串口接收缓存

extern uint32_t LTEUartReceiveBuffIndex;
extern uint8_t  LTEUartRx_OK_Flag;
extern uint32_t LTEUartRx_Data_Length;
extern uint32_t LTEUartRx_Time_Tick;
extern uint8_t  LTEUartTx_OK_Flag;



extern uint8_t UartReceiveBuff[20];

extern uint8_t UartSendBuff[100];
extern uint8_t UartReceiveBuffIndex;


extern int Dev_Recv(uint8_t *buf, uint16_t timeout);
extern int Dev_Send(uint8_t *buf, uint16_t len);



#endif
