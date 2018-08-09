/*
* codrv_bxcan - driver for bxcan
*
* Copyright (c) 2012-2016 emtas GmbH
*-------------------------------------------------------------------
* SVN $Id: codrv_bxcan.c 18373 2017-05-30 11:41:10Z hil $
*
*
*-------------------------------------------------------------------
*
*
*/

/*
Functionality Singleline:
- TX/RX - RTR supported (Filter only for 11bit)
- TX/RX - extended ID Support (without Filter)
- Error state change check by polling
- 11bit ID filter support (incl. group filter)
- CAN2 supported
Functionality Multiline:
- Basiccan functionality without filter like Singleline
*/





/********************************************************************/
/**
* \file
* \brief   bxCAN CAN driver (ST, e.g. STM32)
*
*
* FullCAN mode:
*	CO_DRV_FILTER has to be set
*	COB Setting: canChan == 0xFFFF - not assigned to a filter	
*				channel 0..3 are reserved
*				filter start with 4 == second filter bank
*   CO_DRV_GROUP_FILTER - receive all HB Consumer and EMCY Consumer
*               messages independend of its configuration.
*				(first filter bank is for group filter (HB and EMCY Cons)
*
* Note: This settings should be part of gen_define.h!
* 
*/

/* #define CO_DRV_FILTER 1 */
/* #define CO_DRV_GROUP_FILTER 1 */


/**
* \define POLLING
* work without interrupts
* !! Seldom tested - support is not guaranteed !!
*/
/* #define POLLING 1 */


/*lint -emacro(835,BXCAN_*) use shift/add/divide with zero for better overview */
/*lint -emacro(778,BXCAN_*) calc also for result 0 for better logical overview */
/*lint -emacro((845),BXCAN_*) calc also for result 0 for better logical overview */
/*lint -estring(960,17.4) pointer arithmetic by using calculated register addresses */

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stddef.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_datatype.h>
#include <co_drv.h>
#include <codrv_error.h>
#include <co_commtask.h>

#include "codrv_bxcan.h"

/* constant definitions
---------------------------------------------------------------------------*/
/**
* \define CODRV_DEBUG
* UART Debug output
*/
/* #define CODRV_DEBUG 1 */

#if defined(CONFIG_BXCAN1) || defined(CONFIG_BXCAN2)
#else
	/* default CAN1 */
# define CONFIG_BXCAN1 1
/* #define CONFIG_BXCAN2 1 */
#endif

/* OS related macros - default definition */
#ifdef CO_OS_SIGNAL_CAN_TRANSMIT
#else
#  define CO_OS_SIGNAL_CAN_TRANSMIT()
#endif

#ifdef CO_OS_SIGNAL_CAN_RECEIVE
#else
#  define CO_OS_SIGNAL_CAN_RECEIVE()
#endif

#ifdef CO_OS_SIGNAL_CAN_STATE
#else
#  define CO_OS_SIGNAL_CAN_STATE()
#endif

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/
#ifdef CODRV_DEBUG
# ifdef CO_DRV_FILTER
void printFilter(void);
# endif
#endif

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T codrvCanInitController(UNSIGNED16 bitRate);

static RET_T codrvCanTransmit(CO_CONST CO_CAN_MSG_T * pBuf);
static CO_CONST CODRV_BTR_T * codrvCanGetBtrSettings(UNSIGNED16 bitRate);


#ifdef CO_DRV_FILTER
#  ifdef CO_DRV_GROUP_FILTER
static BOOL_T codrvCanCheckGroupFilter(CO_CONST CO_CAN_COB_T * CO_CONST pCob);
#   endif
static UNSIGNED16 codrvCanGetChan(CO_CAN_COB_T * CO_CONST pCob);
static void codrvCanSetIdFilter(UNSIGNED8 filterNr, UNSIGNED16 accId, BOOL_T rtr);
#endif

#ifdef CODRV_AUTOBAUD
RET_T codrvAutoBaudNextBitrate(void);
UNSIGNED16 codrvAutoBaudGetActualBitrate(void);
#endif

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static BOOL_T canEnabled = { CO_FALSE };
static volatile BOOL_T transmissionIsActive = { CO_FALSE };

static UNSIGNED32 oldEsrValue = { 0u };

/** currently TX message buffer */
static CO_CONST CO_CAN_MSG_T *pTxBuf = { NULL };


/** CAN Controller address */
#define CODRV_BXCAN1_ADDR	((void*)0x40006400ul)
#define CODRV_BXCAN2_ADDR	((void*)0x40006800ul)

/* filter is everytime part of *CAN1* and never part of *CAN2* ! */
static volatile UNSIGNED32 * const pCanFilterAddr = CODRV_BXCAN1_ADDR; /*lint !e923 CAN controller address */


/** \var pCan - used CAN controller address */
#  ifdef CONFIG_BXCAN1 
static volatile UNSIGNED32 * const pCan = CODRV_BXCAN1_ADDR; /*lint !e923 CAN controller address */
const UNSIGNED16 filterOffset = 0u;	/*lint -esym(835,filterOffset) -esym(845,filterOffset) */
const UNSIGNED32 filterMask = 0x00003FFFul;
#  endif
#  ifdef CONFIG_BXCAN2
static volatile UNSIGNED32 * const pCan = CODRV_BXCAN2_ADDR; /*lint !e923 CAN controller address */
const UNSIGNED16 filterOffset = 14u;
const UNSIGNED32 filterMask = 0x0FFFC000ul;
#  endif


#ifdef CODRV_BIT_TABLE_EXTERN 

extern CO_CONST CODRV_BTR_T codrvCanBittimingTable[];

#else /* CODRV_BIT_TABLE_EXTERN */

/** can bittiming table */
static CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 36MHz table, prescaler 10bit (max 1024) */
		{   10u, 225u, 0u, 13u, 2u },
		{   20u, 120u, 0u, 12u, 2u }, /* 86.7% */
		{   50u,  45u, 0u, 13u, 2u },
		{  100u,  18u, 0u, 16u, 3u }, /* 85% */
		{  125u,  18u, 0u, 13u, 2u },
		{  250u,   9u, 0u, 13u, 2u },
		{  500u,   9u, 0u,  6u, 1u }, /* only 8tq, 87.5% */
		{  800u,   3u, 0u, 12u, 2u }, /* 15tq, 86.7% */
		{ 1000u,   4u, 0u,  7u, 1u }, /*  9tq, 88.9% */
		{0u, 0u, 0u, 0u, 0u} /* last */
	};

#endif /* CODRV_BIT_TABLE_EXTERN */

#ifdef CODRV_AUTOBAUD
static UNSIGNED16	autoBaudBitRateIdx = { 0u };
static UNSIGNED8	autoBaudActive = { 0u };
#endif /* CODRV_AUTOBAUD */


/*---------------------------------------------------------------------------*/
/* #define DEBUG_SEND_TESTMESSAGE */
#ifdef DEBUG_SEND_TESTMESSAGE
static void codrvSendTestMessage(
		void	/* no parameter */
	)
{

	/* use fifo 0 */
	while ((pCan[BXCAN_CANTSR] & BXCAN_CANTSR_TME0) == 0) {}

    pCan[BXCAN_CANTDTXR(0)] = 8; /* dlc */
	pCan[BXCAN_CANTDLXR(0)] = 0x04030201ul;
	pCan[BXCAN_CANTDHXR(0)] = 0x08070605ul;
	pCan[BXCAN_CANTIXR(0)] = ((0x555ul
							) << BXCAN_CANTIXR_STID_SHIFT)
					| BXCAN_CANTIXR_TXRQ;

}
#endif /* DEBUG_SEND_TESTMESSAGE */
/*---------------------------------------------------------------------------*/

#ifdef CO_DRV_FILTER
/**
* nextAccEntry counts the used acceptace filter IDs.
* There are all 16bit ID filters.
* For every filter bank there are 4 base ID filters.
*/
static UNSIGNED16 nextAccEntry;
#endif


/***************************************************************************/
/**
* \brief codrvCanInit - init CAN controller
*
* This Function inits the CAN controller and setup the bitrate
* After that, the CAN controller is disabled.
*
* \param
*	bitrate	- CANopen bitrate
* \results
*	RET_T
*/
RET_T codrvCanInit(
		UNSIGNED16	bitRate		/**< Bitrate */
	)
{
RET_T	retVal;

#ifdef CO_DRV_FILTER
	/* start of ID filter 
     * currently fix to the second filter bank */
	nextAccEntry = 4u + (filterOffset * 4u);  /*lint !e845 filterOffset depend of the CAN controller */
#endif

	retVal = codrvCanInitController(bitRate);

	return (retVal);
}

/***************************************************************************/
/**
* \brief codrvCanReInit - reinit CAN controller
*
* This Function reinits the CAN controller after deactivation.
* 
* In Filter mode: After this function call all Filter are reset and must
* be reconfigured!
*
* At the end of the function, the CAN controller should be in state disabled.
*
* \param
*	bitrate	- CANopen bitrate
* \results
*	RET_T
*/

RET_T codrvCanReInit(
		UNSIGNED16	bitRate		/**< Bitrate */
	)
{
RET_T	retVal;

	retVal = codrvCanInitController(bitRate);

	return (retVal);
}

/***************************************************************************/
/**
* \brief codrvCanInitController - init CAN controller
*
* \internal
*
* This Function inits the CAN controller and setup the bitrate
* After that, the CAN controller is disabled.
*
* \param
*	bitrate	- CANopen bitrate
* \results
*	RET_T
*/

static RET_T codrvCanInitController(
		UNSIGNED16 bitRate
	)
{
RET_T	retVal = RET_OK;  
UNSIGNED32 mcr;
UNSIGNED16 i;
volatile UNSIGNED32 *pCanFilter = pCanFilterAddr;

	/* reset CAN */
	mcr = pCan[BXCAN_CANMCR] & BXCAN_CANMCR_DBF; /* read debug setting */  /*lint !e960 !e835 BXCAN_CANMCR is a constant value 0 */
	mcr = 0ul; /* don't stop the CAN controller */

	/* master reset, req. for 2 CANs only */
	pCan[BXCAN_CANMCR] = mcr | BXCAN_CANMCR_RESET; 	/*lint !e960 !e835 BXCAN_CANMCR is a constant value 0 */
	while((pCan[BXCAN_CANMCR] & BXCAN_CANMCR_RESET) != 0ul) {}	/*lint !e960 !e835 BXCAN_CANMCR is a constant value 0 */

#ifdef not_used
	/* in case, the sleep mode is set and cannot reset,
	* check that the CAN clock is enabled. After setting the
	* CAN clock, the sleepmode is reset by INRQ (with SLEEP reset)
	*/
	pCan[BXCAN_CANMCR] = mcr; /* reset sleep mode */
	while((pCan[BXCAN_CANMSR] & BXCAN_CANMSR_SLAK) != 0ul) {
		pCan[BXCAN_CANMCR] = 0;
	}
#endif

	pCan[BXCAN_CANMCR] = mcr | BXCAN_CANMCR_INRQ; /*lint !e960 !e835 BXCAN_CANMCR is a constant value 0 */
	while((pCan[BXCAN_CANMSR] & BXCAN_CANMSR_INAK) == 0ul) {} /*lint !e960 BXCAN_CANMSR is a constant value */

	/* configure can */
	pCan[BXCAN_CANMCR] = (mcr | BXCAN_CANMCR_INRQ  	  /*lint !e960 BXCAN_CANMCR is a constant value */
				| BXCAN_CANMCR_ABOM /* auto buson */
				| BXCAN_CANMCR_RFLM /* receive fifo locked mode */
				| BXCAN_CANMCR_TXFP /* priority by message */
				);

	pCanFilter[BXCAN_CANFMR] |= BXCAN_CANFMR_FINIT; /* filter in init mode */	/*lint !e960 BXCAN_CANFMR is a constant value */
	pCanFilter[BXCAN_CANFA1R] &= ~filterMask; /* filter not active */  /*lint !e960 BXCAN_CANFA1R is a constant value */

	/* init memory to seldom 16bit ID */
	for( i = 0u + filterOffset; i <= BXCAN_MAX_FILTERBANK + filterOffset; i++) {  /*lint !e835 !e845 for better logical show */
		pCanFilter[BXCAN_CANFIRX(i,1u)] = (0x7FFul << 21) | (0x3ul << 19)
								|  (0x7FFul << 5) | (0x3ul << 3);		   /*lint !e845 use zero for better logical overview */
		pCanFilter[BXCAN_CANFIRX(i,2u)] = (0x7FFul << 21) | (0x3ul << 19)
								|  (0x7FFul << 5) | (0x3ul << 3);		   /*lint !e845 use zero for better logical overview */
 
	}

	/* configure one filter to receive all messages in fifo 0 */
	pCanFilter[BXCAN_CANFM1R] &= ~filterMask; /* all filter in mask mode (==0) */  
	pCanFilter[BXCAN_CANFS1R] &= ~filterMask; /* all filter in 16bit mode (==0) */ 
	pCanFilter[BXCAN_CANFFA1R] &= ~filterMask; /* all filter assigned to Fifo 0 (==0) */ 

#ifdef CO_DRV_FILTER
	/* all filter in list mode, but first bank in masked (0 or 14) */
	pCanFilter[BXCAN_CANFM1R] |= (filterMask & ~(1ul << filterOffset)); /*lint !e845 zero only for CAN1 */

#  ifdef CO_DRV_GROUP_FILTER 
#    ifdef CO_HB_CONSUMER_CNT
	/* configure HB Consumer */
		/* all IDs from 0x700..0x77F, only data and 11bit / filter (0,1)*/ 
	pCanFilter[BXCAN_CANFIRX(filterOffset , 1)] = 
					FIRX_MASK(FIRX_ID(0x780ul) | FIRX_RTR | FIRX_IDE)
				|	FIRX_ID(0x700ul);

	pCanFilter[BXCAN_CANFA1R] |= (1ul << filterOffset); /* activate first filter bank (0 or 14) */
#    endif /* CO_HB_CONSUMER_CNT */

#    ifdef CO_EMCY_CONSUMER_CNT
	/* configure EMCY Consumer */
		/* all IDs from 0x080..0x0FF, only data and 11bit / filter (0,2) */ 
	pCanFilter[BXCAN_CANFIRX(filterOffset , 2)] = 
					FIRX_MASK(FIRX_ID(0x780ul) | FIRX_RTR | FIRX_IDE)
				|	FIRX_ID(0x080ul);

	pCanFilter[BXCAN_CANFA1R] |= (1ul << filterOffset); /* activate filter bank 0 */
#    endif /* CO_EMCY_CONSUMER_CNT */
#  endif /* CO_DRV_GROUP_FILTER */

#else

	/* all IDs, but only data and 11bit */ 
	/* pCanFilter[BXCAN_CANFIRX(filterOffset,1)] = FIRX_MASK(FIRX_RTR | FIRX_IDE); */
		/* all messages */ 
	pCanFilter[BXCAN_CANFIRX(filterOffset,1u)] = 0u;
	pCanFilter[BXCAN_CANFA1R] |= (1ul << filterOffset); /* activate filter 0 */

#endif

	pCanFilter[BXCAN_CANFMR] &= ~BXCAN_CANFMR_FINIT; /* filter active */

	/* init req. variables */
	canEnabled = CO_FALSE;
	transmissionIsActive = CO_FALSE;
	pTxBuf = NULL;

	/* error states */
	codrvCanErrorInit();

	oldEsrValue = 0ul;

#ifdef CODRV_AUTOBAUD
	/* autobaud ? */
	if (bitRate == 0)  {
		autoBaudActive = 1u;
		autoBaudBitRateIdx = 0u;
		retVal = codrvAutoBaudNextBitrate();
	} else {
		autoBaudActive = 0u;
		/* set bitrate */
		retVal = codrvCanSetBitRate(bitRate);  /*lint !e838 initialization for long function */
	}
#else
	/* set bitrate */
	retVal = codrvCanSetBitRate(bitRate);  /*lint !e838 initialization for long function */
#endif


#ifdef DEBUG_SEND_TESTMESSAGE
	codrvCanEnable();
	codrvSendTestMessage();
#endif /* DEBUG_SEND_TESTMESSAGE */

#ifdef CO_DRV_FILTER
#  ifdef CODRV_DEBUG
	printFilter();
#  endif
#endif


	return(retVal);
}


#ifdef CODRV_AUTOBAUD
/***********************************************************************/
/**
*
* codrvAutoBaudNextBitrate - set next bitrate for autobaud
*
* \internal
*
* \returns
*	RET_T
*/
RET_T codrvAutoBaudNextBitrate(
		void	/* no parameter */
	)
{
RET_T retVal;

	autoBaudBitRateIdx++;
	if (codrvCanBittimingTable[autoBaudBitRateIdx].bitRate == 0u)  {
		autoBaudBitRateIdx = 0u;
	}

	retVal = codrvCanSetBitRate(codrvCanBittimingTable[autoBaudBitRateIdx].bitRate);  /*lint !e838 initialization for long function */

	return(retVal);
}


/***********************************************************************/
/**
* codrvAutoBaudGetActualBitrate - get actual used bitrate
*
* This function returns the actual used bitrate.
* They haven't be valid.
*
*
* \returns
*	bitrate
*/
UNSIGNED16 codrvAutoBaudGetActualBitrate(
		void	/* no parameter */
	)
{
	return(codrvCanBittimingTable[autoBaudBitRateIdx].bitRate);
}
#endif


/***********************************************************************/
/**
* codrvCanGetBtrSettings - get pointer to the btr value structure
*
* \internal
*
* \returns
*	pointer to an btr table entry
*/

static CO_CONST CODRV_BTR_T * codrvCanGetBtrSettings(
		UNSIGNED16 bitRate	/**< required bitrate */
	)
{
CO_CONST CODRV_BTR_T * pBtrEntry = NULL;
UNSIGNED8 i;

	i = 0u;
	while (codrvCanBittimingTable[i].bitRate != 0u) {
		if (codrvCanBittimingTable[i].bitRate == bitRate) {
			pBtrEntry = &codrvCanBittimingTable[i];
			break;
		}
		i++;
	}

	return pBtrEntry;

}

/***********************************************************************/
/**
* codrvCanSetBitRate - sets the CAN bit rate
*
* \internal
*
* Changing the Bitrate only if the CAN controller is in Reset
*
* \param
*	bitRate in kbit/s
* \results
*	RET_T
*
*/
RET_T codrvCanSetBitRate(
		UNSIGNED16		bitRate		/**< bit rate in kbit/s */
	)
{
CO_CONST CODRV_BTR_T * pBtrEntry;

UNSIGNED32 pre;
UNSIGNED32 seg1;
UNSIGNED32 seg2;

	/* stop CAN controller */
	(void)codrvCanDisable();

	pBtrEntry = codrvCanGetBtrSettings(bitRate);

	if (pBtrEntry == NULL) {
		/* if bitrate not supported */
		return(RET_DRV_WRONG_BITRATE); 
	}

	pre = (UNSIGNED32)pBtrEntry->pre; 
	seg1 = (UNSIGNED32)pBtrEntry->seg1 + (UNSIGNED32)pBtrEntry->prop; 
	seg2 = (UNSIGNED32)pBtrEntry->seg2; 

#ifdef CODRV_AUTOBAUD
	if (autoBaudActive != 0u)  {
		pCan[BXCAN_CANBTR] = (
						 	((1ul - 1u) << BXCAN_CANBTR_SJW_SHIFT)	 /*lint !e845 !e778 calc zero for better logical overview */
						 |	((seg2 - 1u) << BXCAN_CANBTR_TS2_SHIFT)
						 |  ((seg1 - 1u) << BXCAN_CANBTR_TS1_SHIFT)
						 |  ((pre - 1u) << BXCAN_CANBTR_BRP_SHIFT)
						 |  BXCAN_CANBTR_SILM /* must changed, silent only during the scan */);
	} else
#endif
	{
		pCan[BXCAN_CANBTR] = (
						 	((1ul - 1u) << BXCAN_CANBTR_SJW_SHIFT)	 /*lint !e845 !e778 calc zero for better logical overview */
						 |	((seg2 - 1u) << BXCAN_CANBTR_TS2_SHIFT)
						 |  ((seg1 - 1u) << BXCAN_CANBTR_TS1_SHIFT)
						 |  ((pre - 1u) << BXCAN_CANBTR_BRP_SHIFT));
	}

    return(RET_OK);
}


/***********************************************************************/
/**
* codrvCanEnable - enable CAN controller
*
* \internal
*
* \param
*	none
* \results
*	RET_T
*/
RET_T codrvCanEnable(
		void	/* no parameter */
	)
{
RET_T	retVal = RET_OK;
UNSIGNED32 tmp;

	pCan[BXCAN_CANIER] |= BXCAN_CANIER_TMEIE; /* TX IRQ */
	pCan[BXCAN_CANIER] |= BXCAN_CANIER_FMPIE0; /* RX Fifo 0 IRQ */

#ifdef CODRV_AUTOBAUD
	/* silent mode active ? */
	if ((pCan[BXCAN_CANBTR] & BXCAN_CANBTR_SILM) == 0u)
#endif
	{
		pCan[BXCAN_CANIER] |= BXCAN_CANIER_ERRIE | BXCAN_CANIER_EPVIE | BXCAN_CANIER_BOFIE; /* Passive/Busoff */
	}

	/* enable CAN controller */
	tmp = pCan[BXCAN_CANMCR];
	tmp &= ~BXCAN_CANMCR_INRQ;
	pCan[BXCAN_CANMCR] = tmp; 

	/* Error active is later checked */

#ifdef CO_DRV_FILTER
#  ifdef CODRV_DEBUG
	printFilter();
#  endif
#endif

	/* enable interrupts */
	codrvCanEnableInterrupt();

	return(retVal);
}


/***********************************************************************/
/**
* codrvCanDisable - disable CAN controller
*
* This function disables the CAN controller.  The function waits
* for the CAN controller being disabled. Code calling this function
* typically expects that after returning the CAN controller is in Init mode.
*
* But note, the time the CAN controller needs to enter the Init mode
* can be as long as the duration of one CAN frame.
*
* \internal
*
* \param
*	none
* \results
*	RET_OK CAN controller is set to be disabled
*/
RET_T codrvCanDisable(
		void	/* no parameter */
	)
{
RET_T	retVal = RET_OK;

	/* disable CAN controller */
	pCan[BXCAN_CANMCR] |= BXCAN_CANMCR_INRQ;
	while((pCan[BXCAN_CANMSR] & BXCAN_CANMSR_INAK) == 0ul) {} /* 500us on 250kbit/s */

	canEnabled = CO_FALSE;

	pCan[BXCAN_CANIER] &= ~BXCAN_CANIER_TMEIE; /* TX IRQ */
	pCan[BXCAN_CANIER] &= ~BXCAN_CANIER_FMPIE0; /* RX Fifo 0 IRQ */
	pCan[BXCAN_CANIER] &= ~(BXCAN_CANIER_ERRIE | BXCAN_CANIER_EPVIE | BXCAN_CANIER_BOFIE); /* Passive/Busoff */

	return(retVal);
}

#ifdef CO_DRV_FILTER

#  ifdef CO_DRV_GROUP_FILTER
/***********************************************************************/
/**
* \brief codrvCheckGroupFilter - check, if the canId part of the group filter
*
* Depend of some settings the group filter are for the IDs
*	0x700..0x77F - Heartbeat
*	0x80..0xFF   - Emergency (and default Sync)
*
* \return BOOL_T
* \retval CO_TRUE
*	The ID is part of the group filter.
* \retval CO_FALSE
*	The ID is not part of the group filter.
*	
*	
*
*/
static BOOL_T codrvCanCheckGroupFilter(
		CO_CONST CO_CAN_COB_T * CO_CONST pCob
	)
{
BOOL_T retval;
UNSIGNED32 canId = pCob->canId;

	retval = CO_FALSE;


	if (pCob->rtr == CO_FALSE)
	{
#    ifdef CO_HB_CONSUMER_CNT
		if ((canId & 0x0780u) == 0x700u) {
			/* part of the group filter */
			retval = CO_TRUE;
		} 
#    endif /* CO_HB_CONSUMER_CNT */
#    ifdef CO_EMCY_CONSUMER_CNT
		if ((canId & 0x0780u) == 0x80u) {
			/* part of the group filter */
			retval = CO_TRUE;
		} 
#    endif /* CO_EMCY_CONSUMER_CNT */
	}

	return retval;
}
#  endif /* CO_DRV_GROUP_FILTER */

/***********************************************************************/
/**
* codrvCanGetChan - get or calculate the filter entry 
*
* Check, if the COB require a receive filter entry (Chan = channel).
* If required, calculate a new entry.
*
* The new calculate filter number is written within the COB entry.
*
* \code
* const UNSIGNED16 maxFilter = 4 * BXCAN_FILTERBANK_CNT - 4 - 1;
*
* 4x 16bit ID filter for every bank
* first bank is used for masked filter (HB Cons, Emcy Cons)
*
*
* \returns
* filter entry
*
* \retval 0xFFFF
*	no filter required or no free entry
*
* 
*/
static UNSIGNED16 codrvCanGetChan(
		CO_CAN_COB_T * CO_CONST pCob
	)
{
/* 4x 11bit ID filter for every bank */
const UNSIGNED16 maxFilter = (4u * (filterOffset + BXCAN_FILTERBANK_CNT) - 4u) - 1u;
BOOL_T reserveChan = CO_FALSE;


#ifdef CODRV_DEBUG
	if (pCob->canChan < 4) {
		printf("Error codrvCanGetChan() - wrong channel number!\n");
	}

#endif

#  ifdef CO_DRV_GROUP_FILTER
	if (codrvCanCheckGroupFilter(pCob) == CO_TRUE) {
		/* nothing */
	} else
#  endif /* CO_DRV_GROUP_FILTER */

	if (pCob->enabled == CO_FALSE) {
		/* no channel/filter reservation req.  
		* But in case the COB has a filter assigned, 
		* this filter has to be disabled
		*/
	} else {
		reserveChan = CO_TRUE;
	}
	
	if ((reserveChan == CO_TRUE) && (pCob->canChan == 0xFFFFu)) 
	{
		if (nextAccEntry > maxFilter) {
			/* no free filter */
		} else {
			pCob->canChan = nextAccEntry;
			nextAccEntry++;
		}
	}

	return (pCob->canChan); /* no Channel == 0xFFFFu */
}

/***********************************************************************/
/**
* codrvCanSetIdFilter - configure one 16bit ID filter, base frame format
*
* All filters are 16bit filter in general.
* Inactive filter set to 0x7FF data frame, because every time only blocks
* of 4 filters can be enabled/disabled.
*
*/

static void codrvCanSetIdFilter(
		UNSIGNED8 filterNr,
		UNSIGNED16 accId,
		BOOL_T rtr
	)
{
UNSIGNED8 curFilterEntry;
UNSIGNED32 regVal;
UNSIGNED32 filterVal;
UNSIGNED16 filterRegIdx;
volatile UNSIGNED32 *pCanFilter = pCanFilterAddr;

#ifdef CODRV_DEBUG
	printf("Filter (%d): Id 0x%04x rtr %d\n", 
			(int)filterNr, accId, (rtr == CO_TRUE)?1:0);
#endif

	curFilterEntry = filterNr / 4u;

	/* deactivate filter bank - shortly all 4 IDs cannot be received! */
	pCanFilter[BXCAN_CANFA1R] &= ~(1ul << curFilterEntry); 

	filterRegIdx = BXCAN_CANFIRX(filterNr / 4u , (filterNr % 4u) / 2u + 1u );

	regVal = pCanFilter[filterRegIdx];

	if (rtr == CO_TRUE) {
		filterVal = (FIRX_ID(accId) | FIRX_RTR);
	} else {
		filterVal = FIRX_ID(accId);
	}

	if ((filterNr % 2u) != 0u) {
		filterVal <<= 16;
		regVal &= 0x0000FFFFu;
		regVal |= filterVal;
	} else {
		regVal &= 0xFFFF0000u;
		regVal |= filterVal;
	}

	pCanFilter[filterRegIdx] = regVal;

	pCanFilter[BXCAN_CANFA1R] |= 1ul << curFilterEntry; /* activate filter bank */
}


/***********************************************************************/
/**
* codrvCanSetFilter - activate and configure the receive filter
*
* Depend of the COB entry's the driver specific filter will 
* be configured. 
*
* For the bxCan only Base Identifiere are allowed.
* 
*
* \retval RET_OK
*	OK
* \retval RET_INVALID_PARAMETER
*	invalid COB reference
* \retval RET_DRV_ERROR
*	filter cannot be set, e.g. no free entry
*
*/

RET_T codrvCanSetFilter(
		CO_CAN_COB_T * pCanCob /**< COB reference */
	)
{
UNSIGNED16 canChan;
BOOL_T fError = CO_FALSE;

#ifdef CODRV_DEBUG
	printf("codrvCanSetFilter: 0x%04x rtr: %d enabled: %d\n", pCanCob->canId, pCanCob->rtr, pCanCob->enabled);
#endif

#  ifdef CO_DRV_GROUP_FILTER
	if (codrvCanCheckGroupFilter(pCanCob) == CO_TRUE) {
		/* nothing to do */
		return RET_OK;
	}
#  endif


	/* get a filter entry in case of:
	 * - COB has a filter entry from older setting or
	 * - COB is enabled and
	 * - COB is a Receive Data Frame or
	 * - COB is a Transmit Data Frame, but can be query by RTR
     */

	canChan = codrvCanGetChan(pCanCob);

	if (pCanCob->extended == CO_TRUE) {
		fError = CO_TRUE; /* filter not supported */
	} else
	if (pCanCob->enabled == CO_TRUE) 
	{
		if (pCanCob->rtr == CO_FALSE)
		{
			/* Receive Data frame */
			if (canChan == 0xFFFFu) {
				fError = CO_TRUE;
			} else {
				codrvCanSetIdFilter((UNSIGNED8)canChan, (UNSIGNED16)pCanCob->canId, CO_FALSE);
			}
			
		} else {
			/* Receive RTR frame */
			if (canChan == 0xFFFFu) {
				fError = CO_TRUE;
			} else {
				codrvCanSetIdFilter((UNSIGNED8)canChan, (UNSIGNED16)pCanCob->canId, CO_TRUE);
			}
		}

	} else {
		/* deactivate filter */
		if (canChan == 0xFFFFu) {
			/* nothing to do */	
		} else {
			codrvCanSetIdFilter((UNSIGNED8)canChan, (UNSIGNED16)0x7FFu, CO_FALSE);
		}
	}

	if (fError == CO_TRUE) {
		return (RET_DRV_ERROR);
	}

#  ifdef CODRV_DEBUG
	printFilter();
#  endif

	return(RET_OK);
}
#endif /* CO_DRV_FILTER */

/***********************************************************************/
/**
* codrvCanStartTransmission - start can transmission if not active
*
* Transmission of CAN messages should be interrupt driven.
* If a message was sent, the Transmit Interrupt is called
* and the next message can be transmitted.
* To start the transmission of the first message,
* this function is called from the stack.
*
* The easiest way to implement this function is
* to trigger the transmit interrupt, 
* but only if the transmission is not already active.
*
* \internal
*
* \return RET_T
* \retval RET_OK
*	start transmission was successful
*
*/
#ifdef POLLING
void codrvCanTransmitInterrupt(void);
#endif

RET_T codrvCanStartTransmission(
		void	/* no parameter */
	)
{
	/* if can is not enabled, return with error */
	if (canEnabled != CO_TRUE)  {
		return(RET_DRV_ERROR); /*lint !e904 function entry check */
	}

	if (transmissionIsActive == CO_FALSE)  {
		/* trigger transmit interrupt */
#ifdef POLLING
		codrvCanTransmitInterrupt();
#else
		/* enable global interrupt pending bit to call interrupt */
		codrvCanSetTxInterrupt();
#endif
	}

	return(RET_OK);
}
 

/***********************************************************************
* codrvCanTransmit - transmit can message
*
* This function writes a new message to the CAN controller and transmits it.
* Normally called from transmit Interrupt
*
* \internal
*
* \results
*	RET_T
*/
static RET_T codrvCanTransmit(
		CO_CONST CO_CAN_MSG_T * pBuf		/**< pointer to data */
	)
{
RET_T			retVal = RET_OK;
const UNSIGNED8 mbNr = 0u; /* currently, general MB 0 */
UNSIGNED32 rtr = 0ul;

	/* busy check */
	if ((pCan[BXCAN_CANTIXR(mbNr)] & BXCAN_CANTIXR_TXRQ) != 0ul) {
		return RET_DRV_BUSY;  /*lint !e904 function entry check */
	}

	/* write message to the CAN controller */
    pCan[BXCAN_CANTDTXR(mbNr)] = pBuf->len; /* dlc */

	if (pBuf->canCob.rtr == CO_FALSE) {
		pCan[BXCAN_CANTDLXR(mbNr)] = ((UNSIGNED32)pBuf->data[0]
						| ((UNSIGNED32)pBuf->data[1] << 8)
						| ((UNSIGNED32)pBuf->data[2] << 16)
						| ((UNSIGNED32)pBuf->data[3] << 24));

		pCan[BXCAN_CANTDHXR(mbNr)] = ((UNSIGNED32)pBuf->data[4]
						| ((UNSIGNED32)pBuf->data[5] << 8)
						| ((UNSIGNED32)pBuf->data[6] << 16)
						| ((UNSIGNED32)pBuf->data[7] << 24));
	} else {
		rtr |= BXCAN_CANTIXR_RTR; 
	}

	/* transmit it */
	if (pBuf->canCob.extended != CO_FALSE) {
		pCan[BXCAN_CANTIXR(mbNr)] = (pBuf->canCob.canId << BXCAN_CANTIXR_EXID_SHIFT)
					| rtr
					| BXCAN_CANTIXR_IDE
					| BXCAN_CANTIXR_TXRQ;
	} else {
		pCan[BXCAN_CANTIXR(mbNr)] = (pBuf->canCob.canId << BXCAN_CANTIXR_STID_SHIFT)
					| rtr
					| BXCAN_CANTIXR_TXRQ;
	}

	transmissionIsActive = CO_TRUE;

	return(retVal);
}


/***********************************************************************
* codrvCanDriverTransmitInterrupt - can driver transmit interrupt
*
* \internal
*
* This function is called, after message was transmitted.
* As first, inform stack about message transmission.
* Get the next message from the transmit buffer, write it to the CAN controller
* and transmit it.
*
* \param
*	none
* \results
*	none
*/

void codrvCanTransmitInterrupt(
		void	/* no parameter */
	)
{

	/* reset interrupt request 
	* => currently reset all possible TX IRQ requests
	*
	* -> very simple implementation
	*/
	pCan[BXCAN_CANTSR] = 
		(BXCAN_CANTSR_RQCP0 | BXCAN_CANTSR_RQCP1 | BXCAN_CANTSR_RQCP2);


	if ((pCan[BXCAN_CANTSR] & BXCAN_CANTSR_TME_MSK) == BXCAN_CANTSR_TME_ALL) {
		transmissionIsActive = CO_FALSE;

		/* inform stack about transmitted message */
		if (pTxBuf != NULL)  {
			coQueueMsgTransmitted(pTxBuf);
			pTxBuf = NULL;

			/* signal transmitted message */
			CO_OS_SIGNAL_CAN_TRANSMIT();
		}

		/* get next message from transmit queue */
		pTxBuf = coQueueGetNextTransmitMessage();
		if (pTxBuf != NULL)  {
			/* and transmit it - CAN message buffer is free */
			(void)codrvCanTransmit(pTxBuf);
		}
	}
}


/***********************************************************************
* codrvCanReceiveInterrupt - can driver receive interrupt
*
* \internal
*
* This function is called, if a new message was received
* As first get the pointer to the receive buffer
* and save the message there.
* Then set the buffer as filled and inform the lib about new data.
*
*
* \param
*	none
* \results
*	none
*/
void codrvCanReceiveInterrupt(
		void	/* no parameter */
	)
{
CO_CAN_MSG_T *pRecBuf;
const UNSIGNED8 mbNr = 0u;
UNSIGNED32 id;
BOOL_T extended = CO_FALSE;
BOOL_T rtr = CO_FALSE;
UNSIGNED8 len;
UNSIGNED32 u32Data;
CAN_ERROR_FLAGS_T * pError;

#ifdef CODRV_AUTOBAUD
	/* silent mode ? */
	if ((pCan[BXCAN_CANBTR] & BXCAN_CANBTR_SILM) != 0u)  {
		autoBaudActive = 0u;

		/* stop CAN controller */
		(void)codrvCanDisable();

		/* reset silent mode bit */
		u32Data = pCan[BXCAN_CANBTR];
		pCan[BXCAN_CANBTR] = u32Data & ~BXCAN_CANBTR_SILM;

		/* start CAN controller */
		(void)codrvCanEnable();
	}
#endif

	/* currently only Fifo 0 */
	if ((pCan[BXCAN_CANRFXR(mbNr)] & BXCAN_CANRFXR_FMP_MSK) == 0ul ) {
		/* empty */
		return;	  /*lint !e904 function entry check */
	}


	id = pCan[BXCAN_CANRIXR(mbNr)];

	/* 29bit IDs */
	if ((id & BXCAN_CANRIXR_IDE) != 0ul) {
		extended = CO_TRUE;
	}

	/* rtr */
	if ((id & BXCAN_CANRIXR_RTR) != 0ul) {
		rtr = CO_TRUE;
	}

	if (extended == CO_FALSE) {
		id = (id >> BXCAN_CANRIXR_STID_SHIFT) & 0x7FFul;
	} else {
		id = (id >> BXCAN_CANRIXR_EXID_SHIFT) & 0x1FFFFFFFul;
	}

	/* get receiveBuffer */
	pRecBuf = coQueueGetReceiveBuffer();
	if (pRecBuf == NULL)  {
		/* error, no buffer available */

		/* release mailbox */
		pCan[BXCAN_CANRFXR(mbNr)] = BXCAN_CANRFXR_RFOM; 	

		/* signal received message - but there was generated
		 * signals for all older received messages 
		 * that are in the buffer 
		 */
		/* CO_OS_SIGNAL_CAN_RECEIVE(); */

		return;	/*lint !e904 no buffer possible */
	}

	/* save message at buffer */
	pRecBuf->canCob.canId = id;


	pRecBuf->canCob.extended = extended;
	pRecBuf->canCob.rtr = rtr;

	len = (UNSIGNED8)(pCan[BXCAN_CANRDTXR(mbNr)] & BXCAN_CANRDTXR_DLC_MSK);
	if (len > 8u) {
		len = 8u;
	}
	pRecBuf->len = len;
	
	/* data */
	if (rtr == CO_FALSE) {
		u32Data = pCan[BXCAN_CANRDLXR(mbNr)];
		pRecBuf->data[0] = (UNSIGNED8)(u32Data & 0xFFu);
		pRecBuf->data[1] = (UNSIGNED8)((u32Data >> 8) & 0xFFu);
		pRecBuf->data[2] = (UNSIGNED8)((u32Data >> 16) & 0xFFu);
		pRecBuf->data[3] = (UNSIGNED8)((u32Data >> 24) & 0xFFu);
		if (len > 4u) {
			u32Data = pCan[BXCAN_CANRDHXR(mbNr)];
			pRecBuf->data[4] = (UNSIGNED8)(u32Data & 0xFFu);
			pRecBuf->data[5] = (UNSIGNED8)((u32Data >> 8) & 0xFFu);
			pRecBuf->data[6] = (UNSIGNED8)((u32Data >> 16) & 0xFFu);
			pRecBuf->data[7] = (UNSIGNED8)((u32Data >> 24) & 0xFFu);
		}
	}

	if ((pCan[BXCAN_CANRFXR(mbNr)] & BXCAN_CANRFXR_FOVR) != 0ul) {
		pError = codrvCanErrorGetFlags();
		pError->canErrorRxOverrun = CO_TRUE;

		/* reset error */
		pCan[BXCAN_CANRFXR(mbNr)] = BXCAN_CANRFXR_FOVR;
	}

	/* release mailbox */
	pCan[BXCAN_CANRFXR(mbNr)] = BXCAN_CANRFXR_RFOM; 	

	/* set buffer filled */
	coQueueReceiveBufferIsFilled();

	/* inform stack about new data */
	coCommTaskSet(CO_COMMTASK_EVENT_MSG_AVAIL);

	/* signal received message */
	CO_OS_SIGNAL_CAN_RECEIVE();

}

/***********************************************************************
* codrvCanErrorInterrupt - can driver error interrupt
*
* \internal
*
* This function is called, if a error passive or Busoff event
* occurs.
*
* \param
*	none
* \results
*	none
*/
void codrvCanErrorInterrupt(
		void	/* no parameter */
	)
{
CAN_ERROR_FLAGS_T * pError;
UNSIGNED32 lESR;

	/* set flag to signal the state change */
	pError = codrvCanErrorGetFlags();

	lESR = pCan[BXCAN_CANESR];
	if ((lESR & BXCAN_CANESR_BOFF ) != 0) {
		pError->canErrorBusoff = CO_TRUE;
	} else 
	if ((lESR & BXCAN_CANESR_EPVF ) != 0) {
		pError->canErrorPassive = CO_TRUE;
	} else {
		/* never occour, because the error interrupt is not 
		 * calling for this state change */
		pError->canErrorActive = CO_TRUE;
	}
	
	CO_OS_SIGNAL_CAN_STATE();

	/* clear interrupt bit */
	pCan[BXCAN_CANMSR] = BXCAN_CANMSR_ERRI;

}

/***********************************************************************/
/**
* codrvCanErrorHandler - Error handler
*
* This function polls the current state of the CAN controller
* and checks explicitly all situation that are not signaled
* within the interrupts.
*
* Call outside of interrupts!
* Typical call in codrvCanDriverHandler().
*/

static void codrvCanErrorHandler(
		void	/* no parameter */
	)
{
UNSIGNED32 err;
UNSIGNED32 change;
CAN_ERROR_FLAGS_T * pError;
static UNSIGNED16 errorWaitCounter = {0u};
BOOL_T fStartTransmission = CO_FALSE;

	pError = codrvCanErrorGetFlags();

	/*
	* Error active/passive/busoff check 
	*/
	err = pCan[BXCAN_CANESR];

#ifdef CODRV_AUTOBAUD
	/* silent mode active ? */
	if ((pCan[BXCAN_CANBTR] & BXCAN_CANBTR_SILM) != 0u)  {
		static UNSIGNED32	rCnt = {0u};

		if ((err >> 24u) > rCnt)  {
			/* use next bitrate */
			codrvAutoBaudNextBitrate();
			codrvCanEnable();
		}
		/* save current rx error counter for comparing */
        rCnt = err >> 24u;
	}
#endif

	/* check only changes of passive and busoff */
	change = err ^ oldEsrValue;
	change &= (BXCAN_CANESR_BOFF | BXCAN_CANESR_EPVF);

	if (change != 0ul) {
		if ((err & BXCAN_CANESR_BOFF) != 0ul) {
			/* busoff */
			/* canErrorBusoff = CO_TRUE;*/
			pError->canNewState = Error_Busoff; /* current state */
		} else 
		if ((err & BXCAN_CANESR_EPVF) != 0ul) {
			/* error passive */
			/* canErrorPassive = CO_TRUE; */
			pError->canNewState = Error_Passive; /* current state */
		} else {
			/* error active */
			/* canErrorActive = CO_TRUE; */
			pError->canNewState = Error_Active; /* current state */
		}
	}
	oldEsrValue = err; 

	if(transmissionIsActive != CO_FALSE) {
		/* correct possible Errors -> CAN has deactivated the transmission */
		if ((pCan[BXCAN_CANTSR] & BXCAN_CANTSR_TME_MSK) == BXCAN_CANTSR_TME_ALL) {
			errorWaitCounter++;
			if (errorWaitCounter > 20) {
				transmissionIsActive = CO_FALSE;
			}
		} else {
			errorWaitCounter = 0u;
		}
	} else {
		errorWaitCounter = 0u;
	}

	if (canEnabled == CO_TRUE) {
		if ((pCan[BXCAN_CANMSR] & BXCAN_CANMSR_INAK) != 0ul) {
			/* printf("INAK reset - stop can\n"); */
			canEnabled = CO_FALSE;
			pError->canNewState = Error_Offline; /* current state */
		}
	} else {
		if ((pCan[BXCAN_CANMSR] & BXCAN_CANMSR_INAK) == 0ul) {
			/* case: start can */
			canEnabled = CO_TRUE;
			/* error active */
			/* canErrorActive = CO_TRUE;*/
			pError->canNewState = Error_Active; /* current state */
		
			/* check for buffered messages */
			fStartTransmission = CO_TRUE;
		}
	}

	if (canEnabled == CO_TRUE) {
		/* check for stopped transmissions */
		if ((transmissionIsActive == CO_FALSE) && (pTxBuf != NULL)) {
			/* transmission aborted, e.g. busoff, 
		     * discard message -> is done within the tx interrupt
			*/
			fStartTransmission = CO_TRUE;
		}
	}
	
	if (fStartTransmission == CO_TRUE) {
		(void)codrvCanStartTransmission(); /* -> call Interrupt at this point */
	}
}


/***********************************************************************
* codrvCanDriverHandler - can driver handler
*
* \internal
*
* is cyclically called from stack to get actual CAN state
* (BUS_OFF, PASSIVE, ACTIVE)
* and return to bus on again after bus off was occurred.
*
* \param
*	none
* \results
*	none
*/
void codrvCanDriverHandler(
		void	/* no parameter */
	)
{
	/* check current state */
	codrvCanErrorHandler();

	/*
	* inform the CANopen Stack about state changes
	*/
	(void)codrvCanErrorInformStack();

#ifdef POLLING
	codrvCanReceiveInterrupt();
	codrvCanTransmitInterrupt();
#endif

    return;
}


#ifdef CODRV_DEBUG
#include <stdio.h>
void printFilter(void)
{
#  ifdef CODRV_DEBUG_MAX
int i,j;
volatile UNSIGNED32 *pCanFilter = pCanFilterAddr;

	printf("\nFiltersettings:\n");
	printf("FINIT %d\n", pCanFilter[BXCAN_CANFMR] & BXCAN_CANFMR_FINIT );
	printf("Filter active (0 off 1 on)       0x%08lx\n", pCanFilter[BXCAN_CANFA1R]);
	printf("Filtermode (0 mask 1 list)       0x%08lx\n", pCanFilter[BXCAN_CANFM1R]);
	printf("Filterscale (0 16-bit 1 32-bit)  0x%08lx\n", pCanFilter[BXCAN_CANFS1R]);
	printf("Filter assignm (0 fifo0 1 fifo1) 0x%08lx\n", pCanFilter[BXCAN_CANFFA1R]);

	printf("\n");
	//for(i = 0; i < 14; i++) 
	for(i = 0; i <= 0; i++) 
	{
		for (j = 1; j <= 2; j++) {
			printf("\n");
			printf("Filterbank %d Register %d\n", i,j );
			printf("Adr: 0x%08lx\n", &pCanFilter[BXCAN_CANFIRX(i,j)]); 

			printf("  ID 0x%04x\n", (pCanFilter[BXCAN_CANFIRX(i,j)] & 0xFFFFul) >> 5 );
			if( (pCanFilter[BXCAN_CANFM1R] & (1ul << i)) != 0) {
				printf("  ID 0x%04x\n", (pCanFilter[BXCAN_CANFIRX(i,j)] >> (16 + 5) )& 0xFFFFul);
			} else {
				printf("Mask 0x%04x\n", (pCanFilter[BXCAN_CANFIRX(i,j)] >> (16 + 5) )& 0xFFFFul);
			}
		}

	}

	for(i = 1; i < 14; i++) 
	{
		if( (pCanFilter[BXCAN_CANFA1R] & (1ul << i)) != 0) {
			for (j = 1; j <= 2; j++) {
				printf("\n");
				printf("Filterbank %d Register %d Val 0x%08lx\n", i,j, pCanFilter[BXCAN_CANFIRX(i,j)] );

				printf("  ID 0x%04x\n", (pCanFilter[BXCAN_CANFIRX(i,j)] & 0xFFFFul) >> 5);


				printf("  ID 0x%04x\n", ((pCanFilter[BXCAN_CANFIRX(i,j)] >> 16 )& 0xFFFFul) >> 5);
			}
		}

	}
#  endif	

}

#endif

