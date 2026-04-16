/*----------------------------------------------------------------------
  File Name          : XL355.h
  Author             : MPD Application Team
  Version            : V1.0
  Date               : 04/15/2015
  Description        : XL355 Registers and bits values
  File ID            : $Id: xl355.h,v 1.1.1.1 2015/04/15$

  Analog Devices ADXL355 digital output accelerometer 
  with advanced digital features.

  Based on preliminary data sheet PreB V5

  (c) 2015 Analog Devices application support team.
  neil.zhao@analog.com

  ----------------------------------------------------------------------

  The present firmware which is for guidance only aims at providing
  customers with coding information regarding their products in order
  for them to save time.  As a result, Analog Devices shall not be
  held liable for any direct, indirect or consequential damages with
  respect to any claims arising from the content of such firmware and/or
  the use made by customers of the coding information contained herein
  in connection with their products.

----------------------------------------------------------------------*/


#ifndef ADXL355_H_
#define ADXL355_H_

#include "main.h"

/* ------- Register names ------- */
#define XL355_DEVID_AD              0x00
#define XL355_DEVID_MST             0x01
#define XL355_PARTID                0x02
#define XL355_REVID                 0x03
#define XL355_STATUS                0x04
#define XL355_FIFO_ENTRIES          0x05
#define XL355_TEMP2                 0x06
#define XL355_TEMP1                 0x07
#define XL355_XDATA3                0x08
#define XL355_XDATA2                0x09
#define XL355_XDATA1                0x0A
#define XL355_YDATA3                0x0B
#define XL355_YDATA2                0x0C
#define XL355_YDATA1                0x0D
#define XL355_ZDATA3                0x0E
#define XL355_ZDATA2                0x0F
#define XL355_ZDATA1                0x10
#define XL355_FIFO_DATA             0x11
#define XL355_OFFSET_X_H            0x1E
#define XL355_OFFSET_X_L            0x1F
#define XL355_OFFSET_Y_H            0x20
#define XL355_OFFSET_Y_L            0x21
#define XL355_OFFSET_Z_H            0x22
#define XL355_OFFSET_Z_L            0x23
#define XL355_ACT_EN                0x24
#define XL355_ACT_THRESH_H          0x25
#define XL355_ACT_THRESH_L          0x26
#define XL355_ACT_COUNT             0x27
#define XL355_FILTER                0x28
#define XL355_FIFO_SAMPLES          0x29
#define XL355_INT_MAP               0x2A
#define XL355_SYNC                  0x2B
#define XL355_RANGE                 0x2C
#define XL355_POWER_CTL             0x2D
#define XL355_SELF_TEST             0x2E
#define XL355_RESET                 0x2F


//用户查询命令
#define ORDER_WRITE                 0x1
#define ORDER_READ                  0X0

#define ORDER_DEVID_AD              0x00
#define ORDER_DEVID_MST             0x01
#define ORDER_PARTID                0x02
#define ORDER_REVID                 0x03
#define ORDER_STATUS                0x04
#define ORDER_FILTER                0x05
#define ORDER_RANGE                 0x06
#define ORDER_SELF_TEST             0x07
#define ORDER_RESET                 0x08



/* Accelerometer write command */
#define ADXL355_WRITE         0x0

/* Accelerometer read command */
#define ADXL355_READ          0x1


#define       ADXL355_CS           GPIO_PIN_4  //PA_4
#define       ADXL355_SCK          GPIO_PIN_5  //PA_5
#define       ADXL355_MISO         GPIO_PIN_6  //PA_6
#define       ADXL355_MOSI         GPIO_PIN_7  //PA_7
#define       ADXL355_CS_L         HAL_GPIO_WritePin(GPIOA, ADXL355_CS, GPIO_PIN_RESET)
#define       ADXL355_CS_H         HAL_GPIO_WritePin(GPIOA, ADXL355_CS, GPIO_PIN_SET)


typedef enum {
   SPI_READ_ONE_REG = 1,            /* Read one ACC register */
   SPI_READ_TWO_REG,                /* Read two ACC registers */
   SPI_READ_THREE_REG,              /* Read X,Y,Z ACC registers */

} enRegsNum;



struct ADXL_Reg
{
    U8    u8_DEVID_AD;
    U8    u8_DEVID_MST;
    U8    u8_PARTID;
    U8    u8_REVID;
    U8    u8_Status;
    U8    u8_FIFO_ENTRIES;
    U16   u16_TEMP2;
    U8    u8_TEMP1;
    U8    u8_XDATA3;
    U8    u8_XDATA2;
    U8    u8_XDATA1;
    U8    u8_YDATA3;
    U8    u8_YDATA2;
    U8    u8_YDATA1;
    U8    u8_ZDATA3;
    U8    u8_ZDATA2;
    U8    u8_ZDATA1;
    U8    u8_FIFO_DATA;
    U8    u8_OFFSET_X_H;
    U8    u8_OFFSET_X_L;
    U8    u8_OFFSET_Y_H;
    U8    u8_OFFSET_Y_L;
    U8    u8_OFFSET_Z_H;
    U8    u8_OFFSET_Z_L;
    U8    u8_ACT_EN;
    U8    u8_ACT_THRESH_H;
    U8    u8_ACT_THRESH_L;
    U8    u8_ACT_COUNT;
    U8    u8_Filter;
    U8    u8_FIFO_SAMPLES;
    U8    u8_INT_MAP;
    U8    u8_Sync;
    U8    u8_Range;
    U8    u8_POWER_CTL;
    U8    u8_SELF_TEST;
    U8    u8_Reset;

    U32   u32_ReadBuffer;

    I32   i32_X_Accl_DATA;
    I32   i32_Y_Accl_DATA;
    I32   i32_Z_Accl_DATA;

};
typedef union 
{
    U16                     all;
    struct ADXL_Reg   ADXLDATA;          // 

}ADXL355_REGS; 



//extern s32 MEMS_Buffer[50][3];          //用于缓存的数据
extern u8 W_data[9];
extern u8 R_data[9];


//u16 ADXL355_STATUS();
extern u16  ADXL355_ReadID(void);
extern void ADXL355_POWER_ON(void);
extern void ADXL355_RESET(void);
extern void SPI_ADXL355_S_WRITE(u8 adress, u8 data) ;
extern void SPI_ADXL355_M_WRITE(u8 adress, u8 TXCONT);
extern u8   SPI_ADXL355_S_READ(u8 adress);
extern U32  SPI_ADXL355_M_READ(u8 adress, enRegsNum enRegs);
extern void ADXL355_Init(void);

extern ADXL355_REGS sXD355_DATA;

extern void ADXL355_Check(U16 RW,U16 Order,U16 Data);
extern uint8_t sADXL355_THRESH_H;
extern uint8_t sADXL355_THRESH_L;
void  ADXL355_SPI_Init(void);








#endif 

