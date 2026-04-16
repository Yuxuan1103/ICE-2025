/*********************************************************************************************************
**
**    Project:  BootLoader
**
**    Componet: STM32L071
**	
**    Filename: ymodem.c
**	
**    Abstract: 以Ymodem的通信协议为基础（对协议稍有调整），完成应用程序的程序下载和烧写功能。
**	                                                        
**    Compiler: Keil 5.15
**           
**    Function List：  
**	
**        1.uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
**        
**        2.uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
**	
**        3.void Lora_Send_Ymodem (uint8_t *data,uint16_t length) 
**        
**        4.uint8_t Lora_Receive_Ymodem (uint8_t *receivedata, uint32_t waittime)
**	
**        5.uint8_t Ymodem_Receive_Packet (uint8_t *databuff, int32_t *length, uint32_t timeout) 
**        
**        6.void Ymodem_ReturnNAK(uint16_t ReceivingIndexes)
**	
**        7.uint8_t Ymodem_ErrorHandle(uint8_t sessionbegin, uint8_t *errors_times, uint16_t ReceivingIndexes) 
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

/* 变量声明 -----------------------------------------------------------------*/
uint8_t  device_code[6] = {0x53,0x52,0x59,0x4E,0x30,0x32};//装置码 123456(ID)
uint8_t  ciphertext[4] = {0x36,0x36,0x36,0x46};//密文验证码 1234
uint8_t  start_code = 0x68;  //起始码
uint8_t  end_code = 0x16;//结束码
uint8_t  control_code = 0x47;//控制码
uint8_t  function_code = 0x0B;//功能识别码（切斜监测终端）,需要初始化此处是默认值
uint8_t  target_code = 0xFF;  //设备标号（所有终端）,需要初始化此处是默认值


uint8_t file_name[FILE_NAME_LENGTH];//文件名
uint8_t file_size[FILE_SIZE_LENGTH];//文件大小

uint16_t PageSize = PAGE_SIZE;
uint32_t NbrOfPage = 0;
HAL_StatusTypeDef FLASHStatus = HAL_OK;
uint32_t RamSource;
extern uint8_t tab_1024[1024];
/*------------------------------------------------------------------------*/
/*| 起始码 | 装置号码 | 控制字  | 数据长度 | 数据域   |   校验   | 结束码 |*/
/*| 0x68   | 6个字节  | 0x47   | 2个字节  |  0x91    |  1字节  |   0x16  |*/
/*-------------------------------------------------------------------------*/


/*****************************************************************************************
Function Name       :  Cal_CRC16
Description         :  对字节数组进行ＣＲＣ校验
Input               :  data：待校验的字节数组
	                     size：数组要校验的长度
Output              :  返回CRC校验值
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
    uint32_t crc = 0xffff;
    uint32_t n, TT;
		uint8_t i;
		uint16_t crc16;
    for (n = 0; n < size; n++)
    {
        crc = crc ^ data[n];
        for (i = 0; i < 8; i++)
        {
            TT = crc & 1;
            crc = crc >> 1;
            crc = crc & 0x7fff;
            if (TT == 1)
            {
                crc = crc ^ 0xa001;
            }
                crc = crc & 0xffff;
         }
    }
		crc16 = crc;
    return crc16;
}

/*****************************************************************************************
Function Name       :  Sum8
Description         :  对字节数组进行求和校验（最终值保留低8位，再取反）
Input               :  data：待校验的字节数组
	                     size：数组要校验的长度
Output              :  返回校验值
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t Sum8(const uint8_t* data, uint32_t size)
{
		uint32_t   i ;
		uint32_t  sum = 0;
		
		for(i = 0; i < size; i++)
		{
				sum += data[i];
		}
	  sum = sum & 0xff;
    return ~sum ;
}
/*****************************************************************************************
Function Name       :  Send_Ymodem_4G
Description         :  4G发送Ymodem协议帧 
Input               :    data：待发送的字节数组
	                     length：数组要发送的长度
Output              :  无
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
void Send_Ymodem_4G (uint8_t *data,uint16_t length)
{
		EC20_Tcp_Send_Hex(&EC20_obj, data, length);
}

/*****************************************************************************************
Function Name       :  Lora_Receive_Ymodem
Description         :  从发送端接收一帧Ymodem数据
Input               :  receivedata：数据暂存数组
	                        waittime：超时设置（ms）
Output              :  1：接收成功
											 0：接收超时
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t Receive_Ymodem_4G (uint8_t *receivedata, uint32_t waittime)// waittime * 200ms
{
		uint8_t len = 0;
	  memset(receivedata,0,200);//清除数组内容
		len = EC20_Tcp_Recv(&EC20_obj, receivedata, waittime);
		if(len > 0)
		{
				return 1;
		}
		return 0;
}

/*****************************************************************************************
Function Name       :  Ymodem_Receive_Packet
Description         :  从发送端接收一帧数据包 
Input               :  databuff：数据暂存数组
											   length：数据包长度
												timeout：超时设置（timeout * 200ms）
Output              :  接收的结果
                       0: 正常返回
                       1: 接收超时
                       2: 撤销传输
                       3: 数据包错误（帧错误）
                       4: 帧序号验证错误
                       5: CRC帧校验错误
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
uint8_t Ymodem_Receive_Packet (uint8_t *databuff, int32_t *length, uint32_t timeout)
{
		uint16_t packet_size, sum, crc;
    uint16_t c;
//	  uint8_t n;
    *length = 0;
	
		if(Receive_Ymodem_4G(databuff,timeout) == 0)
		{
				  return 1; //超时
		}
		
		/*帧头——装置号码——控制码——功能识别码——设备标号*/
		if((databuff[0] == start_code) && (databuff[1] == device_code[0]) && (databuff[2] == device_code[1])
			&& (databuff[3] == device_code[2]) && (databuff[4] == device_code[3]) && (databuff[5] == device_code[4])
	  	&& (databuff[6] == device_code[5]) && (databuff[7] == control_code) && (databuff[14] == function_code) && (databuff[15] == target_code))
		{
				c = (databuff[8]<< 8) + databuff[9];
				if(databuff[c + 11] == end_code) //帧尾
				{
						//crc = Cal_CRC16(databuff + 1, c + 4);
						sum = Sum8(databuff + 1, c + 9);
						if(sum != databuff[c + 10] ) //和验证
						{
								return 5;//校验错误
						}
						c = databuff[16];//数据内容第一个有效字节
				}
				else
				{
						c = 0xFF; //无关通信
				}
		}
		else
		{
				c = 0xFF; //无关通信
		}
		
		if((databuff[14] != function_code)||(databuff[15] != target_code))//不是中继网关程序数据
		{
				return 3;//帧错误无关通信
		}
		
    switch (c)
    {
		    case SOH:      //128字节数据包开始
            packet_size = PACKET_SIZE;
            break;
        case STX:      //1024字节的数据包开始  Lora模式不用
            packet_size = PACKET_1K_SIZE;
            break;
        case EOT:      //结束传输 (通信正常)
				    *length = 0 ;
            return 0; 
        case CaN:       //撤销传输
				    return 2;
        default:  //帧错误无关通信
            return 3;
    }
		
		crc = Cal_CRC16(databuff + 19, packet_size); 
		if(crc != ((databuff[packet_size + 19] << 8) + databuff[packet_size + 20])) //CRC验证
		{
				 return 5;
		}
    *length = packet_size;
    return 0;
}

/***************************************************************************************************************************
Function Name       :  Receiver_Return
Description         :  接收端回应功能函数
Input               :  functioncode: 功能识别码（0x0C：中继网关，0x0B：铁塔倾斜监测终端）
											 targetcode: 设备标号  （0x00：更新目标为中继网关，0xFF：更新目标为所有铁塔倾斜监测终端，0x01-0xFE：更新目标为对应终端ID）
								       FunctionReturnCode: 功能回应码  （对通信过程的各类回应）
                       Responseder: 回应装置  （0x00：中继网关，0xFF：监测终端）
											 ReceivingIndexes：帧序号  
Output              :  无
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
****************************************************************************************************************************/ 

void Receiver_Return(uint8_t functioncode, uint8_t targetcode,  uint8_t FunctionReturnCode,  uint8_t Responseder, uint16_t ReceivingIndexes)
{
	uint8_t Ymodem_SendBuf[18];
	Ymodem_SendBuf[0]  = start_code; //起始码
	Ymodem_SendBuf[1]  = device_code[0];//装置码
	Ymodem_SendBuf[2]  = device_code[1];
	Ymodem_SendBuf[3]  = device_code[2];
	Ymodem_SendBuf[4]  = device_code[3];
	Ymodem_SendBuf[5]  = device_code[4];
	Ymodem_SendBuf[6]  = device_code[5];
	Ymodem_SendBuf[7]  = control_code;//控制码
	Ymodem_SendBuf[8]  = 0x00;//数据长度
	Ymodem_SendBuf[9]  = 0x06;
	Ymodem_SendBuf[10] = functioncode;//功能识别码
	Ymodem_SendBuf[11] = targetcode;  //设备标号
	Ymodem_SendBuf[12] = FunctionReturnCode;  //Ymodem指令码((功能编码)
	Ymodem_SendBuf[13] = Responseder; //回应装置
	Ymodem_SendBuf[14] = ReceivingIndexes >> 8;//提取高8位
	Ymodem_SendBuf[15] = (uint8_t)ReceivingIndexes;//提取低8位
	Ymodem_SendBuf[16] = Sum8(Ymodem_SendBuf + 1, 15); //校验位
	Ymodem_SendBuf[17] = end_code;//结束码
	Send_Ymodem_4G (Ymodem_SendBuf,18);//上传
}

/***************************************************************************************************************************
Function Name       :  Ymodem_ErrorHandle
Description         :  对通信错误进行判断和处理
Input               :  sessionbegin:通信过程中标志
                       ReceivingIndexes: 应该接收的数据帧序号(ReceivingIndexes = 0时不表示应该接收的帧序号 )
                       errors_times：发生错误次数										 
Output              :  0：重试
                       1：退出本次程序更新通信
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
****************************************************************************************************************************/ 
uint8_t Ymodem_ErrorHandle(uint8_t sessionbegin, uint8_t *errors_times, uint16_t ReceivingIndexes)
{
		uint8_t errorstimes;
		if (sessionbegin > 0)  
		{
				errorstimes = *errors_times;
				errorstimes ++;
				*errors_times = errorstimes;
		  if (errorstimes > MAX_ERRORS)
			{
					HAL_Delay(10);
					//Lora_Send_Ymodem(Ymodem_CAN,9);//数据错误终止传输
					Receiver_Return(function_code, target_code, CaN, 0x00, 0);
//					HAL_Delay(10);
//					//Lora_Send_Ymodem(Ymodem_CAN,9);
//				  Receiver_Return(function_code, target_code, CaN, 0x00, 0);
					return 1;
			}
			else
			{
				//Ymodem_ReturnNAK(ReceivingIndexes);
				Receiver_Return(function_code, target_code, NAK, 0x00, ReceivingIndexes);
			}
		}
		else
	  {  
				HAL_Delay(10);
				//Lora_Send_Ymodem(Ymodem_CAN,9);//数据错误终止传输
				Receiver_Return(function_code, target_code, CaN, 0x00, 0);
//				HAL_Delay(10);
//				//Lora_Send_Ymodem(Ymodem_CAN,9);
//				Receiver_Return(function_code, target_code, CaN, 0x00, 0);
				return 1;
		}
		return 0;
}

/*****************************************************************************************
Function Name       :  Ymodem_Receive_file
Description         :  通过ymodem协议接收一个文件并将文件信息存入Flash
Input               :  buf：接收数据包暂存数组
											 filename：接收文件名字
											 filesize：接收文件大小
Output              :  接收的结果
	                     >0: 文件大小
                       -1: 接收超时
                       -2: 撤销传输
                       -3: 数据包错误（帧错误）
                       -4: 帧序号验证错误
                       -5: CRC帧校验错误
											 -6: 数据包序号错误
                       -7: 文件过大
                       -8: Flash擦除失败
                       -9: Flas数据校验失败											 
Author              :  
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
******************************************************************************************/ 
int32_t Ymodem_Receive_file(uint8_t *buf, uint8_t *filename, uint8_t *filesize)
{
	static uint8_t packet_data[200];//[PACKET_1K_SIZE];//使用 static 的原因是数组空间超过了堆栈大小
  	uint8_t *file_ptr, *buf_ptr;
    int32_t i, packet_length, session_done, file_done, packets_send, session_begin, size = 0;
	int16_t packets_received = 0;
	uint8_t EotNum = 0, errors = 0;
    int32_t FlashDestination1 = ApplicationAddress;//初始化Flash地址变量		
	for (session_done = 0, errors = 0, session_begin = 0; ;)
    {
			  //packets_received：接收帧序号；file_done：文件传输完成file_done=1；将buf_ptr指向buf地址。
        for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
        {
			switch (Ymodem_Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT))//一帧
            {
				case 0 ://通信正常
					if(packet_length != 0)//数据包
					{									
						packets_send = (packet_data[17] << 8) + packet_data[18];
						if (packets_send != packets_received) //帧序号不等于该接收到的数据包序号
						{
							printf("The communication serial number is error!\r\n");
							HAL_Delay(10);
							//Lora_Send_Ymodem(Ymodem_CAN,9);//数据包序号错误终止传输
							Receiver_Return(function_code, target_code, CaN, 0x00, 0);
							return -6;
						}
						else
						{
							errors = 0;
							if (packets_received == 0)//第一帧（文件信息数据帧）
							{
								//文件名信息
								if (packet_data[19] != 0)//数据包第一个字节值不为0 
								{
									//文件名数据包有效数据区域
									for (i = 0, file_ptr = packet_data + 19; (*file_ptr != 0x00) && (i < FILE_NAME_LENGTH);)
									{
										filename[i++] = *file_ptr++;  //文件名
									}
									filename[i++] = '\0';//写0作为分隔
									for (i = 0, file_ptr ++; (*file_ptr != 0x00) && (i < FILE_SIZE_LENGTH);)
									{
										filesize[i++] = *file_ptr++;  //数据大小
									}
									filesize[i++] = '\0';//写0作为分隔
									Str2Int(file_size, &size);//把数字字符串转换成数字
									//测试数据包是否过大
									if (size > (FLASH_SIZE_Program - 1))
									{
										printf("The image size is higher than the allowed space memory!\r\n");
										//结束
										HAL_Delay(10);																		
										Receiver_Return(function_code, target_code, CaN, 0x00, 0);
										return -7;  //文件过大
									}																	
										
									//计算需要擦除Flash的页
									NbrOfPage = FLASH_PagesMask(size);	
									FLASH_ErasePage(ApplicationAddress, NbrOfPage+1);//擦除APP存放空间(此处没有失败判断可能存在风险)
									//存入文件信息
									memcpy(buf_ptr, packet_data + 16, packet_length + 5);//将提取数据存入缓存(buf_ptr，地址等于buf)
									RamSource = (uint32_t)buf;//地址赋值													
									if(FlashProgramData(FlashDestination1, buf_ptr , packet_length + 5) == 1)
									{
										printf("Flash data validation failed!\r\n");
										HAL_Delay(10);
										//Lora_Send_Ymodem(Ymodem_CAN,9);//校验失败结束
										Receiver_Return(function_code, target_code, CaN, 0x00, 0);

										return -9;//数据校对，不相等撤销传输	
									}
									FlashDestination1 += (packet_length + 5);								
									HAL_Delay(10);
									packets_received = 1;
									Receiver_Return(function_code, target_code, ACK, 0x00, packets_received);
								}
								else //文件名数据包空，结束传输  （完成传输）
								{
									HAL_Delay(10);
									Receiver_Return(function_code, target_code, ACK, 0x00, 0);
									//Lora_Send_Ymodem(Ymodem_ACK,9);//认可回应应答
									file_done = 1;
									session_done = 1;
									break;
								}
							}                  
							else//文件内容数据包
							{
							  ////区别于程序更新，+5 存放的不光是程序数据还有标志，序号，校验
								memcpy(buf_ptr, packet_data + 16, packet_length + 5);//将提取数据存入缓存(buf_ptr，地址等于buf)
								RamSource = (uint32_t)buf;//地址赋值													
								if(FlashProgramData(FlashDestination1, buf_ptr , packet_length + 5) == 1)
								{
									printf("Flash data validation failed!\r\n");
									HAL_Delay(10);
									Receiver_Return(function_code, target_code, CaN, 0x00, 0);								
									return -9;//数据校对，不相等撤销传输	
								}
								FlashDestination1 += (packet_length + 5);
								packets_received ++;//接收帧序号加 1
								printf("Receiving:%d...\r\n",packets_received);
								HAL_Delay(10);
								Receiver_Return(function_code, target_code, ACK, 0x00, packets_received);
								//Lora_Send_Ymodem(Ymodem_ACK,9);//认可回应应答
							}
							//packets_received ++;//接收帧序号加 1
							session_begin = 1;//开始传输标志
						}
					}
					else 
						if(EotNum == 0)
						{
							HAL_Delay(10);
							Receiver_Return(function_code, target_code, NAK, 0x00, packets_received);
							//Ymodem_ReturnNAK(0);//不认可回应应答
							EotNum = 1;
						}
						else 
							if(EotNum > 0)
							{
								HAL_Delay(10);
								//Lora_Send_Ymodem(Ymodem_ACK,9);//认可回应应答
								packets_received = 0;
								EotNum = 0;
								Receiver_Return(function_code, target_code, ACK, 0x00, packets_received);
								//HAL_Delay(10);
								//Lora_Send_Ymodem(Ymodem_C,9);//请求文件传输
							}
				break;
				case 1://接收超时		
					printf("Time Out.\r\n");
					if(Ymodem_ErrorHandle(session_begin, &errors, packets_received) == 1)
					{
						return -1;
					}
				break;//撤销传输	
				case 2 ://撤销传输
					//HAL_Delay(10);
					//Lora_Send_Ymodem(Ymodem_ACK,9);//认可回应应答;//认可回应应答   可以不用
				return -2;//撤销传输	
				case 3 ://数据包错误（帧错误,无关通信）
					printf("error.\r\n");
					if(Ymodem_ErrorHandle(session_begin, &errors, packets_received) == 1)
					{
						return -3;
					}
					break;//撤销传输	
				case 4 ://帧序号验证（正反码）错误
					printf("Index check error.\r\n");
					if(Ymodem_ErrorHandle(session_begin, &errors, packets_received) == 1)
					{
						return -4;
					}
				break;//撤销传输	
				case 5 ://帧校验错误
					printf("Data validation error.\r\n");
					if(Ymodem_ErrorHandle(session_begin, &errors, packets_received) == 1)
					{
						return -5;
					}
					break;//撤销传输	
				default:
				break;
			}				
            if (file_done != 0)//结束传输(空文件)
            {
                break;
            }
        }
        if (session_done != 0)//完成传输
        {
            break;
        }
    }
    return size;//正常完成传输,返回文件大小
}


/*******************************文件结束***************************************/
