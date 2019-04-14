/*
 * tsk_weldpwm.c
 *
 *  Created on: 2019��4��12��
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
					daOutputPwmTime[0] =ptrCurrWeldSeg->currHighMs;
					daOutputPwmTime[1] =ptrCurrWeldSeg->currLowMs;
					daOutputPwm[0] = GetCurrOutput(ptrCurrWeldSeg->currHigh);
					daOutputPwm[1] = GetCurrOutput(ptrCurrWeldSeg->currLow);

					tickOut = daOutputPwmTime[pwmCnt%2];
					daOutputSet[CHN_DA_CURR_OUT] = daOutputPwm[pwmCnt%2];
					SendTskMsg(OUTPUT_QID, TSK_INIT, OUTPUT_REFRESH, NULL, NULL);
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
				daOutputSet[CHN_DA_CURR_OUT] = 0;
				digitOutput &= ~(1<<CHN_OUT_ARC_ON);
				SendTskMsg(OUTPUT_QID, TSK_INIT, OUTPUT_REFRESH, NULL, NULL);

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


