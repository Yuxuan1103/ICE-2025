
/*********************************************************************************************************
**
**    Project:  BootLoader
**
**    Componet: STM32L071
**	
**    Filename: common.h
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


/* 防止重定义 ----------------------------------------------------------------*/
#ifndef _COMMON_H
#define _COMMON_H

/* 包含头文件 *****************************************************************/
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "ymodem.h"
//#include "w25qxx.h"


/* 类型声明 -----------------------------------------------------------------*/
typedef  void (*pFunction)(void);

////* 宏 ------------------------------------------------------------------------*/
//#define CMD_STRING_SIZE      128

#define ApplicationAddress     APP_Saddr//APP_Saddr = 0x530000 引用程序存放起始地址//0x8005000      //  bootloader 最大不能超过   20k
   
#define PAGE_SIZE              0x1000     //4k( 0x1000) FLASH_PAGE_SIZE// ((uint32_t)0x800)        /* 页大小2048Bytes */

#define FLASH_SIZE_Program     (0x19000) //程序大小小于100k      /*应用程序最大空间 512 - 20  = 492KBytes */

#define IS_AF(c)               ((c >= 'A') && (c <= 'F'))
#define IS_af(c)               ((c >= 'a') && (c <= 'f'))
#define IS_09(c)               ((c >= '0') && (c <= '9'))
#define ISVALIDHEX(c)          IS_AF(c) || IS_af(c) || IS_09(c)
#define ISVALIDDEC(c)          IS_09(c)
#define CONVERTDEC(c)          (c - '0')

#define CONVERTHEX_alpha(c)    (IS_AF(c) ? (c - 'A'+10) : (c - 'a'+10))
#define CONVERTHEX(c)          (IS_09(c) ? (c - '0') : CONVERTHEX_alpha(c))


/* 函数声明 ------------------------------------------------------------------*/
void Int2Str(uint8_t* str,int32_t intnum);
uint32_t Str2Int(uint8_t *inputstr,int32_t *intnum);
uint32_t FLASH_PagesMask(__IO uint32_t Size);
uint8_t ProgramDataDownload(void);
void FLASH_ErasePage(uint32_t Page_Address, uint32_t Number_Pages);
uint8_t FlashProgramData(uint32_t address, uint8_t* databuff , uint32_t size);
uint8_t TerminalProgramUpdateControlTask(uint8_t gateway_id, uint8_t functioncode, uint8_t terminal_id);

uint32_t TerminalProgramReceiveSaveTask(int8_t  functioncode,uint8_t  targetcode);
uint8_t TerminalProgramUpdateTask(uint8_t gateway_id, uint8_t functioncode, uint8_t terminal_id, uint32_t program_start_address, uint32_t file_size);

#endif  /* _COMMON_H */

/*******************************文件结束***************************************/
