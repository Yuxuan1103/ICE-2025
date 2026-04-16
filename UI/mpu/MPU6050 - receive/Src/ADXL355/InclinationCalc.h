
#ifndef INCLINATIONCALC_H_
#define INCLINATIONCALC_H_


#include "main.h"




#define ADXL355_TEMP_BIAS       (float)1852.0      /* Accelerometer temperature bias(in ADC codes) at 25 Deg C */
#define ADXL355_TEMP_SLOPE      (float)-9.05       /* Accelerometer temperature change from datasheet (LSB/degC) */

#define Cnst_GB_g                    (float)9.80665

#define G_Range 2                    //此处设置总的ADXL355测量范围

#if    (G_Range == 2)
#define Cnst_G_convertion            (float)0.0000078678206
#define XL355_Range_Set              0xC1
#elif  (G_Range == 4)
#define Cnst_G_convertion            (float)0.0000157356412
#define XL355_Range_Set              0x82
#elif  (G_Range == 8)
#define Cnst_G_convertion            (float)0.0000314712824
#define XL355_Range_Set              0x83
#endif




#define Cnst_G_offset                (float)1.0
#define Cnst_P_ArctoAngle            (float)57.295779578

#define Cnst_Kx_Init                 (float)0.9823
#define Cnst_Ky_Init                 (float)0.9804
#define Cnst_Kz_Init                 (float)0.9592
#define Cnst_Bx_Init                 (float)0.0108
#define Cnst_By_Init                 (float)0.0029
#define Cnst_Bz_Init                 (float)0.0312






typedef struct
{

    U32    u32_XD355_Temprature;
    float  f32_XD355_Temprature;

    float  f32_MPU6050_Temp;
    U32    u32_Accl_Xaxis;
    U32    u32_Accl_Yaxis;
    U32    u32_Accl_Zaxis;
    float  f32_Accl_Xaxis;
    float  f32_Accl_Yaxis;
    float  f32_Accl_Zaxis;


    float  f32_Accl_Xaxis_orn;
    float  f32_Accl_Yaxis_orn;
    float  f32_Accl_Zaxis_orn;

    float  f32_shunxian_Angle;
    float  f32_hengxian_Angle;
    float  f32_zonghe_Angle;
    
    float  f32_shunxian_Angle_arc;
    float  f32_hengxian_Angle_arc;
    float  f32_zonghe_Angle_arc;

    float  f32_shunxian_Angle_arc_o;
    float  f32_hengxian_Angle_arc_o;
    float  f32_zonghe_Angle_arc_o;
    
    float  f32_shunxian_Angle_add;
    float  f32_hengxian_Angle_add;
    float  f32_zonghe_Angle_add;
    float  f32_shunxian_Angle_offset;
    float  f32_hengxian_Angle_offset;
    float  f32_zonghe_Angle_offset;

    float  f32_shunxian_Angle2;
    float  f32_hengxian_Angle2;
    float  f32_zonghe_Angle2;
	
    float  f32_XAccl_add;
    float  f32_YAccl_add;
    float  f32_ZAccl_add;
    float  f32_XAccl_offset;
    float  f32_YAccl_offset;
    float  f32_ZAccl_offset;


    U16    u16_MainPower_Vol;
    float    f32_MainPower_Vol;

}Inclination_Data;


typedef struct
{
    float f32_Xoffset;
    float f32_Yoffset;
    float f32_Zoffset;
}ntc_t;


extern void sInclinationCalc_TSK(void);
extern void XL355_Temp_Calc(void);

extern void XL355_Accel_Read(void);

extern void Tilt_Angle_Calc(Inclination_Data * pIncl);


extern Inclination_Data sIncl_Data;


#endif /* PARAMETER_H_ */

