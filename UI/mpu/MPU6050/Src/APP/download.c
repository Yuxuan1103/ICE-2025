/*********************************************************************************************************
**
**    Project:  BootLoader
**
**    Componet: STM32L071
**	
**    Filename: download.c
**	
**    Abstract: 以Ymodem的通信协议为基础（对协议稍有调整），完成应用程序的程序下载和烧写功能。
**	                                                        
**    Compiler: Keil 5.15
**           
**    Function List：  
**	
**        1.uint8_t ProgramDataDownload(void)
**
**    History：
**	
**        <auther>    <date>    <version>   <desc>
**		
**          凡文    2020/01/19     1.0       规范该文件的排版
**   
*************************************************************************************************************/

/* 包含头文件 *****************************************************************/

#include "common.h"

/* 变量声明 ------------------------------------------------------------------*/
uint8_t tab_1024[1024] =
{
    0
};

/*****************************************************************************************
Function Name       :  ProgramDataDownload
Description         :  完成文件下载接收文件存储功能，并输出结果 
Input               :  无
Output              :  接收的结果
	                     0: 完成
                       1: 接收超时
                       2: 撤销传输
                       3: 数据包错误（帧错误）
                       4: 帧序号验证错误
                       5: CRC帧校验错误
											 6: 数据包序号错误
                       7: 文件过大
                       8: Flash擦除失败
                       9: Flas数据校验失败		
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t ProgramDataDownload(void)
{
    int32_t Size = 0;
		Size = Ymodem_Receive_file(&tab_1024[0], file_name, file_size);
		HAL_Delay(1000);
    if (Size > 0)
    {
				printf( "Download Complete!\r\n");
				return 0;
    }
		else if (Size == -9) //Flash 数据验证错误
    {
        //printf("Flash data validation failed!\r\n");
				Receiver_Return(function_code, target_code, 0x90,  0x00, 0);
				return 1;
    }
		else if (Size == -8) //Flash 数据擦除错误   (存在风险,文件信息区域)
    {
        //printf("Flash erase failed!\r\n");
				Receiver_Return(function_code, target_code, 0x80,  0x00, 0);
				return 1;
    }
		else if (Size == -7) //文件过大
    {
        //printf("The image size is higher than the allowed space memory!\r\n");
				Receiver_Return(function_code, target_code, 0x70,  0x00, 0);
				return 1;
    }
    else if (Size == -6) //通信数据序列错误
    {
				//printf("The communication serial number is error!\r\n");
				Receiver_Return(function_code, target_code, 0x60,  0x00, 0);
				return 1;
    }
    else if (Size == -5) //CRC校验失败
    {
        //printf("Data validation error.\r\n");
				Receiver_Return(function_code, target_code, 0x50,  0x00, 0);
				return 1;
    }
		else if (Size == -4) //帧序号验证错误
    {
        //printf("Index check error.\r\n");
				
				return 1;
    }
		else if (Size == -3) //通信错误无关通信
    {
        //printf("error.\r\n");
				Receiver_Return(function_code, target_code, 0x40,  0x00, 0);
				return 1;
    }
		else if (Size == -2) //传输撤销
    {
        printf("Aborted by user.\r\n");
				return 1;
    } 
		else if (Size == -1) //接收超时
    {
       //printf("Time Out.\r\n");
			 Receiver_Return(function_code, target_code, 0x30,  0x00, 0);
			 return 1;
    }
		else
			return 2;
}



/*******************************文件结束***************************************/
