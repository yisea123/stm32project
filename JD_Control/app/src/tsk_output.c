/*
 * tsk_interrupt.c
 *
 *  Created on: 2019��3��27��
 *      Author: pli
 */


#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_ad5689.h"
#include "dev_ad7190.h"

static const PinInst digitInputPins[CHN_IN_MAX] =
{
		{IN1_GPIO_Port, IN1_Pin},
		{IN2_GPIO_Port, IN2_Pin},
		{IN3_GPIO_Port, IN3_Pin},
		{IN4_GPIO_Port, IN4_Pin},
		{IN5_GPIO_Port, IN5_Pin},
};

#if 0
static const PinInst digitOutputPins[CHN_OUT_MAX] =
{
		{OUT1_GPIO_Port, OUT1_Pin},
		{OUT2_GPIO_Port, OUT2_Pin},
		{OUT3_GPIO_Port, OUT3_Pin},
		{OUT4_GPIO_Port, OUT4_Pin},
};
#endif
static const PinInst digitOutputPins_Relay[CHN_OUT_MAX] =
{
		{RO_1_GPIO_Port, RO_1_Pin},
		{RO_2_GPIO_Port, RO_2_Pin},
		{RO_3_GPIO_Port, RO_3_Pin},
		{RO_4_GPIO_Port, RO_4_Pin},
		{RO_5_GPIO_Port, RO_5_Pin},
		{RO_6_GPIO_Port, RO_6_Pin},
		{RO_7_GPIO_Port, RO_7_Pin},
		{RO_8_GPIO_Port, RO_8_Pin},
		{RO_9_GPIO_Port, RO_9_Pin},
		{RO_10_GPIO_Port, RO_10_Pin},
		{RO_11_GPIO_Port, RO_11_Pin},
		{RO_12_GPIO_Port, RO_12_Pin},
		{RO_13_GPIO_Port, RO_13_Pin},
		{RO_14_GPIO_Port, RO_14_Pin},
		{RO_15_GPIO_Port, RO_15_Pin},
		{RO_16_GPIO_Port, RO_16_Pin},
};

static void OutputPins(uint32_t _digitOutput, uint16_t chnNum)
{
	assert(chnNum <= CHN_OUT_MAX);
	for(uint16_t i=0; i< chnNum;i++)
	{
		if( (_digitOutput & (1<<i)) != 0)
			HAL_GPIO_WritePin(digitOutputPins_Relay[i].port, digitOutputPins_Relay[i].pin, GPIO_PIN_SET );
		else
			HAL_GPIO_WritePin(digitOutputPins_Relay[i].port, digitOutputPins_Relay[i].pin, GPIO_PIN_RESET );
	}
}


void SetDAOutputFloat(uint16_t chn, float val)
{
	assert(chn < CHN_DA_MAX);
	if(val < 0)
		val = -val;
	val = (val*6553.6f);
	if(val >= 65536)
		val = 65535;
	daOutputRawDA[chn] = (uint16_t)(val);
	SigPush(outputTaskHandle, (DA_OUT_REFRESH_SPEED|DO_OUT_REFRESH));
}


uint32_t GetInputPins(void)
{
#define MAX_SAMPLE_NUM			3
	static uint32_t cnt = 0;
	static uint32_t resultPins = 0;
	static uint32_t lastInputPins[MAX_SAMPLE_NUM] = {0,0,0};
	uint32_t inputPins = 0;
	for(uint16_t i=0; i< CHN_IN_MAX;i++)
	{
		GPIO_PinState state = HAL_GPIO_ReadPin(digitInputPins[i].port, digitInputPins[i].pin);

		if(state == GPIO_PIN_RESET)
			inputPins = (inputPins | (1<<i));
	}
	lastInputPins[cnt%MAX_SAMPLE_NUM] = inputPins;
	cnt++;
	if((lastInputPins[0] == lastInputPins[1]) &&(lastInputPins[1] == lastInputPins[2]))
	{
		resultPins = lastInputPins[0];
	}
	return resultPins;
}

void StartOutputTsk(void const * argument)
{
	(void)argument; // pc lint
	osEvent evt;
	const uint8_t taskID = TSK_ID_OUTPUT;
	uint32_t valMsg = 0;
	TracePrint(taskID,"started  \n");
	OutputPins(digitOutput, CHN_OUT_MAX);
#if USE_EXT_DEV
	AD5689_Init();
	uint16_t data = 0x1000;
	AD5689_WriteUpdate_DACREG(DAC_A,data);
	AD5689_WriteUpdate_DACREG(DAC_B,data);
#endif
//	TraceUser("data:%d\n",data);
	while (TASK_LOOP_ST)
	{

		evt = osSignalWait(OUTPUT_REFRESH, osWaitForever);

		if(evt.status==osEventSignal)
		{
			valMsg = evt.value.signals;
			if(((valMsg & DA_OUT_REFRESH_CURR) != 0) || ((valMsg & DA_OUT_REFRESH_SPEED) != 0))
			{
#if USE_EXT_DEV
				AD5689_WriteUpdate_DACREG(DAC_A,daOutputRawDA[CHN_DA_CURR_OUT]);
				AD5689_WriteUpdate_DACREG(DAC_B,daOutputRawDA[CHN_DA_SPEED_OUT]);
#endif
			//	TraceUser("DA out:%x,%x\n",daOutputRawDA[CHN_DA_CURR_OUT], daOutputRawDA[CHN_DA_SPEED_OUT]);
			}
			if((valMsg & DO_OUT_REFRESH) != 0)
			{
				OutputPins(digitOutput, CHN_OUT_MAX);
			//	TraceUser("DO out:%x\n",digitOutput);
			}
		}	//uint32_t weight_Zero_Data = weight_ad7190_ReadAvg(6);
		//TraceUser("zero:%d\n",weight_Zero_Data);
	}

}

