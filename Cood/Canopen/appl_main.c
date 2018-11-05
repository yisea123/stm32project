/*
* main.c - contains program main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 14734 2016-08-03 12:55:38Z phi $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief main routine
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <main.h>
/* header of project specific types
---------------------------------------------------------------------------*/
#include <co_datatype.h>
#include <gen_define.h>

#include <co_p401.h>
#include <usr_401.h>
#include <co_canopen.h>
#include "gen_indication.h"
#include "tim.h"
#include "main.h"
//#include "terminal.h" 	/* for coloring LED output */
#include <cpu_stm32.h>

#include "unit_rtc_cfg.h"
#include "unit_io_cfg.h"
#include "unit_sys_diagnosis.h"
#include "unit_sch_cfg.h"
#include "tsk_sch.h"
#include "GPIO.h"

#ifdef BETA_V

#define LED_GREEN_ON		HAL_GPIO_WritePin(CAN1_LED_DIR1_GPIO_Port, CAN1_LED_DIR1_Pin, GPIO_PIN_SET)
#define LED_GREEN_OFF		HAL_GPIO_WritePin(CAN1_LED_DIR1_GPIO_Port, CAN1_LED_DIR1_Pin, GPIO_PIN_RESET)

#define LED_RED_ON			HAL_GPIO_WritePin(CAN1_LED_DIR2_GPIO_Port, CAN1_LED_DIR2_Pin, GPIO_PIN_SET)
#define LED_RED_OFF			HAL_GPIO_WritePin(CAN1_LED_DIR2_GPIO_Port, CAN1_LED_DIR2_Pin, GPIO_PIN_RESET)

#else

#define LED_GREEN_ON		//HAL_GPIO_WritePin(CAN1_LED_DIR1_GPIO_Port, CAN1_LED_DIR1_Pin, GPIO_PIN_SET)
#define LED_GREEN_OFF		//HAL_GPIO_WritePin(CAN1_LED_DIR1_GPIO_Port, CAN1_LED_DIR1_Pin, GPIO_PIN_RESET)

#define LED_RED_ON			//HAL_GPIO_WritePin(CAN1_LED_DIR2_GPIO_Port, CAN1_LED_DIR2_Pin, GPIO_PIN_SET)
#define LED_RED_OFF			//HAL_GPIO_WritePin(CAN1_LED_DIR2_GPIO_Port, CAN1_LED_DIR2_Pin, GPIO_PIN_RESET)

#endif

/* constant definitions
---------------------------------------------------------------------------*/

static __IO SemaphoreHandle_t lockSdo = NULL;
char Master[] = "Rainbow";

static uint32_t nodeStateErrId = 0;

#define CAN_OPEN_DELAYTIME		100

/* local defined data types
---------------------------------------------------------------------------*/
typedef enum {
	NO_ACTION = 0,
	SLAVE_STARTED,
	SDO_SEND,
	SDO_CONFIRMED,
	PDO_SENDING
} APPL_T;

typedef struct{
	uint8_t hbCount;
	uint8_t errState;
	uint8_t nmtState;
	uint8_t lastCount;
}NodeMonitor;

static uint8_t slaveStateCfg[16];

uint16_t canOpenState = STATE_INIT;

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static void emcyConsumer(UNSIGNED8	node, UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister, UNSIGNED8 const	*addErrorBytes);
static uint32_t littleApplication(void);
static void sdoClientWriteInd(
	UNSIGNED8	sdoNr,
	UNSIGNED16	index,
	UNSIGNED8	subIndex,
	UNSIGNED32	errorVal
	);

static void sdoClienReadInd(
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		UNSIGNED32	size,
		UNSIGNED32	result);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED8 ledOut = 0;
#if 0
APPL_T    applState = NO_ACTION;
#endif

CO_TIMER_T	myTimer;
__IO uint16_t ioStart = 0;

/* local defined variables
---------------------------------------------------------------------------*/



static void ExitMsg(uint16_t idx)
{
	TraceDBG(TSK_ID_CAN_MASTER,"Error in Can: %d\n", idx);
	assert(0);
}

char deviceName[32] = "Hach Main Board";
uint8_t	devAdr = 0x01;
uint32_t devType = 0x6000;
/*********************************************************************/
static RET_T loadPara(
		UNSIGNED8	sIndex
	)
{

	TraceMsg(TSK_ID_CAN_MASTER,"load para indication %d\n", sIndex);

	#if 0
UNSIGNED16	i, startIdx, lastIdx;
RET_T	retVal;
//const CO_OBJECT_DESC_T *pDesc;
void	*pObj;


	if (saveOdValid != CO_TRUE)  {
		return(RET_OK);
	}

	switch (sIndex)  {
		case 1: /* all */
			startIdx = 0x1000;
			lastIdx = 0xffff;
			break;
		case 2: /* comm */
			startIdx = 0x1000;
			lastIdx = 0x1fff;
			break;
		default:
			startIdx = 0x1000;
			lastIdx = 0x1000;
			break;
	}

	for (i = 0; i < savedCnt; i++)  {
		if ((savedData[i].index >= startIdx)
		 && (savedData[i].index <= lastIdx))  {

			/* get object description */
			retVal = coOdGetObjDescPtr(savedData[i].index, savedData[i].subIndex, &pDesc);
			if (retVal != RET_OK)  {
				return(retVal);
			}

			/* get pointer to object */
			pObj = coOdGetObjAddr(savedData[i].index, savedData[i].subIndex);
			if (pObj == NULL)  {
				return(RET_IDX_NOT_FOUND);
			}

printf("Load %x:%d, size %d\n", savedData[i].index, savedData[i].subIndex, savedData[i].size);
			/* get data type of this index */
			switch (savedData[i].size)  {
				case 1:
				case 2:
				case 4:
					coNumMemcpy(pObj, &savedData[i], savedData[i].size, CO_ATTR_NUM);
					break;
				default:
					return(RET_SDO_INVALID_VALUE);
			}
		}
	}

#endif
	return(RET_OK);
}


static void UpdateMappingTable(uint8_t* mapTable, uint8_t* enable,uint16_t maxIdx)
{
	for(uint16_t i=0;i<maxIdx;i++)
	{
		coOdSetCobid((uint16_t)(0x1800u+i), 1, 0x80000000);
		coOdSetCobid((uint16_t)(0x1400u+i), 1, 0x80000000);
		(void)coOdPutObj_u8 ((uint16_t)(0x1600u + i), 0, 0);
		(void)coOdPutObj_u8 ((uint16_t)(0x1a00u + i), 0, 0);
		if(enable[i] != 0)
		{
			if(mapTable[i] == DI_BOARD)
			{
				//map rpdo cobid
				(void)coOdPutObj_u8 ((uint16_t)(0x1600u + i), 0, 2);
				coOdSetCobid((uint16_t)(0x1400u+i), 1, (uint32_t)(0x180u+i+DI_ADR));

			}
			else if(mapTable[i] == DO_BOARD)
			{


				uint32_t newValue = (uint32_t)(0x20000108 + i*0x200);
				(void)coOdPutObj_u32 ((uint16_t)(0x1a00u + i), 1, newValue);
				newValue = (uint32_t)(0x20000208 + i*0x200);
				(void)coOdPutObj_u32 ((uint16_t)(0x1a00u + i), 2, newValue);
				(void)coOdPutObj_u8 ((uint16_t)(0x1a00u + i),  0, 2);


			//	coOdPutObj_u8(0x1800+i, 2, 255);
				//map tpdo cobid
				(void)coOdSetCobid((uint16_t)(0x1800u+i), 1,  (uint32_t)(0x200u+i+DO_ADR));
			}
			else if(mapTable[i] == AO_BOARD)
			{
				uint32_t newValue = (uint32_t)(0x20010110 + i*0x200);
				(void)coOdPutObj_u32 ((uint16_t)(0x1a00u + i), 1, newValue);
				newValue = (uint32_t)(0x20010210 + i*0x200);
				(void)coOdPutObj_u32 ((uint16_t)(0x1a00u + i), 2, newValue);
				(void)coOdPutObj_u8 ((uint16_t)(0x1a00u + i),  0, 2);
			//	coOdPutObj_u8(0x1800+i, 2, 255);
				//map tpdo cobid
				(void)coOdSetCobid((uint16_t)(0x1800u+i), 1, (uint32_t)(0x200u+i+AO_ADR));
			}
		}
	}
}


/*********************************************************************/
static void ledGreenInd(
		BOOL_T	on
	)
{
	if(on)
	{
		LED_GREEN_ON;
	}
	else
	{
		LED_GREEN_OFF;
	}

	//shell_printf("GREEN: %d\n", on);
}


/*********************************************************************/
static void ledRedInd(
		BOOL_T	on
	)
{
	if(on)
	{
		LED_RED_ON;
	}
	else
	{
		LED_RED_OFF;
	}

	//shell_printf("RED: %d\n", on);
}


/***************************************************************************/
/**
* \brief main entry
*
* \param
*	nothing
* \results
*	nothing
*/
void StartCanopenTask(void const * argument)
{
	(void)argument;
	CO_TIME_T       cotime;


	/* OS specific setup */
	if (codrvOSConfig() != RET_OK)  {
		ExitMsg(3);
	}

	lockSdo = OS_CreateSemaphore();
	/* HW initialization */
	codrvHardwareInit();

	/* init can driver */
	if (codrvCanInit(125) != RET_OK)  {
		ExitMsg(1);
	}

	/* init canopen */
    if (coCanOpenStackInit(loadPara) != RET_OK)  {
    	TraceDBG(TSK_ID_CAN_MASTER,"error init library\n");
		ExitMsg(1);
	}

	/* setup timer */
//	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
//		ExitMsg(2);
//	}


	/* register event functions */
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		ExitMsg(3);
	}
	if (coEventRegister_EMCY_CONSUMER(emcyConsumer) != RET_OK)  {
		ExitMsg(4);
	}
	if (codrvCanEnable() != RET_OK)  {
		ExitMsg(5);
	}
	if (coEventRegister_LED_GREEN(ledGreenInd) != RET_OK)  {
		ExitMsg(10);
	}
	if (coEventRegister_LED_RED(ledRedInd) != RET_OK)  {
		ExitMsg(11);
	}
	/* send out time message */
	cotime.msec = 0x1234;
	cotime.days = 0x7890;
	coTimeWriteReq(&cotime);
	/* setup indication for SDO client */
	coEventRegister_SDO_CLIENT_WRITE(sdoClientWriteInd);
	coEventRegister_SDO_CLIENT_READ(sdoClienReadInd);

	if (codrvCanEnable() != RET_OK)  {
		ExitMsg(6);
	}

	/* reconfigure 1st local SDO client parameter to send SDOs to slave */



	/* reconfigure local TPDO to be send to slave */
//	coOdSetCobid(0x1800, 1, 0x300 + EXAMPLE_401_SLAVE_NODE_ID);

//	StopAllNode();
//	StartAllNode();




	/* register timer to handle application every seconds */
//	coTimerStart(&myTimer, 1000ul*1000ul, littleApplication, NULL, CO_TIMER_ATTR_ROUNDUP_CYCLIC);
	/* start master itself */
//	coNmtLocalStateReq(CO_NMT_STATE_OPERATIONAL);
#if 0
		/* write emcy */
	if (coEmcyWriteReq(0x1234, &emcyData[0]) != RET_OK)  {
		ExitMsg(15);
	}
#endif
//	Test();
/* --- 401 specific ------------------------------------------------*/
	/* profile 401 HW acess registration */
	if (co401Init() != RET_OK)  {
		TraceMsg(TSK_ID_CAN_MASTER,"error init 401 profile handler\n");
		ExitMsg(14);
	}

	if (coEventRegister_401(
				byte_in_piInd,		/* digital in  HW API function   */
				byte_out_piInd,		/* digital out HW API function   */
				analog_in_piInd,	/* the analog in HW API function */
				analog_out_piInd	/* analog out HW API function    */
				) != RET_OK)  {
		TraceMsg(TSK_ID_CAN_MASTER,"registration of 401 HW acess functions failed\n");
		ExitMsg(14);
	}

	/* -----------------------------------------------------------------*/


	/* all is ready now,
	 * enable CAN driver to receive and transmit CAN frames
	 */
	if (codrvCanEnable() != RET_OK)  {
		ExitMsg(14);
	}
	ioStart = 1;
	//StartTimer();
		
	const uint16_t taskID = TSK_ID_CAN_MASTER;

	while (1)
	{
		freeRtosTskTick[taskID]++;
		if(FVT_Test)
		{
			//do nothing
			osDelay(10);
		}
		else
		{
			//(void)osSignalWait(SIGNAL_CANOPEN_ALL, osWaitForever);//every signal
			codrvWaitForEvent(portMAX_DELAY);

			coCommTask();
		}
		TickCntCanOpen[0]++;
	}
}


uint16_t SdoWrite(uint8_t adr, uint16_t idx, uint8_t subIdx, uint8_t* data, uint16_t len)
{

	uint16_t ret = FATAL_ERROR;
	OS_Use(lockSdo);

	uint32_t msgId = (uint32_t)(idx<<16);
	msgId += subIdx;
	coOdSetCobid(0x1280,1, (UNSIGNED32)(0x600+adr));
	coOdSetCobid(0x1280,2, (UNSIGNED32)(0x580+adr));
	coSdoWrite(1, idx, subIdx, (UNSIGNED8*)data, len, CO_TRUE, CAN_OPEN_DELAYTIME);
	osEvent event = osMessageGet(SDO_Q_ID, 200);
	// substep
	//new event
	if (event.status == osEventMessage)
	{
		if(event.value.v == 0)
			ret = OK;
	}
	OS_Unuse(lockSdo);
	return ret;
}

uint16_t SdoRead(uint8_t adr, uint16_t idx, uint8_t subIdx, uint8_t* data, uint16_t len)
{

	uint16_t ret = FATAL_ERROR;
	OS_Use(lockSdo);

	uint32_t msgId = (uint32_t)(idx<<16);
	msgId += subIdx;
	coOdSetCobid(0x1280,1, (uint32_t)(0x600+adr));
	coOdSetCobid(0x1280,2, (uint32_t)(0x580+adr));

	coSdoRead(1, idx, subIdx, (UNSIGNED8*)data, len, CO_TRUE, CAN_OPEN_DELAYTIME);
	osEvent event = osMessageGet(SDO_Q_ID, CAN_OPEN_DELAYTIME+20);
	// substep
	//new event
	if (event.status == osEventMessage)
	{
		if(event.value.v == 0)
			ret = OK;
	}
	OS_Unuse(lockSdo);
	return ret;
}


uint16_t CaliAOCurrent(uint8_t adr, uint8_t chn, uint16_t val)
{
	assert(adr < 16);
	assert(chn < 2);
	uint16_t ret = FATAL_ERROR;
	if( (IO_Connected[adr] != 0) && (IO_BoardType[adr] == AO_BOARD))
	{
		ret = SdoWrite( (uint8_t)(AO_ADR+adr), 0x2001, (uint8_t)(1+chn), (uint8_t*)&val, 2);
	}
	return ret;
}


uint16_t CaliAOReadback(uint8_t adr, uint8_t chn, uint16_t val)
{
	assert(adr < 16);
	assert(chn < 2);
	uint16_t ret = FATAL_ERROR;
	if( (IO_Connected[adr] != 0) && (IO_BoardType[adr] == AO_BOARD))
	{
		ret = SdoWrite( (uint8_t)(AO_ADR+adr), 0x2001, (uint8_t)(3+chn), (uint8_t*)&val, 2);
	}
	return ret;
}



uint16_t CaliAOReset(uint8_t adr, uint8_t chn)
{
	assert(adr < 16);
	assert(chn < 2);
	uint16_t ret = FATAL_ERROR;
	uint16_t val = 0;
	if( (IO_Connected[adr] != 0) && (IO_BoardType[adr] == AO_BOARD))
	{
		ret = SdoWrite( (uint8_t)(AO_ADR+adr), 0x2001, (uint8_t)(5+chn), (uint8_t*)&val, 2);
	}
	return ret;
}



uint16_t CaliAOStatus(uint8_t adr, uint8_t chn, uint16_t* ptrVal)
{
	assert(adr < 16);
	assert(chn < 2);
	uint16_t ret = FATAL_ERROR;
	if( (IO_Connected[adr] != 0) && (IO_BoardType[adr] == AO_BOARD))
	{
		ret = SdoRead( (uint8_t)(AO_ADR+adr), 0x2001, (uint8_t)(5+chn), (uint8_t*)ptrVal, 2);
	}
	return ret;
}

static void Update_SimuSet(uint16_t st)
{
	TraceDBG(TSK_ID_CAN_MASTER, "Can Simulate is called: %x\n",st);
	uint16_t result = OK;
	static uint8_t idx = 0;
	for(; idx < MAX_IO_BOARD_NUM; idx++)
	{
		if( (IO_Connected[idx] != 0) && (IO_BoardEnable[idx] != 0))
		{
			//board is enabled and connected
			if(IO_BD_TypeSetting[idx] == IO_BoardType[idx])
			{
				//do
				if(IO_BD_TypeSetting[idx] == DO_BOARD)
				{
					if(IO_SimuCfgChn1[idx] == SIMU_IO)
					{
						if(OK != SdoWrite( (uint8_t)(DO_ADR+idx), 0x2000, 1, (uint8_t*)&IO_DO_Chn1_Simu[idx], 1))
						{
							result = (uint16_t)(0x100+idx);
						}
					}
					else
					{
						uint8_t val = 0;
						if(OK != SdoWrite( (uint8_t)(DO_ADR+idx), 0x2000, 1, (uint8_t*)&val, 1))
						{
							result = (uint16_t)(0x200+idx);
						}
					}

					if(IO_SimuCfgChn2[idx] == SIMU_IO)
					{
						if(OK != SdoWrite( (uint8_t)(DO_ADR+idx), 0x2000, 2, (uint8_t*)&IO_DO_Chn2_Simu[idx], 1))
						{
							result = (uint16_t)(0x300+idx);
						}
					}
					else
					{
						uint8_t val = 0;
						if(OK != SdoWrite( (uint8_t)(DO_ADR+idx), 0x2000, 2, (uint8_t*)&val, 1))
						{
							result = (uint16_t)(0x400+idx);
						}
					}
				}
				else if(IO_BD_TypeSetting[idx] == AO_BOARD)
				{
					if(IO_SimuCfgChn1[idx] == SIMU_IO)
					{
						if(OK != SdoWrite( (uint8_t)(AO_ADR+idx), 0x2001, 1, (uint8_t*)&IO_AO_Chn1_Simu[idx], 2))
						{
							result = (uint16_t)(0x100+idx);
						}
					}
					else
					{
						uint16_t val = 0;
						if(OK != SdoWrite( (uint8_t)(AO_ADR+idx), 0x2001, 1, (uint8_t*)&val, 2))
						{
							result = (uint16_t)(0x200+idx);
						}

					}
					if(IO_SimuCfgChn2[idx] == SIMU_IO)
					{
						if(OK != SdoWrite( (uint8_t)(AO_ADR+idx), 0x2001, 2, (uint8_t*)&IO_AO_Chn2_Simu[idx], 2))
						{
							result = (uint16_t)(0x300+idx);
						}
					}
					else
					{
						uint16_t val = 0;
						if(OK != SdoWrite( (uint8_t)(AO_ADR+idx), 0x2001, 2, (uint8_t*)&val, 2))
						{
							result = (uint16_t)(0x400+idx);
						}

					}
				}
				else
				{}

				if(st == 0)
				{
			//		break;
				}
			}


		}
	}
	if(idx >= MAX_IO_BOARD_NUM)
		idx = 0;
	if(result != 0)
		TraceDBG(TSK_ID_CAN_MASTER, "Can Simulate error: %x\n",result);
}
//this is the buffer for canopen;

uint8_t doValue[32];
uint8_t diValue[32];
uint16_t aoValue[32];
uint16_t newBoardId = 0;

static void UpdateIOBoardInfo(void)
{
#define CANOPEN_ADO_COUNT_MAX		4

	static uint16_t countTimes = 0;
	uint16_t errId = 0;
	uint16_t errCount = 0;
	uint16_t ret = OK;
	for(uint16_t idx=0; idx < MAX_IO_BOARD_NUM; idx++)
	{
		if(IO_Connected[idx] != 0)
		{
#if 1
			if(IO_BoardType[idx] == DI_BOARD)
			{
				ret = SdoRead( (uint8_t)(DI_ADR+idx), 0x2002, 1, (uint8_t*)&DI_Chn1_Value[idx], 1);
				ret |= SdoRead( (uint8_t)(DI_ADR+idx), 0x2002, 2, (uint8_t*)&DI_Chn2_Value[idx], 1);

				if(ret != OK)
				{
					errCount++;
					errId = idx;
				}
			}
			else
#endif
			if(IO_BoardType[idx] == AO_BOARD)
			{
				ret = SdoRead( (uint8_t)(AO_ADR+idx), 0x2001, 7, (uint8_t*)&AO_Chn1_Value[idx], 2);
				ret |= SdoRead( (uint8_t)(AO_ADR+idx), 0x2001, 8, (uint8_t*)&AO_Chn2_Value[idx], 2);
				if(ret != OK)
				{
					errCount++;
					errId = idx;
				}
			}
			else if(IO_BoardType[idx] == DO_BOARD)
			{
				ret = SdoRead( (uint8_t)(DO_ADR+idx), 0x2000, 3, (uint8_t*)&DO_Chn1_Value[idx], 1);
				ret |= SdoRead( (uint8_t)(DO_ADR+idx), 0x2000, 4, (uint8_t*)&DO_Chn2_Value[idx], 1);
				if(ret != OK)
				{
					errCount++;
					errId = idx;
				}
			}
		}
		else
		{
			DI_Chn1_Value[idx] = 0;
			DI_Chn2_Value[idx] = 0;
			DO_Chn1_Value[idx] = 0;
			DO_Chn2_Value[idx] = 0;
			AO_Chn1_Value[idx] = 0;
			AO_Chn2_Value[idx] = 0;
		}
	}
	if(errCount != 0)
	{
		timeCanIO_ST[IO_ADO_ERR] = GetCurrentST();
		timesErrCnt[IO_ADO_ERR]++;
		TraceDBG(TSK_ID_CAN_MASTER,"ADO canopen error: %d, times: %d\n",errId, countTimes);
		countTimes++;
	}
	else if(countTimes)
	{
		countTimes--;
	}
	else
	{}
}
enum
{
	E_CAN_DUPLICATE_ADR_BOARD = 0x01,
	E_CAN_WRONG_CFG_BOARD = 0x02,
	E_CAN_NEW_BOARD = 0x04,
	E_CAN_MISS_BOARD = 0x08,

};

static uint8_t	IO_Attached[MAX_IO_BOARD_NUM] = {0,0,};
static uint8_t	IO_Type[MAX_IO_BOARD_NUM] = {0,0,};

static uint16_t CheckIOConnection(uint8_t adrType, uint32_t _devType, uint8_t boardType)
{
	uint16_t error = 0;
	for(uint8_t idx = 0; idx < MAX_IO_BOARD_NUM;idx++)
	{
		uint32_t type = 0;
		uint16_t ret = SdoRead((uint8_t)(adrType+idx), 0x1000, 0, (uint8_t*)&type,sizeof(type));
		if(ret == OK)
		{
			type = type & 0xFFF0;
			if(IO_Attached[idx] != 0)
			{
				error |= E_CAN_DUPLICATE_ADR_BOARD;
				TraceDBG(TSK_ID_CAN_MASTER, "two or more device is connected in the same adr: %d => t1: %x t2: %x\n", idx, IO_Connected[idx], adrType);

				if(_devType != type)
				{
					error |= E_CAN_WRONG_CFG_BOARD;
					TraceDBG(TSK_ID_CAN_MASTER, "wrong device type in adr: %d => t1: %x t2: %x\n", idx, _devType, type);
				}
			}
			IO_Attached[idx] = adrType;
			IO_Type[idx] = boardType;
			//new board is found
			if(IO_BD_TypeSetting[idx] != boardType)
			{
				error |= E_CAN_NEW_BOARD;
				TraceDBG(TSK_ID_CAN_MASTER, "new board is found: %d => connect: %x type: %x\n", idx, IO_Connected[idx], adrType);
			}
		}
		else
		{
			if((IO_BoardEnable[idx] != 0) && (IO_BD_TypeSetting[idx] == boardType) )
			{
				error |= E_CAN_MISS_BOARD;
			}
		}
	}
	return error;

}




static uint16_t UpdateIOConn(void)
{
	uint16_t status = OK;
	for(uint16_t i = 0; i< 4; i++)
	{
		memset(IO_Attached, 0, sizeof(IO_Attached));
		memset(IO_Type, 0, sizeof(IO_Type));
		status = CheckIOConnection(DI_ADR,TYPE_DI_BOARD,DI_BOARD);
		status |= CheckIOConnection(DO_ADR,TYPE_DO_BOARD,DO_BOARD);
		status |= CheckIOConnection(AO_ADR,TYPE_AO_BOARD,AO_BOARD);
		if(status == OK)
		{
			break;
		}
	}
	Dia_UpdateDiagnosis(CANOPEN_WARNING, ((status & E_CAN_NEW_BOARD) != 0));
	Dia_UpdateDiagnosis(CANOPEN_ER_CONNECT, ((status&(E_CAN_DUPLICATE_ADR_BOARD|E_CAN_MISS_BOARD)) != 0));
	Dia_UpdateDiagnosis(CANOPEN_ER_CONFIG, ((status&(E_CAN_WRONG_CFG_BOARD|E_CAN_MISS_BOARD)) != 0));
	if(status != OK)
	{
		AlignFunc_IO();
	}
	status = OK;
	for(uint8_t idx = 0; idx < MAX_IO_BOARD_NUM;idx++)
	{
		if(IO_Connected[idx] != IO_Attached[idx])
		{
			status = WARNING;
			TraceMsg(TSK_ID_CAN_MASTER, "IO board connection is updated: adr: %d, old:%d,new:%d\n", idx, IO_BoardType[idx], IO_Type[idx]);
		}
	}
	if(status == WARNING)
	{
		memcpy(IO_Connected, IO_Attached, sizeof(IO_Attached));
		memcpy(IO_BoardType, IO_Type, sizeof(IO_Type));
		timeCanIO_ST[ID_IO_MONITOR] = GetCurrentST();
		timesErrCnt[ID_IO_MONITOR]++;
		TraceMsg(TSK_ID_CAN_MASTER, "IO board connection Status is updated\n");
	}
	return status;
}
static uint16_t CheckIOState(void)
{
	uint16_t ret = OK;
	for(uint16_t i =0; i<16;i++)
	{
		if((slaveStateCfg[i] != slaveState[i]) && (slaveStateCfg[i] == CO_NMT_STATE_OPERATIONAL))
		{
			ret = FATAL_ERROR;
			break;
		}
	}
	return ret;
}
//extern __IO uint16_t setObj;
void StartCanIOTask(void const * argument)
{
	(void)argument;
	uint32_t tickOut = 100;
	uint32_t count = 0;
	osEvent event;
	const uint16_t taskID = TSK_ID_CAN_IO;
	tickOut = 10;
	uint16_t startFlag = 0;
	while (1)
	{
//		count++;
		freeRtosTskTick[taskID]++;
		event = osMessageGet(CAN_IO_ID, tickOut );
		TickCntCanOpen[1]++;
		tickOut = IO_Poll_Time;
		if(FVT_Test)
		{

			//do nothing
		}
		else
		{
			if(event.status != osEventMessage )
			{
				if(ioStart == 1)
				{
					//handle the input/output mappings
					if(STATE_CAN_MAX > canOpenState)
					{
						tickOut = littleApplication();
					}
					else
					{
						if(count % 8 == 7)
						{
							UpdateIOConn();
						}
						UpdateIOBoardInfo();
						count++;
						if(startFlag == 0)
						{
							Update_SimuSet(0);
							startFlag = 1;
						}

					}
				}
			}
			else
			{

				switch(event.value.v)
				{
				case E_CAN_OPEN_SLAVE_UPDATE:
					TraceDBG(TSK_ID_CAN_MASTER, "IO board config is updated, %x\n",	event.value.v);
					canOpenState = STATE_PREOP;
					tickOut = 0;
				//	ReInit_Canopen();
					break;
				case E_CAN_OPEN_SLAVE_EN:
					CanOpenOnLine();
					break;
				case E_CAN_OPEN_SLAVE_REFRESH:
					UpdateIOConn();
					break;
				case E_CAN_OPEN_SIMU_UPDATE:

					Update_SimuSet(0);
					break;
				case E_CAN_OPEN_HEART_BEAT:
					UpdateHeartbeat(heartBeat);
					break;
				default:
					if( event.value.v <= E_CAN_OPEN_DIAG )
					{
						TraceMsg(TSK_ID_CAN_MASTER, "IO board status %x\n",	event.value.v);
						TraceDBG(TSK_ID_CAN_MASTER, "IO board status Error %x\n",	event.value.v);

						//event-> update status
						if(CheckIOState() != OK)
						{
							Dia_UpdateDiagnosis(CANOPEN_ERROR, 1);
							ReInit_Canopen();
						}


					}
					break;
				}

			}
		}
	}
}

static uint8_t adrUpdated[16];


/*********************************************************************/
static void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	/* send start command to master, if its heartbeat has been started */

	if(nodeId >= DI_ADR)
	{
		uint8_t id = (uint16_t)(nodeId - DI_ADR)%16;
		if(id<16)
		{

			IO_Status[id] = (uint16_t)(nmtState<<8);
			IO_Status[id] = (uint16_t)(IO_Status[id] + state);
			slaveState[id] = nmtState;

			if((CO_ERRCTRL_HB_FAILED == state) || (CO_ERRCTRL_BOOTUP == state ))
			{
				uint32_t val = 0;
				adrUpdated[nodeStateErrId % 16] = 1;

				val = state;
				val <<= 8;
				val |= nodeStateErrId;

				MsgPush(CAN_IO_ID, (uint32_t )val, 0);
				nodeStateErrId++;

			}
			else if(nmtState != CO_NMT_STATE_OPERATIONAL)
			{
				if(OK != DiagIOState(nodeId, id, nmtState) )
				{
					uint32_t val = state;
					val <<= 8;
					val |= nodeStateErrId;
					nodeStateErrId++;
					MsgPush(CAN_IO_ID, (uint32_t )val, 0);
				}
			}
			else
			{}
			timesErrCnt[ID_HBSTATE]++;
			timeCanIO_ST[ID_HBSTATE] = GetCurrentST();
			TraceMsg(TSK_ID_CAN_MASTER,"Device Status Updated: %d; state: %d, nmtState: %d\n",id,state,nmtState);


		}
	}
	else if(nodeId == CO_NODE_IDS)
	{
		masterState = nmtState;
		TraceMsg(TSK_ID_CAN_MASTER,"Master Status Updated: %d; state: %d, nmtState: %d\n",nodeId,state,nmtState);
	}

	return;
}

/*********************************************************************/
static void sdoClientWriteInd(
	UNSIGNED8	sdoNr,
	UNSIGNED16	index,
	UNSIGNED8	subIndex,
	UNSIGNED32	errorVal
	)
{
	TracePrint(TSK_ID_CAN_MASTER,"SDO client %d WRITE transfer %x:%d returns 0x%lx\n",
		sdoNr, index, subIndex, errorVal);

	uint32_t msgId = (uint32_t)(index<<16);

	msgId += subIndex;

	MsgPush(SDO_Q_ID, (uint32_t) errorVal, 0);

}


static void sdoClienReadInd(
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		UNSIGNED32	size,
		UNSIGNED32	result)
{
	TracePrint(TSK_ID_CAN_MASTER,"SDO client %d read  %x:%d returns len %d->ret: 0x%lx\n",
			sdoNr, index, subIndex, size,result);

	MsgPush(SDO_Q_ID, (uint32_t) result, 0);
#if 0
	if ((sdoNr == 1) && (index == 0x1017) && (errorVal == 0)) {
		applState = SDO_CONFIRMED;
	}
#endif

}



void UpdateHeartbeat(uint32_t duringTime)
{

	(void)coOdPutObj_u16 (0x1017, 0, (uint16_t)duringTime);

	for(uint8_t idx = 0; idx < MAX_IO_BOARD_NUM;idx++)
	{
		if((IO_Connected[idx] != 0) && (IO_BoardEnable[idx] != DISABLE_MODE))
		{
			uint8_t adr = 0;


			if(IO_BoardType[idx] == DI_BOARD)
			{
				adr = (uint8_t)(DI_ADR+idx);
			}
			else if(IO_BoardType[idx] == DO_BOARD)
			{
				adr = (uint8_t)(DO_ADR+idx);
			}
			else if(IO_BoardType[idx] == AO_BOARD)
			{
				adr = (uint8_t)(AO_ADR+idx);
			}
			else
			{}

			if(adr != 0)
			{
				if(IOConsumer[0] != 0)
				{
					uint16_t ret = coHbConsumerSet(adr,(uint16_t) (duringTime*IOConsumer[0]));
					if(ret != OK)
					{
						TraceDBG(TSK_ID_CAN_MASTER, "Master set Consumer failed: %d, ret:%d\n",adr, ret);
					}
				}
				else
				{
					uint16_t ret = coHbConsumerSet(adr,0);
					if(ret != OK)
					{
						TraceDBG(TSK_ID_CAN_MASTER, "Master set Consumer failed: %d, ret:%d\n",adr, ret);
					}
				}

				SdoWrite(adr,0x1017,0,(uint8_t*)&duringTime, 2);

				if(IOConsumer[1] != 0)
				{
					uint32_t masterAdr = devAdr;
					uint32_t timeOut = (uint32_t) (duringTime*IOConsumer[1]);
					SdoWrite(adr,0x2004,1,(uint8_t*)&masterAdr, 4);
					SdoWrite(adr,0x2004,2,(uint8_t*)&timeOut, 4);
				}

		//		consumer = adr;
		//		consumer<<=16;
		//		consumer +=(uint32_t) (duringTime*1.3);
		//		(void)coOdPutObj_u32 (0x1016, idx, consumer);
			}
		}
	}
}


static void SetDeviceState(uint8_t idx, uint16_t state)
{
	//operational for the board is enabled!
	assert(idx < MAX_IO_BOARD_NUM);
	uint16_t ret = OK;
	uint8_t adr = 0;
	if(IO_BD_TypeSetting[idx] == DI_BOARD)
		adr = (uint8_t)(DI_ADR+idx);
	else if(IO_BD_TypeSetting[idx] == DO_BOARD)
		adr = (uint8_t)(DO_ADR+idx);
	else if(IO_BD_TypeSetting[idx] == AO_BOARD)
		adr = (uint8_t)(AO_ADR+idx);
	else
	{}
	slaveStateCfg[idx] = CO_NMT_STATE_PREOP;
	if(adr != 0)
	{
		if(state == CO_NMT_STATE_OPERATIONAL)
		{
			if(IO_BoardEnable[idx] != 0)
			{
				if(IO_Connected[idx] != 0)
				{
					ret = coNmtStateReq( adr, state, CO_FALSE);
					slaveStateCfg[idx] = CO_NMT_STATE_OPERATIONAL;
				}
			}
		}
		else
		{
			ret = coNmtStateReq( adr, state, CO_FALSE);

		}
	}
	if(ret)
		TraceDBG(TSK_ID_CAN_MASTER, "Master coNmtStateReq failed: adr %x : req: %d, ret:%d\n",adr,state, ret);
}

void UpdateIOState(uint8_t id, uint16_t state)
{

	if(id == 0xFF)
	{
		for(uint8_t idx =0; idx < MAX_IO_BOARD_NUM; idx++)
		{
			SetDeviceState(idx,state);
		}
	}
	else if(id< MAX_IO_BOARD_NUM)
	{
		SetDeviceState(id,state);
	}
}

void ReInit_Canopen(void)
{
	canOpenState = STATE_INIT;
	timeCanIO_ST[IO_APP_INIT] = GetCurrentST();
	timesErrCnt[IO_APP_INIT]++;
}

void FakeInit_Setting(void)
{
	memset(IO_BoardEnable,0,sizeof(IO_BoardEnable));

	for(uint8_t idx =0; idx < MAX_IO_BOARD_NUM; idx++)
	{
		if(IO_BoardType[idx])
		{
			IO_BoardEnable[idx] = 1;
			IO_BD_TypeSetting[idx] = IO_BoardType[idx];
		}
	}
	timeCanIO_ST[ID_FAKEINIT] = GetCurrentST();
	timesErrCnt[ID_FAKEINIT]++;
	ReInit_Canopen();
	TraceMsg(TSK_ID_CAN_MASTER, "Fake init all setting to make connection work\n");
}



uint16_t CheckIOErrorSetting(void)
{
	uint16_t status = OK;
	for(uint16_t idx=0;idx<MAX_IO_BOARD_NUM;idx++)
	{
		if(IO_BoardEnable[idx] != 0)
		{
			if(IO_Connected[idx] != 0)
			{
				if(IO_BoardType[idx] != IO_BD_TypeSetting[idx])
				{

					TraceDBG(TSK_ID_CAN_MASTER, "IO board connection error: adr: %d, new:%d,set:%d\n", idx, IO_BoardType[idx], IO_BD_TypeSetting[idx]);
					status = E_CAN_WRONG_CFG_BOARD;
					break;
				}
			}
			else
			{
				//not connect
				status = E_CAN_MISS_BOARD;
				break;
			}
		}
	}
	if(status != OK)
	{
		timeCanIO_ST[IO_ERR_SET] = GetCurrentST();
		timesErrCnt[IO_ERR_SET]++;
	}
	Dia_UpdateDiagnosis(CANOPEN_ER_CONFIG, ((status&(E_CAN_WRONG_CFG_BOARD|E_CAN_MISS_BOARD)) != 0));
	return status;
}


void CanOpenOnLine()
{
	TraceMsg(TSK_ID_CAN_MASTER, "CO_NMT_STATE_OPERATIONAL\n");
	CheckIOErrorSetting();
	(void)coOdPutObj_u32 (0x1006, 0, syncTime_us);
	UpdateIOState(0xFF, CO_NMT_STATE_OPERATIONAL);

	coNmtLocalStateReq(CO_NMT_STATE_OPERATIONAL);
	masterState = CO_NMT_STATE_OPERATIONAL;
	canOpenState = STATE_CAN_MAX;
	//slaveState = CO_NMT_STATE_OPERATIONAL;
	Dia_UpdateDiagnosis(CANOPEN_ERROR, 0);
//	Dia_UpdateDiagnosis(CANOPEN_WARNING, 0);
}

void CanOpenOffLine()
{
	TraceMsg(TSK_ID_CAN_MASTER, "CO_NMT_STATE_PREOP\n");
	coNmtLocalStateReq(CO_NMT_STATE_PREOP);
	UpdateIOState(0xFF, CO_NMT_STATE_PREOP);
	masterState = CO_NMT_STATE_PREOP;
//	slaveState = CO_NMT_STATE_PREOP;
	canOpenState = STATE_MAP;
}
extern uint16_t heartBeat;
/*********************************************************************/
static uint32_t littleApplication(void)
{
	static uint16_t callTimes = 0;
	uint32_t  tickOut = 30;
	switch(canOpenState)
	{
	case STATE_INIT:
		canOpenState = STATE_SCAN;
		//todo
		break;
	case STATE_SCAN:
		//todo
		UpdateIOConn();
		canOpenState = STATE_HEART;
		break;
	case STATE_HEART:

		UpdateHeartbeat(heartBeat);
		canOpenState = STATE_SETCHECK;
		break;
	case STATE_SETCHECK:
		//if( OK == SdoWrite(95,0x1017,0,&duringTime, 2))
		//todo:
		canOpenState = STATE_PREOP;
		break;
	case STATE_PREOP:
		CanOpenOffLine();
		tickOut = 500;
		break;
	case STATE_MAP:
		canOpenState = STATE_OPERATION;
		UpdateMappingTable(IO_BD_TypeSetting, IO_BoardEnable,MAX_IO_BOARD_NUM);
		break;

	case STATE_OPERATION:

		CanOpenOnLine();
		if(callTimes == 0)
			WakeUpSchedule();
		callTimes++;
	//	osDelay(10000);
		break;
	case STATE_CAN_MAX:
		tickOut = 500;


		//TEST_SimulateTPDO();
		break;
	default:
		break;
	}

	return tickOut;
}






/*********************************************************************/
static void emcyConsumer(
		UNSIGNED8	node,
		UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister,
		UNSIGNED8	const *addErrorBytes
	)
{
	static uint32_t tick = 0;
	if(tick == 0)
	{
		TickDelayCanOpen[0] = 1000000;
		TickDelayCanOpen[1]= 0;

		tick = HAL_GetTick();
	}
	else
	{
		uint32_t tickDelay = GetTickDeviation(tick,HAL_GetTick());
		if(tickDelay < TickDelayCanOpen[0])
		{
			TickDelayCanOpen[0] = tickDelay;
		}
		else if(tickDelay > TickDelayCanOpen[1])
		{
			TickDelayCanOpen[1] = tickDelay;
			timeCanIO_ST[IO_TICK_CONSUMER] = GetCurrentST();
			timesErrCnt[IO_TICK_CONSUMER]++;
		}

	}
	TraceMsg(TSK_ID_CAN_MASTER,"emcyConsumer: node %d, EmcyErrorCode %x, errorRegister: %x, addBytes %x %x %x %x %x\n",
		node, emcyErrorCode, errorRegister, addErrorBytes[0],
		addErrorBytes[1], addErrorBytes[2], addErrorBytes[3], addErrorBytes[4]);
		
}
