/*
*********************************************************************
**
**    Project:  杆塔倾斜2.0版本
**
**    Componet: STM32L071CBT6
**
**    Filename: Tilt_TSK.c
**
**    Abstract: 倾斜数据采集计算代码源文件
**
**    Compiler: KEIL 5.0
**    
**
**    History：
**
**        <auther>    <date>    <version>   <desc>
**
**            YXD        20/04/26        1.0
**
**    Copyright @cdstarriver All Rights Reserved.
**
*********************************************************************
*/


#include "main.h"


uint8_t CRC16H = 0, CRC16L = 0;

 uint8_t Sendcnt = 0;

void DataSamplesTask( U16 * thresholdvalue);
void DataCommunication_TSK(void);
void PowerUpZeroClab(void);

U16 AngelTrans(float AdxlAngle);


/***********************************************************
Function Name       :  DataSamplesTask
Description         :  角度数据采集 
Input               :  thresholdvalue：阈值
Output              :  无
                       
Author              :  YXD
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
************************************************************/ 
INT16U  ACnt = 0;
INT16U  Aflag = 1;   //调平标志

float f32datacn_Sx = 0;  //用于存储上次的倾斜值
float f32datacn_Hx = 0;

void DataSamplesTask(U16 * U16_thresholdvalue)
{
//    float f32thresholdvalue;
    static unsigned char U8_henxiangCnt = 0;
    static unsigned char U8_shunxiangCnt = 0;

//    f32thresholdvalue = (float)((*U16_thresholdvalue) * g_SysData.u16_TiltSend_K);
//    f32thresholdvalue = f32thresholdvalue * 0.1;
	
//  	printf("YUZHI= %4f;\n\r", f32thresholdvalue );    //27
	
    sInclinationCalc_TSK();    //角度采集

    if(sIncl_Data.f32_shunxian_Angle_arc_o >= 0)
    {
        if(sIncl_Data.f32_shunxian_Angle_arc_o >= (f32thresholdvalue_Sx1))
        {
            U8_shunxiangCnt++;
            if(U8_shunxiangCnt >= 10)
            {
                U8_shunxiangCnt = 0;
                g_statecheck.bit.bShunxianTiltState = cFlagYes;           // 系统存在杆塔倾斜标志位
            }
        }
        else
        {
            U8_shunxiangCnt = 0;
            g_statecheck.bit.bShunxianTiltState = cFlagNo;           // 系统存在杆塔倾斜标志位
        }
    }
    else
    {
        if(sIncl_Data.f32_shunxian_Angle_arc_o <= (-f32thresholdvalue_Sx2))
        {
            U8_shunxiangCnt++;
            if(U8_shunxiangCnt >= 10)
            {
                U8_shunxiangCnt = 0;
                g_statecheck.bit.bShunxianTiltState = cFlagYes;           // 系统存在杆塔倾斜标志位
            }
        }
        else
        {
            U8_shunxiangCnt = 0;
            g_statecheck.bit.bShunxianTiltState = cFlagNo;           // 系统存在杆塔倾斜标志位
        }
    }

    if(sIncl_Data.f32_hengxian_Angle_arc_o >= 0)
    {
        if(sIncl_Data.f32_hengxian_Angle_arc_o >= (f32thresholdvalue_Hx1))
        {
            U8_henxiangCnt++;
            if(U8_henxiangCnt >= 10)
            {
                U8_henxiangCnt = 0;
                g_statecheck.bit.bHenxianTiltState = cFlagYes;           // 系统存在杆塔倾斜标志位
            }
        }
        else
        {
            U8_henxiangCnt = 0;
            g_statecheck.bit.bHenxianTiltState = cFlagNo;           // 系统存在杆塔倾斜标志位
        }
    }
    else
    {
        if(sIncl_Data.f32_hengxian_Angle_arc_o <= (-f32thresholdvalue_Hx2))
        {
            U8_henxiangCnt++;
            if(U8_henxiangCnt >= 10)
            {
                U8_henxiangCnt = 0;
                g_statecheck.bit.bHenxianTiltState = cFlagYes;           // 系统存在杆塔倾斜标志位
            }
        }
        else
        {
            U8_henxiangCnt = 0;
            g_statecheck.bit.bHenxianTiltState = cFlagNo;           // 系统存在杆塔倾斜标志位
        }
    }


		  g_SysData.f32_ShunxianAngle  += sIncl_Data.f32_shunxian_Angle_arc_o;
		 g_SysData.f32_HenxianAngle  += sIncl_Data.f32_hengxian_Angle_arc_o;
		 g_SysData.f32_ZongheAngle  += sIncl_Data.f32_zonghe_Angle_arc_o;
      ACnt ++;
		
		 if (ACnt >= 10)
		 {
    // 真实值转化为上传值
    g_SysData.f32_ShunxianAngle   = g_SysData.f32_ShunxianAngle * 0.1;
    g_SysData.f32_HenxianAngle    = g_SysData.f32_HenxianAngle * 0.1;
    g_SysData.f32_ZongheAngle     = g_SysData.f32_ZongheAngle  * 0.1;
    g_SysData.f32_AdxlTemperature = sIncl_Data.f32_XD355_Temprature;
			 
		if (fabs(g_SysData.f32_ShunxianAngle - f32datacn_Sx) > 0.5 || fabs(g_SysData.f32_HenxianAngle - f32datacn_Hx) > 0.5 )  //间隔倾斜的绝对值
		{
		  g_statecheck.bit.bShunxianTiltState = cFlagYes;
			f32datacn_Sx = g_SysData.f32_ShunxianAngle;
			f32datacn_Hx = g_SysData.f32_HenxianAngle;
		}
			
			 
		   ACnt = 0 ;
		 }
		 
    if((g_statecheck.bit.bShunxianTiltState == cFlagYes)
    || (g_statecheck.bit.bHenxianTiltState == cFlagYes))
    {
        g_statecheck.bit.bSysTiltState = cFlagYes;
    }
    else
    {
        g_statecheck.bit.bSysTiltState = cFlagNo;
			  Sendcnt++;
    }
}

/***********************************************************
Function Name       :  DataCommunication_TSK
Description         :  倾斜数据处理总任务 
Input               :  void
Output              :  无
                       
Author              :  YXD
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
************************************************************/ 

void DataCommunication_TSK(void)
{
    //g_SysData.u16_TiltWarningTreshold = 0x01F4;

    DataSamplesTask(&g_SysData.u16_TiltWarningTreshold);//数据采集

    PowerUpZeroClab();

}


/***********************************************************
Function Name       :  AngelTrans
Description         :  将ADXL355读取到的浮点型角度值转换为发送的数据格式
Input               :  (float * AdxlAngle)
Output              :  u16_SendAngle
                       
Author              :  YXD
Creation Date       :  2020/01/19
Revisor             :  1.0
Revision Date       :  2020/01/19
Note                :  None
************************************************************/ 
U16 AngelTrans(float AdxlAngle)
{
    U16    u16_SendAngle = 0;
    U16    u16_integer = 0;

    //正负位赋值
    if( AdxlAngle >= 0.0f)
    {
        u16_integer = (U16)(AdxlAngle);
        u16_integer = u16_integer | 0X0000;

    }
    else
    {
        AdxlAngle = AdxlAngle * (-1.0);
        u16_integer = (U16)(AdxlAngle);
        u16_integer = u16_integer | 0X8000;
    }
    
    u16_SendAngle = u16_integer;
    return u16_SendAngle;

}


//用于开机后角度数据的找平
void PowerUpZeroClab(void)
{
	    static U8 TiltAddCnt = 0;
    ubitfloat f32temp;
    if(g_statecheck.bit.bTiltCalbFlag == cFlagYes)
    {
        TiltAddCnt++;
        sIncl_Data.f32_shunxian_Angle_add += sIncl_Data.f32_shunxian_Angle_arc;
        sIncl_Data.f32_hengxian_Angle_add += sIncl_Data.f32_hengxian_Angle_arc;
        sIncl_Data.f32_zonghe_Angle_add   += sIncl_Data.f32_zonghe_Angle_arc;

        if(TiltAddCnt >= 10)
        {
            TiltAddCnt = 0;
            g_statecheck.bit.bTiltCalbFlag = cFlagNo;
					
            sIncl_Data.f32_shunxian_Angle_offset = sIncl_Data.f32_shunxian_Angle_add * 0.1;
            sIncl_Data.f32_hengxian_Angle_offset = sIncl_Data.f32_hengxian_Angle_add * 0.1;
            sIncl_Data.f32_zonghe_Angle_offset   = sIncl_Data.f32_zonghe_Angle_add   * 0.1;

            sIncl_Data.f32_shunxian_Angle_add = 0.0;
            sIncl_Data.f32_hengxian_Angle_add = 0.0;
            sIncl_Data.f32_zonghe_Angle_add   = 0.0;

            g_SysData.u16_TiltSend_K = 0x01;

            // 向EEPROM中写校准值
//            f32temp.fd = sIncl_Data.f32_shunxian_Angle_offset;
//            unEeprom.stData.wShunxianOffset_H = f32temp.intdata[1].id;
//            unEeprom.stData.wShunxianOffset_L = f32temp.intdata[0].id;
//            f32temp.fd = sIncl_Data.f32_hengxian_Angle_offset;
//            unEeprom.stData.wHengxianOffset_H = f32temp.intdata[1].id;
//            unEeprom.stData.wHengxianOffset_L = f32temp.intdata[0].id;
//            f32temp.fd = sIncl_Data.f32_zonghe_Angle_offset;
//            unEeprom.stData.wZongheOffset_H = f32temp.intdata[1].id;
//            unEeprom.stData.wZongheOffset_L = f32temp.intdata[0].id;

            //printf(" LeveL COMPLETE! ");
             Aflag = 0;
					 }
				 }

}

//用于开机后对加速度值进行调平
void Accl_ZeroCal()
{
	XL355_Temp_Calc();
	XL355_Accel_Read(); //读传感器加速度
	XL355_Temp_Calc();
	XL355_Accel_Read(); //读传感器加速度
	XL355_Temp_Calc();
	XL355_Accel_Read(); //读传感器加速度
	//printf("%.5f,%.5f,%.5f\n",  sIncl_Data.f32_Accl_Xaxis,  sIncl_Data.f32_Accl_Yaxis,  sIncl_Data.f32_Accl_Zaxis);
	
	sIncl_Data.f32_XAccl_add  +=  sIncl_Data.f32_Accl_Xaxis; //求和
	sIncl_Data.f32_YAccl_add  +=  sIncl_Data.f32_Accl_Yaxis;
	sIncl_Data.f32_ZAccl_add  +=  sIncl_Data.f32_Accl_Zaxis;

}
	
void Accl_ZeroProg()
{
	sIncl_Data.f32_XAccl_add = 0;
	sIncl_Data.f32_YAccl_add = 0;
	sIncl_Data.f32_ZAccl_add = 0;
	Accl_ZeroCal(); 			//第一次数据舍去
	
	sIncl_Data.f32_XAccl_add = 0;
	sIncl_Data.f32_YAccl_add = 0;
	sIncl_Data.f32_ZAccl_add = 0;
	for(uint16_t i=0;i<5000;i++)
	{
		Accl_ZeroCal();
	}
	sIncl_Data.f32_XAccl_offset = sIncl_Data.f32_XAccl_add * 0.0002;
	sIncl_Data.f32_YAccl_offset = sIncl_Data.f32_YAccl_add * 0.0002;
	sIncl_Data.f32_ZAccl_offset = sIncl_Data.f32_ZAccl_add * 0.0002;
	sIncl_Data.f32_XAccl_add = 0;
	sIncl_Data.f32_YAccl_add = 0;
	sIncl_Data.f32_ZAccl_add = 0;
	sIncl_Data.f32_XAccl_offset = sIncl_Data.f32_XAccl_offset * Gvalue;
	sIncl_Data.f32_YAccl_offset = sIncl_Data.f32_YAccl_offset * Gvalue;
	sIncl_Data.f32_ZAccl_offset = sIncl_Data.f32_ZAccl_offset * Gvalue;
}


