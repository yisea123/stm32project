/*
 * tsk_motor.c
 *
 *  Created on: 2019年3月27日
 *      Author: pli
 */



#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"
#include "rtc.h"
/* 私有类型定义 --------------------------------------------------------------*/
typedef struct
{
  __IO int32_t  SetPoint;    //设定目标 Desired Value
  __IO float    SumError;    //误差累计
  __IO float    Proportion;  //比例常数 Proportional Const
  __IO float    Integral;    //积分常数 Integral Const
  __IO float    Derivative;  //微分常数 Derivative Const
  __IO int      LastError;   //Error[-1]
  __IO int      PrevError;   //Error[-2]
  __IO float	MaxOutput;
  __IO float	MinOutput;
}PID_TypeDef;

/* 私有宏定义 ----------------------------------------------------------------*/

/*************************************/
// 定义PID相关宏
// 这三个参数设定对电机运行影响非常大
// PID参数跟采样时间息息相关
/*************************************/
#define  SPD_P_DATA      1.025f       // P参数
#define  SPD_I_DATA      0.215f       // I参数
#define  SPD_D_DATA      0.1f         // D参数
#define  TARGET_LOC      6600        // 目标速度

#define ENCODER         11   // 编码器线数
#define SPEEDRATIO      30   // 电机减速比
#define PPR             (SPEEDRATIO*ENCODER*4) // Pulse/r 每圈可捕获的脉冲数
#define MAX_SPEED       380  // 空载满速380r/m

#define FB_USE_GRAPHIC          // 使用图像曲线作为反馈
/* 私有变量 ------------------------------------------------------------------*/
__IO uint8_t  Start_flag = 0;       // PID 开始标志
uint16_t  Motor_Dir = MOTOR_DIR_CW;  // 电机方向
__IO int32_t Loc_Pulse;             // 编码器捕获值 Pulse

/* PID结构体 */
PID_TypeDef  sPID;               // PID参数结构体
PID_TypeDef  *ptr =  &sPID;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
float lastDuty = 0;



static const char* taskStateDsp[] =
{
	TO_STR(ST_MOTOR_IDLE),
	TO_STR(ST_MOTOR_JOG),
	TO_STR(ST_MOTOR_JOG_CYC),
	TO_STR(ST_MOTOR_JOG_FINISH),
	TO_STR(ST_MOTOR_WELD),
	TO_STR(ST_MOTOR_WELD_CYC),
	TO_STR(ST_MOTOR_HOME),
	TO_STR(ST_MOTOR_HOME_PID),
	TO_STR(ST_MOTOR_HOME_FINISH),
	TO_STR(ST_MOTOR_FINISH),
};

void PID_ParamInit(float maxOut) ;
float LocPIDCalc(int32_t NextPoint);



/******************** PID 控制设计 ***************************/
/**
  * 函数功能: PID参数初始化
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void PID_ParamInit(float maxOut)
{
    sPID.LastError = 0;               // Error[-1]
    sPID.PrevError = 0;               // Error[-2]
    sPID.Proportion = SPD_P_DATA; // 比例常数 Proportional Const
    sPID.Integral = SPD_I_DATA;   // 积分常数  Integral Const
    sPID.Derivative = SPD_D_DATA; // 微分常数 Derivative Const
    sPID.SetPoint = TARGET_LOC;     // 设定目标Desired Value
    sPID.MaxOutput = maxOut;
    sPID.MinOutput = 50.0f;
}

/**
  * 函数名称：位置闭环PID控制设计
  * 输入参数：当前控制量
  * 返 回 值：目标控制量
  * 说    明：无
  */
float LocPIDCalc(int32_t NextPoint)
{
	float OutVal;
	int32_t iError, dError;
	iError = sPID.SetPoint - NextPoint; //偏差

	if ((iError < 50) && (iError > -50))
		iError = 0;

	/* 限定积分区域 */
	if ((iError < 400) && (iError > -400))
	{
		sPID.SumError += iError; //积分
		/* 设定积分上限 */
		if (sPID.SumError >= (TARGET_LOC * 10))
			sPID.SumError = (TARGET_LOC * 10);
		if (sPID.SumError <= -(TARGET_LOC * 10))
			sPID.SumError = -(TARGET_LOC * 10);
	}

	dError = iError - sPID.LastError; //微分
	sPID.LastError = iError;
	OutVal = ((sPID.Proportion * (float) iError) //比例项
	+ (sPID.Integral * (float) sPID.SumError) //积分项
			+ (sPID.Derivative * (float) dError)); //微分项
	if (OutVal > sPID.MaxOutput)
	{
		OutVal = sPID.MaxOutput;
	}
	else if (OutVal < -sPID.MaxOutput)
	{
		OutVal = -sPID.MaxOutput;
	}

	if(iError != 0)
	{
		if((OutVal >0) &&(OutVal < sPID.MinOutput))
			OutVal = sPID.MinOutput;
		if((OutVal < 0) &&(OutVal > -sPID.MinOutput))
			OutVal = -sPID.MinOutput;
	}
	return OutVal/100.0f;
}


void SetSpeedOutVolt(float duty)
{
	float actDuty = duty;
	if(duty <= 0)
	{
		actDuty = -duty;
	}
	else
	{
		actDuty = duty;
	}
	if((actDuty < 0.4) &&(actDuty >0.01))
	{
		//0.4v
		actDuty = 0.4f;
	}
	if(duty <= 0)
	{
		Motor_Dir = MOTOR_DIR_CCW;
		digitOutput |= (1<<CHN_OUT_MOTOR_DIR);
	}
	else
	{
		Motor_Dir = MOTOR_DIR_CW;
		digitOutput &= ~(1<<CHN_OUT_MOTOR_DIR);
	}

	lastDuty = duty;
	SetDAOutputFloat(CHN_DA_SPEED_OUT, actDuty);
}

void SetCurrOutVolt(float curr)
{
	SetDAOutputFloat(CHN_DA_CURR_OUT, curr);
}

void OutputCurrent(float currValue)
{
	SetCurrOutVolt(GetCurrCtrlOutput(currValue));
}



void UpdateWeldSetting(void)
{
	static int32_t   last_motorPos_Read = 0;
	static uint32_t  cnt = 0;
	static uint32_t  last_cnt = 0;
	motorPos_Read = (OverflowCount*CNT_MAX) + (int32_t)__HAL_TIM_GET_COUNTER(&htimx_Encoder) + (int32_t)lastMotorPos_PowerDown;
	RTC_WriteMotorPos(motorPos_Read);
	cnt++;
	if(cnt > last_cnt)
	{
		if(cnt - last_cnt >= 200)
		{
			int32_t cntLoc = motorPos_Read - last_motorPos_Read;
			last_motorPos_Read = motorPos_Read;
			if(cntLoc < 0)
				cntLoc = -cntLoc;
			motorSpeed_Read = (float)((cntLoc*0.8333333333333333)/ang2CntRation);//5*cntLoc*60/(ang2CntRation*360.0)
			last_cnt = cnt;
		}
	}
	else
	{
		last_cnt = cnt;
		last_motorPos_Read = motorPos_Read;
	}

}


void StartMotorTsk(void const * argument)
{
	(void) argument;
	uint32_t tickOut = 1000;
	osEvent event;
	float duty = 0;
	TSK_MSG locMsg;
	const uint8_t taskID = TSK_ID_MOTOR;
	MOTOR_STATE tskState = ST_MOTOR_IDLE;
	uint32_t jogStartTick = 0;
	float targetJogDuty;
	float targetJogDutyAcc = 0.1f;
	float highSpeedDuty = 5.0f;
	uint16_t jogDir = MOTOR_DIR_CW;
	InitTaskMsg(&locMsg);
	TracePrint(taskID,"started  \n");
	ENCODER_TIMx_Init();
	SetSpeedOutVolt(0);
	PID_ParamInit(500);
	while (TASK_LOOP_ST)
	{
		event = osMessageGet(MOTOR_CTRL, tickOut);
		if (event.status != osEventMessage)
		{
			TracePrint(taskID, "Timeout: %d,\t%s, Time%d\n",tskState, taskStateDsp[tskState], tickOut);
			tickOut = osWaitForever;
			switch (tskState)
			{
			case ST_MOTOR_IDLE:
				break;
			case ST_MOTOR_JOG_DELAY:
			{
				float targetJogDuty1 = targetJogDuty;
				float targetJogDutyAcc1 = targetJogDutyAcc;
				uint32_t newTick = HAL_GetTick();
				uint32_t tickDiff;
				if(jogDir != MOTOR_DIR_CW)
				{
					targetJogDuty1 = -targetJogDuty1;
					targetJogDutyAcc1 = -targetJogDutyAcc1;
				}
				duty = lastDuty + targetJogDutyAcc1;
				if(jogDir != MOTOR_DIR_CW)
				{
					if(duty <= targetJogDuty1)
					{
						duty = targetJogDuty1;
					}
				}
				else
				{
					if(duty >= targetJogDuty1)
					{
						duty = targetJogDuty1;
					}
				}
				SetSpeedOutVolt(duty);
				tskState = ST_MOTOR_JOG_DELAY;
				tickOut = MOTOR_CTRL_TIME;
				if(newTick > jogStartTick)
				{
					tickDiff = newTick - jogStartTick;
				}
				else
				{
					tickDiff = newTick + (0xFFFFFFFF - jogStartTick)+1;
				}
				if(tickDiff > JOG_TIME)
				{
					tskState = ST_MOTOR_JOG_FINISH;
					SendTskMsgLOC(MOTOR_CTRL, &locMsg);
				}
			}
				break;
			case ST_MOTOR_HOME_PID:
			{
				int32_t iError;
				sPID.SetPoint = motorPosHome;
				tskState = ST_MOTOR_HOME_PID;
				duty = LocPIDCalc(motorPos_Read);
				if(motorPos_Read < motorPosHome)
				{
					iError = motorPosHome - motorPos_Read;
				}
				else
				{
					iError = motorPos_Read - motorPosHome;
				}
				if(iError<MOTOR_HOME_DEVIATION)
				{
					duty = 0;
					tskState = ST_MOTOR_HOME_FINISH;
					SendTskMsgLOC(MOTOR_CTRL, &locMsg);
				}
				/* 输出PWM */
				SetSpeedOutVolt( duty);
				tickOut = MOTOR_CTRL_TIME;
			}
			break;
			case ST_MOTOR_WELD_MOTION_CYC:
				actWelSpeedUsed = ptrCurrWeldSeg->weldSpeed * speedAdjust;
				duty = GetSpeedCtrlOutput(actWelSpeedUsed);
				if(weldDir != MOTOR_DIR_CW)
				{
					duty = -duty;
				}
				SetSpeedOutVolt(duty);
				tickOut = MOTOR_SPEED_UPDATE_TIME;
				break;
			default:
				tskState = ST_WELD_IDLE;
				SendTskMsgLOC(MOTOR_CTRL, &locMsg);
				break;
			}
		}
		else
		{
			const TSK_STATE mainTskState = TSK_MSG_CONVERT(event.value.p)->tskState;
			TSK_MSG_RELASE;
			tickOut = osWaitForever;
			if ( mainTskState == TSK_FORCE_BREAK)
			{
			//	Break_RO();
				tskState = ST_WELD_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(MOTOR_CTRL, &locMsg);
				//force state change to be break;
			}
			else if (TSK_RESETIO == mainTskState)
			{
				tskState = ST_WELD_FINISH;
				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				SendTskMsgLOC(MOTOR_CTRL, &locMsg);

			//	Reset_IO();
			}
			else if ( mainTskState == TSK_INIT)
			{
				//idle
				//this function could be trigger always by other task;

				locMsg = *(TSK_MSG_CONVERT(event.value.p));
				locMsg.tskState = TSK_SUBSTEP;
				tskState = locMsg.val.value;
				SendTskMsgLOC(MOTOR_CTRL, &locMsg);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
				switch (tskState)
				{
				case ST_MOTOR_IDLE:
					break;
				case ST_MOTOR_JOGP:
					jogDir = MOTOR_DIR_CW;
					tskState = ST_MOTOR_JOG_INIT;
					SendTskMsgLOC(MOTOR_CTRL, &locMsg);
					break;
				case ST_MOTOR_JOGN:
					jogDir = MOTOR_DIR_CCW;
					tskState = ST_MOTOR_JOG_INIT;
					SendTskMsgLOC(MOTOR_CTRL, &locMsg);
					break;
				case ST_MOTOR_JOG_INIT:
					actWelSpeedUsed = motorSpeedSet.jogSpeed * speedAdjust;
					targetJogDuty = GetSpeedCtrlOutput(actWelSpeedUsed);
					targetJogDutyAcc = GetSpeedCtrlOutput(motorSpeedSet.accSpeedPerSeond);
					tickOut = 0;
					tskState = ST_MOTOR_JOG_DELAY;
					jogStartTick = HAL_GetTick();
					break;
				case ST_MOTOR_JOG_FINISH:
					SetSpeedOutVolt(0);
					tskState = ST_WELD_IDLE;
					break;
				case ST_MOTOR_HOME:
					highSpeedDuty = GetSpeedCtrlOutput(motorSpeedSet.homeSpeed);
					PID_ParamInit(highSpeedDuty*100.0f);
					tickOut = 0;
					tskState = ST_MOTOR_HOME_PID;
					break;
				case ST_MOTOR_WELD_MOTION_START:
					actWelSpeedUsed = ptrCurrWeldSeg->weldSpeed * speedAdjust;
					duty = GetSpeedCtrlOutput(actWelSpeedUsed);
					if(weldDir != MOTOR_DIR_CW)
					{
						duty = -duty;
					}
					SetSpeedOutVolt(duty);
					tskState = ST_MOTOR_WELD_MOTION_CYC;
					tickOut = MOTOR_SPEED_UPDATE_TIME;
					break;
				case ST_MOTOR_WELD_MOTION_CYC:
					tickOut = 0;
					break;
				case ST_MOTOR_WELD_MOTION_FINISH:
					TSK_FINISH_ACT(&locMsg,taskID,OK,OK);
					tskState = ST_WELD_IDLE;
					//finish
					break;
				default:
					tskState = ST_WELD_IDLE;
					SendTskMsgLOC(MOTOR_CTRL, &locMsg);
					break;
				}
				//when finish->call back;
			}

		}
		//call when IO task is executed;

	}
}

