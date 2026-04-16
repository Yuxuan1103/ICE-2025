/*********************************************************************************************************
**
**    Project:  BootLoader
**
**    Componet: STM32L071
**	
**    Filename: common.c
**	
**    Abstract: Flash操作函数，完成应用程序的程序下载和烧写功能控制。
**	                                                        
**    Compiler: Keil 5.15
**           
**    Function List：  
**	
**        1.void Int2Str(uint8_t* str, int32_t intnum)
**        
**        2.uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
**	
**        3.uint32_t FLASH_PagesMask(__IO uint32_t Size)
**        
**        4.HAL_StatusTypeDef FLASH_ErasePage(uint32_t Page_Address, uint32_t Number_Pages)
**	
**        5.uint8_t FlashProgramData(uint32_t address, uint8_t* databuff , uint32_t size)
**        
**        6.uint8_t ReadFlashData(uint32_t address, uint8_t* databuff , uint32_t size)
**	
**        7.void Main_Menu(void)
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
#include "ymodem.h"
#include "terminal_program_transmission.h"
#include "main.h"
/* 变量 ----------------------------------------------------------------------*/

/*****************************************************************************************
Function Name       :  Int2Str
Description         :  整形数据转换成字符串
Input               :  str：字符串
                       intnum：整形数据
Output              :  无
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
void Int2Str(uint8_t* str, int32_t intnum)
{
    uint32_t i, Div = 1000000000, j = 0, Status = 0;

    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;

        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*****************************************************************************************
Function Name       :  Str2Int
Description         :  字符串转换成整形数据
Input               :  str：字符串
                       intnum：整形数据
Output              :  转换结果
                       1：正确
                       0：错误
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint32_t Str2Int(uint8_t *inputstr, int32_t *intnum)
{
    uint32_t i = 0, res = 0;
    uint32_t val = 0;

    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }

        if (i >= 11)
        {
            res = 0;
        }
    }
    else//最多10为2输入
    {
        for (i = 0; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                //返回1
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
            {
                val = val * 10 + CONVERTDEC(inputstr[i]);
            }
            else
            {
                //无效输入返回0
                res = 0;
                break;
            }
        }
        //超过10位无效，返回0
        if (i >= 11)
        {
            res = 0;
        }
    }

    return res;
}

/*****************************************************************************************
Function Name       :  FLASH_PagesMask
Description         :  计算文件所需要占用的Flash页数
Input               :  Size：文件大小
Output              :  pagenumber：页数
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint32_t FLASH_PagesMask(__IO uint32_t Size)
{
    uint32_t pagenumber = 0x0;
    uint32_t size = Size;

    if ((size % PAGE_SIZE) != 0)
    {
        pagenumber = (size / PAGE_SIZE) + 1;
    }
    else
    {
        pagenumber = size / PAGE_SIZE;
    }
    return pagenumber;
}

/*****************************************************************************************
Function Name       :  FLASH_ErasePage
Description         :  擦除指定 页 内容
Input               :  Page_Address：指定页起始地址
											 Number_Pages：擦除页数 (4K每页)
Output              :   无     （没有擦除失败判断可能存在风险）
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/

void FLASH_ErasePage(uint32_t Page_Address, uint32_t Number_Pages)
{ 
	uint8_t n;
	uint32_t Address = 0;
	for(n = 0; n < Number_Pages; n++)
	{
	Address = Page_Address + n * PAGE_SIZE;
	W25QXX_Erase_Sector(Address);//W25擦除一个扇区
	}
}

/*****************************************************************************************
Function Name       :  FlashProgramData
Description         :  将字节数据数组写到Flash指定一个地址块
Input               :   address：目标块起始地址
											 databuff：字节数据数组
											     size：数组长度(小于150)
Output              :  0：写入成功
                       1：Flash 数据校对失败
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/

uint8_t FlashProgramData(uint32_t address, uint8_t* databuff , uint32_t size)
{
	uint32_t i;
	uint8_t pbuffer[150];

	W25QXX_Write(databuff,address,size);//W25写数据
	HAL_Delay(10);
	W25QXX_Read(pbuffer,address,size);  //W25读数据
	//	  HAL_UART_Transmit(&huart2, pbuffer,size,50);
	for(i = 0; i < size; i++)
	{
		if(databuff[i] != pbuffer[i])
		{
			return 1;//数据校验失败
		}
	}

	return 0;
}



/*****************************************************************************************
Function Name       :  ReadFileInformation
Description         :  读取文件信息
Input               :  address：起始地址
											 databuff：字节数据数组
											     size：数组长度
Output              :  j：读取数据有效长度
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/
uint8_t ReadFileInformation(uint32_t address, uint8_t* databuff , uint32_t size)
{
	uint8_t i;
	memset( databuff, 0, size );//将接收数组rfbuffer清空
	W25QXX_Read(databuff,address,size);  //W25读数据
	for(i = 0;i < size; i++)
	{
	  if(databuff[i] == 0x00)
		break;
	}
	return i;
}

/*****************************************************************************************
Function Name       :  TerminalProgramReceiveSaveTask
Description         :  终端程序接收和保存任务
Input               :  functioncode：功能识别码（终端类型，0x0B:倾斜监测终端）
                       targetcode：设置设备标号 (0x01-0xFE:终端ID，0xFF：所有终端)
Output              :  发送的结果
                       0: 程序下载失败
                       file_size: 接收到文件的大小（值必须大于0）
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint32_t TerminalProgramReceiveSaveTask(int8_t  functioncode,uint8_t  targetcode)
{
	uint8_t file_reseve_flag = 0;
	uint8_t C_Times = 0;
	uint8_t n = 0;
	uint8_t UartSendBuff[150];
	int32_t file_size;
	uint8_t uStatus = 0;
    while (1)
    {
		switch(uStatus)
		{
			case 0:
				printf("\r*********************************************************\r\n");
				printf("I need to receive a APP file.\r\n");
				printf( "*********************************************************\r\n");
				printf( "Start receiving a file......\r\n");
					 
				function_code = functioncode; //设置功能识别码(传感器类型)
				target_code = targetcode;     //设置设备标号(ID)		 
				uStatus = 1;
			break;
			case 1:			
				Receiver_Return(function_code, target_code, C,  0x00, 0);//请求数据传送
				file_reseve_flag = ProgramDataDownload();//ProgramDataDownLoad
				if(file_reseve_flag == 0)
				{		
					printf( "File Name: ");
					n = ReadFileInformation(APP_Saddr + 3, UartSendBuff , FILE_NAME_LENGTH);//读取文件名字
					Uart_Send_Array(UartSendBuff, n);
					printf( "\r\n");

					printf( "File Size: ");
					n = ReadFileInformation(APP_Saddr + 3 + n + 1, UartSendBuff , FILE_NAME_LENGTH);//读取文件大小
					Uart_Send_Array(UartSendBuff, n);
					printf( " Bytes\r\n");
					Str2Int(UartSendBuff, &file_size);
					uStatus = 2;  //程序下载成功
				}
				C_Times ++;
				if(C_Times>=10)
				{
					C_Times = 0;
					Receiver_Return(function_code, target_code, CaN, 0x00, 0);//取消传输
					uStatus = 3; //程序下载失败
				}
				HAL_Delay(1000);//1秒
			break;
			default:
			break;
		}  
		if(uStatus == 2)
		{
			//Receiver_Return(function_code, target_code, 0x20,  0x00, 0);//终端程序下载完成
			printf( "APP file downloaded successfully!\r\n");
			return file_size;//更新成功,返回文件大小
		}
		else 
			if(uStatus == 3)
			{
				printf( "APP file downloaded failed!\r\n");
				return  0;//更新失败
			}
     }
}

/*****************************************************************************************
Function Name       :  TerminalProgramUpdateTask
Description         :  终端程序更新任务
Input               :             gateway_id：网关ID
								  functioncode：功能识别码（终端类型，0x0B:倾斜监测终端）
                                  terminal_id：终端ID
								  program_start_address：程序文件存放起始地址
							      file_size: 文件大小（Byte）
Output              :  发送的结果
                       0: 程序更新成功
                       1: 失败
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t TerminalProgramUpdateTask(uint8_t gateway_id, uint8_t functioncode, uint8_t terminal_id, uint32_t program_start_address, uint32_t file_size)
{
	uint8_t  n = 0;
	uint8_t  flag = 0;
	uint8_t  times = 0;
	uint8_t  UartSendBuff[150];
	uint16_t PackageSerialNumberMax;

	TerminalID  = terminal_id;  //终端ID
	GatewayID   = gateway_id;   //网关ID
	ControlCode = functioncode; //控制码

	printf( "File Name: ");
	n = ReadFileInformation(APP_Saddr + 3, UartSendBuff , FILE_NAME_LENGTH);//读取文件名字
	Uart_Send_Array(UartSendBuff, n);
	printf( "\r\n");
					 
	printf( "File Size: ");
	n = ReadFileInformation(APP_Saddr + 3 + n + 1, UartSendBuff , FILE_NAME_LENGTH);//读取文件大小
	Uart_Send_Array(UartSendBuff, n);
	printf( " Bytes\r\n");

	if((file_size % 128) > 0)
	{
		PackageSerialNumberMax = file_size/128 + 1;
	}
	else
	{
		PackageSerialNumberMax = file_size/128;
	}

	while(1)
	{		 
		flag = TerminalProgramDataSendControl (10000, PackageSerialNumberMax, program_start_address);
		if(flag > 0)
		{
			times++;
			printf( "Fail: %d ,flag = %d\r\n",times,flag);
		}
		else
		{
			return 0;//更新成功
		}
		if(times >= 10)
		{ 
			printf( "Go out! \r\n");
			return 1;//更新失败
		}
	}
	  
}

/*****************************************************************************************
Function Name       :  TerminalProgramUpdateControlTask
Description         :  终端程序更新控制功能函数（对程序文件进行接收、保存和下发程序到终端）
Input               :             gateway_id：网关ID
											          functioncode：功能识别码（终端类型，0x0B:倾斜监测终端）
                                 terminal_id：终端ID
Output              :  发送的结果
                       0: 失败
                       1: 成功
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t TerminalProgramUpdateControlTask(uint8_t gateway_id, uint8_t functioncode, uint8_t terminal_id)
{
	uint32_t program_start_address;
	uint32_t file_size;
	uint8_t  updata_flag;

	program_start_address = ApplicationAddress;//文件起始地址

	file_size = TerminalProgramReceiveSaveTask(functioncode, terminal_id);//文件接收和存储，file_size：文件大小(Byte)
	//file_size = 36884;

	if(file_size > 0)
	{
		updata_flag = TerminalProgramUpdateTask(gateway_id, functioncode, terminal_id, program_start_address, file_size);//对终端进行更新
		if(updata_flag == 1)
		{
			return 1;//失败，终端程序更新过程出错
		}
		else
		{
			return 0;//成功
		}
	}
	else
	{
		return 2;//失败，文件接收存储过程出错
	}
}

/*******************************文件结束***************************************/
