#include "spi.h"

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  
  hspi1.State = HAL_SPI_STATE_RESET;


  
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    //Error_Handler();
  }
}

void SPI1_Init(void)
{
	MX_SPI1_Init();
}

void MX_SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;//主模式
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;//方向
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;//8位
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;//极性
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;//相位
    hspi2.Init.NSS = SPI_NSS_SOFT;//SPI_NSS_HARD_OUTPUT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;//速率
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;//高位在前
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;//失能中断
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 7;
    hspi2.State = HAL_SPI_STATE_RESET;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
    //Error_Handler();
    }
}

void SPI2_Init(void)
{
    MX_SPI2_Init();
}




/************************************************/
/*                LoRa模块初始                  */
/********************************************** */ 

