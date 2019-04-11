/*
 * tsk_motor.c
 *
 *  Created on: 2019��3��27��
 *      Author: pli
 */



#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "dev_encoder.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
typedef struct
{
  __IO int32_t  SetPoint;    //�趨Ŀ�� Desired Value
  __IO float    SumError;    //����ۼ�
  __IO float    Proportion;  //�������� Proportional Const
  __IO float    Integral;    //���ֳ��� Integral Const
  __IO float    Derivative;  //΢�ֳ��� Derivative Const
  __IO int      LastError;   //Error[-1]
  __IO int      PrevError;   //Error[-2]
  __IO float	MaxOutput;
}PID_TypeDef;

/* ˽�к궨�� ----------------------------------------------------------------*/

/*************************************/
// ����PID��غ�
// �����������趨�Ե������Ӱ��ǳ���
// PID����������ʱ��ϢϢ���
/*************************************/
#define  SPD_P_DATA      1.025f       // P����
#define  SPD_I_DATA      0.215f       // I����
#define  SPD_D_DATA      0.1f         // D����
#define  TARGET_LOC      6600        // Ŀ���ٶ�

#define ENCODER         11   // ����������
#define SPEEDRATIO      30   // ������ٱ�
#define PPR             (SPEEDRATIO*ENCODER*4) // Pulse/r ÿȦ�ɲ����������
#define MAX_SPEED       380  // ��������380r/m

#define FB_USE_GRAPHIC          // ʹ��ͼ��������Ϊ����
/* ˽�б��� ------------------------------------------------------------------*/
__IO uint8_t  Start_flag = 0;       // PID ��ʼ��־
uint16_t  Motor_Dir = MOTOR_DIR_CW;  // �������
__IO int32_t Loc_Pulse;             // ����������ֵ Pulse

/* PID�ṹ�� */
PID_TypeDef  sPID;               // PID�����ṹ��
PID_TypeDef  *ptr =  &sPID;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
float lastDuty = 0;

typedef enum
{
	ST_MOTOR_IDLE,
	ST_MOTOR_JOG,
	ST_MOTOR_JOG_CYC,
	ST_MOTOR_JOG_FINISH,
	ST_MOTOR_WELD_MOTION,
	ST_MOTOR_WELD_CYC,
	ST_MOTOR_WELD_END,
	ST_MOTOR_HOME,
	ST_MOTOR_HOME_PID,
	ST_MOTOR_HOME_FINISH,
	ST_MOTOR_FINISH,
}MOTOR_STATE;


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

void PID_ParamInit(void) ;
float LocPIDCalc(int32_t NextPoint);
#define MOTOR_CTRL_TIME 50
#define MOTOR_WELD_CYC_TIME 10


/******************** PID ������� ***************************/
/**
  * ��������: PID������ʼ��
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void PID_ParamInit()
{
    sPID.LastError = 0;               // Error[-1]
    sPID.PrevError = 0;               // Error[-2]
    sPID.Proportion = SPD_P_DATA; // �������� Proportional Const
    sPID.Integral = SPD_I_DATA;   // ���ֳ���  Integral Const
    sPID.Derivative = SPD_D_DATA; // ΢�ֳ��� Derivative Const
    sPID.SetPoint = TARGET_LOC;     // �趨Ŀ��Desired Value
    sPID.MaxOutput = 10000.0;
}

/**
  * �������ƣ�λ�ñջ�PID�������
  * �����������ǰ������
  * �� �� ֵ��Ŀ�������
  * ˵    ������
  */
float LocPIDCalc(int32_t NextPoint)
{
	float OutVal;
	int32_t iError, dError;
	iError = sPID.SetPoint - NextPoint; //ƫ��

	if ((iError < 50) && (iError > -50))
		iError = 0;

	/* �޶��������� */
	if ((iError < 400) && (iError > -400))
	{
		sPID.SumError += iError; //����
		/* �趨�������� */
		if (sPID.SumError >= (TARGET_LOC * 10))
			sPID.SumError = (TARGET_LOC * 10);
		if (sPID.SumError <= -(TARGET_LOC * 10))
			sPID.SumError = -(TARGET_LOC * 10);
	}

	dError = iError - sPID.LastError; //΢��
	sPID.LastError = iError;
	OutVal = ((sPID.Proportion * (float) iError) //������
	+ (sPID.Integral * (float) sPID.SumError) //������
			+ (sPID.Derivative * (float) dError)); //΢����
	if (OutVal > sPID.MaxOutput)
	{
		OutVal = sPID.MaxOutput;
	}
	else if (OutVal < sPID.MaxOutput)
	{
		OutVal = -sPID.MaxOutput;
	}
	return OutVal/100.0f;
}

void SetMotorSpeed(float duty, uint16_t updateType)
{
	float val = (duty*655.36f);
	if(duty >= 0)
	{
		Motor_Dir = MOTOR_DIR_CW;
		digitOutput |= (1<<CHN_OUT_MOTOR_DIR);
	}
	else
	{
		val = -val;
		Motor_Dir = MOTOR_DIR_CCW;
		digitOutput &= ~(1<<CHN_OUT_MOTOR_DIR);
	}
	lastDuty = duty;
	if(val >= 65536)
		val = 65535;
	daOutputSet[CHN_DA_SPEED_OUT] = (uint16_t)(val);
	if(updateType == 0)
		SendTskMsg(OUTPUT_QID, TSK_INIT, (DA_OUT_REFRESH_SPEED|DO_OUT_REFRESH), NULL, NULL);
}



void UpdateWeldSetting(void)
{
	static int32_t   last_motorPos_Read = 0;
	static uint32_t  cnt = 0;
	static uint32_t  last_cnt = 0;
	motorPos_Read = (OverflowCount*CNT_MAX) + (int32_t)__HAL_TIM_GET_COUNTER(&htimx_Encoder) + (int32_t)lastMotorPos_PowerDown;
	cnt++;
	if(cnt > last_cnt)
	{
		if(cnt - last_cnt >= 200)
		{
			int32_t cntLoc = motorPos_Read - last_motorPos_Read;
			motorSpeed_Read = (cntLoc*0.8333333333333333f)/ang2CntRation;//5*cntLoc*60/(ang2CntRation*360.0)
			last_cnt = cnt;
		}
	}
	else
	{
		last_cnt = cnt;
		last_motorPos_Read = motorPos_Read;
	}
	if(weldState != ST_WELD_IDLE)
	{
		//update weldSpeed
		//Update DA timeSetting
		//Update DA value;
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

	float targetJogDuty;
	float targetJogDutyAcc = 0.1f;

	InitTaskMsg(&locMsg);
	TracePrint(taskID,"started  \n");
	/* ��������ʼ����ʹ�ܱ�����ģʽ */
	ENCODER_TIMx_Init();
	/* �趨ռ�ձ� */

	SetMotorSpeed(0, 0);  // 0% ռ�ձ�
	/* PID ������ʼ�� */
	PID_ParamInit();
	/* ����ѭ�� */
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
			case ST_MOTOR_JOG_CYC:
			{
				uint32_t dir1 = (digitInput & (1<<CHN_IN_JOG_DIR));
				float targetJogDuty1 = targetJogDuty;
				float targetJogDutyAcc1 = targetJogDutyAcc;
				if(dir1)
				{
					targetJogDuty = -targetJogDuty1;
					targetJogDutyAcc1 = -targetJogDutyAcc1;
				}
				duty = lastDuty + targetJogDutyAcc1;
				if(dir1)
				{
					if(duty <= targetJogDuty)
					{
						duty = targetJogDuty;
					}
				}
				else
				{
					if(duty >= targetJogDuty)
					{
						duty = targetJogDuty;
					}
				}
				SetMotorSpeed(duty, 0);
				tskState = ST_MOTOR_JOG_CYC;
				tickOut = MOTOR_CTRL_TIME;
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
				/* ���PWM */
				SetMotorSpeed( duty, 0 );
				tickOut = MOTOR_CTRL_TIME;
			}
			break;
			case ST_MOTOR_WELD_CYC:

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
				tskState = GetStateRequest(tskState);
				SendTskMsgLOC(MOTOR_CTRL, &locMsg);

				targetJogDuty = GetSpeedDuty(motorSpeedSet.jogSpeed);
				targetJogDutyAcc = GetSpeedDuty(motorSpeedSet.accSpeedPerSeond);
			}
			else if ( mainTskState == TSK_SUBSTEP)
			{
				//run step by step;
/*
				ST_MOTOR_IDLE,
					ST_MOTOR_JOG,
					ST_MOTOR_JOG_FINISH,
					ST_MOTOR_WELD,
					ST_MOTOR_WELD_CYC,
					ST_MOTOR_HOME,
					ST_MOTOR_HOME_PID,
					ST_MOTOR_HOME_FINISH,
					ST_MOTOR_FINISH,
*/
				switch (tskState)
				{
				case ST_MOTOR_IDLE:
					break;
				case ST_MOTOR_HOME:
					tickOut = 0;
					tskState = ST_MOTOR_HOME_PID;
					break;
				case ST_MOTOR_WELD_MOTION:
					//
					weldDir = MOTOR_DIR_CW;
					UpdateWeldFInishPos();
					motorPos_WeldStart = motorPos_Read;
					duty = GetSpeedOutput(GetWeldSegSpeed(0));
					if(weldDir == MOTOR_DIR_CW)
					{
						motorPos_WeldStart = motorPos_Read;
						motorPos_WeldFinish = GetWeldFinishPos(0xFFFF) + motorPos_WeldStart;
					}
					else
					{
						motorPos_WeldStart = motorPos_Read;
						motorPos_WeldFinish = motorPos_WeldStart-GetWeldFinishPos(0xFFFF);
						duty = -duty;
					}
					SetMotorSpeed(duty, 0);
					tickOut = 0;
					tskState = ST_MOTOR_WELD_CYC;
					break;
				case ST_MOTOR_WELD_END:
					//
					//sendfinish task
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

