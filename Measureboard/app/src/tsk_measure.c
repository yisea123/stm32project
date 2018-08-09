
#include <string.h>
#include <stdint.h>
#include "shell_io.h"
#include "mxconstants.h"
#include "main.h"
#include "dev_spi.h"
#include "ad717x.h"
#include "dev_ad7172.h"
#include "unit_meas_cfg.h"
#include "unit_temp_cfg.h"
#include "dev_temp.h"
#include <math.h>
#include "spi.h"
#include "tsk_measure.h"
#include "tsk_sch.h"
#include "unit_flow_act.h"
#include "unit_sys_diagnosis.h"
#define TIMES_PER_SECOND			8
#define MAX_AVG_NUM					16
#define MAX_AD_SIZE					(TIMES_PER_SECOND*MAX_AVG_NUM)//actual is 128
#define VALID_DATA_PER_CYCLE		2
#define SPI_TIME_DATA_RDY			0x07
#define SPI_MAX_DELAY_COUNT			0x1784//(0x1784*11)// normal is 0x1784; *2 as max
#define MAX_TICK_SPI_DELAY			12050u

static int32_t adValueMea[VALID_DATA_PER_CYCLE][MAX_AD_SIZE];
static int32_t adValueRef[VALID_DATA_PER_CYCLE][MAX_AD_SIZE];


extern __IO ENUM_MEA_STEP mea_step;
extern  SPI_HandleTypeDef hspi6;
extern SPI_HandleTypeDef hspi3;



static void HW_AD_DETECT_INIT(void)
{
	// Init AD7172
	DevAD_Init();
	osDelay(200);
	DevAD_SelectChn(AD7172_REF_ID, AD_CHN_LONG);
	DevAD_SelectChn(AD7172_MEA_ID, AD_CHN_LONG);
	ADDetectStart();
}



void StartAdc(void)
{
	SPI3_CS_L;
	SPI6_CS_L;
	SYNC_L;
	for (int32_t i=0; i<15; i++);
	SYNC_H;
}



static void GetADC_Data1(MEAS_DATA* _msgSt, int32_t idx)
{
	static 	int32_t adcIdx = 0;
	static 	MEAS_DATA msgData;
	int countTime = 0;

	// Wait util both SPI3 & SPI6 are low
	while (((SPI3_DRDY == GPIO_PIN_SET) || (SPI6_DRDY == GPIO_PIN_SET)) && (countTime<SPI_MAX_DELAY_COUNT))
		countTime++;
	adcIdx = (adcIdx/MAX_AVG_NUM) * MAX_AVG_NUM + mea_step*4 + idx;

	// Received a failing edge on DRDY, read data from the ADs
	uint16_t ret1  = DevAD_ReadData(AD7172_MEA_ID, &_msgSt->meas[adcIdx]);
	if(ret1 != OK)
	{

		ADDetectStop();
		TraceDBG(TSK_ID_AD_DETECT, "SPI_Mea failed once.-->error code %d\n",ret1);
		//init SPI  driver;
		HAL_SPI_DeInit(&hspi6);
		MX_SPI6_Init();
		ADDetectStart();
	}
	else
	{
		ret1 = DevAD_ReadData(AD7172_REF_ID, &_msgSt->ref[adcIdx]);
		if(ret1 != OK)
		{
			ADDetectStop();
			TraceDBG(TSK_ID_AD_DETECT, "SPI_REF failed once.-->error code %d\n",ret1);
			//init SPI  driver;
			HAL_SPI_DeInit(&hspi3);
			MX_SPI3_Init();
			ADDetectStart();
		}
	}
	Dia_UpdateDiagnosis(AD_SPI_FAILURE, (ret1 != OK));
	//CalcSPIRdyTime(taskID, countTime);
	if(ret1 == OK)
	{
		adcIdx++;
		if(adcIdx >= MAX_AD_SIZE)
		{
			msgData.meas = _msgSt->meas;
			msgData.ref = _msgSt->ref;
			msgData.len = MAX_AD_SIZE;
			MsgPush ( MEAS_CTL_ID, (uint32_t)&msgData, 0);

			adcIdx = 0;
			if(_msgSt->meas == &adValueMea[0][0])
			{
				_msgSt->meas = &adValueMea[1][0];
				_msgSt->ref = &adValueRef[1][0];
			}
			else
			{
				_msgSt->meas = &adValueMea[0][0];
				_msgSt->ref = &adValueRef[0][0];
			}
		}
	}
}


void StartAdTask(void const* argument)
{
	(void)argument; // pc lint
	osEvent evt;

	int32_t smpl_num = 1;

	int32_t idx = 0;
	MEAS_DATA msgData;
	//lint -e550

	const uint8_t taskID = TSK_ID_AD_DETECT;
	msgData.meas = &adValueMea[0][0];
	msgData.ref = &adValueRef[0][0];
	msgData.len = MAX_AD_SIZE;
	HW_AD_DETECT_INIT();

	/* Infinite loop */
	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = 0;

		// Wait until entering any data reading session
		evt = osSignalWait(AD_SAMPLING_ALL, osWaitForever);

		if(evt.status==osEventSignal)
		{
			smpl_num = 0;
			// Get sampling period
			switch (evt.value.signals)
			{
				case AD_SAMPLING_D0:
					smpl_num = 1;
					idx = 0;
					break;
				case AD_SAMPLING_D1:
					smpl_num = 1;
					idx = 1;
					break;
				case AD_SAMPLING_D2:
					smpl_num = 1;
					idx = 2;
					break;
				case AD_SAMPLING_D3:
					smpl_num = 1;
					idx = 3;
					break;
				case AD_SAMPLING_CHNL_SHORT:
					DevAD_SelectChn(AD7172_MEA_ID, AD_CHN_SHORT);
					DevAD_SelectChn(AD7172_REF_ID, AD_CHN_SHORT);
					break;
				case AD_SAMPLING_CHNL_LONG:
					DevAD_SelectChn(AD7172_MEA_ID, AD_CHN_LONG);
					DevAD_SelectChn(AD7172_REF_ID, AD_CHN_LONG);
					break;
				case AD_SAMPLING_RDY:
					smpl_num = 2;
					break;

				default:
					continue;
			}
			if(smpl_num == 1)
			{
				GetADC_Data1(&msgData, idx);
			}
		}
	}

}


