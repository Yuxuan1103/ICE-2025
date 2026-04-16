#ifndef __4G_AT_H
#define	__4G_AT_H
//#include "main.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
/* Exported typedef ----------------------------------------------------------*/
#define OK        "OK"
#define ERROR  "ERROR"
#define AT_4G_DATA_SIZE     4000                                         
#define AT_RECV_TIMES       5
extern  const uint8_t Buffer_access_mode;
extern const uint8_t Direct_push_mode;
extern const uint8_t Transparent_access_mode;
typedef int (*Dev_Send_Func)( uint8_t *, uint16_t);
typedef int (*Dev_Receive_Func)(uint8_t *,  uint16_t);

typedef enum{
    LOCAL_TIME      = 0,
    CMT_TIME        = 1,
    NET_TIME        = 2,
}Time_Mode;


typedef enum{
    AT_4G_STATUS_OK         = 0,
    AT_4G_STATUS_ERR0R      = 1,
    AT_4G_STATUS_TIMEOUT    =  2,
    AT_4G_STATUS_DEV_ERR0R  =  3,

}AT_4G_Status_t;

typedef struct
{
    Dev_Send_Func       dev_send;                 //发送函数
    Dev_Receive_Func    dev_receive;            //接受函数
}AT_4G_Dev_t;

typedef struct{
    const char *host;				//远程服务器的域名或者ip地址
    const char *service_type;		//服务器类型 TCP/UDP/TCP LISTENER/UDP SERVICE
    uint8_t contextID;				//Integer type. The context ID. The range is 1-16.
    uint8_t connectID;				//Integer type. The socket service index. The range is 0-11.
    uint16_t remote_port;			//服务器端口，only valid when <service_type> is "tcp"or"udp". The range is 0-65535
    uint16_t local_port; 			//本地端口，The range is 0-65535
    uint8_t access_mode;			//访问模式 0： Buffer access mode 1：Direct push mode 2：Transparent access mode
    uint8_t keepalivemode;
	
}AT_4G_Conn_Info;


//同通信数据  超时
typedef struct {
    AT_4G_Dev_t fops;
    uint8_t cmddata[AT_4G_DATA_SIZE];
    uint32_t timeout;
}AT_4G_Object_t;

typedef  struct{
	uint8_t  	Year;     	//年
	uint8_t	 	Month; 		//月
	uint8_t  	Day;		//日
	uint8_t 	Hour;		//时
	uint8_t 	Minute;		//分
	uint8_t		Second;		//秒
} tTime_Parameter;

AT_4G_Status_t AT_CMD_SEND(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata);
static AT_4G_Status_t AT_CMD_Execute(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata);
AT_4G_Status_t AT_4G_Ate(AT_4G_Object_t *obj, uint8_t switch_num);
AT_4G_Status_t AT_4G_Rdy(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Cfun(AT_4G_Object_t *obj, uint8_t Func);
AT_4G_Status_t AT_4G_Cmee(AT_4G_Object_t *obj, uint8_t switch_num);
AT_4G_Status_t AT_4G_Ifc(AT_4G_Object_t *obj, uint8_t dce_by_dte, uint8_t dte_by_dce);
AT_4G_Status_t AT_4G_Dtr(AT_4G_Object_t *obj, uint8_t switch_num);
AT_4G_Status_t AT_4G_Dcd(AT_4G_Object_t *obj, uint8_t switch_num);
AT_4G_Status_t AT_4G_Icf(AT_4G_Object_t *obj, uint8_t format, uint8_t parity);
AT_4G_Status_t AT_4G_Ipr(AT_4G_Object_t *obj, uint32_t Baud_Rate);
AT_4G_Status_t AT_4G_Cpin(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Csq(AT_4G_Object_t *obj);

AT_4G_Status_t AT_4G_Qnwinfo(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Ato(AT_4G_Object_t *obj,uint8_t switch_num);
AT_4G_Status_t AT_4G_Cgreg(AT_4G_Object_t *obj,uint8_t switch_num);
AT_4G_Status_t AT_4G_Creg(AT_4G_Object_t *obj,uint8_t switch_num);
AT_4G_Status_t AT_4G_Creg_ASK(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Qpowd(AT_4G_Object_t *obj,uint8_t switch_num);
AT_4G_Status_t AT_4G_Qsclk(AT_4G_Object_t *obj,uint8_t switch_num);


AT_4G_Status_t AT_4G_CIPMUX(AT_4G_Object_t *obj, uint8_t switch_num);
AT_4G_Status_t AT_CIPSEND_Execute(AT_4G_Object_t *obj, uint8_t* cmd, uint8_t *pdata);


AT_4G_Status_t AT_4G_Qiact_sech(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Qideact(AT_4G_Object_t *obj,uint8_t contextID);
AT_4G_Status_t AT_4G_CIPSTART(AT_4G_Object_t *obj,AT_4G_Conn_Info *conn_info);
AT_4G_Status_t AT_4G_CIPCLOSE(AT_4G_Object_t *obj,uint8_t conn_id);
AT_4G_Status_t AT_4G_Qiswtme(AT_4G_Object_t *obj,uint8_t connectID, uint8_t access_mode);

AT_4G_Status_t AT_4G_Exit_Transparent(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Disconnect(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_At(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_Cgreg_sech(AT_4G_Object_t *obj);

AT_4G_Status_t AT_4G(AT_4G_Object_t *obj);
AT_4G_Status_t  G8100_Tcp_Send(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length);
int  G8100_Tcp_Recv(AT_4G_Object_t *obj, uint8_t *buf, uint16_t  waittime);
AT_4G_Status_t  AT_QLTS(AT_4G_Object_t *obj, tTime_Parameter *Time_Parameter, Time_Mode  tMode);
AT_4G_Status_t  G8100_Tcp_Send_Hex(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length);
AT_4G_Status_t  G8100_Tcp_Send_ASK(AT_4G_Object_t *obj, uint16_t Length);
AT_4G_Status_t  AT_4G_CIPCFG(AT_4G_Object_t *obj, uint16_t a, char b, uint16_t c);
AT_4G_Status_t  AT_4G_CIPPACK(AT_4G_Object_t *obj, uint16_t switch_num, U8 * TX_Buff);
AT_4G_Status_t  AT_4G_CSCLK(AT_4G_Object_t *obj, uint16_t a, uint16_t b);
AT_4G_Status_t  G8100_Tcp_Send_2(AT_4G_Object_t *obj, uint8_t *buf,uint16_t Length);
AT_4G_Status_t AT_4G_Qiact(AT_4G_Object_t *obj, uint8_t contextID);
AT_4G_Status_t AT_4G_Qiopen(AT_4G_Object_t *obj,AT_4G_Conn_Info *conn_info);
AT_4G_Status_t AT_4G_Qiclose(AT_4G_Object_t *obj,uint8_t connectID);
AT_4G_Status_t  EC20_Tcp_Send_Hex(AT_4G_Object_t *obj, uint8_t *buf, uint16_t Length);
AT_4G_Status_t AT_4G_QURCCFG(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_QCFG(AT_4G_Object_t *obj);
AT_4G_Status_t AT_4G_QISWTMD(AT_4G_Object_t *obj,uint8_t connect_ID,uint8_t Send_MODE);
AT_4G_Status_t AT_4G_ExtiTransparent(AT_4G_Object_t *obj);
int  EC20_Tcp_Recv(AT_4G_Object_t *obj, uint8_t *buf, uint16_t  waittime);
AT_4G_Status_t AT_4G_Csq_HeartBeat(AT_4G_Object_t *obj);

void Data_4G_Emitte(u8 *buffer);



#endif /* 4G_at.h*/




