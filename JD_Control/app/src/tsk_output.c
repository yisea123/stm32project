/*
 * tsk_interrupt.c
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
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


static const PinInst digitOutputPins[CHN_OUT_MAX] =
{
		{OUT1_GPIO_Port, OUT1_Pin},
		{OUT2_GPIO_Port, OUT2_Pin},
		{OUT3_GPIO_Port, OUT3_Pin},
		{OUT4_GPIO_Port, OUT4_Pin},
};


static void OutputPins(uint32_t _digitOutput, uint16_t chnNum)
{
	assert(chnNum <= CHN_OUT_MAX);
	for(uint16_t i=0; i< chnNum;i++)
	{
		if( (_digitOutput & (1<<i)) != 0)
			HAL_GPIO_WritePin(digitOutputPins[i].port, digitOutputPins[i].pin, GPIO_PIN_SET );
		else
			HAL_GPIO_WritePin(digitOutputPins[i].port, digitOutputPins[i].pin, GPIO_PIN_RESET );
	}
}

uint32_t GetInputPins(void)
{
	uint32_t inputPins = 0;
	for(uint16_t i=0; i< CHN_IN_MAX;i++)
	{
		GPIO_PinState state = HAL_GPIO_ReadPin(digitInputPins[i].port, digitInputPins[i].pin);

		if(state == GPIO_PIN_SET)
			inputPins = (inputPins | (1<<i));
	}
	return inputPins;
}
static const char* taskStateDsp[] =
{
	TO_STR(ST_WELD_IDLE),
	TO_STR(ST_WELD_INITPARA),
};

void StartOutputTsk(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = 1000;
	osEvent event;

	TSK_MSG localMsg;
	const uint8_t taskID = TSK_ID_OUTPUT;
	ST_WELD_STATE tskState = ST_WELD_IDLE;
	uint32_t valMsg = 0;
	InitTaskMsg(&localMsg);
	TracePrint(taskID,"started  \n");

	Adc_Setup();
	AD5689_Init();
	uint16_t data = 0x2000;
//	AD5689_WriteUpdate_DACREG(DAC_A,data);
//	AD5689_WriteUpdate_DACREG(DAC_B,0xFFFF-data);
//	TraceUser("data:%d\n",data);
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(OUTPUT_QID, tickOut);
		if (event.status == osEventMessage)
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			valMsg = TSK_MSG_CONVERT(event.value.p)->val.value;
			TSK_MSG_RELASE;
			localMsg = *(TSK_MSG_CONVERT(event.value.p));
			if(((valMsg & DA_OUT_REFRESH_CURR) != 0) || ((valMsg & DA_OUT_REFRESH_SPEED) != 0))
			{
				AD5689_WriteUpdate_DACREG(DAC_A,daOutput[CHN_DA_CURR_OUT]);
				AD5689_WriteUpdate_DACREG(DAC_B,daOutput[CHN_DA_SPEED_OUT]);
				TraceUser("DA out:%x,%x\n",daOutput[CHN_DA_CURR_OUT], daOutput[CHN_DA_SPEED_OUT]);
			}
			if((valMsg & DO_OUT_REFRESH) != 0)
			{
				OutputPins(digitOutput, CHN_OUT_MAX);
				TraceUser("DO out:%x\n",digitOutput);

			}
			tickOut = 5;
		}
		else
		{
			AD5689_WriteUpdate_DACREG(DAC_A,daOutput[CHN_DA_CURR_OUT]);
			AD5689_WriteUpdate_DACREG(DAC_B,daOutput[CHN_DA_SPEED_OUT]);
		}
		//uint32_t weight_Zero_Data = weight_ad7190_ReadAvg(6);
		//TraceUser("zero:%d\n",weight_Zero_Data);
	}

}

