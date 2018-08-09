/*
 * tsk_meas_tim.c
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */



#include "tsk_measure.h"

#include "string.h"
#include "shell_io.h"

#include "main.h"
#include "ad717x.h"
#include "dev_ad7172.h"
#include "unit_meas_cfg.h"

#define REF_GIN_PIN_NUM			2

typedef enum _TIME_STATE
{
	e_ad_t1 = 0, //2000
	e_ad_t1_1,
	e_ad_t2 = e_ad_t1_1,//500
	e_ad_t3,
	e_ad_t4,
	e_ad_t5,
	e_ad_t5_1,
	e_ad_t6,
	e_ad_t7,
	e_ad_t7_1,
	e_ad_t8 = e_ad_t7_1,//not exist
	e_ad_t9,
	e_ad_t10,
	e_ad_t11,
	e_ad_t11_1,
	e_ad_t12,
	e_ad_tmax,
} E_AD_TIME_STATE;

extern TIM_HandleTypeDef htim9;
extern uint8_t gainMaskMeaSet[MEA_STEP_DONE];
extern uint8_t gainMaskRefSet[MEA_STEP_DONE];

static __IO E_AD_TIME_STATE ad_test_state = e_ad_t12;
__IO ENUM_MEA_STEP mea_step = MEA_STEP_START;

//first long, then short
static const PinInst LampPin[MEA_DATA_MAX]=
{
	//todo change as schematic is changed
	{LED_660_L_GPIO_Port,	LED_660_L_Pin},//ph13
	{LED_880_L_GPIO_Port,	LED_880_L_Pin},//ph14
	{LED_660_S_GPIO_Port,	LED_660_S_Pin},//pa8
	{LED_880_S_GPIO_Port,	LED_880_S_Pin},//pc9
};


//first long, then short
static const PinInst MeaGainPins[MEA_DATA_MAX][REF_GIN_PIN_NUM]=
{

	{
		{GAIN_L_MEA1_GPIO_Port,	GAIN_L_MEA1_Pin},//pk7
		{GAIN_L_MEA2_GPIO_Port,	GAIN_L_MEA2_Pin},//pk6
	},
	{
		{GAIN_L_MEA1_GPIO_Port,	GAIN_L_MEA1_Pin},//pk7
		{GAIN_L_MEA2_GPIO_Port,	GAIN_L_MEA2_Pin},//pk6
		},
	//short
	{
		{GAIN_S_MEA1_GPIO_Port,	GAIN_S_MEA1_Pin},//pg10
		{GAIN_S_MEA2_GPIO_Port,	GAIN_S_MEA2_Pin},//pg9
	},
	{
		{GAIN_S_MEA1_GPIO_Port,	GAIN_S_MEA1_Pin},//pg10
		{GAIN_S_MEA2_GPIO_Port,	GAIN_S_MEA2_Pin},//pg9
	},
};


//first long, then short
static const PinInst RefGainPins[MEA_DATA_MAX][REF_GIN_PIN_NUM]=
{
	{
		{GAIN_L_REF1_GPIO_Port,	GAIN_L_REF1_Pin},//pj13
		{GAIN_L_REF2_GPIO_Port,	GAIN_L_REF2_Pin},//pj12
	},
	{
		{GAIN_L_REF1_GPIO_Port,	GAIN_L_REF1_Pin},//pj13
		{GAIN_L_REF2_GPIO_Port,	GAIN_L_REF2_Pin},//pj12
	},
	//short
	{
		{GAIN_S_REF1_GPIO_Port,	GAIN_S_REF1_Pin},//pd3
		{GAIN_S_REF2_GPIO_Port,	GAIN_S_REF2_Pin},//pd2
	},
	{
		{GAIN_S_REF1_GPIO_Port,	GAIN_S_REF1_Pin},//pd3
		{GAIN_S_REF2_GPIO_Port,	GAIN_S_REF2_Pin},//pd2
	},
};


static const PinInst ledCurrentControl[MEA_DATA_MAX] =
{
		{LED_660_L_IC_GPIO_Port,LED_660_L_IC_Pin},
		{LED_880_L_IC_GPIO_Port,LED_880_L_IC_Pin},//??
		{LED_660_S_IC_GPIO_Port,LED_660_S_IC_Pin},//??
		{LED_880_S_IC_GPIO_Port,LED_880_S_IC_Pin},//??
};


void SetLedCurrentGain(void)
{
	for(uint8_t idx=0;idx<MEA_DATA_MAX;idx++)
	{
		GPIO_PinState pinVal = GPIO_PIN_RESET;
		if(ledCurrentCtrolSet[idx])
			pinVal = GPIO_PIN_SET;
		HAL_GPIO_WritePin(ledCurrentControl[idx].port, ledCurrentControl[idx].pin, pinVal);

	}

}

void SetLedOutMask(uint16_t id, uint16_t val)
{
	if(MEA_DATA_MAX <= id)
		return;
	const PinInst* Inst = &LampPin[id];
	GPIO_InitTypeDef GPIO_InitStruct;
	if(val != 0)//normal actions
	{
		HAL_GPIO_WritePin(Inst->port, Inst->pin, GPIO_PIN_SET);
		GPIO_InitStruct.Pin = Inst->pin;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(Inst->port, &GPIO_InitStruct);
	}
	else
	{
		HAL_GPIO_WritePin(Inst->port, Inst->pin, GPIO_PIN_SET);
		GPIO_InitStruct.Pin = Inst->pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(Inst->port, &GPIO_InitStruct);
	}
}



void InitMeasPins(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  for(uint8_t idx=0;idx<MEA_DATA_MAX;idx++)
	  {
		  const PinInst* Inst = &LampPin[idx];
		  HAL_GPIO_WritePin(Inst->port, Inst->pin, GPIO_PIN_SET);
		  GPIO_InitStruct.Pin = Inst->pin;
		  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		  GPIO_InitStruct.Pull = GPIO_NOPULL;
		  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		  HAL_GPIO_Init(Inst->port, &GPIO_InitStruct);
	  }
	  for(uint8_t idx=0;idx<MEA_DATA_MAX;idx++)
	  {
		  for(uint8_t idx1=0;idx1<MEA_DATA_MAX;idx1++)
		  {
			  const PinInst* Inst = &MeaGainPins[idx][idx1];
			  HAL_GPIO_WritePin(Inst->port, Inst->pin, GPIO_PIN_RESET);
			  GPIO_InitStruct.Pin = Inst->pin;
			  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			  GPIO_InitStruct.Pull = GPIO_NOPULL;
			  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			  HAL_GPIO_Init(Inst->port, &GPIO_InitStruct);
		  }
	  }
	  for(uint8_t idx=0;idx<MEA_DATA_MAX;idx++)
	  {
		  for(uint8_t idx1=0;idx1<MEA_DATA_MAX;idx1++)
		  {
			  const PinInst* Inst = &RefGainPins[idx][idx1];
			  HAL_GPIO_WritePin(Inst->port, Inst->pin, GPIO_PIN_RESET);
			  GPIO_InitStruct.Pin = Inst->pin;
			  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			  GPIO_InitStruct.Pull = GPIO_NOPULL;
			  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			  HAL_GPIO_Init(Inst->port, &GPIO_InitStruct);
		  }
	  }
}



static inline void OutPinOne(const PinInst* Inst, GPIO_PinState pinStatus)
{
	HAL_GPIO_WritePin(Inst->port, Inst->pin, pinStatus);
}

static void OutPin4(const PinInst* Inst, uint8_t num, const uint8_t status)
{
	for(uint8_t i=0;i<num;i++)
	{
		if(status & (0x01<<i))
		{
			HAL_GPIO_WritePin(Inst[i].port, Inst[i].pin, GPIO_PIN_SET);
		}
		else
		{
			HAL_GPIO_WritePin(Inst[i].port, Inst[i].pin, GPIO_PIN_RESET);
		}
	}
}

static void SetTim9Value(uint32_t t)
{
	TIM_OC_InitTypeDef sConfigOC;

	if (t > 0xFFFF)
	{
		htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 180MHz
		htim9.Init.Prescaler = 35999; // 180MHz -> 5KHz, 200us/count (prescaler must be less than 0xffff)
		t /= 200; // 200us/count -> 1ms/count
	}
	else
	{
		htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 180MHz
		htim9.Init.Prescaler = 179; // 180MHz -> 1MHz, 1us/count (prescaler must be less than 0xffff)
	}
	HAL_TIM_OC_Init(&htim9);

	// Only TIM9 channel 1 used
	sConfigOC.OCMode = TIM_OCMODE_TIMING;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.Pulse = t;

	HAL_TIM_OC_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1);
}



static void _ad_test_tim_clear(TIM_HandleTypeDef *htim)
{
//	HAL_TIM_OC_Stop_IT(htim, TIM_CHANNEL_1);
	__HAL_TIM_SET_COUNTER(htim, 0);
	__HAL_TIM_CLEAR_FLAG(htim, TIM_FLAG_CC1);
}



static void LampOnOff(ENUM_MEA_STEP step, int on_off)
{
	if(step < MEA_STEP_DONE)
	{
		if(ledOutMask[step] == 1)
		{
			if (on_off)
				OutPinOne(&LampPin[step],GPIO_PIN_RESET);
			else
				OutPinOne(&LampPin[step],GPIO_PIN_SET);
		}
		else if(ledOutMask[step] == 2)
		{
			OutPinOne(&LampPin[step],GPIO_PIN_RESET);
		}
		else if(ledOutMask[step] == 3)
		{
			OutPinOne(&LampPin[step],GPIO_PIN_SET);
		}
	}
}



static void SetADGain(ENUM_MEA_STEP step)
{
	if(step<MEA_STEP_DONE)
	{
		OutPin4(&MeaGainPins[step][0],	2, gainMaskMeaSet[step]);
		OutPin4(&RefGainPins[step][0],	2, gainMaskRefSet[step]);
	}
}



static uint16_t Tim9Delay[e_ad_tmax];



#define SYNC_DELAY 1990u
extern void StartAdc(void);
void Init_Tim9_Value(void)
{
	Tim9Delay[e_ad_t1] = SYNC_DELAY;
	Tim9Delay[e_ad_t1_1] = (uint16_t)(measTimeCfg[e_ad_test_t1] - SYNC_DELAY);
	Tim9Delay[e_ad_t3] = measTimeCfg[e_ad_test_t3];
	Tim9Delay[e_ad_t4] = measTimeCfg[e_ad_test_t4];
	Tim9Delay[e_ad_t5] = SYNC_DELAY;
	Tim9Delay[e_ad_t5_1] = (uint16_t)(measTimeCfg[e_ad_test_t5] - SYNC_DELAY);
	Tim9Delay[e_ad_t6] = measTimeCfg[e_ad_test_t6];
	Tim9Delay[e_ad_t7] =  SYNC_DELAY;
	Tim9Delay[e_ad_t7_1] = (uint16_t)(measTimeCfg[e_ad_test_t7] - SYNC_DELAY);
	Tim9Delay[e_ad_t9] = measTimeCfg[e_ad_test_t9];
	Tim9Delay[e_ad_t10] = measTimeCfg[e_ad_test_t10];
	Tim9Delay[e_ad_t11] = SYNC_DELAY;
	Tim9Delay[e_ad_t11_1] = (uint16_t)(measTimeCfg[e_ad_test_t11] - SYNC_DELAY);
	Tim9Delay[e_ad_t12] = measTimeCfg[e_ad_test_t12];
}
void HAL_TIM9_IT(TIM_HandleTypeDef *htim)
{


//	if ((&htim9 == htim) && (HAL_TIM_ACTIVE_CHANNEL_1 == htim->Channel)) // AD test timer
	{
		// Stop and clear active timer
		_ad_test_tim_clear(htim);

		if(ad_test_state<=e_ad_t12)
		{

			htim->Instance->CCR1 = Tim9Delay[ad_test_state];
			__HAL_TIM_ENABLE_IT(htim, TIM_IT_CC1);
			__HAL_TIM_ENABLE(htim);
			//TIM_CCxChannelCmd(htim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
		//	SET_TIM(htim,timeCfg[ad_test_state] );
			//SetTim9Value(timeCfg[ad_test_state]);
			//HAL_TIM_OC_Start_IT(htim, TIM_CHANNEL_1);
			// Big switch case for timer transition
			switch (ad_test_state)
			{
				case e_ad_t1:
				case e_ad_t5:
				case e_ad_t7://t7 start
					StartAdc();
					break;
				case e_ad_t11:
					LampOnOff(mea_step, 0);
					StartAdc();
					break;
				case e_ad_t1_1://t1 start
					SigPush(adDetTaskHandle, AD_SAMPLING_D0); // Unlock the task
				break;
				case e_ad_t4:
					SH_CIRCUIT_EN;
					break;
				case e_ad_t5_1://t5 start
					SigPush(adDetTaskHandle, AD_SAMPLING_D1);
					break;
				case e_ad_t6://t6 start
					SH_CIRCUIT_DIS;
					break;
				case e_ad_t7_1://t7 start
					// D2 reading start
					SigPush(adDetTaskHandle, AD_SAMPLING_D2);
					break;
				case e_ad_t9:
					// Lamp on
					LampOnOff(mea_step, 1);
					break;
				case e_ad_t10:
					SH_CIRCUIT_EN;
					break;
				case e_ad_t11_1:
					// D3 reading start
					SigPush(adDetTaskHandle, AD_SAMPLING_D3);
					break;
				case e_ad_t12:
					SH_CIRCUIT_DIS;
					break;
				default:
					break;
			}
		}
		++ad_test_state;
		// Update state
		if (ad_test_state > e_ad_t12)
		{
			ad_test_state = e_ad_t1;
			// Switch AD channels in advance
			if (mea_step == MEA_STEP_880nm_SHORT)
				SigPush(adDetTaskHandle, AD_SAMPLING_CHNL_LONG);
			else if(mea_step == MEA_STEP_880nm_LONG)
				SigPush(adDetTaskHandle, AD_SAMPLING_CHNL_SHORT);
			else
			{}

			if (++mea_step == MEA_STEP_DONE)
			{
				mea_step = MEA_STEP_START;
			//not stop
			//	_ad_test_tim_clear(htim);

			}
			SetADGain(mea_step);

		}
		// Start next timer
	}
}





void ADDetectStart(void)
{
	Init_Tim9_Value();
	// Kick off timer chain
	SetTim9Value(100);//100ms later to start
	SetADGain(MEA_STEP_START);
//	SigPush(adDetTaskHandle, AD_SAMPLING_D0); // Unlock the task
	HAL_TIM_OC_Start_IT(&htim9, TIM_CHANNEL_1);
}


void ADDetectStop(void)
{
	HAL_TIM_OC_Stop_IT(&htim9, TIM_CHANNEL_1);
}




