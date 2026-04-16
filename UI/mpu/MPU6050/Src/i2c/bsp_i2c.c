#include "bsp_i2c.h"

I2C_HandleTypeDef  hi2c1;
/* STM32 I2C 快速模式 */
#define I2C_Speed              400000  //*

/* 这个地址只要与STM32外挂的I2C器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7      0X0A   
   
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance               = I2C2;
    hi2c1.Init.Timing            = 0x00300F38;
    hi2c1.Init.OwnAddress1       = I2Cx_OWN_ADDRESS7;
    hi2c1.Init.AddressingMode    = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode   = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2       = I2Cx_OWN_ADDRESS7;
    hi2c1.Init.OwnAddress2Masks  = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode   = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode     = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
    //Error_Handler();
    }
    /** Configure Analogue filter 
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
    //Error_Handler();
    }
    /** Configure Digital filter 
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
    {
    //Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */

}


/**
  * @brief  I2C 外设初始化
  * @param  无
  * @retval 无
  */
void I2C_Bus_Init(void)
{
    MX_I2C1_Init();
}
