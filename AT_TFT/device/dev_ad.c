/*
 * dev_ad.c
 *
 *  Created on: 2016��8��2��
 *      Author: pli
 */
#include "At_File.h"
#include "dev_ad.h"
#include "main.h"
#include "adc.h"
#include <string.h>
#include <math.h>
#include "dev_ad.h"
#include "t_unit.h"
#include "unit_parameter.h"
#include "parameter_idx.h"

#define ADC_TIMEOUT			10


#define AD_READY_ONCE	0x01


extern ADC_HandleTypeDef hadc1;
static uint16_t ADCValue;
static uint16_t tempAdcValue = 0;
static const uint32_t AllADCChn = ADC_CHANNEL_6;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc == &hadc1)
	{
		tempAdcValue = HAL_ADC_GetValue(&hadc1);
		SigPush(monitorAdTaskHandle, AD_READY_ONCE);
	}
}


#pragma GCC diagnostic ignored "-Waggregate-return"
static uint16_t GetADC1_Value(void)
{
	ADC_ChannelConfTypeDef sConfig;
	osEvent evt;
	static uint16_t adValueOld = 0;

	if (HAL_ADC_Start_IT(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
	//event = osMessageGet(MONITOR_TEMP_TSK_ID, 10 );
	evt = osSignalWait(AD_READY_ONCE, 10);
	if(evt.status==osEventSignal)
	{
		adValueOld = tempAdcValue;
	}
	return adValueOld;
}

#pragma GCC diagnostic pop

uint16_t GetAllADCValue(uint16_t* adValue, uint16_t avgNum)
{
//	uint32_t adcValue[ADC_MAX_CHN];
	static uint16_t idx = 0;
	uint16_t ret = 0;
	uint32_t ad= 0;

	if(idx >= avgNum)
	{
		idx = 0;
		ret = avgNum;
	}

	adValue[idx]=GetADC1_Value();

	idx++;

	if(ret)
	{
			ad = 0;
			for(uint16_t i =0; i<avgNum; i++)
			{
				ad = ad + adValue[i];
			}
			ad = (uint16_t)((ad+4)/avgNum);
			ADCValue = (uint16_t)((uint16_t)((ADCValue*2+2)/3) + (uint16_t)((ad +2)/3));
	}

	return ret;

}

#define AD_MIN_AV			7
#define AD_MAX_AV			13
	static uint16_t admin = AD_MIN_AV;
	static uint16_t admax = AD_MAX_AV;


void UpdateSpeed(void)
{
#define MAX_DIFF		14
#define SPEED_FACTOR	0.078125f


	static uint16_t adcValue = 0;
	float factor = (float)(parameterStaticRare.speedMax)/(40960.0);

	if(abs(adcValue-ADCValue) <= MAX_DIFF)
	{
	}
	else
	{
		adcValue = ADCValue;
	}
	int16_t val = (int16_t)(adcValue * factor);
	if(val <= admax)
	{
		if(val <= admin)
			val = 0;
		else
			val = (val - admin)*admax/(float)(admax-admin);

	}



	parameter_Put(PARA_IDX_weldSpeed,WHOLE_OBJECT,&val);
}



#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Waggregate-return"
void MonitorADTask(void const * argument)
{
#define MAX_AVG_ADC			8
#define AVG_TEMP_TIME		70

	uint16_t ADCValueTemp[MAX_AVG_ADC];
	uint16_t count = 0;
	uint16_t tick = 0;
	while(1)
	{
		while(GetAllADCValue(&ADCValueTemp, MAX_AVG_ADC) > 0);
		osDelay(15);
		UpdateSpeed();
	}
}
#pragma GCC diagnostic pop


