/*
* main.c - contains programm main
*
* Copyright (c) 2012 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 5910 2014-05-19 07:46:35Z ro $
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
#include <co_nmt.h>
#include <co_commtask.h>
#include "gen_indication.h"
//#include "terminal.h" 	/* for coloring LED output */
#include <cpu_stm32.h>
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "can.h"







/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static RET_T sdoServerReadInd(BOOL_T execute, UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex);
static RET_T sdoServerCheckWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex, const UNSIGNED8 *pData);
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);
static void pdoRecEvent(UNSIGNED16);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void ledGreenInd(BOOL_T);
static void ledRedInd(BOOL_T);
static void syncInd(UNSIGNED8	commEvent);

static void exit_main(int error);
OS_RSEMA canopenOdLockId;
/* external variables
---------------------------------------------------------------------------*/

uint32_t masterAdr = 0x01;
uint32_t heartBeatTime = 0x0;

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;
static uint16_t ioStart = 0;
BoradType boardType = TYPE_NONE;
/* local defined variables
---------------------------------------------------------------------------*/
static osMutexDef(canopenOdLockId);

void BoardMappingUpdate()
{
	coOdSetCobid(0x1800u, 1, 0x80000000);
	coOdSetCobid(0x1400u, 1, 0x80000000);
	(void)coOdPutObj_u8 (0x1600u + CO_401_PROFILE_OFFSET,
											0, 0);
	(void)coOdPutObj_u8 (0x1a00u + CO_401_PROFILE_OFFSET,
											0, 0);


	if(boardType == TYPE_DI_BOARD)
	{
		//set mapping functions:
		//rpdo = 0;


		uint32_t newValue = 0;
		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								1, newValue);
		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								2, newValue);
		//tpdo
		newValue = 0x60000108;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								1, newValue);
		newValue = 0x60000208;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								2, newValue);
		(void)coOdPutObj_u8 (0x1a00u + CO_401_PROFILE_OFFSET,
													0, 2);
		coOdSetCobid(0x1800u, 1, 0x180 + devAdr);

	}
	else if(boardType == TYPE_DO_BOARD)
	{
		uint32_t newValue = 0x62000108;

		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								1, newValue);
		newValue = 0x62000208;
		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								2, newValue);
		//tpdo

		newValue = 0;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								1, newValue);
		newValue = 0;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								2, newValue);

		(void)coOdPutObj_u8 (0x1600u + CO_401_PROFILE_OFFSET,
															0, 2);
		coOdSetCobid(0x1400u, 1, 0x200 + devAdr);
	}
	else if(boardType == TYPE_AO_BOARD)
	{
		uint32_t newValue = 0x64110110;
		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								1, newValue);
		newValue = 0x64110210;
		(void)coOdPutObj_u32 (0x1600u + CO_401_PROFILE_OFFSET,
								2, newValue);
		//tpdo
		newValue = 0;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								1, newValue);
		newValue = 0;
		(void)coOdPutObj_u32 (0x1a00u + CO_401_PROFILE_OFFSET,
								2, newValue);

		(void)coOdPutObj_u8 (0x1600u + CO_401_PROFILE_OFFSET,
															0, 2);
		coOdSetCobid(0x1400u, 1, 0x200 + devAdr);
	}
	else
	{}



	UpdateOutput();
}


/*
 * retval = coOdGetObj_u8(0x6000u + CO_401_PROFILE_OFFSET,
					port, &currentValue);

					RET_T coOdPutObj_u32(UNSIGNED16 index,
					UNSIGNED8 subIndex, UNSIGNED32 newVal);
 */

/***************************************************************************/
/**
* \brief main entry
*
* \param
*	nothing
* \results
*	nothing
*/
int canopen_main(
	void
  )
{



UNSIGNED8	emcyData[5] = { 1, 2, 3, 4, 5 };

	/* HW initialization */
	codrvHardwareInit();
//	Test();
	canopenOdLockId = OS_CreateSemaphore();
	if (codrvCanInit(125) != RET_OK)  {
		exit_main(1);
	}
//	Test();
	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit_main(2);
	}
	StartTimer();
    if (coCanOpenStackInit(NULL) != RET_OK)  {
    	shell_Add("error init library\n");
		exit_main(1);
	}
    UpdateVersion();
	/* register event functions */

    //is called in usr401

//	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
//		exit_main(3);
//	}
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit_main(4);
	}
	if (coEventRegister_SDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		exit_main(5);
	}

	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		exit_main(7);
	}
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		exit_main(8);
	}
	if (coEventRegister_PDO_REC_EVENT(pdoRecEvent) != RET_OK)  {
		exit_main(9);
	}
	if (coEventRegister_LED_GREEN(ledGreenInd) != RET_OK)  {
		exit_main(10);
	}
	if (coEventRegister_LED_RED(ledRedInd) != RET_OK)  {
		exit_main(11);
	}
	//if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
	//	exit_main(12);
	//}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		exit_main(13);
	}
//	if (coEventRegister_SYNC(syncInd) != RET_OK)  {
//			exit_main(16);
//		}
//	__enable_irq();

	if (codrvCanEnable() != RET_OK)  {
		exit_main(14);
	}

	/* write emcy */
	if (coEmcyWriteReq(0x1234, &emcyData[0]) != RET_OK)  {
		exit_main(15);
	}

//	Test();
	/* --- 401 specific ------------------------------------------------*/
		/* profile 401 HW acess registration */
   BoardMappingUpdate();

    if (co401Init() != RET_OK)  {
    	shell_Add("error init 401 profile handler\n");
			exit_main(14);
		}



		if (coEventRegister_401(
					byte_in_piInd,		/* digital in  HW API function   */
					byte_out_piInd,		/* digital out HW API function   */
					analog_in_piInd,	/* the analog in HW API function */
					analog_out_piInd	/* analog out HW API function    */
					) != RET_OK)  {
			shell_Add("registration of 401 HW acess functions failed\n");
			exit_main(14);
		}
		/* -----------------------------------------------------------------*/


		/* all is ready now,
		 * enable CAN driver to receive and transmit CAN frames
		 */
		if (codrvCanEnable() != RET_OK)  {
			exit_main(14);
		}
		ioStart = 1;
		if(boardType == TYPE_AO_BOARD)
			StartPWM();
	while (1)
	{

		(void)osSignalWait(SIGNAL_CANOPEN_ALL, osWaitForever);//every signal
    	coCommTask();

	}
}

void StartIOTask()
{
	while (1)
	{
		if(ioStart)
			co401Task();
		osDelay(20);
		FCT_Test();
	}
}


/*********************************************************************/
static RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	shell_Add("nmtInd: New Nmt state %d - execute %d\n", newState, execute);

	return(RET_OK);
}


/*********************************************************************/
static void pdoInd(
		UNSIGNED16	pdoNr
	)
{
	shell_Add("pdoInd: pdo %d received\n", pdoNr);
}


/*********************************************************************/
static void pdoRecEvent(
		UNSIGNED16	pdoNr
	)
{
	shell_Add("pdoRecEvent: pdo %d time out\n", pdoNr);
}


/*********************************************************************/
static void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{

	static CO_NMT_STATE_T lastState = CO_NMT_STATE_UNKNOWN;
	if(state == CO_ERRCTRL_HB_FAILED)
	{
		//output change to default value;
		if(boardType == TYPE_DO_BOARD)
		{
			OutputDO(1,0);
			OutputDO(2,0);
		}
		else if(boardType == TYPE_AO_BOARD)
		{
			OutputAO(1,0);
			OutputAO(2,0);
		}
		else
		{}
		//todo: whether change the state to other state

	}
	shell_Add("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

    return;
}


/*********************************************************************/
static RET_T sdoServerReadInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	shell_Add("sdo server read ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}


/*********************************************************************/
static RET_T sdoServerCheckWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		const UNSIGNED8	*pData
	)
{
	shell_Add("sdo server check write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);
	coHbConsumerSet(0x01,(uint32_t) (2000));
   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}

uint16_t CalcGainOffset(const uint16_t setValue, const uint16_t measValue, uint8_t idx);

uint32_t fctTest = 0;
enum
{
	UART_TEST = 0x40,
	CAN_TEST = 0x80,
};

void FCT_Test(void)
{
	switch(fctTest)
	{
	case UART_TEST:
		shell_Add(deviceName,sizeof(deviceName));
		//
		break;
	case CAN_TEST:
		SendData_Can(deviceName,sizeof(deviceName));
		break;
	}
	fctTest = 0x0;
}

uint32_t enableConsumer = 1;
uint32_t errCnt_Clr = 0;
uint32_t errCnt_Can = 0;
uint32_t errCnt_Nmt = 0;


uint16_t dicObjType;
uint16_t dicAtrId;
uint16_t dicObjId;
uint32_t objValueU32;
float objValueR32;


extern volatile uint16_t eepChanged;
/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	//shell_Add("sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
	//	execute, sdoNr, index, subIndex);
	if(execute)
	{
		if(index == 0x2004)
		{
			if(subIndex >= 1)//adrId
			{
				coHbConsumerSet((uint8_t)masterAdr,heartBeatTime);
			}
		}
		if(index == 0x2005)
		{
			if(subIndex == 1)//errCnt_Clr
			{
				errCnt_Can = 0;
				errCnt_Nmt = 0;
				eepChanged = 1;
			}
		}
		if(index == 0x2005)
		{
			if(subIndex <2)//config
			{
				dicObjType = 0;
				errCnt_Nmt = 0;
				eepChanged = 1;
				GetObj(dicObjId,dicAtrId, 1,NULL);
			}
			else if(subIndex == 4)
			{
				PutObj(dicObjId,dicAtrId,(double)(objValueU32),NULL);
			}
			else if(subIndex == 5)
			{
				PutObj(dicObjId,dicAtrId,objValueR32,NULL  );
			}
		}
		if(boardType == TYPE_DI_BOARD)
		{

		}
		//simulation
		else if(boardType == TYPE_DO_BOARD)
		{
			//simulate
			if(index == 0x2000)
			{
				//shell_Add("DO Simulate\n");
			}
		}
		else if(boardType == TYPE_AO_BOARD)
		{
			if(index == 0x2001)
			{
				if(subIndex == 0x03)
				{
					//4000; -> 4mA
					if(simuAO1 == 4000)
					{
						CalcGainOffset(4000,readBackAO1, 0 );
						//shell_Add("cali 4000\n");
						//cali 4mA
						//400;
					}
					else if(simuAO1 == 20000)
					{
						//cali 20mA
						CalcGainOffset(20000,readBackAO1, 0 );
						//shell_Add("cali 20000\n");
					}
				}
				else if(subIndex == 0x04)
				{
					//4000; -> 4mA
					if(simuAO2 == 4000)
					{
						//cali 4mA
						CalcGainOffset(4000,readBackAO2, 1 );
						//shell_Add("cali 4000\n");
					}
					else if(simuAO2 == 20000)
					{
						//cali 20mA
						CalcGainOffset(20000,readBackAO2, 1 );
						//shell_Add("cali 20000\n");
					}
				}
				else if(subIndex == 0x05)
				{
					ResetCalibration(0);
				}
				else if(subIndex == 0x06)
				{
					ResetCalibration(1);
				}
			}
			//simuAO1 = 0;
		}
		else if(0x2003 == index)//fct
		{
			if(subIndex == 2)
			{

			}
		}

	}
	UpdateOutput();
   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}

void sdoWrite(uint16_t obj,uint16_t atr)
{
	sdoServerWriteInd(1,0,obj,atr);
}
/*********************************************************************/
static void canInd(
	CO_CAN_STATE_T	canState
	)
{
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			shell_Add("CAN: Bus Off\n");
			break;
		case CO_CAN_STATE_BUS_ON:
			shell_Add("CAN: Bus On\n");
			break;
		case CO_CAN_STATE_PASSIVE:
			shell_Add("CAN: Passive\n");
			break;
		case CO_CAN_STATE_UNCHANGED:
			break;
		default:
			break;
	}
}


/*********************************************************************/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent
	)
{
	switch (commEvent)  {
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			shell_Add("COMM-Event CAN Overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			shell_Add("COMM-Event Rec Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			shell_Add("COMM-Event Rec Queue Overflow\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_EMPTY:
			shell_Add("COMM-Event Rec Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_FULL:
			shell_Add("COMM-Event Tr Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_OVERFLOW:
			shell_Add("COMM-Event Tr Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_EMPTY:
			shell_Add("COMM-Event Tr Queue Empty\n");
			break;
		default:
			shell_Add("COMM-Event - unknown event %d\n", (int)commEvent);
			break;
	}

}



/*********************************************************************/
static void syncInd(
		UNSIGNED8	commEvent
	)
{

			shell_Add("Sync event %d\n", (int)commEvent);

}

/*********************************************************************/
static void ledGreenInd(
		BOOL_T	on
	)
{
	if(on)
		LED_GREEN_ON;
	else
		LED_GREEN_OFF;

	//shell_Add("GREEN: %d\n", on);
}


/*********************************************************************/
static void ledRedInd(
		BOOL_T	on
	)
{
	if(on)
		LED_RED_ON;
	else
		LED_RED_OFF;

	//shell_Add("RED: %d\n", on);
}

/*********************************************************************/
static void exit_main(
	int error
	)
{
	shell_Add("exit_main %d\n", error);
	while(1) {}
}
