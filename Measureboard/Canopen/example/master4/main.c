/*
* main.c - enthält programm main
*
* Copyright (c) 2014 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 2029 2012-12-17 15:30:58Z  $
*
*
*-------------------------------------------------------------------
* Changelog:
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
#include <stdlib.h>
#include <string.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>
//#include <Dave3.h>


/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern void coInitObjDictionary();

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static void emcyConsumer(UNSIGNED8	node, UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister, UNSIGNED8 const	*addErrorBytes);
static void sdoClientReadInd(UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8 subIndex,
		UNSIGNED32	size, UNSIGNED32	result);
static void sdoClientWriteInd(UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex, UNSIGNED32	result);
static void setupNode();
static RET_T setupLocalMapping(UNSIGNED8 mapCnt, UNSIGNED32	mapEntry[]);


/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED8	app_u8[8];
UNSIGNED16	app_u16[4];
UNSIGNED32	app_u32[4];

/* local defined variables
---------------------------------------------------------------------------*/
static UNSIGNED8	cfgNodeId = 0;


/***************************************************************************/
/**
* \brief main entry
*
* Master example:
* - send reset communication and wait for bootup message
* - setup HB monitoring for the first node
* - write HB time to remote node
* - read PDO transmit configuration from remote node
*	and setup local PDO receive
* - start remote and local node
*
* \param
*	nothing
* \results
*	nothing
*/
int main(
	void
  )
{
//   Dave_Init();

	/* HW initialization */
	codrvHardwareInit();

	/* init can driver */
	if (codrvCanInit(250) != RET_OK)  {
		exit(1);
	}

	/* init canopen */
    if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	/* setup timer */
	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit(2);
	}

	/* register event functions */
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit(3);
	}
	if (coEventRegister_EMCY_CONSUMER(emcyConsumer) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_SDO_CLIENT_READ(sdoClientReadInd) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_SDO_CLIENT_WRITE(sdoClientWriteInd) != RET_OK)  {
		exit(6);
	}

	if (codrvCanEnable() != RET_OK)  {
		exit(7);
	}

	// send reset comm for all devices
	coNmtStateReq(0, CO_NMT_STATE_RESET_COMM, CO_FALSE);

	while (1)  {
    	coCommTask();
	}
}


/*********************************************************************/
/*********************************************************************/
void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

	/* start node, if heartbeat of the node has been started */
	if (state == CO_ERRCTRL_BOOTUP)  {
		/* example is limited to only one node */
		if (cfgNodeId == 0)  {
			cfgNodeId = nodeId;
			setupNode();
		}
	}

    return;
}

/*********************************************************************/
static void emcyConsumer(
		UNSIGNED8	node,
		UNSIGNED16	emcyErrorCode,
		UNSIGNED8	errorRegister,
		UNSIGNED8	const *addErrorBytes
	)
{
	printf("emcyConsumer: node %d, EmcyErrorCode %x, errorRegister: %x, addBytes %x %x %x %x %x\n",
		node, emcyErrorCode, errorRegister, addErrorBytes[0],
		addErrorBytes[1], addErrorBytes[2], addErrorBytes[3], addErrorBytes[4]);
		
}


/*********************************************************************/
static void sdoClientReadInd(
		UNSIGNED8 sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		UNSIGNED32	size,
		UNSIGNED32	result
	)
{
	if (result != 0)  {
		printf("error sdo read transfer %lx, abort\n", result);
	}

	/* continue setup node */
	setupNode();
}


/*********************************************************************/
static void sdoClientWriteInd(
		UNSIGNED8 sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		UNSIGNED32	result
	)
{
	if (result != 0)  {
		printf("error sdo write transfer %lx, abort\n", result);
	}

	/* continue setup node */
	setupNode();
}


/*********************************************************************/
/*
* setup node 
*/

static void setupNode()
{
static UNSIGNED8	cfgState = 0;
static UNSIGNED32	mapEntry[8];
static UNSIGNED8	mapCnt;
static UNSIGNED8	recMapEntry;
UNSIGNED16	hbTime = 1000;

	switch (cfgState)  {
		case 0:		/* setup heartbeat node */
			/* setup heartbeat monitoring local */
			coOdPutObj_u32(0x1016, 1, (cfgNodeId << 16) | (hbTime + 200));

			/* write sdo to remote node */
			coOdSetCobid(0x1280, 1, 0x600 + cfgNodeId);
			coOdSetCobid(0x1280, 2, 0x580 + cfgNodeId);
			
			coSdoWrite(1, 0x1017, 0, (UNSIGNED8 *)&hbTime, 2, 1, 1000);

			/* wait for answer */
			cfgState = 1;
			break;

		case 1:		/* read TPDO count from remote node */
			coSdoRead(1, 0x1a00, 0, &mapCnt, 1, 1, 1000);
			recMapEntry = 0;

			/* disable local RPDO 1 */
			coOdSetCobid(0x1400, 1, 0x80000000ul);

			/* wait for answer */
			cfgState = 2;
			break;

		case 2:		/* read TPDO entry from remote node */
			/* at mapCnt the number of mappings are already saved */
			/* all mapentry received ? */
			if (recMapEntry < mapCnt)  {
				/* not yet */
				coSdoRead(1, 0x1a00, recMapEntry + 1,
						(UNSIGNED8 *)&mapEntry[recMapEntry], 4, 1, 1000);
				recMapEntry++;
			} else {
				setupLocalMapping(mapCnt, &mapEntry[0]);
				/* start remote and lokal node */
				coNmtStateReq(0, CO_NMT_STATE_OPERATIONAL, CO_TRUE);
				
				/* ready */
				cfgState = 3;
			}
			break;
	}
}


/*********************************************************************/
/*
* setup local mapping with mapping infos from remote node 
*
* Only provide 8, 16 and 32 bit variables
*
*/
RET_T setupLocalMapping(
		UNSIGNED8	mapCnt,
		UNSIGNED32	mapEntry[]
	)
{
UNSIGNED8	i;
UNSIGNED8	cnt_u8 = 0;
UNSIGNED16	cnt_u16 = 0;
UNSIGNED32	cnt_u32 = 0;
UNSIGNED32	map;

	for (i = 0; i < mapCnt; i++)  {
		/* use mapping len for type detection */
		switch (mapEntry[i] & 0xff)  {
			case 8:
				/* calculate subindex */
				map = 0x20000008 | ((cnt_u8 + 1) << 8);
				coOdPutObj_u32(0x1600, i + 1, map);
				cnt_u8++;
				break;
			case 16:
				/* calculate subindex */
				map = 0x20010010 | ((cnt_u16 + 1) << 8);
				coOdPutObj_u32(0x1600, i + 1, map);
				cnt_u16++;
				break;
			case 32:
				/* calculate subindex */
				map = 0x20020020 | ((cnt_u32 + 1) << 8);
				coOdPutObj_u32(0x1600, i + 1, map);
				cnt_u32++;
				break;
			default:
				return(RET_MAP_ERROR);
		}
	}

	/* write mapping count to local OD */
	coOdPutObj_u8(0x1600, 0, mapCnt);

	/* setup RPDO 1 cob-id */
	coOdSetCobid(0x1400, 1, 0x180 + cfgNodeId);


	return(RET_OK);
}
