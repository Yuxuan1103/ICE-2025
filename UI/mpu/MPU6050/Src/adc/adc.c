#include "adc.h"

ADC_HandleTypeDef hadc;
ADC_ChannelConfTypeDef sConfig = {0};

static void MX_ADC_Init(void)
{

    hadc.Instance = ADC1;
    hadc.Init.OversamplingMode = DISABLE;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.SamplingTime = ADC_SAMPLETIME_160CYCLES_5;
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerFrequencyMode = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    hadc.State = HAL_ADC_STATE_RESET;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
    //Error_Handler();
    }
    /** Configure for the selected ADC regular channel to be converted. 
    */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
    //Error_Handler();
    }
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
    //Error_Handler();
    }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */
}

void ADC_Init(void)
{
    MX_ADC_Init();
    HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
}

uint16_t ADCDataGet(uint32_t CH)
{
    uint16_t data = 0;

    sConfig.Channel = CH;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;	// 设置通道
    HAL_ADC_ConfigChannel(&hadc, &sConfig);

    HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, 10);
    if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc), HAL_ADC_STATE_REG_EOC))
    {
        data = HAL_ADC_GetValue(&hadc);
    }
    HAL_ADC_Stop(&hadc);
        // 清除通道
    sConfig.Rank = ADC_RANK_NONE;	// 清除通道
    HAL_ADC_ConfigChannel(&hadc, &sConfig);
    return data;
}

uint16_t Buff_ADC_CHANNEL_0 = 0, Buff_ADC_CHANNEL_VREFINT = 0;
float voltage_data = 0;

float VoltageDataADC_get(void)
{
  	static float voltage_data = 0;
    //uint16_t Buff_ADC_CHANNEL_0 = 0, Buff_ADC_CHANNEL_VREFINT = 0;
    //float voltage_data = 0;
    Buff_ADC_CHANNEL_0 = ADCDataGet(ADC_CHANNEL_2);
    Buff_ADC_CHANNEL_VREFINT = ADCDataGet(ADC_CHANNEL_VREFINT);
    voltage_data = Buff_ADC_CHANNEL_0;
	  voltage_data = voltage_data * 1.26;
    voltage_data = voltage_data / 4096.0;
    //voltage_data = Buff_ADC_CHANNEL_0 * 0.00080078125;
    voltage_data = voltage_data * 2 * 4.2;//电压分压   
//     printf("电压= %f \n\r", voltage_data );	
     return voltage_data;
}

