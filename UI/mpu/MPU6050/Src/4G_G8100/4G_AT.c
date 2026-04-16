#include "main.h"
#include "string.h"

const char *URCPORT = "urcport";				
const char *URCPORTConfig = "uart1";		
const char *APREADY = "apready";
const uint8_t Buffer_access_mode = 0;
const uint8_t Direct_push_mode = 1;
const uint8_t Transparent_access_mode = 2;
uint8_t Rssi; //信号强度0-199

/****
	*@功能： 执行一个AT指令
	*@参数： Obj: pointer to module handle
	*@参数： cmd: pointer to command string
	*@参数： pdata: pointer to returned data
	*@返回： 运行状态
*/
AT_4G_Status_t AT_CMD_SEND(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;

	ret = obj->fops.dev_send(cmd, strlen((char *)cmd));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata,(const char *) pdata))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
 				return AT_4G_STATUS_ERR0R;
			}
		}
	}	
	return AT_4G_STATUS_DEV_ERR0R;
}

/****
	*@功能： 执行一个AT指令
	*@参数： Obj: pointer to module handle
	*@参数： cmd: pointer to command string
	*@参数： pdata: pointer to returned data
	*@返回： 运行状态
*/
static AT_4G_Status_t AT_CMD_Execute(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;

	ret = obj->fops.dev_send(cmd, strlen((char *)cmd));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, OK))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
 				return AT_4G_STATUS_ERR0R;
			}
		}
	}	
	return AT_4G_STATUS_DEV_ERR0R;
}




AT_4G_Status_t AT_CIPSEND_Execute(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;

	ret = obj->fops.dev_send(cmd, strlen((char *)cmd));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, ">"))
				{
					return AT_4G_STATUS_OK;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
 				return AT_4G_STATUS_ERR0R;
			}
		}
	}	
	return AT_4G_STATUS_DEV_ERR0R;
}


/****
	*@功能： 等待开机完成信息
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Rdy(AT_4G_Object_t *obj)
{
	int len;
	len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
	if((len > 0) && (len < AT_4G_DATA_SIZE ))
	{
		*(obj->cmddata+len) = 0;
		if(strstr((char *)obj->cmddata, "RDY"))
		{
			return AT_4G_STATUS_OK;
		}
		else
		{
			return AT_4G_STATUS_ERR0R;
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}
/****
	*@功能： 开关回显功能
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:turn 0ff 1:turn on
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_At(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT\r\n");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}


/****
	*@功能： 开关回显功能
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:turn 0ff 1:turn on
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Ate(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "ATE%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置功能级别
	*@参数： Obj: pointer to module handle
	*@参数： Func: 0:Minimum functionality  1:Full functionality (Default) 
	*@*****  4:Disable the ME from both transmitting and receiving RF signals
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Cfun(AT_4G_Object_t *obj, uint8_t Func)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CFUN=%d\r\n", Func);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 错误消息格式
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Disable result code 
	*@*****  1：Enable result code and use numeric values
	*@*****  2：Enable result code and use verbose values
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Cmee(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CMEE=%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置本地串口数据流控制
	*@参数： Obj: pointer to module handle
	*@参数： dce_by_dte: 0 Non 2 RTS flow control
    *@参数： dce_by_dte: 0 Non 2 CTS flow control                    
	*@返回： 运行状态
*/
AT_4G_Status_t  AT_4G_Ifc(AT_4G_Object_t *obj, uint8_t dce_by_dte, uint8_t dte_by_dce)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+IFC=%d,%d\r\n", dce_by_dte, dte_by_dce);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置DTR功能模式
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:TA ignores status on DTR
	*@***:   1：Low->High on DTR Change to command mode while remaining the connected call.
	*@***:   2: Disconnect data call, and change to command mode. When DTR is at high level, 
	*@***:		auto-answer function is disabled
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Dtr(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT&D%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置DCD功能模式
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:DCD function is always ON
	*@***:   1：DCD function is ON only in the presence of data carrier
*/

AT_4G_Status_t AT_4G_Dcd(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT&C%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置TE_TA的字符侦格式和奇偶校验
	*@参数： Obj: pointer to module handle
	*@参数： format: 3 8 data 0 parity 1 stop
    *@参数： parity: 9600,19200,38400,57600,115200,230400,460800,921600m    
	*@返回： 运行状态
*/
AT_4G_Status_t  AT_4G_Icf(AT_4G_Object_t *obj, uint8_t format, uint8_t parity)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+ICF=%d,%d\r\n", format, parity);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置本地波特率
	*@参数： Obj: pointer to module handle
	*@参数： Baud_Rate: 0:DCD function is always ON
	*@***:   1：DCD function is ON only in the presence of data carrier
*/

AT_4G_Status_t AT_4G_Ipr(AT_4G_Object_t *obj, uint32_t Baud_Rate)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+IPR=%d\r\n", Baud_Rate);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 信号质量报告
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Csq(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret;
	sprintf((char *)obj->cmddata, "AT+CSQ\r\n");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	return ret;
}
//获取信号强度，放在心跳包
AT_4G_Status_t AT_4G_Csq_HeartBeat(AT_4G_Object_t *obj)
{
	uint8_t i = 0;
	uint8_t buff[3];
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CSQ\r\n");
	AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	memset(buff,0,sizeof(buff));
	while(*(obj->cmddata+i) != 0x3A)    //0x3A ascii ":"
	{
		++i;
		if(i > 30)
			{
				i = 0;
				Rssi = 0xFF;
				return ret;
			}
	}
		
	buff[0] = *(obj->cmddata+i+1);
	buff[1] = *(obj->cmddata+i+2);
	buff[2] = *(obj->cmddata+i+3);
	if(buff[0] == 0x20 && buff[1] != 0x20)
		Rssi = (buff[1] - 0x30)*10 + (buff[2] - 0x30);
	else if(buff[0] != 0x20 && buff[1] != 0x20)
		Rssi = (buff[0] - 0x30)*100 + (buff[1] - 0x30)*10 + (buff[2] - 0x30);
	else if(buff[0] == 0x20 && buff[1] == 0x20)
		Rssi = buff[2] - 0x30;
	else
		Rssi = 0xFF;
	return ret;
}

/****
	*@功能： 读取PIN
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Cpin(AT_4G_Object_t *obj)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "AT+CPIN?\r\n" );
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, "READY"))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}


/****
	*@功能： 查询网络状态
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Qnwinfo(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QNWINFO\r\n" );
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 从指令模式转为数据模式
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Switch from command mode to data mode
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Ato(AT_4G_Object_t *obj, uint8_t switch_num)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "ATO%d\r\n", switch_num );
	
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, "CONNECT"))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, "NO CARRIER"))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}

/****
	*@功能： 设置网络
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Disable network registration unsolicited result code
	*@***:   1：Enable network registration unsolicited result code
	*@***:   2：Enable network registration and location information unsolicited result code
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Cgreg(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CGREG=%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

AT_4G_Status_t AT_4G_Cgreg_sech(AT_4G_Object_t *obj)
{
	int ret = 0;
	int len = 0;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "AT+CGREG?\r\n");
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, "+CGREG: 0,1"))
				{
					return AT_4G_STATUS_OK;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_ERR0R;
}


/****
	*@功能： 链接模式
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Disable network registration unsolicited result code
	*@***:   1：Enable network registration unsolicited result code
	*@***:   2：Enable network registration and location information unsolicited result code
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_CIPMUX(AT_4G_Object_t *obj, uint8_t switch_num)
{
    AT_4G_Status_t ret ;
    sprintf((char *)obj->cmddata, "AT+CIPMUX=%d\r\n",switch_num);
    ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);

    return ret;
}


/****
	*@功能： 网络注册状态
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Disable network registration unsolicited result code
	*@***:   1：Enable network registration unsolicited result code
	*@***:   2：Enable network registration and location information unsolicited result code
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Creg(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CREG=%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}


/****
	*@功能： 查询网络注册状态
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Disable network registration unsolicited result code
	*@***:   1：Enable network registration unsolicited result code
	*@***:   2：Enable network registration and location information unsolicited result code
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Creg_ASK(AT_4G_Object_t *obj)
{
    AT_4G_Status_t ret ;
    sprintf((char *)obj->cmddata, "AT+CGREG?\r\n");
    ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);

    return ret;
}

/****
	*@功能： 关机
	*@参数： Obj: pointer to module handle
	*@参数： switch_num: 0:Immediately power down 1: Normal power down
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Qpowd(AT_4G_Object_t *obj, uint8_t switch_num)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QPOWD=%d\r\n", switch_num);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 设置睡眠模式
	*@参数： Obj: pointer to module handle
	*@参数： contextID: 0:Disable slow clock 1: Enable slow clock. It is controlled by DTR
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Qsclk(AT_4G_Object_t *obj, uint8_t contextID)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QSCLK=%d\r\n", contextID);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 激活PDP上下文
	*@参数： Obj: pointer to module handle
	*@参数： contextID: integer type. The context ID. The range is 1-16.
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_CGACT(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CGACT=%d\r\n", 1);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}



/****
	*@功能： 激活PDP上下文
	*@参数： Obj: pointer to module handle
	*@参数： contextID: nteger type. The context ID. The range is 1-16.
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Qiact_sech(AT_4G_Object_t *obj)
{
	uint8_t ret ;
    char cmd_ack[20];
	int len;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "AT+QIACT?\r\n");    //配置目标TCP服务器
	sprintf(cmd_ack, "+QIACT:\r\n%d",1);    //配置目标TCP服务器
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, cmd_ack))
				{
					return AT_4G_STATUS_OK;
				}
//				else if(strstr((char *)obj->cmddata, OK))
//				{
//					return AT_4G_STATUS_OK;
//				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
//	return ret;
}

/****
	*@功能： 停用PDP上下文
	*@参数： Obj: pointer to module handle
	*@参数： contextID: nteger type. The context ID. The range is 1-16.
	*@返回： 运行状态
*/

AT_4G_Status_t AT_4G_Qideact(AT_4G_Object_t *obj, uint8_t contextID)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QIDEACT=%d\r\n", contextID);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}
/****
	*@功能： 打开一个socket服务器
	*@参数： Obj: pointer to module handle
	*@参数： conn_info
	*@返回： 运行状态
*/


AT_4G_Status_t AT_4G_CIPSTART(AT_4G_Object_t *obj,AT_4G_Conn_Info *conn_info)
{
	int ret ;
	int len;
	int itimes = 0;
//    sprintf((char*)obj->cmddata,"AT+CIPSTART=%d,%s,%s,%d,%d,%d\r\n",\
//   conn_info->contextID,conn_info->connectID,conn_info->service_type,conn_info->host,conn_info->remote_port, conn_info->local_port,conn_info->access_mode);    //配置目标TCP服务器


    sprintf((char*)obj->cmddata,"AT+CIPSTART=\"%s\",\"%s\",%d,%d\r\n",\
    conn_info->service_type,conn_info->host,conn_info->remote_port,conn_info->keepalivemode);    //配置目标TCP服务器

    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;
                if(strstr((char *)obj->cmddata, "CONNET OK"))
                {
                    return AT_4G_STATUS_OK;
                }
                else if(strstr((char *)obj->cmddata, OK))
                {
                    return AT_4G_STATUS_OK;
                }
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
            }
            else
            {
                itimes++;
            }

            if(itimes > AT_RECV_TIMES)
            {
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}

/****
	*@功能： 关闭一个socket 连接
	*@参数： Obj: pointer to module handle
	*@参数： connectID: Integer type. The socket service index. The range is 0-11
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_CIPCLOSE(AT_4G_Object_t *obj,uint8_t conn_id)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+CIPCLOSE=%d,%d\r\n",  conn_id,1);  //0缓慢关闭，1快速关闭，一种是缓慢关闭，这种方式，模块会发送TCP的FIN, RST协议包进行优雅关闭链接；一种是快速关闭，类似强制关闭
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

/****
	*@功能： 选择数据模式
	*@参数： Obj: pointer to module handle
	*@参数： access_mode 	0 Buffer access mode
							1 Direct push mode
							2 Transparent access mode
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Qiswtme(AT_4G_Object_t *obj,uint8_t connectID, uint8_t access_mode)
{
	int ret ;
	int len;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "AT+QISWTMD=%d,%d\r\n",  connectID, access_mode);
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, "CONNECT"))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, OK))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}


/****
	*@功能： 退出透传模式
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Exit_Transparent(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "+++");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
	
}

/****
	*@功能： 退出连接
	*@参数： Obj: pointer to module handle
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Disconnect(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QICLOSE=?\r\n");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	sprintf((char *)obj->cmddata, "AT+QNWINFO\r\n");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	sprintf((char *)obj->cmddata, "AT+QIACT=1\r\n");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	if(ret != AT_4G_STATUS_OK)
	{
		return ret;
	}
	return ret;
}


/****
	*@功能： 非透传模式下，TCP发送
	*@参数： Obj: pointer to module handle
	*@参数： buf:需要发送的数据
	*@参数： Length: 发送数据的长度
	*@返回： 运行状态
*/
AT_4G_Status_t  G8100_Tcp_Send(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length)
{
	int ret ;
	int len;
	int itimes = 0;
	sprintf((char *)obj->cmddata, "AT+CIPSEND=1,%d\r\n",  Length);
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, ">"))
				{
					ret = obj->fops.dev_send(buf, Length);
					len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
					if((len > 0) && (len < AT_4G_DATA_SIZE ))
					{
						*(obj->cmddata+len) = 0;
						if(strstr((char *)obj->cmddata, "SEND OK"))
						{
							return AT_4G_STATUS_OK;
						}
						else
						{
							itimes++;
						}
						
					}
					else
					{
						itimes++;
					}
					
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}

char ilen[4];
int  ilen1;
/****
	*@功能： 非透传模式下，TCP接收
	*@参数： Obj: pointer to module handle
	*@参数： buf:用于存储接收的数据
	*@参数： waittime: 接收超时时间 waittime*obj->timeout*200ms
	*@返回： 返回接收的数据长度，0：接收超时
*/
int  G8100_Tcp_Recv(AT_4G_Object_t *obj, uint8_t *buf, uint16_t  waittime)
{
    int ret = 0;
    int len;
    int i;
    int itimes = 0;
    uint8_t *ptr ;
    while(1)
    {
        len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
        if((len > 0) && (len < AT_4G_DATA_SIZE ))
        {
            *(obj->cmddata + len) = 0;
            if(strstr((char*)obj->cmddata,"+IPD,"))
            {
                ptr = &obj->cmddata[7];
                for(i =0; i < 4; i++)
                {
                    if(*(ptr+i) == ':')
                    {

                        ilen1 = atoi(ilen);
                        break;
                    }
                    ilen[i] = *(ptr+i);
                }
                ptr+=i;
                for(i = 0; i < ilen1; i++)
                {
                    buf[i] = ptr[i+1];
                }
                ret = ilen1;
                return ret;
            }
            else
            {
                itimes++;
            }
        }
        else 
        {
            itimes++;
        }
        if(itimes > waittime)
        {
            return 0;
        }
    }

//	return ret;
}


/****
	*@功能： 非透传模式下，TCP接收
	*@参数： Obj: pointer to module handle
	*@参数： buf:用于存储接收的数据
	*@参数： waittime: 接收超时时间 waittime*obj->timeout*200ms
	*@返回： 返回接收的数据长度，0：接收超时
*/
AT_4G_Status_t  AT_QLTS(AT_4G_Object_t *obj, tTime_Parameter *Time_Parameter, Time_Mode  tMode)
{
	int ret = 0;
	int len;
	int itimes = 0;
	char Data[2];
	uint8_t *ptr ;
	sprintf((char *)obj->cmddata, "AT+QLTS=%d\r\n", tMode);
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;
                if(strstr((char*)obj->cmddata,"+QLTS:"))
                {
                    /**************年****************/
                    ptr = &obj->cmddata[12];
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Year =  atoi(Data);
                    /**************月****************/
                    ptr+=1;
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Month =  atoi(Data);
                    /**************日****************/
                    ptr+=1;
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Day =  atoi(Data);
                    /**************时****************/
                    ptr+=1;
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Hour =  atoi(Data);
                    /**************分****************/
                    ptr+=1;
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Minute =  atoi(Data);
                    /**************秒****************/
                    ptr+=1;
                    Data[0] = *ptr++;
                    Data[1] = *ptr++;
                    Time_Parameter->Second =  atoi(Data);
                    
                    return AT_4G_STATUS_OK;
                }
                else
                {
                        itimes++;
                }
            }
            else 
            {
                itimes++;
            }
            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
	return AT_4G_STATUS_ERR0R;
}

void HexToStr(char *pbDest, char *pbSrc, int nLen)
{
char ddl,ddh;
int i;
    for (i=0; i<nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) 
            ddh = ddh + 7;
        if (ddl > 57)
            ddl = ddl + 7;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
    }

    pbDest[nLen*2] = '\0';
}


/****
	*@功能： 非透传模式下，TCP发送二进制
	*@参数： Obj: pointer to module handle
	*@参数： buf:需要发送的数据
	*@参数： Length: 发送数据的长度
	*@返回： 运行状态
*/
 //char u9[4000] ;
AT_4G_Status_t  G8100_Tcp_Send_2(AT_4G_Object_t *obj, uint8_t *buf,uint16_t Length)
{
    int ret;
    int len;
    int itimes = 0;
    char u9[400] ;
    //HexToStr(u9,(char *)buf,Length);
    //strncpy(u9,(char *)buf,Length);
    HexToStr(u9,(char *)buf,Length);



    sprintf((char *)obj->cmddata, "AT+CIPSEND=%d,",Length);
    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));

    sprintf((char *)obj->cmddata, "%s",u9);
    ret = obj->fops.dev_send(obj->cmddata,Length);

    sprintf((char *)obj->cmddata, ",%d\r\n",1);
    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, ">"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
        }
        else
        {
            itimes++;
        }

            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
    return AT_4G_STATUS_DEV_ERR0R;
}

/****
	*@功能： 非透传模式下，TCP发送二进制
	*@参数： Obj: pointer to module handle
	*@参数： buf:需要发送的数据
	*@参数： Length: 发送数据的长度
	*@返回： 运行状态
*/
 //char u9[4000] ;
AT_4G_Status_t  G8100_Tcp_Send_ASK(AT_4G_Object_t *obj, uint16_t Length)
{
    int ret;
    int len;
    int itimes = 0;
    //char u9[400] ;
    //HexToStr(u9,(char *)buf,Length);
    //strncpy(u9,(char *)buf,Length);
    sprintf((char *)obj->cmddata, "AT+CIPSEND=%d,%d\r\n",(Length),1);
    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, ">"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
			}
			else
			{
				itimes++;
			}

			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
    }
    return AT_4G_STATUS_DEV_ERR0R;
}


AT_4G_Status_t  G8100_Tcp_Send_Hex(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length)
{
    int ret;
    int len;
    int itimes = 0;
    memcpy(obj->cmddata,(char *)buf,Length);
    ret = obj->fops.dev_send(obj->cmddata, Length);

    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, "SEND OK"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
            }
            else
            {
                itimes++;
            }

            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
    return AT_4G_STATUS_DEV_ERR0R;
}





//AT_4G_Status_t  G8100_Tcp_Send_Hex(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length)
//{
//	int ret ;
//	int len;
//	int itimes = 0;
//  
//    HexToStr(u9,(char *)buf,Length);
//   // strncpy(u9,(char *)buf,Length);
//	sprintf((char *)obj->cmddata, "AT+QISENDEX=0,\"%s\"\r\n",u9);
//	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
//	if(ret > 0)
//	{
//		while(1)
//		{
//			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
//			if((len > 0) && (len < AT_4G_DATA_SIZE ))
//			{
//				*(obj->cmddata+len) = 0;
//				
//                if(strstr((char *)obj->cmddata, "SEND OK"))
//                {
//                    return AT_4G_STATUS_OK;
//                }
//				else 
//					if(strstr((char *)obj->cmddata, ERROR))
//					{
//						return AT_4G_STATUS_ERR0R;
//					}
//					else
//					{
//						itimes++;
//					}
//			}
//			else
//			{
//				itimes++;
//			}
//			
//			if(itimes > AT_RECV_TIMES)
//			{
//				return AT_4G_STATUS_ERR0R;
//			}
//		}
//	}
//	return AT_4G_STATUS_DEV_ERR0R;
//}



/****
	*@功能： 非透传模式下，TCP发送二进制
	*@参数： Obj: pointer to module handle
	*@参数： buf:需要发送的数据
	*@参数： Length: 发送数据的长度
	*@返回： 运行状态
*/
 //char u9[4000] ;
AT_4G_Status_t  AT_4G_CIPCFG(AT_4G_Object_t *obj, uint16_t a, char b, uint16_t c)
{
    int ret;
    int len;
    int itimes = 0;
    
    sprintf((char *)obj->cmddata, "AT+CIPCFG=%d,%d,%d\r\n",a,b,c);
    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, "OK"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
        }
        else
        {
            itimes++;
        }

            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
    return AT_4G_STATUS_DEV_ERR0R;
}

AT_4G_Status_t  AT_4G_CIPPACK(AT_4G_Object_t *obj, uint16_t switch_num, U8 * TX_Buff)
{
    int ret;
    int len;
    int itimes = 0;
    char u9[100];

    HexToStr(u9,(char *)TX_Buff,20);

    
    sprintf((char *)obj->cmddata, "AT+CIPPACK=%d,\"%s\"\r\n",switch_num,u9);
    ret = obj->fops.dev_send(obj->cmddata,strlen((char *)obj->cmddata));


/*
    
    sprintf((char *)obj->cmddata,"%c",'"');
    ret = obj->fops.dev_send(obj->cmddata,strlen((char *)obj->cmddata));
    memcpy(obj->cmddata,(char *)TX_Buff,20);
    ret = obj->fops.dev_send(obj->cmddata, 20);
    sprintf((char *)obj->cmddata,"%c\r\n",'"');
    ret = obj->fops.dev_send(obj->cmddata,strlen((char *)obj->cmddata));

*/


/*
    sprintf((char *)obj->cmddata,"%c",'"');
    ret = obj->fops.dev_send(obj->cmddata,strlen((char *)obj->cmddata));

    memcpy(obj->cmddata,(char *)TX_Buff,20);
    ret = obj->fops.dev_send(obj->cmddata, 20);

    sprintf((char *)obj->cmddata,"%c\r\n",'"');
    ret = obj->fops.dev_send(obj->cmddata,strlen((char *)obj->cmddata));
*/


    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, "OK"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
        }
        else
        {
            itimes++;
        }

            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
    return AT_4G_STATUS_DEV_ERR0R;
}



AT_4G_Status_t  AT_4G_CSCLK(AT_4G_Object_t *obj, uint16_t a, uint16_t b)
{
    int ret;
    int len;
    int itimes = 0;
    
    sprintf((char *)obj->cmddata, "AT+CSCLK=%d,%d\r\n",a,b);
    ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
    if(ret > 0)
    {
        while(1)
        {
            len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
            if((len > 0) && (len < AT_4G_DATA_SIZE ))
            {
                *(obj->cmddata+len) = 0;

                if(strstr((char *)obj->cmddata, "OK"))
                {
                    return AT_4G_STATUS_OK;
                }    
                else if(strstr((char *)obj->cmddata, ERROR))
                {
                    return AT_4G_STATUS_ERR0R;
                }
                else
                {
                    itimes++;
                }
        }
        else
        {
            itimes++;
        }

            if(itimes > AT_RECV_TIMES)
            {
                return AT_4G_STATUS_ERR0R;
            }
        }
    }
    return AT_4G_STATUS_DEV_ERR0R;
}
/****
	*@功能： 激活PDP上下文
	*@参数： Obj: pointer to module handle
	*@参数： contextID: nteger type. The context ID. The range is 1-16.
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Qiact(AT_4G_Object_t *obj, uint8_t contextID)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QIACT=%d\r\n", contextID);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}
/****
	*@功能： 打开一个socket服务器
	*@参数： Obj: pointer to module handle
	*@参数： conn_info
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Qiopen(AT_4G_Object_t *obj,AT_4G_Conn_Info *conn_info)
{
	int ret ;
	int len;
	int itimes = 0;
	sprintf((char*)obj->cmddata,"AT+QIOPEN=%d,%d,\"%s\",\"%s\",%d,%d,%d\r\n",\
	conn_info->contextID,conn_info->connectID,conn_info->service_type,conn_info->host,conn_info->remote_port, conn_info->local_port,conn_info->access_mode);    //配置目标TCP服务器
	
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;
				if(strstr((char *)obj->cmddata, "CONNECT"))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, OK))
				{
					return AT_4G_STATUS_OK;
				}
				else if(strstr((char *)obj->cmddata, ERROR))
				{
					return AT_4G_STATUS_ERR0R;
				}
				else
				{
					itimes++;
				}
			}
			else
			{
				itimes++;
			}
			
			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}

/****
	*@功能： 关闭一个socket 连接
	*@参数： Obj: pointer to module handle
	*@参数： connectID: Integer type. The socket service index. The range is 0-11
	*@返回： 运行状态
*/
AT_4G_Status_t AT_4G_Qiclose(AT_4G_Object_t *obj,uint8_t connectID)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QICLOSE=%d\r\n",  connectID);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}
/****
	*@功能： 非透传模式下，TCP发送
	*@参数： Obj: pointer to module handle
	*@参数： buf:需要发送的数据
	*@参数： Length: 发送数据的长度
	*@返回： 运行状态
*/
 //char u9[4000] ;
AT_4G_Status_t  EC20_Tcp_Send_Hex(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length)
{
	int ret ;
	int len;
	int itimes = 0;
	char u9[400] ;
	HexToStr(u9,(char *)buf,Length);
	// strncpy(u9,(char *)buf,Length);
	sprintf((char *)obj->cmddata, "AT+QISENDEX=0,\"%s\"\r\n",u9);
	ret = obj->fops.dev_send(obj->cmddata, strlen((char *)obj->cmddata));
	if(ret > 0)
	{
		while(1)
		{
			len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
			if((len > 0) && (len < AT_4G_DATA_SIZE ))
			{
				*(obj->cmddata+len) = 0;

				if(strstr((char *)obj->cmddata, "SEND OK"))
				{
					return AT_4G_STATUS_OK;
				}    
				else
					if(strstr((char *)obj->cmddata, ERROR))
					{
					 return AT_4G_STATUS_ERR0R;
					}
					else
					{
					 itimes++;
					}
			}
			else
			{
				itimes++;
			}

			if(itimes > AT_RECV_TIMES)
			{
				return AT_4G_STATUS_ERR0R;
			}
		}
	}
	return AT_4G_STATUS_DEV_ERR0R;
}
/*
************************************************************
****************
*Function Name :    void sMy_4G_Emitte(void)
*Description   :    4G????
*Input         :    buffer:????????
*Output        :    None
*Author        :    ???
*Creation Data :    2020/01/13
*Revisor       :    Rev1.0
*Note          :    ??????u16_Send_Len,???????????0???
************************************************************
****************
*/
void Data_4G_Emitte(u8 *buffer)
{
	u16 u16_Send_Len = 13;
	
    EC20_obj.fops.dev_receive = Dev_Recv;  //  
    EC20_obj.fops.dev_send = Dev_Send;     //  
    EC20_obj.timeout = 200;
	/*建立UDP连接*/
	//AT_4G_Qiopen(&EC20_obj, &Conn_Info);
	/*数据以HEX码进行发送*/
	EC20_Tcp_Send_Hex(&EC20_obj, buffer, u16_Send_Len);
	
	u16_Send_Len = 0;
}
/*
************************************************************
****************
过 AT+QURCCFG 命令，可将主串口、USB AT 端口或 USB 调制端口设置
为 URC 输出串口。默认为USB AT 端口。
************************************************************
****************
*/
AT_4G_Status_t AT_4G_QURCCFG(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QURCCFG=\"%s\",\"%s\"\r\n", URCPORT, URCPORTConfig);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}
/*
************************************************************
****************
主机系统使用AP_READY功能，AP_READY引脚上低电平为有效状态，
即当 AP_READY 为低时表示主机处在唤醒就绪状态，此时若模块有主
动事件数据上报，模块将直接输出数据到主机端；而当 AP_READY 引
脚为高电平时，表示主机处在未就绪状态，此时模块端若有主动事件数
据上报，模块端不会直接输出数据，而是以 200ms 为周期对 AP_READY
引脚进行循环检测，直到 AP_READY 引脚电平变低（主机系统就绪）
才输出主动事件数据到主机端。
************************************************************
****************
*/
AT_4G_Status_t AT_4G_QCFG(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QCFG=\"%s\",1,0,200\r\n", APREADY);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	
	return ret;
}

//非透传模式下设置传输方式 0 Buffer access mode  1 Direct push mode  2 Transparent access mode
AT_4G_Status_t AT_4G_QISWTMD(AT_4G_Object_t *obj,uint8_t connect_ID,uint8_t Send_MODE)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "AT+QISWTMD=%d,%d\r\n", connect_ID, Send_MODE);
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	return ret;
}
//配置发送"+++"退出透传模式
AT_4G_Status_t AT_4G_ExtiTransparent(AT_4G_Object_t *obj)
{
	AT_4G_Status_t ret ;
	sprintf((char *)obj->cmddata, "+++");
	ret = AT_CMD_Execute(obj, obj->cmddata, obj->cmddata);
	return ret;
}

/****
	*@功能： 非透传模式下，TCP接收
	*@参数： Obj: pointer to module handle
	*@参数： buf:用于存储接收的数据
	*@参数： waittime: 接收超时时间 waittime*obj->timeout*200ms
	*@返回： 返回接收的数据长度，0：接收超时
*/
int  EC20_Tcp_Recv(AT_4G_Object_t *obj, uint8_t *buf, uint16_t  waittime)
{
	int ret = 0;
	int len;
	int i;
	int itimes = 0;
	uint8_t *ptr ;
	while(1)
	{
		len = obj->fops.dev_receive(obj->cmddata, obj->timeout);
		if((len > 0) && (len < AT_4G_DATA_SIZE ))
		{
			*(obj->cmddata+len) = 0;
			if(strstr((char*)obj->cmddata,"+QIURC: \"recv\""))
			{
				ptr = &obj->cmddata[19];
				for(i =0; i < 4; i++)
				{
					if(*(ptr+i) == 0X0D && *(ptr+i+1) == 0x0A)  //ascii码表示<CR> <LF>
					{
						
						ilen1 = atoi(ilen);
						break;
					}
					ilen[i] = *(ptr+i);
				}
                ptr+=i;
				for(i = 0; i < ilen1; i++)
				{
					buf[i] = ptr[i+2];
				}
				ret = ilen1;
				return ret;
			}
			else
			{
					itimes++;
			}
		}
		else 
		{
			itimes++;
		}
		if(itimes > waittime)
		{
			return 0;
		}
	}
	
//	return ret;
}


