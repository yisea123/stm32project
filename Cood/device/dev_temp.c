/*
 * dev_temp.c
 *
 *  Created on: 2016��8��2��
 *      Author: pli
 */
#include "main.h"
#include "dev_eep.h"
#include "gpio.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include "dev_spi.h"
#include <string.h>
#include <math.h>


extern SemaphoreHandle_t spiLock;
#define ADC_TIMEOUT			10



#define A_FACTOR		0.0039083
#define B_FACTOR		-0.0000005775

#define A_FIX_FACTOR		0.39083f

#define	R0_CST				100.0f
#define GET_RESIST(t)		(R0_CST*(1+A_FACTOR*(t) + B_FACTOR*(t)*(t)))
#define GET_TEMP(r)			(((r)-R0_CST)/A_FIX_FACTOR)

#define A_F					-0.00005775
#define B_F					0.39083
#define C_F(r)				(100.0-(r))

#define C_FIX_F				3383.809523809524//== -B_F/2*A








uint16_t ADCValue[MAX_ADC_CHN];


//binary ((1<<15)|(7<<10)|(ch<<7)|(1<<6)|(1<<0))<<2
//0xF124
extern SPI_HandleTypeDef hspi5;
#define TEMP_SPI		&hspi5

#define TEMP_CS_H		HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_SET);
#define TEMP_CS_L		HAL_GPIO_WritePin(SPI5_CS_GPIO_Port, SPI5_CS_Pin, GPIO_PIN_RESET);
static uint8_t adStart = 0;
static uint16_t dmaSpi = 1;

static double GetTemp(double x)
{
	return ((sqrt(B_F*B_F-4*A_F*C_F(x))/2.0/A_F) + C_FIX_F );
}

static uint16_t ReadOnce(uint8_t* sendData,uint8_t* revData,uint8_t ch )
{

	uint16_t ret = OK;
	memset(revData,0,2);


	if(dmaSpi)
	{
		ret = SPI_Read_DMA(TEMP_SPI,sendData,revData,2);

#if 0
		OS_Use(spiLock);
		TEMP_CS_L;
		ret = HAL_SPI_TransmitReceive_DMA(TEMP_SPI,sendData,revData,2);
		osDelay(1);
		TEMP_CS_H;
		OS_Unuse(spiLock);
#endif
	}
	else
	{
		OS_Use(spiLock);
		TEMP_CS_L;
		ret = HAL_SPI_TransmitReceive(TEMP_SPI,sendData,revData,2,50);
		TEMP_CS_H;
		OS_Unuse(spiLock);
	}

	if(ret)
		TraceDBG(TSK_ID_TEMP_CTRL,"AD_SPI error:%d -> ch: %d\n",ret,ch);

	return ret;
}

static uint16_t ReadAd_Init(uint8_t* sendData,uint8_t* revData )
{
	uint16_t ret = OK;
	static uint16_t state = 0;
	TEMP_CS_H;
	osDelay(5);
	adStart = 10;
	if(state == 0)
	{
		for(uint8_t ch = 0; ch <2;ch++)
		{
			sendData[0] = 0xFF;
			sendData[1] = 0xFC;
			ret |= ReadOnce(sendData,revData,0xFF);
			osDelay(1);
		}
		state = 1;
	}
	for(uint8_t ch=0;ch<2;ch++)
	{
		sendData[0] = (uint8_t)(0xF1 | (ch<<1));
		sendData[1] = 0x24;
		ret |= ReadOnce(sendData,revData,(0x80 | ch));
		osDelay(1);
	}
	return ret;
}

static uint16_t ReadAd(uint16_t* data,uint16_t countChn)
{
	//for dma transfer
	static uint8_t sendData[4];
	static uint8_t revData[4] = {0,0,0,0};

	static uint8_t ch = 2;
	uint16_t ret = OK;


	if(adStart == 0)
	{
		ReadAd_Init(sendData,revData);
		ch = 2;
	}




	for(uint8_t ch1 = 0;ch1 <countChn;ch1++)
	{
		ch = (uint8_t)(ch%countChn);
		sendData[0] = (uint8_t)(0xF1 | (ch<<1));
		sendData[1] = 0x24;
		uint16_t ret1 = ReadOnce(sendData,revData,ch1);
		if(ret1 == OK)
		{
			uint16_t val = revData[0];
			data[ch1] = (uint16_t)((val<<8) | revData[1]);
		}
		else
		{
			ret = ret1;
			break;
		}
		ch++;
		osDelay(1);
	}
	if(ret)
	{
		//in case error;
	//	TraceDBG(TSK_ID_TEMP_CTRL,"AD_SPI error:%d \n",ret);
		adStart = 0;
	//	osDelay(10);
	//	ReadAd(data,countChn);
	}
//	adStart = 0;
	return ret;
}



static uint16_t tolerance = 1000;
static uint16_t chnMax = MAX_ADC_CHN;
#define MAX_TIMES_ADC	300

static uint16_t GetAllADCValue(void)
{
	static uint16_t currentAdc[MAX_ADC_CHN];
	uint16_t ret = 0;
	static uint32_t testId = 0;
	ret = ReadAd(&currentAdc[0],chnMax);
	while(ret != OK)
	{
		osDelay(5);
		ret = ReadAd(&currentAdc[0],chnMax);
	}
	if(OK == ret )
	{
		if(testId>MAX_TIMES_ADC)
		{
			for(uint16_t chn=0;  (chn<MAX_TEMP_CHN) && (chn < chnMax); chn++)
			{
				uint32_t test = currentAdc[chn];
				uint32_t testMax = ((uint32_t)ADCValue[chn]+ tolerance);
				uint32_t testMin = ((uint32_t)ADCValue[chn]- tolerance);

				if( (test > testMax) || (test < testMin))
				{
					TestPinSet(0,1);
					adStart = 0;
					testId = MAX_TIMES_ADC - 10;
					TraceDBG(TSK_ID_TEMP_CTRL,"temperature big: test:%d->%d,%04d,\t%04d,\t%04d,\t%04d,%04d,\t%04d,\t%04d,\t%04d,\n", \
								chn,test,\
								currentAdc[0],currentAdc[1],\
								currentAdc[2],currentAdc[3],\
								currentAdc[4],currentAdc[5],\
								currentAdc[6],currentAdc[7] );

					TraceDBG(TSK_ID_TEMP_CTRL,"temperature avg: %04d,\t%04d,\t%04d,\t%04d,%04d,\t%04d,\t%04d,\t%04d,\n\n", \
								ADCValue[0],ADCValue[1],\
								ADCValue[2],ADCValue[3],\
								ADCValue[4],ADCValue[5],\
								ADCValue[6],ADCValue[7] );
				}

			}
		}
		else
		{
			testId++;
			TestPinSet(0,0);
			adStart = 0;
		}
		for(uint16_t chn=0;  (chn < chnMax); chn++)
			ADCValue[chn] = (uint16_t) ( ADCValue[chn]/3*2 + currentAdc[chn]/3 );
	}
	else
	{
	}
	return ret;
}


void GetAllTemperature(void)
{
	if(spiLock == NULL)
		spiLock = OS_CreateSemaphore();
	GetAllADCValue();
}



extern int16_t hadc1080_Val[2];

int16_t GetTempChn(uint16_t chn)
{
	assert(chn <= 3);
	if(chn <= MAX_TEMP_CHN)
	{
		double resistor = GetResistor(chn)/100.0;
		return (int16_t)(GetTemp(resistor)*100.0);
	}
	else if(chn == 3)
	{
		return hadc1080_Val[0];
	}

	return 0;
}



float Get_Temp(uint16_t chn)
{

	const float Rp = 10000.0; //10K
	const float T2 = (273.15 + 25.0);
	//T2
	const float Bx = 3950.0; //B
	const float Ka = 273.15;
	float Rt;
	float temp;

	Rt = (4096.0*ADCValue[chn])/(4096.0-ADCValue[chn]);

//like this R=5000, T2=273.15+25,B=3470, RT=5000*EXP(3470*(1/T1-1/(273.15+25)),

	temp = Rt / Rp;
	temp = log(temp); //ln(Rt/Rp)
	temp /= Bx; //ln(Rt/Rp)/B
	temp += (1 / T2);
	temp = 1 / (temp);
	temp -= Ka;
	return temp;

}

uint16_t GetResistor(uint16_t chn)
{
	return (uint16_t)(((float)ADCValue[chn]*tempCalibration[chn].gain + tempCalibration[chn].offset));
}


void CalcGainOffset(RTS100* temp)
{
	if( (temp->caliState & 0x11) == 0x11)// data is ready;
	{
		temp->gain = (float)((float)temp->resist[1] - (float)temp->resist[0])/((float)temp->voltage[1] - (float)temp->voltage[0]);
		temp->offset = (float)temp->resist[0] - (float)temp->voltage[0] * temp->gain;
		temp->caliState |= 0x22;
		Trigger_EEPSave((void*)temp,sizeof(RTS100),SYNC_IM);
	}
}


uint16_t GetADCValueIdx(uint16_t chn)
{
	if(MAX_ADC_CHN> chn)	return (uint16_t)ADCValue[chn];
	return 0;
}

uint16_t GetVoltageIdx(uint16_t chn)
{
	if(MAX_ADC_CHN> chn) return (uint16_t)(ADCValue[chn]*0.0625);//4096/65536
	return 0;
}

