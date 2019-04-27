/*
 * tsk_weldpwm.c
 *
 *  Created on: 2019Äê4ÔÂ12ÈÕ
 *      Author: pli
 */
#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_ad5689.h"
#include "dev_ad7190.h"



typedef enum
{
	ST_PWM_IDLE,
	ST_PWM_CYC,
	ST_PWM_FINISH,
}ST_PWM_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(ST_PWM_IDLE),
	TO_STR(ST_PWM_CYC),
	TO_STR(ST_PWM_FINISH),
};

void StartPWMTsk(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	uint32_t pwmCnt = 0;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_PWM;
	ST_PWM_STATE tskState = ST_PWM_IDLE;
	uint32_t valMsg = 0;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"started  \n");
//	TraceUser("data:%d\n",data);
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(PWM_CTRL, tickOut);
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
				case ST_PWM_IDLE:
				case ST_PWM_FINISH:
					tskState = ST_PWM_IDLE;
					break;
				case ST_PWM_CYC:
				{
					if(pwmMicroAdjust > 0.01f)
					{
						currOutputPwmTime[0] =(uint16_t)(ptrCurrWeldSeg->currHighMs/pwmMicroAdjust);
						currOutputPwmTime[1] =(uint16_t)(ptrCurrWeldSeg->currLowMs/pwmMicroAdjust);
					}
					pwmActValueUsed = 1000.0f/(float)(currOutputPwmTime[0] + currOutputPwmTime[1]);

					if(currMicroAdjust > 0.01f)
					{
						actWelCurrUsed[0] = ptrCurrWeldSeg->currHigh * currMicroAdjust;
						actWelCurrUsed[1] = ptrCurrWeldSeg->currLow * currMicroAdjust;
					}
					currOutputPwmFloat[0] = GetCurrCtrlOutput(actWelCurrUsed[0]);
					currOutputPwmFloat[1] = GetCurrCtrlOutput(actWelCurrUsed[1]);

					tickOut = currOutputPwmTime[pwmCnt%2];
					SetDAOutputFloat(CHN_DA_CURR_OUT,currOutputPwmFloat[pwmCnt%2]);
					SigPush(outputTaskHandle, (DA_OUT_REFRESH_SPEED|DA_OUT_REFRESH_CURR|DO_OUT_REFRESH));
					pwmCnt = pwmCnt + 1;

				}
					break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(( mainTskState == TSK_FORCE_BREAK) && (TSK_RESETIO == mainTskState))
			{
				pwmCnt = 0;
				daOutputRawDA[CHN_DA_CURR_OUT] = 0;
				OutPutPins_Call(CHN_OUT_ARC_ON, 0);
				SigPush(outputTaskHandle, (DA_OUT_REFRESH_SPEED|DA_OUT_REFRESH_CURR|DO_OUT_REFRESH));

				tskState = ST_PWM_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				MsgPush(PWM_CTRL, (uint32_t ) &locMsg, 0);
				//force state change to be break;
			}
			else if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;
				pwmCnt = 0;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				tskState = ST_PWM_CYC;
				SendTskMsgLOC(PWM_CTRL, &locMsg);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
				case ST_PWM_IDLE:
					break;
				case ST_PWM_CYC:
					tickOut = 0;
					break;

				case ST_PWM_FINISH:
					tskState = ST_WELD_IDLE;
					TSK_FINISH_ACT(&locMsg,taskID,OK,OK);
					break;
				default:
					tskState = ST_WELD_IDLE;
					SendTskMsgLOC(PWM_CTRL, &locMsg);
					break;
				}
				//when finish->call back;
			}

		}
	}

}


