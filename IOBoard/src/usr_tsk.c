/*
 * usr_tsk.c
 *
 *  Created on: 2017骞�4鏈�27鏃�
 *      Author: pli
 */

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "bsp.h"
#include "tim.h"
#include "dev_eep.h"
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"

#include <string.h>




uint32_t serialNumber = 0x00121213;


typedef struct
{
	float offset;
	float gain;
	uint16_t status;
	uint16_t trigger;
} Calibration;


typedef struct
{
	Calibration AO_Calibration[2];
	uint32_t 	errCnt_Can;
	uint32_t 	errCnt_Nmt;

}EEPST;

#define RESERV_ADDR		( ( sizeof(EEPST)+3+16)/4*4)



#define 	CALIBRATION_AO		0x01L
#define 	MASK_CALI			0xFFFFFFFE
#define		EEP_ERROR			0x800L


volatile uint16_t eepChanged = 0;



static EEPST eepST =
{
		{
		{ 0.0f, 0.10f, 0, 0 },
		{ 0.0f, 0.10f, 0, 0 }, },
		0,
		0,
};

Calibration* ptrAOCali = &eepST.AO_Calibration[0];



void ResetCalibration(uint8_t chn)
{
	if(chn < 2)
	{
		ptrAOCali[chn].gain = 0.10f;
		ptrAOCali[chn].offset = 0.0f;
		ptrAOCali[chn].status = 0;
		eepChanged = 1;
	}
}

void InitEEP(void)
{

	uint16_t eepChange = 0;

	EEP_STATUS ret = EEP_Init();
	if(ret != EEP_OK)
	{
		ResetCalibration(0);
		ResetCalibration(1);
		eepChange = 1;
	}
	else
	{
		EEP_ReadAdr(0, (void*)&eepST, sizeof(eepST));
	}
	if(eepChange != 0)
	{
		eepChange = 0;
		EEP_WriteAdr(0, (void*)&eepST, RESERV_ADDR);
	}
}


uint16_t CheckValidation(float gain, float offset)
{
#define OFFSETMAX	3000.0f
#define SLOPEMAX	0.13f
#define SLOPEMIN	0.08f
	uint16_t ret = OK;
	if(offset <= (-1* OFFSETMAX))
	{
		ret = FATAL_ERROR;
	}
	else if(offset >= (OFFSETMAX))
	{
		ret = FATAL_ERROR;
	}
	else if(SLOPEMAX <= gain)
	{
		ret = FATAL_ERROR;
	}
	else if (SLOPEMIN >= gain)
	{
		ret = FATAL_ERROR;
	}
	else
	{}

	return ret;
}
uint16_t CalcGainOffset(const uint16_t setValue, const uint16_t measValue, uint8_t idx)
{
#define CURR_BASECAL_OFFS		0
#define CURR_BASECAL_GAIN		1

#define CURR_TRIMDAC_UPPER_INIT 	20000
#define CURR_TRIMDAC_LOWER_INIT		4000

	float pwm_upper_curr = 0;
	float pwm_lower_curr = 0;
	uint16_t ret = 1;
	float gainOffs[2];
	gainOffs[0] = ptrAOCali[idx].offset;
	gainOffs[1] = ptrAOCali[idx].gain;

	pwm_upper_curr = (CURR_TRIMDAC_UPPER_INIT + gainOffs[CURR_BASECAL_OFFS]) * gainOffs[CURR_BASECAL_GAIN];
	pwm_lower_curr = (CURR_TRIMDAC_LOWER_INIT + gainOffs[CURR_BASECAL_OFFS]) * gainOffs[CURR_BASECAL_GAIN];

	//  if (lower current value shall be trimmed)
	if (setValue == CURR_TRIMDAC_LOWER_INIT)
	{
		//  new resulting gain = (upper PWM current - lower PWM current)/(upper trim value - measValue)
		gainOffs[CURR_BASECAL_GAIN] = (pwm_upper_curr - pwm_lower_curr) /((float)(CURR_TRIMDAC_UPPER_INIT - measValue));

		//  new resulting offset = lower PWM current/new resulting gain - measValue
		gainOffs[CURR_BASECAL_OFFS] = pwm_lower_curr / gainOffs[CURR_BASECAL_GAIN] - measValue;
		ptrAOCali[idx].status |= 0x01;
		ret = 0;
	}
	//  else (if upper current value shall be trimmed)
	else if (setValue == CURR_TRIMDAC_UPPER_INIT)
	{
		//  new resulting gain = (upper PWM current - lower PWM current)/(measValue - lower trim value)
		gainOffs[CURR_BASECAL_GAIN] = (pwm_upper_curr - pwm_lower_curr) / (float)(measValue - CURR_TRIMDAC_LOWER_INIT);

		//  new resulting offset = upper PWM current/new resulting gain - measValue
		gainOffs[CURR_BASECAL_OFFS] = pwm_upper_curr / gainOffs[CURR_BASECAL_GAIN] - measValue;
		ptrAOCali[idx].status |= 0x10;
		ret = 0;
	}
	if(ret == 0)
	{
		if(OK == CheckValidation(gainOffs[1], gainOffs[0]) )
		{
			ptrAOCali[idx].offset = gainOffs[0];
			ptrAOCali[idx].gain = gainOffs[1];
		}
		else
		{
			ptrAOCali[idx].status = 0;
		}
		eepChanged = 1;
	}
	return ret;
}

void OutputAOWithClibration(int16_t ao, uint8_t idx)
{

	float pwm = (ao + ptrAOCali[idx].offset)*ptrAOCali[idx].gain;
	int16_t rawPwm = (int16_t)(pwm+0.5);
	if(rawPwm >= HW_MAX_PWM )
	{
		rawPwm = HW_MAX_PWM-1;
	}
	else if(pwm <= 0)
	{
		rawPwm = 0;
	}
	SetPwm(idx, rawPwm);
}


uint16_t caliStatus1 = 0x0;
uint16_t caliStatus2 = 0x0;
extern uint32_t errCnt_Can ;
extern uint32_t errCnt_Nmt;
void StartLogicTask(void const * argument)
{
	(void)argument;

	InitEEP();
	caliStatus1 = ptrAOCali[0].status;
	caliStatus2 = ptrAOCali[1].status;
	if(OK == CheckValidation(ptrAOCali[0].gain, ptrAOCali[0].offset) )
	{
	}
	else
	{
		ResetCalibration(0);
		eepST.errCnt_Can = 0;
		eepST.errCnt_Nmt = 0;
	}

	if(OK == CheckValidation(ptrAOCali[1].gain, ptrAOCali[1].offset) )
	{
	}
	else
	{
		ResetCalibration(1);
	}


	errCnt_Can = eepST.errCnt_Can;
	errCnt_Nmt = eepST.errCnt_Nmt;

	while(1)
	{
		if(eepChanged)
		{
			eepChanged = 0;
			eepST.errCnt_Can = errCnt_Can;
			eepST.errCnt_Nmt = errCnt_Nmt;
			EEP_WriteAdr(0, (void*)&ptrAOCali[0], RESERV_ADDR);
			caliStatus1 = ptrAOCali[0].status;
			caliStatus2 = ptrAOCali[1].status;

		}
		else
		{
		}
		osDelay(30);
	}

}
