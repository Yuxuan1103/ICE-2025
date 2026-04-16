#include "mpu6050.h"
#include "bsp_i2c.h"
#include <math.h>

extern I2C_HandleTypeDef  hi2c1;

////ID = 11;   1
//#define  Kx  0.993219754938228
//#define  Bx -0.247311718979618
//#define  Ky  1.00030621618863
//#define  By -0.0400122486475452
//#define  Kz  0.987092257872816
//#define  Bz  1.48838706103353

//////ID = 12; 2
//#define  Kx  0.99697854461479
//#define  Bx -0.0659999796534985
//#define  Ky  0.999057012513699
//#define  By -0.0556974284476385
//#define  Kz  0.992058470711498
//#define  Bz  0.921076687132091


////ID = 13;  3
//#define  Kx  1.00199888553185
//#define  Bx -0.604355627808536
//#define  Ky  1.00523130577495
//#define  By -0.0371935583136731
//#define  Kz  0.995813518676584
//#define  Bz  0.779323659716294


////ID = 21;  4
//#define  Kx  0.997689026438759
//#define  Bx -0.207120241888687
//#define  Ky  1.00571100175999
//#define  By  0.0269027692970806
//#define  Kz  0.98873049022872
//#define  Bz  0.853571032214454

////ID = 22;  5
//#define  Kx  1.00525708424157
//#define  Bx -0.379635337863829
//#define  Ky  1.00009184516945
//#define  By -0.04290394015777
//#define  Kz  0.989189571115665
//#define  Bz  1.10106691160885

////ID = 23;  6
//#define  Kx  0.9979633401222
//#define  Bx -0.419144602851324
//#define  Ky  1.00111859679949
//#define  By -0.0779370827608397
//#define  Kz  0.97848335080625
//#define  Bz  1.56390993959363

////ID = 31;  7
//#define  Kx  1.00128225431548
//#define  Bx -0.598316211066212
//#define  Ky  1.00018881115313
//#define  By  0.0371570143343391
//#define  Kz  0.981138120219455
//#define  Bz  1.15342597412999

////ID = 32;  8
//#define  Kx  0.997546849075234
//#define  Bx -0.460068606793498
//#define  Ky  0.99199319776093
//#define  By -0.108028059236165
//#define  Kz  0.980539296613137
//#define  Bz  0.860423232778027

////ID = 33;  9
#define  Kx  0.994151750163579
#define  Bx -0.532219139450072
#define  Ky  1.0073443627262
#define  By -0.125364005941275
#define  Kz  0.98645635685562
#define  Bz  0.727758177270234

float XaOffset[80] = {0.231711479,0.241841634,0.247700071,0.247199975,0.253316741,0.260996708,0.269511847,0.27479185,
                      0.277916341,0.284351833,0.290924744,0.298575577,0.300744987,0.31275932,0.322353154,0.331184269,
                      0.338189055,0.343766611,0.352306491,0.362396521,0.36778001,0.374545589,0.38395934,0.391625198,
										  0.396784053,0.399706828,0.405909848,0.411448761,0.415225421,0.415008032,0.41412316,0.419947095,
                      0.421173529,0.419344887,0.422312469,0.422433806,0.422666237,0.419227807,0.415964772,0.411439959,
                      0.40428434,0.397903115,0.390821904,0.379287039,0.373236606,0.368734419,0.362065276,0.348203254,
                      0.347067102,0.342261249,0.328523375,0.317333121,0.311878777,0.311573533,0.311451612,0.299608258,
                      0.288784745,0.277319751,0.269538506,0.274353109,0.267449238,0.25948011,0.249819384,0.23903668,
                      0.232312489,0.245955288,0.231640308,0.219843856,0.214698899,0.213108791,0.209724245,0.197653051,
                      0.186595,0.188740313,0.169806874,0.169517222,0.175999974,0.16561014,0.163168541,0.157753934};
float YaOffset[80] = {-0.459764747,-0.455827522,-0.451313249,-0.444737843,-0.43401494,-0.424320002,-0.419420778,-0.417278962,
                      -0.414086339,-0.404750313,-0.39686277,-0.390940809,-0.381874163,-0.378282133,-0.369318189,-0.358757504,
                      -0.357534933,-0.352936723,-0.341197618,-0.335273595,-0.324094523,-0.317915332,-0.320036812,-0.31566677,
                      -0.310880025,-0.308340691,-0.308204552,-0.308287015,-0.30805061,-0.30630184,-0.304287979,-0.303135922,
                      -0.300291924,-0.302606225,-0.299175032,-0.296054514,-0.295585207,-0.299758796,-0.298757135,-0.295440674,
                      -0.29644686,-0.298220907,-0.296997726,-0.288530015,-0.288743806,-0.291056188,-0.290188285,-0.290590592,
                      -0.283306217,-0.277251937,-0.285861979,-0.283730587,-0.283178616,-0.281636106,-0.283292356,-0.277788307,
                      -0.27160468,-0.270757307,-0.274644508,-0.277797165,-0.273222287,-0.274863932,-0.266044298,-0.267776364,
                      -0.25664666,-0.255084697,-0.268255721,-0.261741612,-0.263253561,-0.262490528,-0.2562837,-0.25472982,
	                    -0.255121139,-0.249123319,-0.246856497,-0.245937889,-0.242830211,-0.243985443,-0.244670725,-0.239983419};

float ZaOffset[80] = {1.624302578,1.586373087,1.519566668,1.443475666,1.371741585,1.311159343,1.235064444,1.144791186,
                      1.069884062,0.995563961,0.912442621,0.817929369,0.72369994,0.642871178,0.562735747,0.477567373,
                      0.389302839,0.314305974,0.240384544,0.170028289,0.106545075,0.053568364,0.009241645,-0.029008309,
                      -0.074650236,-0.108004099,-0.161542875,-0.198012086,-0.218873002,-0.236262915,-0.266055924,
	                    -0.288417668,-0.286022173,-0.290163818,-0.286783599,-0.291252837,-0.285095139,-0.276154153,
                      -0.262480351,-0.239513091,-0.234791082,-0.22330613,-0.200138855,-0.186802779,-0.166836216,-0.160717464,
	                    -0.143556934,-0.128823911,-0.117751285,-0.111647043,-0.10439942,-0.092921061,-0.078552813,-0.07539169,
	                    -0.077473913,-0.060678767,-0.045774926,-0.04118336,-0.037275798,-0.034149904,-0.018289861,-0.005372604,
	                    -0.009033729,0.003534371,-0.005344082,0.013525169,0.024548308,0.042817213,0.04098086,0.048638863,0.057807773,
                       0.066008047,0.075818829,0.0802078,0.091105026,0.094659266,0.095248876,0.105283256,0.118928336,0.123937425};

void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
	while(HAL_I2C_Mem_Write(&hi2c1, (MPU6050_ADDRESS_AD0_LOW<<1), reg, I2C_MEMADD_SIZE_8BIT, &data,1, I2CT_LONG_TIMEOUT));
}

void MPU6050_Init(void)
{
  uint16_t i=0,j=0;

  for(i=0;i<1000;i++)
  {
    for(j=0;j<1000;j++)
    {
      ;
    }
  }
  MPU6050_WriteReg(MPU6050_RA_PWR_MGMT_1, 0x00);     //时钟电源
	MPU6050_WriteReg(MPU6050_RA_SMPLRT_DIV , 0x07);  //陀螺仪输出分频采样频率 0-7  采样率=输出率/(1+RA_SMPLRT_DIV)
	MPU6050_WriteReg(MPU6050_RA_CONFIG , 0x06);        //外部采样和数字低通滤波器 决定陀螺仪输出率
	MPU6050_WriteReg(MPU6050_RA_ACCEL_CONFIG , 0x01);  //加速度计的量程 +/-2g 开启高通滤波 5HZ
	MPU6050_WriteReg(MPU6050_RA_GYRO_CONFIG, 0x18);  //陀螺仪量程与自检  
	
	 /******* 自由落体中断设置*************/
//  	MPU6050_WriteReg(MPU6050_RA_FF_THR , 0x00);     // 自由落体加速度阈值
//  	MPU6050_WriteReg(MPU6050_RA_FF_DUR , 0x00);     // 自由落体加速度时间阈值
	
	/******** 运动检测中断设置*************/
	  MPU6050_WriteReg(MPU6050_RA_MOT_THR , 0x01);     // 运动检测加速度阈值 1LSB=2mg
  	MPU6050_WriteReg(MPU6050_RA_MOT_DUR , 0x01);     // 运动检测加速度时间阈值
	
		/******** 零运动检测中断设置*************/
//	  MPU6050_WriteReg(MPU6050_RA_ZRMOT_THR , 0x00);     // 零运动检测加速度阈值
//  	MPU6050_WriteReg(MPU6050_RA_ZRMOT_DUR , 0x00);     // 零运动检测加速度时间阈值
	
		/****中断引脚配置寄存器bit7:高电平设置为1 bit6:推挽1 bit5：中断锁存 bit4:锁存清除******/
 	MPU6050_WriteReg(MPU6050_RA_INT_PIN_CFG , 0x00);
	
	/****中断使能 bit0:数据就绪；3：IIC主机所以中断源；4：FIFO溢出；5：零运动检测；6：运动检测；7:自由落体**************/
     MPU6050_WriteReg(MPU6050_RA_INT_ENABLE , 0x50);    
	
}


int Sensors_I2C_WriteRegister(unsigned char slave_addr,
                                        unsigned char reg_addr,
                                        unsigned short len,
                                        const unsigned char *data_ptr)
{
  char retries=0;
  int ret = 0;
  unsigned short retry_in_mlsec = HAL_GetTick();

tryWriteAgain:
  ret = 0;
  ret =	HAL_I2C_Mem_Write(&hi2c1, (MPU6050_ADDRESS_AD0_LOW<<1), reg_addr, I2C_MEMADD_SIZE_8BIT, ( uint8_t *)data_ptr, len, 0xff);//I2CT_LONG_TIMEOUT);
  //ret = ST_Sensors_I2C_WriteRegister( slave_addr, reg_addr, len, ( unsigned char *)data_ptr);
	
  if(ret && retry_in_mlsec)
  {
    if( retries++ > 4 )  //4????
        return ret;

    HAL_Delay(retry_in_mlsec);
    goto tryWriteAgain;
  }
  return ret;
}


int Sensors_I2C_ReadRegister(unsigned char slave_addr,
							        unsigned char reg_addr,
							        unsigned short len,
							        const unsigned char *data_ptr)
{
	char retries=0;
	int ret = 0;
 	unsigned short retry_in_mlsec = HAL_GetTick();
											
tryReadAgain:
 	ret = 0;
	ret = HAL_I2C_Mem_Read(&hi2c1, ((MPU6050_ADDRESS_AD0_LOW<<1)+1), reg_addr, I2C_MEMADD_SIZE_8BIT, ( uint8_t *)data_ptr, len, I2CT_LONG_TIMEOUT);
 	//ret = ST_Sensors_I2C_ReadRegister( slave_addr, reg_addr, len, ( unsigned char *)data_ptr);
											
  	if(ret && retry_in_mlsec)
 	{
		if( retries++ > 4 )
			return ret;
		HAL_Delay(retry_in_mlsec);
		goto tryReadAgain;
	}

	return ret;
							       
}
int get_tick_count(unsigned long *count)
{
        count[0] = (unsigned long)HAL_GetTick;
	return 0;
}
uint8_t mpu6050id(void)
{	
	uint8_t Re;
	HAL_I2C_Mem_Read(&hi2c1, ((MPU6050_ADDRESS_AD0_LOW<<1)+1), MPU6050_RA_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &Re, 1, I2CT_LONG_TIMEOUT);	
	return Re;
}	

/**
  * @brief   读取MPU6050的原始温度数据
  * @param   
  * @retval  
  */
void MPU6050ReadTemp(short *tempData)
{
	  uint8_t buf[2];
    //MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
	  Sensors_I2C_ReadRegister(MPU6050_ADDRESS_AD0_LOW, MPU6050_RA_TEMP_OUT_H, 2, buf);
    *tempData = (buf[0] << 8) | buf[1];
}

/**
  * @brief   读取MPU6050的温度数据，转化成摄氏度
  * @param   
  * @retval  
  */
void MPU6050_ReturnTemp(float *Temperature)
{
	short temp3;
	uint8_t buf[2];
	
	//MPU6050_ReadData(MPU6050_RA_TEMP_OUT_H,buf,2);     //读取温度值
	Sensors_I2C_ReadRegister(MPU6050_ADDRESS_AD0_LOW, MPU6050_RA_TEMP_OUT_H, 2, buf);
  temp3= (buf[0] << 8) | buf[1];	
	*Temperature=((double) temp3/340.0)+36.53;

}

/**
  * @brief   读取MPU6050的加速度数据
  * @param   
  * @retval  
  */


void MPU6050ReadAcc(float *accData)
{
    uint8_t buf[6];
    short databuf[3];
    float temp;
    uint8_t TempIndex;
  //  MPU6050_ReadData(MPU6050_ACC_OUT, buf, 6);

    Sensors_I2C_ReadRegister(MPU6050_ADDRESS_AD0_LOW, MPU6050_ACC_OUT, 6, buf);
    //accData[0] = ((buf[0] << 8) | buf[1])*0.00006103515625;
    //accData[1] = ((buf[2] << 8) | buf[3])*0.00006103515625;
    //accData[2] = ((buf[4] << 8) | buf[5])*0.00006103515625;
    databuf[0] = ((buf[0] << 8) | buf[1]);
    databuf[1] = ((buf[2] << 8) | buf[3]);
    databuf[2] = ((buf[4] << 8) | buf[5]);
//		accData[0] = databuf[0]*0.00006103515625*9.8 * Kx + Bx ;
//		accData[1] = databuf[1]*0.00006103515625*9.8 * Ky + By ;
//		accData[2] = databuf[2]*0.00006103515625*9.8 * Kz + Bz ;


     MPU6050_ReturnTemp(&temp);//获取温度
    //温漂查表
    if((-30 <= temp) && (49 >= temp)) TempIndex = (uint8_t)(temp + 30);
    else if(-30 > temp) TempIndex = 0;
    else if(49 < temp) TempIndex = 79;
    accData[0] = databuf[0]*0.00006103515625*9.8 ;//* XaK[TempIndex];//- XaOffset[TempIndex];
    accData[1] = databuf[1]*0.00006103515625*9.8 ;//* YaK[TempIndex];//- YaOffset[TempIndex];
    accData[2] = databuf[2]*0.00006103515625*9.8 ;//* ZaK[TempIndex];//- ZaOffset[TempIndex];
    accData[0] = accData[0] - XaOffset[TempIndex];//* Kx + Bx ;
    accData[1] = accData[1] - YaOffset[TempIndex];//* Ky + By ;
    accData[2] = accData[2] - ZaOffset[TempIndex];//* Kz + Bz ;

    accData[0] = accData[0] * Kx + Bx ;
    accData[1] = accData[1] * Ky + By ;
    accData[2] = accData[2] * Kz + Bz ;

    // accData[2] = accData[2] - ZaOffset[TempIndex];
    //if(accData[0]>=2) accData[0] = (accData[0] - 4);
    //if(accData[1]>=2) accData[1] = (accData[1] - 4);
    //if(accData[2]>=2) accData[2] = (accData[2] - 4);
}

/**
  * @brief   读取MPU6050的角加速度数据
  * @param   
  * @retval  
  */
void MPU6050ReadGyro(float *gyroData)
{
    uint8_t buf[6];
		short databuf[3];
	
    //MPU6050_ReadData(MPU6050_GYRO_OUT,buf,6);
	
	  Sensors_I2C_ReadRegister(MPU6050_ADDRESS_AD0_LOW, MPU6050_GYRO_OUT, 6, buf);
    databuf[0] = ((buf[0] << 8) | buf[1]);
    databuf[1] = ((buf[2] << 8) | buf[3]);
    databuf[2] = ((buf[4] << 8) | buf[5]);
		gyroData[0] = databuf[0]*0.06097560975  ;
		gyroData[1] = databuf[1]*0.06097560975  ;
		gyroData[2] = databuf[2]*0.06097560975  ;
}

#define P  (double)57.2957795;
#define WindowLong 10
float databuffXa[WindowLong+1];
float databuffYa[WindowLong+1];
float databuffZa[WindowLong+1];
uint8_t WindowIndex = 0;

void SimpleLeapWindow(float *AccelInput,float *AccelOutput, uint32_t windowlong)
{
	uint8_t i;
	
	databuffXa[0] = AccelInput[0];
	databuffYa[0] = AccelInput[1];
	databuffZa[0] = AccelInput[2];
	//WindowIndex++;
	databuffXa[windowlong] = 0;
	databuffYa[windowlong] = 0;
	databuffZa[windowlong] = 0;
	for(i = 0; i < windowlong - 1; i++)
	{
		//移动
		databuffXa[windowlong-1-i] = databuffXa[windowlong-2-i];
		databuffYa[windowlong-1-i] = databuffYa[windowlong-2-i];
		databuffZa[windowlong-1-i] = databuffZa[windowlong-2-i];
		//累加
		databuffXa[windowlong] +=  databuffXa[windowlong-1-i];
		databuffYa[windowlong] +=  databuffYa[windowlong-1-i];
		databuffZa[windowlong] +=  databuffZa[windowlong-1-i];
	}
	AccelOutput[0] = databuffXa[windowlong]/(windowlong - 1);
	AccelOutput[1] = databuffYa[windowlong]/(windowlong - 1);
	AccelOutput[2] = databuffZa[windowlong]/(windowlong - 1);
}
uint8_t MPU6050GetAngle(float *angleX,float *angleY,float *angleYX)
{
    float acc[3],accel[3];
    MPU6050ReadAcc(acc);//获取加速度
    SimpleLeapWindow(acc,accel, WindowLong);//加速度滑窗滤波

    *angleX = atan2(accel[0],accel[2]) * P ;//顺线角度
    *angleY = atan2(accel[1],accel[2]) * P ;//横向角度
    *angleYX = atan2(sqrt(pow(accel[0],2) + pow(accel[1],2)),accel[2])* P;//综合角

    return 1;
}
uint8_t MPU6050DataGet(float *accel, float *temperature)
{
    float acc[3];
    MPU6050ReadAcc(acc);//获取加速度
    SimpleLeapWindow(acc,accel, WindowLong);//加速度滑窗滤波
    MPU6050_ReturnTemp(temperature);//获取温度

    return 0;
}

