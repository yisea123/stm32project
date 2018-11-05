/*
 * tsk_spctrl.c
 *
 *  Created on: 2018Äê3ÔÂ5ÈÕ
 *      Author: pli
 */





#include "includes.h"
#include "dev_temp.h"
typedef enum
{
	SP_IDLE,
	SP_INIT,
	SP_EXEC_START,
	SP_EXEC_DELAY,
	SP_EXEC_END,
	SP_HEAT_START,
	SP_HEAT_DELAY,
	SP_BUBBLE_UP_START,
    SP_BUBBLE_UP_DELAY,
	SP_BUBBLE_DOWN_START,
    SP_BUBBLE_POS_RECOVER,
    SP_BUBBLE_POS_RECOVER_DELAY,
	SP_FINISH,
	SP_FINISH_ER,
}SP_STATE;




static const char* taskStateDsp[] =
{
	TO_STR(SP_IDLE),
	TO_STR(SP_INIT),
	TO_STR(SP_EXEC_START),
	TO_STR(SP_EXEC_DELAY),
	TO_STR(SP_EXEC_END),
	TO_STR(SP_HEAT_START),
	TO_STR(SP_HEAT_DELAY),
	TO_STR(SP_BUBBLE_UP_START),
    TO_STR(SP_BUBBLE_UP_DELAY),
	TO_STR(SP_BUBBLE_DOWN_START),
    TO_STR(SP_BUBBLE_POS_RECOVER),
    TO_STR(SP_BUBBLE_POS_RECOVER_DELAY),
	TO_STR(SP_FINISH),
	TO_STR(SP_FINISH_ER),
};


static volatile uint8_t callBackStatus = 0;
static TSK_MSG  localMsg;
static HeatMsg  heatMsg;

static void TskCallFin_SP(uint16_t ret, uint16_t val)
{
	callBackStatus = ret;
    SendTskMsgLOC( MB_SPCTRL, &localMsg);
}
static void TskUpdate_SP(uint16_t ret, uint16_t val)
{
	callBackStatus = ret;
	SendTskMsg(MB_SPCTRL, TSK_RENEW_STATUS , (uint32_t)ret, NULL,NULL);
}


enum
{
    BUBBLE_UP,
    BUBBLE_DOWN,
    BUBBLE_RECOVERY,
};

static uint16_t BubbleExecuteOnce(BubbleCfg* ptrBubble, uint16_t led, uint16_t type, uint16_t delayCfg)
{
	static PumpCfgEx _pumpCfg;
    _pumpCfg.cfg.dir = 0x0;
    _pumpCfg.cfg.preDefineSteps = 0;
    _pumpCfg.cfg.preDefineTime = 0;
    _pumpCfg.cfg.speed = ptrBubble->speed;
    if(delayCfg != 0)
        _pumpCfg.cfg.speed |= DELAY_CFG_EN ;
        
    _pumpCfg.cfg.steps = ptrBubble->stepEachTime;
    _pumpCfg.cfg.errorAction = ACT_NONE;
    _pumpCfg.delay = ptrBubble->delayMs;
	if(type == BUBBLE_UP)
	{
		if(led == 0)
			_pumpCfg.cfg.dir = PULL | LED0_CHK;
		else
			_pumpCfg.cfg.dir = PULL | LED1_CHK;
	}
	else if(type == BUBBLE_DOWN)
	{
		 _pumpCfg.cfg.speed = 40;
		if(led == 0)
			_pumpCfg.cfg.dir = PUSH | LED0_CHK;
		else
			_pumpCfg.cfg.dir = PUSH | LED1_CHK;
	}
    else if(type == BUBBLE_RECOVERY)
	{
        _pumpCfg.cfg.speed = 40;
        _pumpCfg.cfg.steps = 600;
		if(led == 0)
			_pumpCfg.cfg.dir = PULL | LED0_POS;
		else
			_pumpCfg.cfg.dir = PULL | LED1_POS;
	}

	SendTskMsg(MB_MOTOR, TSK_INIT, (uint32_t)&_pumpCfg, TskCallFin_SP,NULL);

	return OK;
}


void StartSpCtrlTask(void const * argument)
{
	(void)argument;
#define TICK_EACH_TIME		1000

#define TICK_BUBBLE_EXEC	100000
#define TICK_BUBBLE_DOWN_EXEC	100000
	osEvent event;
	uint32_t tickOut = osWaitForever;

	uint16_t result;
	uint16_t subStepId = 0;
	const uint16_t taskID = TSK_ID_SP;
	SP_STATE tskState = SP_IDLE;

    int32_t tickDelay = 0;
    BubbleCfg _bubbleCfg;
	uint16_t bubbleSteps;
    uint16_t bubbleDelayCfg;
	uint16_t bubbleRetryTimes;
	uint16_t ledCheckPos = 0;
	uint16_t bubbleOK = 0;
    uint16_t tempCtrl = 0;
	for(;;)
	{
		event = osMessageGet(MB_SPCTRL, tickOut);
		//		tickOut = osWaitForever;
		if (event.status != osEventMessage)//timeout
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time: %d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch(tskState)
			{
            case SP_BUBBLE_UP_DELAY:
            case SP_BUBBLE_POS_RECOVER:
            case SP_BUBBLE_POS_RECOVER_DELAY:				
				TraceDBG(taskID,"the schedule has bugs, no response from Motor task for bubble check- %d, %s\n", tskState,taskStateDsp[tskState]);
                
				tskState = SP_FINISH_ER;
                SendTskMsgLOC( MB_SPCTRL, &localMsg);
				break;
			case SP_EXEC_DELAY:
				tskState = SP_EXEC_END;
				SendTskMsgLOC( MB_SPCTRL, &localMsg);
				break;
			case SP_HEAT_DELAY:
                if( HEAT_TO_PREHEAT == subStepId)
                    tskState = SP_FINISH;
                else
                    tskState = SP_FINISH_ER;
				TraceDBG(taskID,"the schedule has bugs, SP_HEAT_DELAY\n");
				SendTskMsgLOC( MB_SPCTRL, &localMsg);
				break;
			default:
				tskState = SP_IDLE;
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
				tskState = SP_FINISH;
				SendTskMsgLOC( MB_SPCTRL, &localMsg);
                SendTskMsg(TEMPERATURE_ID, TSK_FORCE_BREAK, (uint32_t )0, NULL, NULL);
				//force state change to be break;
			}
			else if(TSK_RESETIO == mainTskState)
			{
				//do reset action
				//todo
				tskState = SP_FINISH;

				SendTskMsgLOC( MB_SPCTRL, &localMsg);
                SendTskMsg(TEMPERATURE_ID, TSK_RESETIO, (uint32_t )0, NULL, NULL);
			}
			else if(TSK_RENEW_STATUS == mainTskState)
			{
				//renew status;
				result = OK;
			}
			else if(mainTskState == TSK_INIT)
			{
				result = OK;
				if(tskState != SP_IDLE)
				{
					//do nothing;
					//todo may not be right
					TraceDBG(taskID,"the schedule has bugs, original state is,%d,%s!\n",tskState, taskStateDsp[tskState]);
					result = FATAL_ERROR;
					//not finish;
				}
				else
				//this function could not be trigger always by other task;
				{
					subStepId = localMsg.val.value;
					tskState = SP_INIT;

					SendTskMsgLOC( MB_SPCTRL, &localMsg);
					//change to init
					//send msg to be running;
				}
			}
			else if(mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch(tskState)
				{
				case SP_IDLE:
					break;
				case SP_INIT:
					TraceMsg(taskID,"%3d, %s is executed!\n", subStepId, GetStepName((uint16_t)subStepId));
					if(IN_RANGE(subStepId,e_sp_ctrl_start, e_sp_ctrl_end))
					{
						tskState = SP_FINISH;
						switch(subStepId)
						{
						case led0_detect_enable:
							led0DetectLiquid = 1;
							break;
						case led0_detect_disable:
							led0DetectLiquid = 0;
							break;
						case led1_detect_enable:
							led1DetectLiquid = 1;
							break;
						case led1_detect_disable:
							led1DetectLiquid = 0;
							break;
                            
                        case CHK_LEAKAGE_ENABLE:
                            chkLeakageCfg = 1;
                            break;
                        case CHK_LEAKAGE_DISABLE:
                            chkLeakageCfg = 0;
                            break;
                        case CHK_SAFETY_LOCK_ENABLE:
                            chkSafetyLockCfg = 1;
                            break;
                        case CHK_SAFETY_LOCK_DISABLE:
                            chkSafetyLockCfg = 0;
                            break;
						case HEAT_TO_PREHEAT:
						case HEAT_TO_MEASURE:
                        case HEAT_TO_DIG:
						case COOLING_TO_DRAIN:
						case PRE_COOLING_BEFORE_SAMPLING:
                        case DEVICE_DIG_INITIALIZE_COOLING:
                        case COOLING_BEFORE_IDLE:
                        case HEAT_TO_CLEAN:
                        case HEAT_TO_STEP2:
                        case MEASURE_TEMP_HOLD:
                        	tskState = SP_HEAT_START;
							//todo
                            break;
						case MEASURE_STEP:
							tempMeas[0] = (uint16_t)GetTempChn(0);
							tempMeas[1] = (uint16_t)GetTempChn(1);
							tempMeas[2] = (uint16_t)GetTempChn(2);
							tskState = SP_EXEC_START;
							tickOut = TICK_EACH_TIME;
							tickDelay = (int32_t)(spAction[MEASURE_STEP-e_sp_ctrl_start].timeExpect*TIME_UNIT);
                            adcAction = AD_MEASURE_ACT;
                            //todo
                            break;
						case BLANK_STEP:
							tempMeas[0] = (uint16_t)GetTempChn(0);
							tempMeas[1] = (uint16_t)GetTempChn(1);
							tempMeas[2] = (uint16_t)GetTempChn(2);
							tskState = SP_EXEC_START;
							tickOut = TICK_EACH_TIME;
							tickDelay = (int32_t)(spAction[BLANK_STEP-e_sp_ctrl_start].timeExpect*TIME_UNIT);
                            adcAction = AD_BLANK_ACT;
                            //todo
                            break;
                        case PUMP_RESTORE_CHECK:
                        	tskState = SP_FINISH_ER;
                        	TraceMsg(taskID,"PUMP_RESTORE_CHECK Status,%x, pos: %d\n", motorPos_Status, motorPos_Pps);
                        	if(motorPos_Pps == 0)
                        	{
                        		tskState = SP_FINISH;
                        	}
                            else
                            {

                                //130
                            }
                            //todo
                            break;
                        case REMOVE_BURBLE_EX_LED0:
                            bubbleDelayCfg = 1;
                            ledCheckPos = 0;
							tskState = SP_BUBBLE_UP_START;
							_bubbleCfg = bubbleCfg[LED0_BUBBLE_EX];
							bubbleSteps = _bubbleCfg.steps;
							bubbleRetryTimes = _bubbleCfg.retryTimes;
                            break;
						case REMOVE_BURBLE_LED0:
                            bubbleDelayCfg = 0;
							ledCheckPos = 0;
							tskState = SP_BUBBLE_UP_START;
							_bubbleCfg = bubbleCfg[LED0_BUBBLE];
							bubbleSteps = _bubbleCfg.steps;
							bubbleRetryTimes = _bubbleCfg.retryTimes;
                            break;
						case REMOVE_BURBLE_LED1:
							ledCheckPos = 1;
							tskState = SP_BUBBLE_UP_START;
							_bubbleCfg = bubbleCfg[LED1_BUBBLE];
							bubbleSteps = _bubbleCfg.steps;
							bubbleRetryTimes = _bubbleCfg.retryTimes;
							//todo
                            break;                        
                       // case HEAT_INCREASE_DETECT:
                            
                            //todo
                        //    break;
						//case SENSOR_GAIN_UPDATE:
                            //todo
                         //   break;
						case UPDATE_CALI_BLANK:
                            //todo
                            break;
						case UPDATE_CALI_SLOPE:
                            //todo
                            break;
						}
					}
					else
					{
						tskState = SP_FINISH;
					}
					SendTskMsgLOC( MB_SPCTRL, &localMsg);
                    tickOut = 0;
					break;
				case SP_EXEC_START:
					tskState = SP_EXEC_DELAY;
					tickOut = TICK_EACH_TIME;
                    
					if(tickDelay > TICK_EACH_TIME)
					{
						tickDelay -= TICK_EACH_TIME;
					}
					else
					{
						//todo; handle all data
						tskState = SP_FINISH;
						SendTskMsgLOC( MB_SPCTRL, &localMsg);
					}
					//todo
					break;
				case SP_EXEC_END:
					tskState = SP_EXEC_DELAY;
					tickOut = TICK_EACH_TIME;
                    
					if(tickDelay > TICK_EACH_TIME)
					{
						//todo; handle all data
						tickDelay -= TICK_EACH_TIME;
					}
					else
					{
						//todo; handle all data
						tskState = SP_FINISH;
						SendTskMsgLOC( MB_SPCTRL, &localMsg);
					}
					break;
				case SP_HEAT_START:
				{
					tskState = SP_HEAT_DELAY;
					tickOut = TICK_HEAT_DELAY;
					switch (subStepId)
					{
					case HEAT_TO_PREHEAT:
                        tickOut = 90*1000;
						heatMsg.detail.mode = HEAT_MODE_HEAT_HOLD;			//
						heatMsg.detail.val = heatCfg.preHeatTemp;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, TskCallFin_SP,
								NULL);
                        SendTskMsgLOC( MB_SPCTRL, &localMsg);
						//todo
						break;
					case HEAT_TO_MEASURE:
						heatMsg.detail.mode = HEAT_MODE_COOL_HOLD;			//
						heatMsg.detail.val = heatCfg.measureTemp;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, NULL,
								TskCallFin_SP);
                   
                        break;
                    case HEAT_TO_CLEAN:
                        heatMsg.detail.mode = HEAT_MODE_HEAT_TO;			//
						heatMsg.detail.val = heatCfg.cleanTemp;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, TskCallFin_SP,
								NULL);
                        
						//todo
						break;
					case HEAT_TO_DIG:
						heatMsg.detail.mode = HEAT_MODE_HEAT_HOLD;			//
						heatMsg.detail.val = heatCfg.digTemp;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, NULL,
								TskCallFin_SP);
						//todo
						break;
					case COOLING_TO_DRAIN:
						heatMsg.detail.mode = HEAT_MODE_COOL_TO;			//
						heatMsg.detail.val = heatCfg.drainTemp;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, TskCallFin_SP,
								NULL);
						//todo
						break;
					case PRE_COOLING_BEFORE_SAMPLING:
					case DEVICE_DIG_INITIALIZE_COOLING:
					case COOLING_BEFORE_IDLE:
                        tempIdle[0] = (uint16_t)GetTempChn(0);
                        tempIdle[1] = (uint16_t)GetTempChn(1);
                        tempIdle[2] = (uint16_t)GetTempChn(2);
						heatMsg.detail.mode = HEAT_MODE_COOL_TO;			//
						heatMsg.detail.val = heatCfg.idleTemp;
                        if(heatCfg.idleTemp < (uint16_t)(tempIdle[2] + 500))
                            heatMsg.detail.val = (uint16_t)(tempIdle[2] + 500);
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, TskCallFin_SP,
								NULL);
						//todo
						break;
                    case HEAT_TO_STEP2:
                        heatMsg.detail.mode = HEAT_MODE_HEAT_HOLD;			//
						heatMsg.detail.val = heatCfg.tempStep2;
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, NULL,
								TskCallFin_SP);
                        break;
                    case MEASURE_TEMP_HOLD:
                        heatMsg.detail.mode = HEAT_MODE_COOL_HOLD;			//
						heatMsg.detail.val = (tempCtrl - heatCfg.tempDecrease);
						SendTskMsg(TEMPERATURE_ID, TSK_INIT,
								(uint32_t )heatMsg.heatMsg_Val, NULL,
								TskCallFin_SP);
                       break;
					default:
						tskState = SP_FINISH;
						SendTskMsgLOC( MB_SPCTRL, &localMsg);
						break;                    
					}
					//wait time out
                    tempCtrl = heatMsg.detail.val;
				}
					break;

				case SP_BUBBLE_UP_START:
                    if( bubbleSteps )
                    {
                        BubbleExecuteOnce(&_bubbleCfg,ledCheckPos, BUBBLE_UP,bubbleDelayCfg);
                        bubbleSteps--;
                        tskState = SP_BUBBLE_UP_DELAY;
                        tickOut = TICK_BUBBLE_EXEC;
                    }
                    else
                    {
                        bubbleOK = OK;
                        tskState = SP_BUBBLE_DOWN_START;
						SendTskMsgLOC( MB_SPCTRL, &localMsg);
                    }
					break;
				case SP_BUBBLE_UP_DELAY:
                    tickOut = TICK_BUBBLE_EXEC;
					//finished ok;
					bubbleOK = FATAL_ERROR;
					if(callBackStatus == OK)
					{
						if( bubbleSteps )
						{
							tskState = SP_BUBBLE_UP_START;
							SendTskMsgLOC( MB_SPCTRL, &localMsg);
						}
						else
						{
							tskState = SP_BUBBLE_DOWN_START;
							SendTskMsgLOC( MB_SPCTRL, &localMsg);
							bubbleOK = OK;
						}
					}
					else
					{
                        TraceDBG(taskID,"Bubble is detected, %d\n",callBackStatus);
						tskState = SP_BUBBLE_DOWN_START;
						SendTskMsgLOC( MB_SPCTRL, &localMsg);
					}
					break;
				case SP_BUBBLE_DOWN_START:
					BubbleExecuteOnce(&_bubbleCfg,ledCheckPos, BUBBLE_DOWN,bubbleDelayCfg);
                    tskState = SP_BUBBLE_POS_RECOVER;
                    tickOut = TICK_BUBBLE_DOWN_EXEC;
					break;
                case SP_BUBBLE_POS_RECOVER:                    
                    BubbleExecuteOnce(&_bubbleCfg,ledCheckPos, BUBBLE_RECOVERY,bubbleDelayCfg);
                    tskState = SP_BUBBLE_POS_RECOVER_DELAY; 
                    break;
                case SP_BUBBLE_POS_RECOVER_DELAY:
                    if(bubbleOK == OK)
					{
						tskState = SP_FINISH;    
                        TraceMsg(taskID,"Bubble is not detected,  %d\n",bubbleOK);                    
					}
					else
					{
						tskState = SP_FINISH_ER;
						if(bubbleRetryTimes > 0)
						{
							tskState = SP_BUBBLE_UP_START;
                            bubbleRetryTimes--;
                            bubbleSteps = _bubbleCfg.steps; 
						}
                        TraceDBG(taskID,"Bubble is detected, retry again %d\n",bubbleOK);
					}
                    SendTskMsgLOC( MB_SPCTRL, &localMsg);
                    break;
				case SP_HEAT_DELAY:
					tskState = SP_FINISH;
					SendTskMsgLOC( MB_SPCTRL, &localMsg);
					break;
                    
					//change
				case SP_EXEC_DELAY:
					TraceDBG(taskID,"SP task execute error,\n");
					tskState = SP_FINISH_ER;
					SendTskMsgLOC( MB_SPCTRL, &localMsg);
					break;
                    
				case SP_FINISH_ER:
                    if(MEASURE_STEP == subStepId)
                    {
                        adcAction = AD_MEASURE_ACT_FINISH;
                    }
                    else if(BLANK_STEP == subStepId)
                    {
                        adcAction = AD_BLANK_ACT_FINISH;
                    }
                    else
                    {}
                    TSK_FINISH_ACT(&localMsg,taskID, FATAL_ERROR, result);
					tskState = SP_IDLE;
					break;

				case SP_FINISH:
                    if(MEASURE_STEP == subStepId)
                    {
                        adcAction = AD_MEASURE_ACT_FINISH;
                    }
                    else if(BLANK_STEP == subStepId)
                    {
                        adcAction = AD_BLANK_ACT_FINISH;
                    }
                    else
                    {}
                    //OS_Delay(100);
                    TSK_FINISH_ACT(&localMsg,taskID, OK, result);
					tskState = SP_IDLE;
					break;

				}
			}
		}
	}
}
