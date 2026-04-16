/***************************************************************************************************
* Copyright (C)2019
* File Name                             :     Inclination.c 
* Description                           :     重力加速器数据处理及倾斜角度处理
* Version                               :     
* Author                                :     YXD
* Creat Date                            :     20191108
* notice                                :     
****************************************************************************************************/
#include "InclinationCalc.h"
Inclination_Data sIncl_Data;

float accel[3];
float Angle[3];
float MpuTemp[1];

void sInclinationCalc_TSK(void);
void ADXL355_Acceleration_Data_Conversion(U32 *Var, I32 *Varr);


void XL355_Temp_Calc(void);

void XL355_Accel_Read(void);
void Mpu6050_Accel_Read(void);

void Tilt_Angle_Calc(Inclination_Data * pIncl);


const float Offset_X[81]=
{
    -0.0001074 ,
    -0.0000987 ,
    -0.0001022 ,
    -0.0001003 ,
    -0.0000995 ,
    -0.0001017 ,
    -0.0000934 ,
    -0.0000909 ,
    -0.0000911 ,
    -0.0000852 ,
    -0.0000903 ,
    -0.0000831 ,
    -0.0000792 ,
    -0.0000758 ,
    -0.0000551 ,
    -0.0000557 ,
    -0.0000507 ,
    -0.0000393 ,
    -0.0000424 ,
    -0.0000351 ,
    -0.0000331 ,
    -0.0000295 ,
    -0.0000179 ,
    -0.0000139 ,
    -0.0000023 ,
    -0.0000077 ,
    0.0000025 ,
    0.0000024 ,
    0.0000147 ,
    0.0000133 ,
    0.0000198 ,
    0.0000189 ,
    0.0000148 ,
    0.0000270 ,
    0.0000285 ,
    0.0000338 ,
    0.0000317 ,
    0.0000429 ,
    0.0000447 ,
    0.0000520 ,
    0.0000506 ,
    0.0000498 ,
    0.0000471 ,
    0.0000342 ,
    0.0000246 ,
    0.0000094 ,
    -0.0000043 ,
    -0.0000019 ,
    0.0000427 ,
    0.0000293 ,
    0.0000198 ,
    0.0000364 ,
    0.0000621 ,
    0.0000703 ,
    0.0000545 ,
    0.0000433 ,
    0.0000292 ,
    0.0000222 ,
    0.0000043 ,
    -0.0000060 ,
    -0.0000077 ,
    -0.0000173 ,
    -0.0000181 ,
    -0.0000281 ,
    -0.0000386 ,
    -0.0000585 ,
    -0.0000780 ,
    -0.0000871 ,
    -0.0000979 ,
    -0.0001035 ,
    -0.0001263 ,
    -0.0001431 ,
    -0.0001675 ,
    -0.0001983 ,
    -0.0002107 ,
    -0.0002421 ,
    -0.0002583 ,
    -0.0002823 ,
    -0.0003100 ,
    -0.0003575 ,
    -0.0004286 
};

const float Offset_Y[81]=
{
    -0.0009121 ,
    -0.0008971 ,
    -0.0008738 ,
    -0.0008559 ,
    -0.0008384 ,
    -0.0008234 ,
    -0.0008044 ,
    -0.0007900 ,
    -0.0007751 ,
    -0.0007599 ,
    -0.0007381 ,
    -0.0007248 ,
    -0.0007047 ,
    -0.0006846 ,
    -0.0006694 ,
    -0.0006460 ,
    -0.0006293 ,
    -0.0006157 ,
    -0.0005852 ,
    -0.0005717 ,
    -0.0005483 ,
    -0.0005281 ,
    -0.0005124 ,
    -0.0004927 ,
    -0.0004712 ,
    -0.0004480 ,
    -0.0004294 ,
    -0.0004108 ,
    -0.0003864 ,
    -0.0003887 ,
    -0.0003972 ,
    -0.0003722 ,
    -0.0003573 ,
    -0.0003264 ,
    -0.0002964 ,
    -0.0002732 ,
    -0.0002476 ,
    -0.0002167 ,
    -0.0001930 ,
    -0.0001677 ,
    -0.0001453 ,
    -0.0001284 ,
    -0.0000952 ,
    -0.0000844 ,
    -0.0000598 ,
    -0.0000439 ,
    -0.0000004 ,
    0.0000351 ,
    0.0000526 ,
    0.0000727 ,
    0.0001173 ,
    0.0001465 ,
    0.0001718 ,
    0.0001929 ,
    0.0002185 ,
    0.0002418 ,
    0.0002754 ,
    0.0002944 ,
    0.0003203 ,
    0.0003529 ,
    0.0003775 ,
    0.0004045 ,
    0.0004230 ,
    0.0004377 ,
    0.0004252 ,
    0.0004415 ,
    0.0004815 ,
    0.0005113 ,
    0.0005191 ,
    0.0005192 ,
    0.0005424 ,
    0.0005580 ,
    0.0005613 ,
    0.0005855 ,
    0.0005801 ,
    0.0006001 ,
    0.0006188 ,
    0.0006334 ,
    0.0006633 ,
    0.0006911 ,
    0.0006798 
};

const float Offset_Z[81]=
{
    0.0006038 ,
    0.0005531 ,
    0.0005626 ,
    0.0005367 ,
    0.0004981 ,
    0.0004764 ,
    0.0004735 ,
    0.0004529 ,
    0.0004446 ,
    0.0004041 ,
    0.0003917 ,
    0.0003712 ,
    0.0003610 ,
    0.0003272 ,
    0.0003015 ,
    0.0002886 ,
    0.0002734 ,
    0.0002504 ,
    0.0002280 ,
    0.0002040 ,
    0.0001947 ,
    0.0001711 ,
    0.0001342 ,
    0.0001152 ,
    0.0000909 ,
    0.0000882 ,
    0.0000767 ,
    0.0000497 ,
    0.0000337 ,
    0.0000186 ,
    0.0000203 ,
    0.0000020 ,
    -0.0000027 ,
    -0.0000425 ,
    -0.0000502 ,
    -0.0000709 ,
    -0.0001076 ,
    -0.0001259 ,
    -0.0001588 ,
    -0.0001482 ,
    -0.0001471 ,
    -0.0001216 ,
    -0.0000858 ,
    -0.0000473 ,
    0.0000211 ,
    0.0001116 ,
    0.0000406 ,
    -0.0000470 ,
    -0.0000486 ,
    0.0000050 ,
    -0.0000432 ,
    -0.0001567 ,
    -0.0001481 ,
    -0.0001030 ,
    -0.0000637 ,
    -0.0000228 ,
    0.0000300 ,
    0.0000708 ,
    0.0001266 ,
    0.0001633 ,
    0.0002079 ,
    0.0002573 ,
    0.0003414 ,
    0.0004113 ,
    0.0005321 ,
    0.0006535 ,
    0.0007905 ,
    0.0008933 ,
    0.0010768 ,
    0.0012041 ,
    0.0013631 ,
    0.0015225 ,
    0.0016992 ,
    0.0018358 ,
    0.0020036 ,
    0.0021793 ,
    0.0023825 ,
    0.0025415 ,
    0.0026941 ,
    0.0029050 ,
    0.0030673 
};




/*******************************************************************************
* Function Name  : sInclinationCalc_TSK()
* Description    : 计算倾角任务函数  
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/

void sInclinationCalc_TSK(void)
{
  g_SysData.u8_SensorType = SensorType_MPU6050;

    //根据机种初始化
//    if(g_SysData.u8_SensorType == SensorType_MPU6050)//
//    {
//        if(MPU6050DataGet(accel, MpuTemp)==0)
//        {
//            sIncl_Data.f32_Accl_Xaxis = -accel[1];    //由于MPU6050和ADXL355XYZ朝向不一致
//            sIncl_Data.f32_Accl_Yaxis = accel[0];
//            sIncl_Data.f32_Accl_Zaxis = accel[2];
//            sIncl_Data.f32_MPU6050_Temp = *MpuTemp;
//            Tilt_Angle_Calc(&sIncl_Data);
//        }
//    }
     if(g_SysData.u8_SensorType == SensorType_ADXL355)
    {
        sXD355_DATA.ADXLDATA.u8_POWER_CTL  = SPI_ADXL355_S_READ(XL355_POWER_CTL);
        //  delay_us(2);
        sXD355_DATA.ADXLDATA.u8_Status = SPI_ADXL355_S_READ(XL355_STATUS);
        sXD355_DATA.ADXLDATA.u8_Status &= 0x01;
        if(sXD355_DATA.ADXLDATA.u8_Status == 1)
        {
            XL355_Accel_Read();
            XL355_Temp_Calc();
            Tilt_Angle_Calc(&sIncl_Data);
        }
        sXD355_DATA.ADXLDATA.u8_Status = 0x00;
    }
    else
    {
        if(MPU6050DataGet(accel, MpuTemp)==0)
        {
            sIncl_Data.f32_Accl_Xaxis = -accel[1];
            sIncl_Data.f32_Accl_Yaxis = accel[0];
            sIncl_Data.f32_Accl_Zaxis = accel[2];
            sIncl_Data.f32_MPU6050_Temp = *MpuTemp;
            Tilt_Angle_Calc(&sIncl_Data);
        }
    }

}


void XL355_Temp_Calc(void)
{

    float  f32temp;
    U32    u32_temp;

    u32_temp = SPI_ADXL355_M_READ(XL355_TEMP2,SPI_READ_TWO_REG);
    u32_temp = u32_temp & 0x0fff;
    sIncl_Data.u32_XD355_Temprature = u32_temp;
    u32_temp = 0;
    f32temp = ((((float)sIncl_Data.u32_XD355_Temprature - ADXL355_TEMP_BIAS)) / ADXL355_TEMP_SLOPE) + 25.0;
    sIncl_Data.f32_XD355_Temprature = f32temp;

}




void Mpu6050_Accel_Read(void)
{
    float    Mpu6050AccBuff[3];

    // 向Mpu6050读加速度值
    MPU6050ReadAcc(Mpu6050AccBuff);
    sIncl_Data.f32_Accl_Xaxis = Mpu6050AccBuff[0];
    sIncl_Data.f32_Accl_Yaxis = Mpu6050AccBuff[1];
    sIncl_Data.f32_Accl_Zaxis = Mpu6050AccBuff[2];

}


void XL355_Accel_Read(void)
{
    U32    u32_X_temp;
    U32    u32_Y_temp;
    U32    u32_Z_temp;


    // 向ADXL355读加速度值
    u32_X_temp = SPI_ADXL355_M_READ(XL355_XDATA3,SPI_READ_THREE_REG);
    //delay_us(2);
    u32_Y_temp = SPI_ADXL355_M_READ(XL355_YDATA3,SPI_READ_THREE_REG);
    //delay_us(2);
    u32_Z_temp = SPI_ADXL355_M_READ(XL355_ZDATA3,SPI_READ_THREE_REG);
    //delay_us(2);

    u32_X_temp = (u32_X_temp >> 4);
    u32_X_temp = (u32_X_temp & 0x000FFFFF);
    u32_Y_temp = (u32_Y_temp >> 4);
    u32_Y_temp = (u32_Y_temp & 0x000FFFFF);
    u32_Z_temp = (u32_Z_temp >> 4);
    u32_Z_temp = (u32_Z_temp & 0x000FFFFF);


    ADXL355_Acceleration_Data_Conversion(&u32_X_temp,&sXD355_DATA.ADXLDATA.i32_X_Accl_DATA);
    ADXL355_Acceleration_Data_Conversion(&u32_Y_temp,&sXD355_DATA.ADXLDATA.i32_Y_Accl_DATA);
    ADXL355_Acceleration_Data_Conversion(&u32_Z_temp,&sXD355_DATA.ADXLDATA.i32_Z_Accl_DATA);

    sIncl_Data.f32_Accl_Xaxis_orn = (sXD355_DATA.ADXLDATA.i32_X_Accl_DATA) * Cnst_G_convertion;
    sIncl_Data.f32_Accl_Yaxis_orn = (sXD355_DATA.ADXLDATA.i32_Y_Accl_DATA) * Cnst_G_convertion;
    sIncl_Data.f32_Accl_Zaxis_orn = (sXD355_DATA.ADXLDATA.i32_Z_Accl_DATA) * Cnst_G_convertion;

    sIncl_Data.f32_Accl_Xaxis_orn = sIncl_Data.f32_Accl_Xaxis_orn * 0.5f;
    sIncl_Data.f32_Accl_Yaxis_orn = sIncl_Data.f32_Accl_Yaxis_orn * 0.5f;
    sIncl_Data.f32_Accl_Zaxis_orn = sIncl_Data.f32_Accl_Zaxis_orn * 0.5f;

    //sIncl_Data.f32_Accl_Xaxis_orn = sIncl_Data.f32_Accl_Xaxis_orn * Cnst_GB_g;
    //sIncl_Data.f32_Accl_Yaxis_orn = sIncl_Data.f32_Accl_Yaxis_orn * Cnst_GB_g;
    //sIncl_Data.f32_Accl_Zaxis_orn = sIncl_Data.f32_Accl_Zaxis_orn * Cnst_GB_g;

    sIncl_Data.f32_Accl_Xaxis_orn = sIncl_Data.f32_Accl_Xaxis_orn * Cnst_Kx_Init + Cnst_Bx_Init;
    sIncl_Data.f32_Accl_Yaxis_orn = sIncl_Data.f32_Accl_Yaxis_orn * Cnst_Ky_Init + Cnst_By_Init;
    sIncl_Data.f32_Accl_Zaxis_orn = sIncl_Data.f32_Accl_Zaxis_orn * Cnst_Kz_Init + Cnst_Bz_Init;

    if((sIncl_Data.f32_XD355_Temprature <= (-30.0))&&(sIncl_Data.f32_XD355_Temprature >= (51.0)))
    {
        sIncl_Data.f32_Accl_Xaxis = sIncl_Data.f32_Accl_Xaxis_orn - 0.0;//Offset_X[(U16)sIncl_Data.f32_XD355_Temprature];
        sIncl_Data.f32_Accl_Yaxis = sIncl_Data.f32_Accl_Yaxis_orn - 0.0;
        sIncl_Data.f32_Accl_Zaxis = sIncl_Data.f32_Accl_Zaxis_orn - 0.0;
    }
    else
    {
        sIncl_Data.f32_Accl_Xaxis = sIncl_Data.f32_Accl_Xaxis_orn - Offset_X[((U16)sIncl_Data.f32_XD355_Temprature) + 30U];
        sIncl_Data.f32_Accl_Yaxis = sIncl_Data.f32_Accl_Yaxis_orn - Offset_Y[((U16)sIncl_Data.f32_XD355_Temprature)  + 30U];
        sIncl_Data.f32_Accl_Zaxis = sIncl_Data.f32_Accl_Zaxis_orn - Offset_Z[((U16)sIncl_Data.f32_XD355_Temprature)  + 30U];
    }


    u32_X_temp = 0;
    u32_Y_temp = 0;
    u32_Z_temp = 0;

}



void ADXL355_Acceleration_Data_Conversion(U32 *Var, I32 *Varr)
{
    if((*Var & 0x00080000)  == 0x00080000)
    {

        *Varr = (*Var | 0xFFF00000);

    }
    else
    {
        *Varr = *Var;
    }
}



void Tilt_Angle_Calc(Inclination_Data * pIncl)
{

    pIncl->f32_shunxian_Angle = atan2(pIncl->f32_Accl_Xaxis,sqrt(pow(pIncl->f32_Accl_Yaxis,2) + pow(pIncl->f32_Accl_Zaxis,2)));
    pIncl->f32_hengxian_Angle = atan2(pIncl->f32_Accl_Yaxis,sqrt(pow(pIncl->f32_Accl_Xaxis,2) + pow(pIncl->f32_Accl_Zaxis,2)));
    pIncl->f32_zonghe_Angle   = atan2(sqrt(pow(pIncl->f32_Accl_Xaxis,2) + pow(pIncl->f32_Accl_Yaxis,2)),pIncl->f32_Accl_Zaxis);

    pIncl->f32_shunxian_Angle_arc = pIncl->f32_shunxian_Angle * Cnst_P_ArctoAngle;
    pIncl->f32_hengxian_Angle_arc = pIncl->f32_hengxian_Angle * Cnst_P_ArctoAngle;
    pIncl->f32_zonghe_Angle_arc   = pIncl->f32_zonghe_Angle   * Cnst_P_ArctoAngle;

    pIncl->f32_shunxian_Angle_arc_o =  pIncl->f32_shunxian_Angle_arc - pIncl->f32_shunxian_Angle_offset;

    pIncl->f32_hengxian_Angle_arc_o = pIncl->f32_hengxian_Angle_arc - pIncl->f32_hengxian_Angle_offset; 

    pIncl->f32_zonghe_Angle_arc_o  = pIncl->f32_zonghe_Angle_arc - pIncl->f32_zonghe_Angle_offset;

}


