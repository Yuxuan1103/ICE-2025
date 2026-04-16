#ifndef __SX1278_H__
#define __SX1278_H__

#include "stm32l0xx_hal.h"
#include "stm32l0xx_it.h"


#define       SX1278_CS        GPIO_PIN_12    //PB_12
#define       SX1278_SCK       GPIO_PIN_5    //PA_5
#define       SX1278_MISO      GPIO_PIN_6    //PA_6
#define       SX1278_MOSI      GPIO_PIN_7    //PA_7
#define       SX1278_DIO4      GPIO_PIN_9    //PC_9
#define       SX1278_DIO2      GPIO_PIN_7    //PC_7
#define       SX1278_RESET     GPIO_PIN_12   //PA_12
#define       SX1278_DIO0      GPIO_PIN_6    //PC_6
#define       SX1278_DIO1      GPIO_PIN_8    //PC_8
#define       SX1278_DIO3      GPIO_PIN_8    //PA_8
#define       SX1278_DIO5      GPIO_PIN_11    //PA_11
//#define       SX1278_POWER     GPIO_PIN_1    //PA_4
 
#define       SX1278_RTSET_L     HAL_GPIO_WritePin(GPIOA, SX1278_RESET, GPIO_PIN_RESET)
#define       SX1278_RTSET_H     HAL_GPIO_WritePin(GPIOA, SX1278_RESET, GPIO_PIN_SET)
//#define       SX1278_TX_L        HAL_GPIO_WritePin(GPIOB, SX1278_TX, GPIO_PIN_RESET)
//#define       SX1278_TX_H        HAL_GPIO_WritePin(GPIOB, SX1278_TX, GPIO_PIN_SET)
//#define       SX1278_RX_L        HAL_GPIO_WritePin(GPIOB, SX1278_RX, GPIO_PIN_RESET)
//#define       SX1278_RX_H        HAL_GPIO_WritePin(GPIOB, SX1278_RX, GPIO_PIN_SET)
#define       SX1278_CS_L        HAL_GPIO_WritePin(GPIOB, SX1278_CS, GPIO_PIN_RESET)
#define       SX1278_CS_H        HAL_GPIO_WritePin(GPIOB, SX1278_CS, GPIO_PIN_SET)
#define       SX1278_SCK_L       HAL_GPIO_WritePin(GPIOB, SX1278_SCK, GPIO_PIN_RESET)
#define       SX1278_SCK_H       HAL_GPIO_WritePin(GPIOB, SX1278_SCK, GPIO_PIN_SET)
#define       SX1278_MOSI_L      HAL_GPIO_WritePin(GPIOB, SX1278_MOSI, GPIO_PIN_RESET)
#define       SX1278_MOSI_H      HAL_GPIO_WritePin(GPIOB, SX1278_MOSI, GPIO_PIN_SET)
//#define       SX1278_POWER_L     HAL_GPIO_WritePin(GPIOA, SX1278_POWER, GPIO_PIN_RESET)
//#define       SX1278_POWER_H     HAL_GPIO_WritePin(GPIOA, SX1278_POWER, GPIO_PIN_SET)


#define       SX1278_READ_MISO   HAL_GPIO_ReadPin(GPIOB, SX1278_MISO)
#define       SX1278_READ_DIO0   HAL_GPIO_ReadPin(GPIOC, SX1278_DIO0)
#define       SX1278_READ_DIO1   HAL_GPIO_ReadPin(GPIOC, SX1278_DIO1)
#define       SX1278_READ_DIO3   HAL_GPIO_ReadPin(GPIOA, SX1278_DIO3)
#define       SX1278_READ_DIO5   HAL_GPIO_ReadPin(GPIOA, SX1278_DIO5)


// LORA RF FREQ

#define LR_SET_FRFMSB    0x6C
#define LR_SET_FRFMID    0x40
#define LR_SET_FRFLSB    0x00



//LORA
#define REG_LR_FIFO                                 0x00 
// Common settings
#define REG_LR_OPMODE                               0x01 
#define REG_LR_BANDSETTING                          0x04
#define REG_LR_FRFMSB                               0x06 
#define REG_LR_FRFMID                               0x07
#define REG_LR_FRFLSB                               0x08 
// Tx settings
#define REG_LR_PACONFIG                             0x09 
#define REG_LR_PARAMP                               0x0A 
#define REG_LR_OCP                                  0x0B 
// Rx settings
#define REG_LR_LNA                                  0x0C 
// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0D 
#define REG_LR_FIFOTXBASEADDR                       0x0E 
#define REG_LR_FIFORXBASEADDR                       0x0F 
#define REG_LR_FIFORXCURRENTADDR                    0x10 
#define REG_LR_IRQFLAGSMASK                         0x11 
#define REG_LR_IRQFLAGS                             0x12 
#define REG_LR_NBRXBYTES                            0x13 
#define REG_LR_RXHEADERCNTVALUEMSB                  0x14 
#define REG_LR_RXHEADERCNTVALUELSB                  0x15 
#define REG_LR_RXPACKETCNTVALUEMSB                  0x16 
#define REG_LR_RXPACKETCNTVALUELSB                  0x17 
#define REG_LR_MODEMSTAT                            0x18 
#define REG_LR_PKTSNRVALUE                          0x19 
#define REG_LR_PKTRSSIVALUE                         0x1A 
#define REG_LR_RSSIVALUE                            0x1B 
#define REG_LR_HOPCHANNEL                           0x1C 
#define REG_LR_MODEMCONFIG1                         0x1D 
#define REG_LR_MODEMCONFIG2                         0x1E 
#define REG_LR_SYMBTIMEOUTLSB                       0x1F 
#define REG_LR_PREAMBLEMSB                          0x20 
#define REG_LR_PREAMBLELSB                          0x21 
#define REG_LR_PAYLOADLENGTH                        0x22 
#define REG_LR_PAYLOADMAXLENGTH                     0x23 
#define REG_LR_HOPPERIOD                            0x24 
#define REG_LR_FIFORXBYTEADDR                       0x25 
#define REG_LR_MODEMCONFIG3                         0x26 
#define REG_LR_FEIMSB                               0x28 
#define REG_LR_FEIMIB                               0x29 
#define REG_LR_FEILSB                               0x2A 
#define REG_LR_LORADETECTOPTIMIZE                   0x31 
#define REG_LR_INVERTIQ                             0x33 
#define REG_LR_DETECTIONTHRESHOLD                   0x37 
// end of documented register in datasheet
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40 
#define REG_LR_DIOMAPPING2                          0x41 
// Version
#define REG_LR_VERSION                              0x42 
// Additional settings
#define REG_LR_PLLHOP                               0x44 
#define REG_LR_TCXO                                 0x4B 
#define REG_LR_PADAC                                0x4D 
#define REG_LR_FORMERTEMP                           0x5B 
#define REG_LR_BITRATEFRAC                          0x5D 
#define REG_LR_AGCREF                               0x61 
#define REG_LR_AGCTHRESH1                           0x62 
#define REG_LR_AGCTHRESH2                           0x63 
#define REG_LR_AGCTHRESH3                           0x64 



//fsk??
#define REG_FIFO                                    0x00 
// Common settings
#define REG_OPMODE                                  0x01 
#define REG_BITRATEMSB                              0x02 
#define REG_BITRATELSB                              0x03 
#define REG_FDEVMSB                                 0x04 
#define REG_FDEVLSB                                 0x05 
#define REG_FRFMSB                                  0x06 
#define REG_FRFMID                                  0x07 
#define REG_FRFLSB                                  0x08 
// Tx settings
#define REG_PACONFIG                                0x09 
#define REG_PARAMP                                  0x0A 
#define REG_OCP                                     0x0B 
// Rx settings
#define REG_LNA                                     0x0C 
#define REG_RXCONFIG                                0x0D 
#define REG_RSSICONFIG                              0x0E 
#define REG_RSSICOLLISION                           0x0F 
#define REG_RSSITHRESH                              0x10 
#define REG_RSSIVALUE                               0x11 
#define REG_RXBW                                    0x12 
#define REG_AFCBW                                   0x13 
#define REG_OOKPEAK                                 0x14 
#define REG_OOKFIX                                  0x15 
#define REG_OOKAVG                                  0x16 
#define REG_RES17                                   0x17 
#define REG_RES18                                   0x18 
#define REG_RES19                                   0x19 
#define REG_AFCFEI                                  0x1A 
#define REG_AFCMSB                                  0x1B 
#define REG_AFCLSB                                  0x1C 
#define REG_FEIMSB                                  0x1D 
#define REG_FEILSB                                  0x1E 
#define REG_PREAMBLEDETECT                          0x1F 
#define REG_RXTIMEOUT1                              0x20 
#define REG_RXTIMEOUT2                              0x21 
#define REG_RXTIMEOUT3                              0x22 
#define REG_RXDELAY                                 0x23 
// Oscillator settings
#define REG_OSC                                     0x24
// Packet handler settings
#define REG_PREAMBLEMSB                             0x25
#define REG_PREAMBLELSB                             0x26
#define REG_SYNCCONFIG                              0x27
#define REG_SYNCVALUE1                              0x28
#define REG_SYNCVALUE2                              0x29
#define REG_SYNCVALUE3                              0x2A
#define REG_SYNCVALUE4                              0x2B
#define REG_SYNCVALUE5                              0x2C
#define REG_SYNCVALUE6                              0x2D
#define REG_SYNCVALUE7                              0x2E
#define REG_SYNCVALUE8                              0x2F
#define REG_PACKETCONFIG1                           0x30
#define REG_PACKETCONFIG2                           0x31
#define REG_PAYLOADLENGTH                           0x32
#define REG_NODEADRS                                0x33
#define REG_BROADCASTADRS                           0x34
#define REG_FIFOTHRESH                              0x35
// SM settings
#define REG_SEQCONFIG1                              0x36
#define REG_SEQCONFIG2                              0x37
#define REG_TIMERRESOL                              0x38
#define REG_TIMER1COEF                              0x39
#define REG_TIMER2COEF                              0x3A
// Service settings
#define REG_IMAGECAL                                0x3B
#define REG_TEMP                                    0x3C
#define REG_LOWBAT                                  0x3D
// Status
#define REG_IRQFLAGS1                               0x3E
#define REG_IRQFLAGS2                               0x3F
// I/O settings
#define REG_DIOMAPPING1                             0x40
#define REG_DIOMAPPING2                             0x41
// Version
#define REG_VERSION                                 0x42
// Additional settings
#define REG_PLLHOP                                  0x44
#define REG_TCXO                                    0x4B
#define REG_PADAC                                   0x4D
#define REG_FORMERTEMP                              0x5B
#define REG_BITRATEFRAC                             0x5D
#define REG_AGCREF                                  0x61
#define REG_AGCTHRESH1                              0x62
#define REG_AGCTHRESH2                              0x63
#define REG_AGCTHRESH3                              0x64



/*
#define sx1278_CSN     PAout(4)
#define sx1278_RXTX    PAout(3) 
#define DIO0 PCin(11)
#define DIO1 PCin(12)
#define DIO2 PCin(1)
#define DIO3 PCin(5)
#define DIO4 PCin(4)
#define v1    PAout(1)
#define v2    PAout(2) 
*/

#define SyncAdrError  0
#define PreambleError 1
#define RX_ERROR 2
#define RX_OK 3
#define TX_OK 0
#define empty 0
#define busy  1

extern void SX1278_LoraInit(void);
extern void SX1278_SetSleepMode(void);
extern void SX1278_ConfigTxMode(void);
extern void SX1278_ConfigRxMode(void);
extern void SX1278_ReadBuf(uint8_t RegAddr, uint8_t *ReadData, uint8_t len);
extern void SX1278_SendPacket(uint8_t *SendBuff, uint8_t len);
extern uint8_t SX1278_ReceivePacket(uint8_t *RFBuffer,uint8_t * lengthRX ,uint32_t waittime);
extern uint8_t SX1278_ReceivePacket1(uint8_t *RFBuffer, uint8_t* length);

//  LORA∑¢ÀÕ
//  SX1278_SetSleepMode();
//  SX1278_ConfigTxMode();
//  SX1278_SendPacket(eepromreaddata, 17);
//  LORAΩ” ’
//  SX1278_SetSleepMode();
//  SX1278_ConfigRxMode();
//  if(SX1278_ReceivePacket(eepromreaddata) == 1)
//  {
//      Uart_Send_Array(eepromreaddata,10);
//  }


#endif
