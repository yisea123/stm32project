/*
 * tsk_datalog.c
 *
 *  Created on: 2016��11��17��
 *      Author: pli
 */


#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "main.h"
#include "tsk_datalog.h"
#include "dev_log_sp.h"
#include "dev_logdata.h"
#include "dev_flash_fat.h"
#include "unit_sys_diagnosis.h"

//#define DATA_LOG_TEST

#ifdef DATA_LOG_TEST

static uint16_t testIdx = 0;
static const uint16_t testCycl = 3000;

#endif


static const char* taskStateDsp[] =
{
		TO_STR(DATALOG_IDLE),
		TO_STR(DATALOG_SAVE_TRIGGER),
		TO_STR(DATALOG_SAVE_FINISH),
		TO_STR(DATALOG_READ_TRIGGER),
		TO_STR(DATALOG_READ_FINISH),
		TO_STR(DATALOG_FINISH),
};

void StartDataLogTask(void const * argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	osEvent event;
	DataLogState tskState = DATALOG_IDLE;
	TSK_MSG locMsg;
	uint16_t ret = OK;
	const uint8_t taskID = TSK_ID_DATA_LOG;
	locMsg.callBackFinish = NULL;
	locMsg.callBackUpdate = NULL;
	TracePrint(taskID,"started  \n");
	ret = InitAllLogFiles();
	if(ret != OK)
	{
		TraceDBG( taskID,"flash data init failed!\n");
	}
	Dia_UpdateDiagnosis(FLASH_FAILURE, (ret != OK));
//	SPI_Test();
	NewEventLog(EV_RESET,(uint8_t*)"FLASH");

	while (TASK_LOOP_ST)
	{
		freeRtosTskTick[taskID]++;
		freeRtosTskState[taskID] = tskState;
		event = osMessageGet(SCH_DATALOG_ID, tickOut);

		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout, %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			tskState = DATALOG_IDLE;
#ifdef DATA_LOG_TEST
			if(testIdx%3 == 0)
			{
				TestMeas();
			}
			else if(testIdx%3 == 1)
			{
				TestCali();
			}

			testIdx++;
#endif

		}
		else
		{
			TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_INIT)
			{

				if (tskState != DATALOG_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG( taskID,"bugs Time out, original state,%d!\n",tskState);
					//not finish;
				}
				//idle
				{

					locMsg = *(TSK_MSG_CONVERT(event.value.p));
					ret = ((DataLogProcess)locMsg.val.p )();

					if(ret != OK)
					{
						//error handling
						TraceDBG( taskID,"flash log storage failed\n");
					}
					else
					{
						TracePrint(taskID,"flash save once OK! %x\n",locMsg.val.p);
					}
					Dia_UpdateDiagnosis(FLASH_FAILURE, (ret != OK));
					locMsg.tskState = TSK_SUBSTEP;
					tskState = DATALOG_IDLE;
					if(locMsg.callBackFinish)
					{
						locMsg.callBackFinish(ret, 0);
					}

#ifdef DATA_LOG_TEST
					tickOut = testCycl;
#endif
					//change to init
					//send msg to be running;
				}
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{

				//when finish->call back;
			}
			else
			{
				//todo
			}
		}

	}
}

