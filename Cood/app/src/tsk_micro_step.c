/*
* tsk_micro_step.c
*
*  Created on: 2018Äê2ÔÂ9ÈÕ
*      Author: Paul
*/

#include "includes.h"
#include "main.h"
enum
{
	WAIT_MOTOR_STOP,
	MOTOR_EXECUTE_IM,
};
//only control the pump/valve/timedelay actions

typedef enum
{
	M_STEP_IDLE,
	M_STEP_INIT,
	M_STEP_VALVE,
	M_STEP_VALVE_DELAY,
	M_STEP_MOTOR,
	M_STEP_MOTOR_DELAY,
	M_STEP_TIME,
	M_STEP_TIME_DELAY,
	M_STEP_MEAS,
	M_STEP_MEAS_DELAY,
	M_STEP_ACT,
	M_STEP_ACT_DELAY,
	M_STEP_FINISH,
	M_STEP_FINISH_ER,
}M_STEP_STATE;


static const char* taskStateDsp[] =
{
	TO_STR(M_STEP_IDLE),
	TO_STR(M_STEP_INIT),
	TO_STR(M_STEP_VALVE),
	TO_STR(M_STEP_VALVE_DELAY),
	TO_STR(M_STEP_MOTOR),
	TO_STR(M_STEP_MOTOR_DELAY),
	TO_STR(M_STEP_TIME),
	TO_STR(M_STEP_TIME_DELAY),
	TO_STR(M_STEP_MEAS),
	TO_STR(M_STEP_MEAS_DELAY),
	TO_STR(M_STEP_ACT),
	TO_STR(M_STEP_ACT_DELAY),
	TO_STR(M_STEP_FINISH),
	TO_STR(M_STEP_FINISH_ER),
};
static TSK_MSG  localMsg;

static void TskCallFin_Micro(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC( MB_MICROSTEP, &localMsg);
}
static void TskUpdate_Micro(uint16_t ret, uint16_t val)
{
	SendTskMsg( MB_MICROSTEP, TSK_RENEW_STATUS , (uint32_t)0, NULL,NULL);
}

uint16_t GetValveCmd(ValveCmd* valveCmd, uint16_t subId)
{
	//todo

	uint16_t ret = OK;

	valveCmd->valveId = 0xFFFF;
	valveCmd->valveStatus = 0x0;

	{
		valveCmd->valveStatus = 0x1;
		if(subId == open_air_v)
		{
			valveCmd->valveId = 8;
		}
		else if(subId == open_samp_v)
		{
			valveCmd->valveId = 5;
		}
		else if(subId == open_waste_v)
		{
			valveCmd->valveId = 6;
		}
		else if(subId == open_dichro_v)
		{
			valveCmd->valveId = 0;
		}
		else if(subId == open_blank_v)
		{
			valveCmd->valveId = 3;
		}
		else if(subId == open_H2SO4_v)
		{
			valveCmd->valveId = 1;
		}
		else if(subId == open_HgSO4_v)
		{
			valveCmd->valveId = 2;
		}
		else if(subId == open_std_v)
		{
			valveCmd->valveId = 4;
		}
		else if(subId == open_safety_v)
		{
			valveCmd->valveId = 7;
		}
		else
		{
			valveCmd->valveStatus = 0x0;
			if(subId == close_air_v)
			{
				valveCmd->valveId = 8;
			}
			else if(subId == close_samp_v)
			{
				valveCmd->valveId = 5;
			}
			else if(subId == close_waste_v)
			{
				valveCmd->valveId = 6;
			}
			else if(subId == close_dichro_v)
			{
				valveCmd->valveId = 0;
			}
			else if(subId == close_blank_v)
			{
				valveCmd->valveId = 3;
			}
			else if(subId == close_H2SO4_v)
			{
				valveCmd->valveId = 1;
			}
			else if(subId == close_HgSO4_v)
			{
				valveCmd->valveId = 2;
			}
			else if(subId == close_std_v)
			{
				valveCmd->valveId = 4;
			}
			else if(subId == close_safety_v)
			{
				valveCmd->valveId = 7;
			}
			else
			{
				ret = FATAL_ERROR;
			}
		}
		/*
		if( (open_cera_v < subId )&& (subId<close_cera_v))
		{
			valveCmd->valveId = (subId - open_cera_v + 1);
			valveCmd->valveStatus = 0x1;
		}
		else if( (close_cera_v < subId )&& (subId < e_valve_end1))
		{

			valveCmd->valveId = (uint16_t)(subId - close_cera_v + 1);
			valveCmd->valveStatus = 0x1;
		}
		else
		{
			ret = FATAL_ERROR;
		}
		*/
	}

	return ret;
}



void StartMicroStepTask(void const * argument)
{
	(void)argument;
	uint32_t tickOut = osWaitForever;
	uint32_t timeDelayCfg = 0;
	uint16_t ret = 0;
	osEvent event;
	uint16_t result;
	uint16_t subStepId = 0;
	const uint16_t taskID = TSK_ID_MICROSTEP;
	M_STEP_STATE tskState = M_STEP_IDLE;
    PumpCfgEx pumpCfgEx;
    ValveCmd  valveCmd;
	for(;;)
	{
		event = osMessageGet(MB_MICROSTEP, tickOut );
		if( event.status != osEventMessage )
        //timeout
		{
			TracePrint(taskID, "Timeout, %d,\t%s, Time, %d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
            case M_STEP_ACT_DELAY:
                tskState = M_STEP_FINISH_ER;
				TraceDBG(taskID,"SP action delay timeout, %d, %s!\n",tskState, taskStateDsp[tskState]);
                break;
			case M_STEP_MOTOR_DELAY:
				tskState = M_STEP_FINISH_ER;
				TraceDBG(taskID,"Motor delay timeout, %d, %s!\n",tskState, taskStateDsp[tskState]);
				break;
			case M_STEP_VALVE_DELAY :
				//error
				tskState = M_STEP_FINISH_ER;
				break;
			case M_STEP_TIME_DELAY:
				tskState = M_STEP_FINISH;
				break;
			default:
				tskState = M_STEP_FINISH;
			}
			SendTskMsgLOC( MB_MICROSTEP, &localMsg);
		}
		else
		{
			//new msg

			localMsg = *TSK_MSG_CONVERT(event.value.p);
			const TSK_STATE mainTskState = localMsg.tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if(mainTskState == TSK_FORCE_BREAK)
			{
				//do break action
				tskState = M_STEP_FINISH;

				SendTskMsgLOC( MB_MICROSTEP, &localMsg);
				SendTskMsg( MB_SPCTRL, TSK_FORCE_BREAK, (uint32_t)0, NULL,NULL);
                SendTskMsg( MB_MOTOR, TSK_FORCE_BREAK, (uint32_t)0, NULL,NULL);
                //force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				//do reset action
				tskState = M_STEP_FINISH;

                SendTskMsg( MB_SPCTRL, TSK_RESETIO, (uint32_t)0, NULL,NULL);
                SendTskMsg( MB_MOTOR, TSK_RESETIO, (uint32_t)0, NULL,NULL);
				SendTskMsgLOC( MB_MICROSTEP, &localMsg);
			}
			else if(TSK_RENEW_STATUS == mainTskState)
			{
				//renew status;
				result = OK;
			}
			else if(mainTskState == TSK_INIT)
			{
				result = OK;
				if(tskState != M_STEP_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is, %d, %s!\n",tskState, taskStateDsp[tskState]);
					result = FATAL_ERROR;
					//not finish;
				}
				//this function could be trigger always by other task;
				{
					subStepId = localMsg.val.value;
                    
					tskState = M_STEP_INIT;

                    
					SendTskMsgLOC( MB_MICROSTEP, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if(mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
				case M_STEP_IDLE:
					break;
				case M_STEP_INIT:
					gFlowStep.microStep = subStepId;
					flowStepRun[ID_MICRO_STEP].duringTime = (uint32_t)(CalcDuringSecondsStep_WithDelay(gFlowStep.microStep));
					flowStepRun[ID_MICRO_STEP].startTime = GetCurrentST();
					TraceMsg(taskID,"%3d, %s is called During Time, %d, %s\n", subStepId, GetStepName((uint16_t)subStepId), flowStepRun[ID_MICRO_STEP].duringTime);
                    

					if(IN_RANGE(subStepId,e_valve_start, e_valve_end))
					{
						tskState = M_STEP_VALVE;
						timeDelayCfg = VALVE_DURING_TIME;
					}
					else if(IN_RANGE(subStepId,e_timedelay_start, e_timedelay_end))
					{
						timeDelayCfg = timeDelay[subStepId-e_timedelay_start]*TIME_UNIT;
						if(timeDelayCfg)
						{
							tskState = M_STEP_TIME;
						}
						else
						{
							tskState = M_STEP_FINISH;
						}
					}
					else if(IN_RANGE(subStepId,e_pump_start, e_pump_end))
					{
						tskState = M_STEP_MOTOR;
						timeDelayCfg = flowStepRun[ID_MICRO_STEP].duringTime * 1000;
						//Control pump
					}
					else if(IN_RANGE(subStepId,e_sp_ctrl_start, e_sp_ctrl_end))
					{
						
						tskState = M_STEP_ACT;
						timeDelayCfg = flowStepRun[ID_MICRO_STEP].duringTime * 1000;
						//todo
					}
					else
					{
						tskState = M_STEP_FINISH_ER;
						//todo
					}
					SendTskMsgLOC( MB_MICROSTEP, &localMsg);
					break;
                    
                case M_STEP_ACT:
                    {
                        SendTskMsg( MB_SPCTRL, TSK_INIT, (uint32_t)subStepId, TskCallFin_Micro,NULL);
                        tskState = M_STEP_ACT_DELAY;
                        tickOut = timeDelayCfg;
                    }
                break;  
                case M_STEP_ACT_DELAY:
                  tskState = M_STEP_FINISH;
				  SendTskMsgLOC( MB_MICROSTEP, &localMsg);
                  break;  
				case M_STEP_VALVE:
                    if((e_valve_start <= subStepId) && (e_valve_end1>subStepId))
                    {
                    	uint16_t ret = GetValveCmd(&valveCmd, subStepId);
                        tickOut = GetStepsMS_WithDelay(subStepId);
                        if(ret == OK)
                        	SendTskMsg(FLOW_VALVE_CTL_ID, TSK_INIT, (uint32_t)&valveCmd, TskCallFin_Micro,NULL);
                        else
                        {
                        	ret = Valve_Motor(subStepId,TskCallFin_Micro,NULL);
                        	if(ret != OK)
                        	{
                        		SendTskMsgLOC( MB_MICROSTEP, &localMsg);
                        	}
                        }
                        assert(subStepId < e_cmd_id_max);
                        assert(subStepId > 0);
                        tickOut = VALVE_DURING_TIME;
                        tskState = M_STEP_VALVE_DELAY;
                    }
                    else if(e_valve_end1 <= subStepId)
                    {
                        if(e_device_reset == subStepId)
                        {
                        	Trigger_DeviceReset();
                        }
                        tskState = M_STEP_FINISH;
                        SendTskMsgLOC( MB_MICROSTEP, &localMsg);
                    }
					break;
				case M_STEP_TIME:
					tickOut = timeDelayCfg ;//* TIME_UNIT;
					tskState = M_STEP_TIME_DELAY;
					break;

				case M_STEP_MOTOR:
                    pumpCfgEx.cfg = pumpCfg[subStepId-e_pump_start];
                    pumpCfgEx.delay = pumpExecDelay;
					SendTskMsg( MB_MOTOR, TSK_INIT, (uint32_t)&pumpCfgEx, TskCallFin_Micro,TskUpdate_Micro);
					tskState = M_STEP_MOTOR_DELAY;
                    tickOut = timeDelayCfg;
					break;
				case M_STEP_VALVE_DELAY :
				case M_STEP_MOTOR_DELAY:
					tskState = M_STEP_FINISH;
					SendTskMsgLOC( MB_MICROSTEP, &localMsg);
					break;
				case M_STEP_FINISH_ER:
					TSK_FINISH_ACT(&localMsg,taskID,FATAL_ERROR,result);
					tskState = M_STEP_IDLE;
					break;
                    
				case M_STEP_FINISH:
					TSK_FINISH_ACT(&localMsg,taskID,OK,result);
					tskState = M_STEP_IDLE;
					break;
                    
				}
			}
		}
	}
}
