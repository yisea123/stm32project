/*
 * tsk_motor.c
 *
 *  Created on: 2018Äê2ÔÂ28ÈÕ
 *      Author: vip
 */


#include "includes.h"

#define PUMP_POSITION_MAX        	3000
#define LED0_POS_MIN                200

enum
{
	WAIT_MOTOR_STOP,
	MOTOR_EXECUTE_IM,
};


typedef enum
{
	MOTOR_IDLE,
	MOTOR_INIT,
	MOTOR_WAIT_IDLE,//wait motor idle
	MOTOR_SEND_CMD,
	MOTOR_CMD_DELAY, // wait motor execute;
    MOTOR_CMD_WAIT_STABLE,
	MOTOR_POS_CHECK_WAIT,
	MOTOR_POS_CHECK_INIT,
	MOTOR_POS_CHECK,
	MOTOR_FINISH,
	MOTOR_FINISH_ER,
}MOTOR_STATE;

static TSK_MSG  localMsg;


static const char* taskStateDsp[] =
{
	TO_STR(MOTOR_IDLE),
	TO_STR(MOTOR_INIT),
	TO_STR(MOTOR_WAIT_IDLE),
	TO_STR(MOTOR_SEND_CMD),
	TO_STR(MOTOR_CMD_DELAY),
    TO_STR(MOTOR_CMD_WAIT_STABLE),
	TO_STR(MOTOR_POS_CHECK_WAIT),
	TO_STR(MOTOR_POS_CHECK_INIT),
	TO_STR(MOTOR_POS_CHECK),
	TO_STR(MOTOR_FINISH),
	TO_STR(MOTOR_FINISH_ER),
};

static void TskCallFin_Motor(uint16_t ret, uint16_t val)
{
	SendTskMsgLOC(MB_MOTOR, &localMsg);
}

void StartMotorCmdTask(void const* argument)
{
	(void)argument; // pc lint
	uint32_t tickOut = osWaitForever;
	uint16_t ret = 0;
	uint16_t result;
	osEvent event;
	const uint16_t taskID = TSK_ID_MOTOR;
	PumpCmd pmpCmd = {0,
				{0,	0,	0,	0,},
		};
	PumpCfgEx pumpCfgEx;
	uint8_t dir = 0;
	MOTOR_STATE tskState = MOTOR_IDLE;
	MOTOR_STATE nextState = MOTOR_IDLE;

	pmpCmd.pumpId = IDX_PUMP1;
	for(;;)
	{
		event = osMessageGet(MB_MOTOR, tickOut );

		if( event.status != osEventMessage )
		{
			TracePrint(taskID, "Timeout, %d,%s, Time, %d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{

            case MOTOR_CMD_WAIT_STABLE:
                tskState = MOTOR_POS_CHECK_WAIT;
                nextState = MOTOR_POS_CHECK_INIT;
                tickOut = 0;
                break;
			case MOTOR_CMD_DELAY:
				TraceDBG(taskID, "Pump send CMD no response\n");
				tskState = MOTOR_FINISH_ER;
				SendTskMsgLOC( MB_MOTOR, &localMsg);
				break;
			case MOTOR_WAIT_IDLE:
			case MOTOR_POS_CHECK_WAIT:
				if(motorPos_Status == PUMP_IDLE)
				{
					tskState = nextState;
					SendTskMsgLOC( MB_MOTOR, &localMsg);
				}
				else if(motorPos_Status == PUMP_ERROR)
				{
					TraceDBG(taskID, "Pump Status Error,0x%x\n",motorPos_Status);
					tskState = MOTOR_FINISH_ER;
					SendTskMsgLOC( MB_MOTOR, &localMsg);
				}
				else 
				{
                    if(motorPos_Status != PUMP_RUNNING)
                        TraceDBG(taskID, "Pump Status not ready; get result,0x%x\n",motorPos_Status);
                    tskState = nextState;
                    SendTskMsgLOC( MB_MOTOR, &localMsg);
				}
				TracePrint(taskID, "Pump Status get result,0x%x\n",motorPos_Status);
				tickOut = MOTOR_EXEC_TIME;
				break;

			case MOTOR_POS_CHECK:
				//IIC communication error;
				//no feedback in TIME_DELAY_IIC ??
				//recheck position
				TraceDBG(taskID, "POS check ADC not feedback\n");
				tskState = MOTOR_POS_CHECK_INIT;
				SendTskMsgLOC( MB_MOTOR, &localMsg);
				break;

			}
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
				//todo
				SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_FORCE_BREAK, (uint32_t)&pmpCmd, TskCallFin_Motor,NULL);

				tskState = MOTOR_FINISH;
				SendTskMsgLOC( MB_MOTOR, &localMsg);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				//do reset action
				//todo
				tskState = MOTOR_FINISH;
				SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_RESETIO, (uint32_t)&pmpCmd, TskCallFin_Motor,NULL);

				SendTskMsgLOC( MB_MOTOR, &localMsg);
			}
			else if(TSK_RENEW_STATUS == mainTskState)
			{
				//renew status;
				result = OK;
			}
			else if(mainTskState == TSK_INIT)
			{
				result = OK;
				nextState = MOTOR_IDLE;
				if(tskState != MOTOR_IDLE)
				{                    
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is,%d,%s!\n",tskState, taskStateDsp[tskState]);
					result = FATAL_ERROR;
					//not finish;
				}
			//	else
				//this function could not be trigger always by other task;
				{
					pumpCfgEx = *(PumpCfgEx*)localMsg.val.p;
					tskState = MOTOR_INIT;


					SendTskMsgLOC( MB_MOTOR, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if(mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
				case MOTOR_IDLE:
					nextState = MOTOR_IDLE;
					break;
				case MOTOR_INIT:
					TraceMsg(taskID,"Motor Dirpos,%x, step, %d, speed, %d\n", \
							pumpCfgEx.cfg.dir, pumpCfgEx.cfg.steps, pumpCfgEx.cfg.speed);
					tskState = MOTOR_WAIT_IDLE;
                    tickOut = 0;
                    pmpCmd.pumpId = IDX_PUMP1;
                    pmpCmd.pumpCfg = pumpCfgEx.cfg;

                    if(((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_POS) || ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_POS) )
					{
                    	nextState = MOTOR_POS_CHECK_INIT;
					}
					else if( ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_CHK) || ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_CHK) )
					{
                        TraceMsg(taskID,"Motor Chk led, Dirpos,%x, step, %d, speed, %d\n", \
                        		pumpCfgEx.cfg.dir, pumpCfgEx.cfg.steps,pumpCfgEx.cfg.speed);
                        pmpCmd.pumpCfg.dir = ((pmpCmd.pumpCfg.dir & E_POS_NON_MASK) | FREE_POS);

						nextState = MOTOR_SEND_CMD;
					}
                    else
                    {
                        nextState = MOTOR_SEND_CMD;
                    }
					break;
				case MOTOR_SEND_CMD:
					SendTskMsg(FLOW_MOTOR_CTL_ID, TSK_INIT, (uint32_t)&pmpCmd, TskCallFin_Motor,NULL);
					tskState = MOTOR_CMD_DELAY;
					nextState = MOTOR_POS_CHECK_INIT;
                    tickOut = MOTOR_EXEC_TIME;
					break;
					//change
                
				case MOTOR_CMD_DELAY:     
					tickOut = pumpCfgEx.delay;
                    tskState = MOTOR_CMD_WAIT_STABLE;
                    if((DELAY_CFG_EN  & pumpCfgEx.cfg.speed) == 0)
                        tickOut = 0;
					break;
                
				case MOTOR_POS_CHECK_INIT:
					tskState = MOTOR_POS_CHECK;
					if(((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_POS) || ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_CHK) )
					{

					}
					else if(((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_POS) || ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_CHK) )
					{

					}
					else
					{
						tskState = MOTOR_FINISH;

					}
					SendTskMsgLOC( MB_MOTOR, &localMsg);
					break;
				case MOTOR_POS_CHECK:
                    TraceMsg(taskID,"Position check,%x, %d <= %d ? \n",led0Pos,led0Pos,LED_POS_CHK_LIMIT);

					if (((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_CHK)  || ((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_CHK))
                    {
                    	dir = pumpCfgEx.cfg.dir & 0x80;
                    	if(dir == PULL) //
                    	{
							if(led0Pos <= LED_POS_CHK_LIMIT)
							{
								tskState = MOTOR_FINISH;
							}
							else
							{
								tskState = MOTOR_FINISH_ER;
							}
                    	}
                    	else //push
                    	{
                    		if(led0Pos > LED_POS_CHK_LIMIT)
                            {
                                tskState = MOTOR_FINISH;
                            }
                            else
                            {
                                tskState = MOTOR_SEND_CMD;
                                if(motorPos_Pps <= LED0_POS_MIN)
                                {
                                    
                                    //the top position is reached
                                    tskState = MOTOR_FINISH_ER;
                                    TraceDBG(taskID,"pos small but detect liquid,%d <= %d Pos,%d<=%d\n",led0Pos,LED_POS_CHK_LIMIT,motorPos_Pps,LED0_POS_MIN);

                                }
                            }
                    	}
                    }
                    else 
                    {
                    	if (((pumpCfgEx.cfg.dir & E_POS_MASK) == LED0_POS)  && (led0Pos <= LED_POS_CHK_LIMIT))
						{
							tskState = MOTOR_FINISH;
						}
                    	else if (((pumpCfgEx.cfg.dir & E_POS_MASK) == LED1_POS)  && (led1Pos <= LED_POS_CHK_LIMIT))
						{
							tskState = MOTOR_FINISH;
						}
						else
						{
							tskState = MOTOR_SEND_CMD;
							if(PUMP_POSITION_MAX <= motorPos_Pps )
							{
								//the top position is reached
								tskState = MOTOR_FINISH_ER;
								TraceDBG(taskID,"No sample detected %d <= %d and Position Error, %d >= %d\n",led1Pos,LED_POS_CHK_LIMIT,motorPos_Pps,PUMP_POSITION_MAX);

							}
						}
                    }

					SendTskMsgLOC( MB_MOTOR, &localMsg);
					break;
				case MOTOR_FINISH_ER:
					TSK_FINISH_ACT(&localMsg,taskID,FATAL_ERROR,result);

					tskState = MOTOR_IDLE;
					break;

				case MOTOR_FINISH:
					TSK_FINISH_ACT(&localMsg,taskID,OK,result);
					tskState = MOTOR_IDLE;
					TracePrint(taskID,"Finished OK, Motor Status,%x, pos, %d\n", \
							motorPos_Status, motorPos_Pps);

					break;

				}
			}
		}
	}
}

