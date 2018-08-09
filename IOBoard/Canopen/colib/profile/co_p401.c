/*
* co_401.c -  CiA 401 profile implementation
*
* Copyright (c) 2013-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: co_p401.c 10360 2015-08-24 14:42:53Z boe $
*
*
*-------------------------------------------------------------------
*
*/


/********************************************************************/
/**
* \brief device profile implementation according CiA 401
*
* \file co_p401.c
*
* \mainpage CANopen Stack p401 Modul
*
* This a complete implementation of the CiA device profile for generic IO devices.
* Nevertheless not all optional objects and there behaviour are supported.
* The current implementation lacks for example floating point object for analog IO.
* Digital process IO is done via 8-bit ports, 1bit, 16-, and 32bit are not supported.
* Analog process IO is 16bit wide. 8- and 32bit are not supported (yet).
*
*
* To use the functionality provided in the 401 module,
* It first has to be initialized calling co401Init(), which typically is done
* after calling coCanOpenStackInit().
* The next step is telling the module which functions are provided
* to server the real hardware by registering hardware access functions using
* coEventRegister_401().
* Once the hardware can be accessed by the module,
* all output ports have to be set up, whether with the default value
* defined in the object dictionary by calling 
* co401DigOutDef(void) and co401AnOutDef(void)
* or with the error value defined in the object dictionary by calling
* co401DigOutErr(void) and co401AnOutErr(void);
*
* Once this is done, the co401Task() must be called regularly and cyclic.
*
*
* If currently not implemented objects are essential for your project, please contact
* service@emtas.de
*
*
*/


/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stddef.h>
#include <stdio.h>			/* only for debug using printf() */
#include <stdlib.h>			/* abs() */

/* header of CANopen services
---------------------------------------------------------------------------*/
#include "gen_define.h"
#include <co_canopen.h>


/* header of project specific types
---------------------------------------------------------------------------*/


#ifdef CO_PROFILE_401
#include <co_datatype.h>
#include <co_timer.h>
#include <co_p401.h>

/* constant definitions
---------------------------------------------------------------------------*/
/* special trigger flags for
 * Object 6421h : Analog input interrupt trigger selection
 */ 
#define CO_401_ANIN_INT_UPPER_LIMIT		(1 << 0)
#define CO_401_ANIN_INT_LOWER_LIMIT		(1 << 1)
#define CO_401_ANIN_INT_DELTA			(1 << 2)
#define CO_401_ANIN_INT_DELTA_NEG		(1 << 3)
#define CO_401_ANIN_INT_DELTA_POS		(1 << 4)
#define CO_401_ANIN_INT_DELTA_ALL	\
	(CO_401_ANIN_INT_DELTA | CO_401_ANIN_INT_DELTA_NEG \
				 | CO_401_ANIN_INT_DELTA_POS)

#define I16_MIN				(-32768L)
#define I16_MAX 			(32767L)

#define CO_401_AN_VALUE_OK			0
#define CO_401_AN_VALUE_TOO_LOW		1
#define CO_401_AN_VALUE_TOO_HIGH	2


/* possible functions to be called at RPDO indication */
#define CO_401_DIGOUT_MAPPED		1
#define CO_401_ANOUT_MAPPED			2

							
/* set the corresponding bit in the optional object 
* 6422h : Analog input interrupt source */
#define CO_401_SET_ANIN_INT_SRC(port)



/*
#define CO_DEBUG printf
#define CO_DEBUG1(s, a1) printf(s, a1)
#define CO_DEBUG2(s, a1, a2) printf(s, a1, a2)
#define CO_DEBUG3(s, a1, a2, a3) printf(s, a1, a2, a3)
#define CO_DEBUG4(s, a1, a2, a3, a4) printf(s, a1, a2, a3, a4)
*/

/* stop -------------- */
/* local defined data types
---------------------------------------------------------------------------*/


/* size of this table depends on how many objects can be mapped
 * and how often is an entry mapped.
 * This size is needed as information provided by the CANopen Device Designer.
 * the mapping table has three entries
 * 1 byte  specifier for the function to be called
 *   	profile401AnOut(int port)
 *   	profile401DigOut(int port)
 * 1 byte port number 
 * 1 byte associated PDO number
 */
typedef UNSIGNED32 MAPPING_TABLE_T;


/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static void pdoInd(UNSIGNED16 pdo);
static RET_T nmtInd(BOOL_T execute, CO_NMT_STATE_T	newState);
static void profile401AnInTask(void);
static void profile401DigInTask(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static MAPPING_TABLE_T mappingTable[MAPPINGTABLE_MAX];

/* pointer to user provided HW access functions */
static CO_EVENT_401_DO_T pDigOut = NULL;
static CO_EVENT_401_DI_T pDigIn  = NULL;
static CO_EVENT_401_AO_T pAnOut  = NULL;
static CO_EVENT_401_AI_T pAnIn   = NULL;

/*
 * register functions used to register 401 HAL API.
 * Each of the basic HW blocks defined in CiA 401
 * is using a special HW API to access the real HW.
 * This function has to handle the number of instances
 * (e.g. number of input ports, analog channels ...)
 *
 *
 * FIXME
 * could also be only one function
 * coEventRegister_401_HW(pDigOut, pDigIn, pAnOut, pAnIn),
 * used like
 *        coEventRegister_401_HW(pdigOut, pDigIn, NULL, NULL)
 *
 * */

/***************************************************************************/
/**
*
* \brief - coEventRegister_401 register all user provided HW API functions
*
*
* the function pinter is take into internal pointers.
* If one of the basic functionalities \b digital \b in, \b digital \b out
* \b analog \b in or \b analog \b out
* is not provided, NULL should be used as argument.
*
* \param pDI pointer to the digital in  HW API function
* \param pDO pointer to the digital out HW API function
* \param pAI pointer to the analog in HW API function
* \param pAO pointer to the analog out HW API function
* \return RET_OK - always, no checks are possible.
* It is assumed, that all pointers are pointing to the correct functions.
*
*/
RET_T coEventRegister_401(
		CO_EVENT_401_DI_T pDI,
		CO_EVENT_401_DO_T pDO,
		CO_EVENT_401_AI_T pAI,
		CO_EVENT_401_AO_T pAO
	)
{
	CO_DEBUG("Called" __FILE__);
	pDigOut = pDO;
	pDigIn  = pDI;
	pAnOut  = pAO;
	pAnIn   = pAI;

	return RET_OK;
}


/***************************************************************************/
/**
* \internal
* \brief - profile401DigOut prepares the output value for a digout port
*
*  and calls the HW API
*/
static void profile401DigOut(
		int port
	)
{
UNSIGNED8 invert;
UNSIGNED8 value;
RET_T retval;

	CO_DEBUG2("called %s(%d)\n", __func__, port);

	if (pDigOut != NULL) {
		/* only of a HW function is provided */

		/* The value is obtained using the OD access function.
		 * All parameters are coming from the stack,
		 * no need to check the return value */
		(void)coOdGetObj_u8(0x6200u + CO_401_PROFILE_OFFSET, port,
				&value); 
		/* get the polarity invert value for this port */
		retval = coOdGetObj_u8(0x6202u + CO_401_PROFILE_OFFSET, port,
				&invert); 
		if (retval == RET_OK) {
			CO_DEBUG2("401: Polarity for dig out %d available 0x%02x\n",
					port, invert);
			value ^= invert;
		}
		/* Call the HE API */
		pDigOut(port, value);
	}
}


/***************************************************************************/
/**
* \brief - co401DigOutDef Prepares the default output value for a digout port
*
*  Calculates for all digital out ports the default value obtained from 0x6200 
*  and calls the HW API to set the value.
*/
void co401DigOutDef(
		void	/* no parameter */
	)
{
RET_T retval;
UNSIGNED8 maxport;
UNSIGNED8 currentValue;
UNSIGNED8 port; /* 1..n */

	CO_DEBUG("Set all digout ports to it's error value\n");

	if (pDigOut != NULL) {
		/* if HW access is registered */
		retval = coOdGetObj_u8(0x6200u + CO_401_PROFILE_OFFSET, 0,
			&maxport); 
		if (retval != RET_OK) {
			/* Subindex 0 of object 0x6200 does not exist */
			return;
		}
		/* now handle all ports up to maxport, highest available sub-index */
		for (port = 1u; port <= maxport; port++) {
			retval = coOdGetObj_u8(0x6200u + CO_401_PROFILE_OFFSET,
							port, &currentValue); 
			if (retval == RET_OK) {
				/* digout port is available */
				profile401DigOut(port);
		   }
	   }
   }
}


/***************************************************************************/
/**
* \brief - co401DigOutErr Prepares the error output value for a digout port
*
*  Calculates for all digital out ports the error value obtained from 0x6207 
*  and calls the HW API to set the value.
*
*  Real available output ports, object 0x6200, have to be used,
*  because the standard says object 0x6206 is optional,
*  and if not available, the default value, which is 0xFF,
*  should be used.
*
*  An error value is only set,
*  if error mode is defined AND an error value is specified.
*
*  The function is called automatically in case the device is going into
*  an communication error, e.g. a CAN Bus-Off condition
*  or it detects a communication error according object 0x1029.
*
*  After device initialization the function \e can be called
*  to set the error value at device start-up.
*
*/
void co401DigOutErr(
		void	/* no parameter */
	)
{
UNSIGNED8 changePolarity;
UNSIGNED8 errorMode;
UNSIGNED8 errorValue;
UNSIGNED8 currentValue;
UNSIGNED8 outVal;
UNSIGNED8 mask;
RET_T retval;
UNSIGNED8 maxport;
UNSIGNED8 port; /* 1..n */
int bit;

	CO_DEBUG("Set all digout ports to it's error value\n");

	if (pDigOut != NULL) {
		/* if HW access is registered */
		retval = coOdGetObj_u8(0x6200u + CO_401_PROFILE_OFFSET, 0,
				&maxport); 
		if (retval != RET_OK) {
			/* Subindex 0 of object 0x6200 does not exist */
			return;
		}
		/* now handle all ports up to maxport, highest available sub-index */
		for (port = 1u; port <= maxport; port++) {
			retval = coOdGetObj_u8(0x6200u + CO_401_PROFILE_OFFSET,
							port, &currentValue); 
			if (retval == RET_OK) {
				/* digout port is available
				 * look further for Error Mode and Value*/
				retval = coOdGetObj_u8(0x6206u + CO_401_PROFILE_OFFSET,
							port, &errorMode); 
				if (retval != RET_OK) {
					/* Error mode for this port is not defined */
					errorMode = 0xFFu;	/* the default Value */
				}
				/* get the error value for this port */
				retval = coOdGetObj_u8(0x6207u + CO_401_PROFILE_OFFSET,
							port, &errorValue); 
				if (retval != RET_OK) {
					errorValue = 0;		/* the default value */
				}
				
				/* errorMode bit says:
				   1 = output value shall take the pre-defined condition
				       specified in 6207h object
				   0 = output value shall be kept if an error occurs
				 */

				/* This can be only calculated bit by bit */
				outVal  = 0;
				for (bit = 0; bit < 8; bit ++) {
					mask = 1u << bit;
					if ( errorMode & mask) {
						outVal |= (errorValue & mask); 
					} else  {
						outVal |= (currentValue & mask); 
					}
				}
				/* get the polarity invert value for this port
				 * If not implemented, the default value is 0 */
				retval = coOdGetObj_u8(0x6202u + CO_401_PROFILE_OFFSET,
														port, &changePolarity); 
				if (retval == RET_OK) {
					/* if ChangePolarity is available, use it */
					outVal ^= changePolarity;
				}
				/* Call the HE API */
				pDigOut(port, outVal);

			} /* digout port available */
		} /* for(all possible digout ports) */
	}  /* HW access is registered */ 
}


/***************************************************************************/
/**
* \internal
* \brief - profile401AnOut prepares the output value for a digout port
*
*  and calls the HW API
*/

static void profile401AnOut(
		int port
	)
{
INTEGER16 value;

	if (pAnOut != NULL) {
		/* only of a HW function is provided */

		/* The value is obtained using the OD access function.
		 * All parameters are coming from the stack,
		 * no need to check the return value */
		(void)coOdGetObj_i16(0x6411u + CO_401_PROFILE_OFFSET, port, &value); 

		/* value contains now the raw value from the object dictionary entry.
		 * Before writing to the hardware, the value can be scaled
		 * using two optional object dictionary entries
		 * scale (0x6447) and offset (0x6446) - both INTEGER32.
		 *
		 * out value = (value + offset) / scale
		 * See ciA 401
		 * 8.5.4 Object 6413h : Write analog output float
		 *
		 * value comes left-adjusted from the object dictionary
		 */

		/* Call the HE API */
		pAnOut(port, value);
	}
}


/***************************************************************************/
/**
* \brief - co401AnOutDef Prepares the default output value for a digout port
*
*  Calculates for all analog out ports the default value obtained from 0x6411 
*  and calls the HW API to set the value.
*/
void co401AnOutDef(
		void	/* no parameter */
	)
{
RET_T retval;
UNSIGNED8 maxport;
INTEGER16 currentValue;
UNSIGNED8 port; /* 1..n */

	if (pAnOut != NULL) {
		/* if HW access is registered */
		retval = coOdGetObj_u8(0x6411u + CO_401_PROFILE_OFFSET, 0, &maxport); 
		if (retval != RET_OK) {
			/* Subindex 0 of object 0x6411 does not exist */
			return;
		}
		/* now handle all ports up to maxport, highest available sub-index */
		for (port = 1u; port <= maxport; port++) {
			retval = coOdGetObj_i16(0x6411u + CO_401_PROFILE_OFFSET,
							port, &currentValue); 
			if (retval == RET_OK) {
				/* an-out port is available */
				profile401AnOut(port);
			}
		}
	}
}



/***************************************************************************/
/**
* \brief - co401AnOutErr prepares the error output value for a digout port
*
*  Calculates for all analog out ports the error value obtained from  0x6444
*  and calls the HW API to set the value.
*
*  Real available analog output ports, object 0x6411, have to be used,
*  because the standard says object 0x6443 is optional,
*  and if not available, the default value, which is 1 (TRUE),
*  should be used.
*
*  An error value is only set,
*  if error mode is defined AND an error value is specified.
*  Otherwise no value is set, the port stays with its old value. 
*
*  The function is called automatically in case the device is going into
*  an communication error, e.g. a CAN Bus-Off condition
*  or it detects a communication error according object 0x1029.
*
*  After device initialization the function \e can be called
*  to set the error value at device start-up.
*/
void co401AnOutErr(
		void	/* no parameter */
	)
{
UNSIGNED8 errorMode;
INTEGER32 errorValue;
INTEGER16 currentValue;
RET_T retval;
UNSIGNED8 maxport;
UNSIGNED8 port; /* 1..n */
	CO_DEBUG("Set all AO ports to it's error value\n");
	if (pAnOut != NULL) {
		/* if HW access is registered */
		retval = coOdGetObj_u8(0x6411u + CO_401_PROFILE_OFFSET, 0u,
			&maxport); 
		if (retval != RET_OK) {
			/* Subindex 0 of object 0x6411 does not exist */
			return;
		}
		/* now handle all ports up to maxport, highest available sub-index */
		for (port = 1u; port <= maxport; port++) {
			retval = coOdGetObj_i16(0x6411u + CO_401_PROFILE_OFFSET,
							port, &currentValue); 
			if (retval == RET_OK) {
				/* an-out port is available
				 * look further for Error Mode and Value*/
				errorMode = 1u; /* default value */
				retval = coOdGetObj_u8(0x6443u + CO_401_PROFILE_OFFSET,
							port, &errorMode); 
				if (errorMode == 1u) {
					/* use the error value */
					errorValue = 0u;
					/* get the error value for this port */
					retval = coOdGetObj_i32(0x6444u + CO_401_PROFILE_OFFSET,
								port, &errorValue); 

					/* write the error value, whether 0 or the object value */
					/* Call the HE API */
					pAnOut(port, errorValue);
				}
			} /* an-out port available */
		} /* end loop for(all anout ports) */
	}  /* HW access is registered */ 
}



#if defined(CO_PDO_RECEIVE_CNT)
/***************************************************************************/
/**
* \internal
*
* \brief createMappingTable
*
* read all RPDO mapping entries and generate the table
* until it is full. 
* In this case the function returns with RET_EVENT_NO_RESSOURCE
*
* FIXME - currently only PDOs 1 to 100 are scanned 
* */
static RET_T createMappingTable(
		void	/* no parameter */
	)
{
RET_T retvalFunc	= RET_OK;
RET_T retval		= RET_OK;
UNSIGNED8 maxMappings;
UNSIGNED32 mapEntry;
UNSIGNED16 index;
UNSIGNED8 subIndex;
int pdo;
int count = 0;
int rxPdoCnt;
static const int maxPdos = { CO_RXPDO_COUNTS };

	rxPdoCnt = 0;
	for (index = 0x1600u, pdo = 1u; index <= 0x17ffu; 
			index++, pdo++) 
	{
		if (rxPdoCnt >= maxPdos) {
			/* no more RPDOs */
			break;
		}
		
		// CO_DEBUG2("scan mapping table pdo %d at 0x%04x\n", pdo, index);
		/*  get number of mappings for this PDO */
		retval = coOdGetObj_u8(index, 0, &maxMappings);

		if (retval == RET_OK) {
			rxPdoCnt++; /* count RPDOs within the object dictionary */
		}

		if ((retval == RET_OK) && (maxMappings > 0u)) {
			/* PDO available and has mappings */
			CO_DEBUG2("401: max mappings for PDO %d: %d\n", pdo, maxMappings);
			for (subIndex = 1u; subIndex <= maxMappings; subIndex++) {
				mappingTable[count] = 0u;    /* empty entry */
				retval = coOdGetObj_u32(index, subIndex, &mapEntry);
				if (retval == RET_OK) {
					/* FIXME mapping entries shoul not have gaps,
					 * no deed for the RET_OK check */

					switch(mapEntry & 0xFFFF0000u) {
						case 0x62000000ul:
							CO_DEBUG("\tfound dig-out \n");
							/* func | port | pdo */
							mappingTable[count] = CO_401_DIGOUT_MAPPED << 16;
							break;
						case 0x64110000ul:
							CO_DEBUG("\tfound an-out \n");
							mappingTable[count] = CO_401_ANOUT_MAPPED << 16;
							break;
						default:
							break;
					}
					/* add subindex == port */
					mappingTable[count] |= (mapEntry & 0xFF00u);
					/* add pdo */
					mappingTable[count] |= (pdo & 0xffu);
				}
				if (++count == MAPPINGTABLE_MAX) {
					break;
				}
			}
			/* break outer loop */
			if (count == MAPPINGTABLE_MAX) {
				CO_DEBUG(">>> mapping table full !!\n");
				retvalFunc = RET_EVENT_NO_RESSOURCE;
			}
		}	/* for all mappings */
	}	/* for all PDOs */
	return (retvalFunc); 
}


/***************************************************************************/
/**
* \internal
*
* \brief profile401ScanMappingTable
*
*/
static void profile401ScanMappingTable(
		int pdo
	)
{
int count = 0;
int port;

	CO_DEBUG2("called %s(%d)\n", __func__, pdo);

	while (mappingTable[count] != 0) {
		if (pdo == (int)(mappingTable[count] & 0xFFu)) {
			port = ((mappingTable[count] & 0xff00u) >> 8);
			if ((mappingTable[count] & 0xFF0000)
						== (CO_401_DIGOUT_MAPPED << 16)) {
				profile401DigOut(port);

			} else if ((mappingTable[count] & 0xFF0000)
						== (CO_401_ANOUT_MAPPED << 16)) {
				profile401AnOut(port);
			} else {
				;
				CO_DEBUG1("401: wrong process out function for pdo %d\n", pdo);
			}
		}  /* if this pdo */
		count++;
	} /* while table entry is valid */
}
#endif	/* defined(CO_PDO_RECEIVE_CNT) */


/***************************************************************************/
/**
* \brief - sdoServerWriteInd handles write access to the 401 Profile section
*
* If an SDO write is done to process outputs, the according functions
* are executed.
* If SDO write is done to the RPDO mapping parameters
* internal mapping table is newly build.
*
* \return RET_T
*/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,	/* check/execute */
		UNSIGNED8	sdoNr,		/* sdo number */
		UNSIGNED16	index,		/* index */
		UNSIGNED8	subIndex	/* subindex */
	)
{

	CO_DEBUG4("401: sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

	if (execute == CO_FALSE) {
		/* check if the indication can be performed */

		/* e.g. check if a HW API function is registered */
	} else {
		/* execute the request */
		/* check for changing the RPDO mapping tables 
		 * which are used when a PDO indication arrives */
		if ((index >= 0x1600u) && (index <= 0x17ffu)) {
			/* to update the mapping table
			 * only the last acces to subIndex 0 is of interest.
			 * configuration is finished when
			 * subIndex 0 gets the number of mappings
			 */
			if (subIndex == 0u) {
				CO_DEBUG1("changed mapping of PDDO %d\n", index - 0x15ffu);
				(void)createMappingTable();
			}

		}

		/* handle direct SDO write access to process outputs */
		if (index == 0x6200u + CO_401_PROFILE_OFFSET) {
			/* 8bit digital output ports are addressed */
			profile401DigOut(subIndex);
		}

		if (index == 0x6411u + CO_401_PROFILE_OFFSET) {
			/* 8bit digital output ports are addressed */
			profile401AnOut(subIndex);
		}


	}
	return(RET_OK);
}

extern volatile uint16_t eepChanged ;
/*********************************************************************/
/**
* \internal
*
* \brief - canInd
*
*
*/
static void canInd(
		CO_CAN_STATE_T	canState
	)
{
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			CO_DEBUG("401: CAN: Bus Off\n");
			co401DigOutErr();
			co401AnOutErr();
			errCnt_Can++;
			eepChanged = 1;
/* set Error Flag for process io error state handling */ 
			break;
		case CO_CAN_STATE_BUS_ON:
			CO_DEBUG("401: CAN: Bus On\n");

/* set Error Flag for process io error state handling */ 
			break;
		case CO_CAN_STATE_PASSIVE:
			CO_DEBUG("401: CAN: Passiv\n");

/* RE-set Error Flag for process io error state handling */ 
			break;
		case CO_CAN_STATE_UNCHANGED:
			CO_DEBUG("401: CAN: unchanged\n");

/* RE-set Error Flag for process io error state handling */ 
			break;
		default:

/* RE-set Error Flag for process io error state handling */ 
			CO_DEBUG1("401: CAN: state %d ??? (default switch tree)\n",
					canState);

			break;
	}
}


#if defined(CO_PDO_RECEIVE_CNT)
/*********************************************************************/
/**
* \internal
* \brief - pdoInd, handle all receive PDOs
*
* At the moment only static mapping according CiA 401 is supported
*
* The code is only needed if RPDOs are defined in the Device Designer
*
*/
static void pdoInd(
		UNSIGNED16	pdo
	)
{
# if CO_401_STATIC_MAPPING
	CO_DEBUG1("401: pdoInd: pdo %d received\n", pdo);
	if (pdo == 1u) {

		/* how many objects are in it ?
		 * which index:subIndex ?
		 */
		profile401DigOut(1u);
		profile401DigOut(2u);
		 /* ... up to 8 for the 1st RPDO */
	}
	if (pdo == 2u) {      /* RPDO 2,3, an4 are analog values */
		profile401AnOut(1u);
		profile401AnOut(2u);
		profile401AnOut(3u);
		profile401AnOut(4u);
	}
# else
	/* scan the mapping table for PDOs having mapped objects
	 * and call HW API in case we have updates for objects */
	profile401ScanMappingTable(pdo);
# endif
}
#endif	/* defined(CO_PDO_RECEIVE_CNT) */


/*********************************************************************/
static RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	static CO_NMT_STATE_T oldState = 0;
	CO_DEBUG2("401: nmtInd: New Nmt state %d - execute %d\n", newState, execute);
	if(execute == CO_TRUE)
	{
		if (newState == CO_NMT_STATE_STOPPED)
		{
			/* switch outputs in error mode */
			co401DigOutErr();
			//add by paul
			co401AnOutErr();
		}
		else if(CO_NMT_STATE_RESET_NODE == newState)
		{

		}
		else if(CO_NMT_STATE_RESET_COMM == newState)
		{

		}
		else if(CO_NMT_STATE_PREOP == newState)
		{
			BoardMappingUpdate();
			if(oldState == CO_NMT_STATE_OPERATIONAL)
			{
				errCnt_Nmt++;
				extern volatile uint16_t eepChanged;
				eepChanged = 1;
			}
		}
	}
	oldState = newState;

	return(RET_OK);
}



/*********************************************************************/
/**
* \brief - Initializes the modules internal functionalities
*
* The main task of this function is registering itself all CANopen indications
* handled by the module at the CANopen stack.
*/

RET_T co401Init(
		void
	)
{
RET_T retval = RET_OK;

	retval = coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd);
	if (retval != RET_OK)  {
		CO_DEBUG1("401: failed to register SDo write: %d\n", retval); 
		return(retval);	
	}
	CO_DEBUG("registered SDO write indication successful\n");

	/* register NMT indication for the 401 error handling */
	retval = coEventRegister_NMT(nmtInd);
	if (retval != RET_OK)  {
		CO_DEBUG1("401: failed to register NMT state indication: %d\n", retval);
		return(retval);	
	}
	CO_DEBUG("registered NMT State change indication successful\n");

	/* register CAN status indication for the 401 error handling */
	retval = coEventRegister_CAN_STATE(canInd);
	if (retval != RET_OK)  {
		CO_DEBUG1("401: failed to register CAN state indication: %d\n", retval);
		return(retval);	
	}
	CO_DEBUG("registered CAN State change indication successful\n");


#if defined(CO_PDO_RECEIVE_CNT)
	{
		retval = createMappingTable();
		if (retval != RET_OK) {
			/* mapping table memory resource to small,
			 * only warn at this time to optimize memory table allocation
			 */
			CO_DEBUG("401: PDO mapping table full\n");
		}
	}

	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		CO_DEBUG1("401: failed to register PDO indication: %d\n", retval); 
		return(retval);	
	}
	CO_DEBUG("registered PDO indication successful\n");
	//update by paul
#if 1
	if (coEventRegister_PDO_SYNC(pdoInd) != RET_OK)  {
		CO_DEBUG1("401: failed to register PDO indication: %d\n", retval);
		return(retval);
	}
	CO_DEBUG("registered PDO indication successful\n");
#endif

#endif	/* defined(CO_PDO_RECEIVE_CNT) */



	/* FIXME: Es fehlen:
						PDO_SYNC
	 */

	return(retval);
}


/*********************************************************************/
/**
 * \internal
 * reads and checks all digital input 8bit ports
 * in order to generate later on PDOs containing the related object
 * an bit field is used to store the 'interrupt'
 * beginning with bit 0 for port 1 and so on.
 * The first 32 possible ports are stored in an UNSIGNED32 variable
 * profile401DigInInterrupts[1];
 *
 * This code is currently not used !
 * As soon as an interrupt trigger condition is detected
 * a PDO is send calling coPdoReqObj(index, port, 0).
 */

static void profile401DigInTask(
		void	/* no parameter */
	)
{
UNSIGNED8 globalInterrupt = 0;		/* use default value */
UNSIGNED8 maxport; /* 1..254 */
RET_T retval;

	/* check only once for later use, if interrupts
	 * are globally enabled.
	 * Only in this case, bit changes are evaluated to generate TPDOs
	 */
	retval = coOdGetObj_u8(0x6005u + CO_401_PROFILE_OFFSET, 0,
			&globalInterrupt); 

	retval = coOdGetObj_u8(0x6000u + CO_401_PROFILE_OFFSET, 0, &maxport); 
	if (retval == RET_OK) {
		UNSIGNED8 currentValue;
		UNSIGNED8 newValue;
		UNSIGNED8 port; /* 1..n */
		/* Subindex 0 of object 0x6200 does exist, we have some ports */
		for (port = 1u; port <= maxport; port++) {
			/* this is for checking if the port is there */ 
			retval = coOdGetObj_u8(0x6000u + CO_401_PROFILE_OFFSET,
					port, &currentValue); 
			if (retval == RET_OK) {
				/* digin port is available
				 * look further for an optional filter value */
				UNSIGNED8 filter;
				UNSIGNED8 changePolarity;

#ifdef CO_401_DIGIN8_FILTER
				retval = coOdGetObj_u8(0x6003u + CO_401_PROFILE_OFFSET,
						port, &filter); 
				if (retval != RET_OK) {
					filter = 0u;		/* use default value */
				}
#else
				filter = 0u;
#endif
				/* get the content of the hardware port */
				/* ==================================== */
				newValue = pDigIn(port, filter);
				/* ==================================== */

				/* check for change Polarity object */
				retval = coOdGetObj_u8(0x6002u + CO_401_PROFILE_OFFSET,
						port, &changePolarity); 
				if (retval != RET_OK) {
					changePolarity = 0u;		/* use default value */
				}
				newValue ^= changePolarity;

				/* put the value back to OD */
				(void)coOdPutObj_u8 (0x6000u + CO_401_PROFILE_OFFSET,
						port, newValue);

				/* PDO handling */
#if defined(CO_PDO_TRANSMIT_CNT)
				if (globalInterrupt != 0u) {
					UNSIGNED8 tmp;
					int intFlag = 0;
					/* watch changes of dig in signals */
				 	/* check if any port changed and interrupt
					 * for this channel is enabled
					 * The default is 0xff - any change results in an PDO */
					tmp = 0xFFu;
					coOdGetObj_u8(0x6006u + CO_401_PROFILE_OFFSET,
						port, &tmp);
					if (((currentValue ^ newValue) & tmp) != 0u) {

						intFlag = 1;
						CO_DEBUG1("401: detected any change at digin port %d\n", port); 
		
					} else {

						/* if not any change, look for low-high only */
						tmp = 0u;
						coOdGetObj_u8(0x6007u + CO_401_PROFILE_OFFSET,
							port, &tmp);
						if ((((currentValue & newValue) ^ newValue) & tmp)  != 0u) {
							CO_DEBUG1("401: detected Low->High change at digin port %d\n", port); 
							intFlag = 1;
						}

						/* and now high-low changes */
						tmp = 0u;
						coOdGetObj_u8(0x6008u + CO_401_PROFILE_OFFSET,
							port, &tmp);
						if ((((currentValue ^ newValue) & ~newValue) & tmp)  != 0u) {
							CO_DEBUG1("401: detected High-Low change at digin port %d\n", port); 
							intFlag = 1;
						}
					}

					if (intFlag == 1) {
						/* send TPDO object related */
						retval = coPdoReqObj(0x6000u + CO_401_PROFILE_OFFSET, port, 0);
					}
				}
#endif /* defined(CO_PDO_TRANSMIT_CNT) */

			}
		} /* for() all digin ports */
	}
}

/**
 * \internal
 * \brief Read analog Inputs, evaluate interrupt trigger conditions
 */
static void profile401AnInTask(
		void	/* no parameter */
	)
{
UNSIGNED8 globalInterrupt = 0u;		/* use default value */
UNSIGNED8 maxport; /* max subindex of 'read analog input', max. 254 */
RET_T retval;

	/* check only once for later use, if interrupts
	 * are globally enabled.
	 * Only in this case, bit changes are evaluated to generate TPDOs
	 */
	retval = coOdGetObj_u8(0x6423u + CO_401_PROFILE_OFFSET, 0,
			&globalInterrupt); 

	retval = coOdGetObj_u8(0x6401u + CO_401_PROFILE_OFFSET, 0, &maxport); 
	if (retval == RET_OK) {
		INTEGER16 currentValue;
		INTEGER16 newValue;
		UNSIGNED8 port; /* 1..n */

		/* Subindex 0 of object 0x6200 does exist, we have some ports */
		for (port = 1u; port <= maxport; port++) {
			retval = coOdGetObj_i16(0x6401u + CO_401_PROFILE_OFFSET,
					port, &currentValue); 
			if (retval == RET_OK) {
				/* digin port is available
				 *  */

				/* get the content of the hardware port */
				/* ==================================== */
				newValue = pAnIn(port);
				/* ==================================== */


				/* ===========================================
				 * handle offset (0x6431) and scaling (0x6432)
				 * Spec V 1.0 (June/2013) excludes scaling
				 * ===========================================
				 */

				/* put the value back to OD */
				(void)coOdPutObj_i16(0x6401u + CO_401_PROFILE_OFFSET,
						port, newValue);


#if defined(CO_PDO_TRANSMIT_CNT)
				/* PDO handling
				 *
				 * Objekt 6423 Interrupt enable
				 * Objekt 6424 Upper Limit			i32
				 * Objekt 6425 Lower Limit			i32
				 * Objekt 6426 Value Difference		u32
				 *
				 *
				 * 8.6.2 Object 6421h : Analog input interrupt trigger
				 * selection. This object shall determine, which events
				 * shall cause an interrupt for a specific channel.
				 *
				 * Not implemented: 8.6.3 Object 6422h
				 *     Analog input interrupt source
				 *
				 *
				 *
				 */
				if (globalInterrupt != 0u) {
					UNSIGNED8 triggerSelect = 
						  CO_401_ANIN_INT_UPPER_LIMIT
						| CO_401_ANIN_INT_LOWER_LIMIT
						| CO_401_ANIN_INT_DELTA ;
					int intFlag = 0;

					/* get the trigger definition */
					coOdGetObj_u8(0x6421u + CO_401_PROFILE_OFFSET,
							port, &triggerSelect); 

					/* Upper and lower limit compare is using
					 * always measured values, the newValue 
					 * and compares with fixed limits got from the OD.
					 * To handle the one time transition as trigger
					 * the before-value  currentValue is considered as well.
					 */
					/* test for upper limit */
					if ((triggerSelect & CO_401_ANIN_INT_UPPER_LIMIT) != 0u) {
						INTEGER32 upperLimit = 0u; /* default upper limit */
						coOdGetObj_i32(0x6424u + CO_401_PROFILE_OFFSET,
								port, &upperLimit); 

						if ((	currentValue <= upperLimit)
								&& (newValue > upperLimit)) {
							CO_DEBUG1("--> detected upper limit trigger %d\n", port);
							intFlag = 1;
							CO_401_SET_ANIN_INT_SRC(port);
						}
					}

					/* test for lower limit */
					if ((triggerSelect & CO_401_ANIN_INT_LOWER_LIMIT) != 0) {
						INTEGER32 lowerLimit = 0u;	/*default lower limit */
						coOdGetObj_i32(0x6425u + CO_401_PROFILE_OFFSET,
								port, &lowerLimit); 
						if ((   currentValue >= lowerLimit)
								&& (newValue < lowerLimit)) { 
							CO_DEBUG1("--> detected lower limit trigger %d\n", port);
							intFlag = 1;
							CO_401_SET_ANIN_INT_SRC(port);
						}
					}


					/* test for delta
					 * delta limits are compared with the last sent by the TPDO.
					 * (the last communicated value)
					 */
					if ((triggerSelect & CO_401_ANIN_INT_DELTA) != 0u) {
						UNSIGNED32 delta = 0u;	/*default delta */
						coOdGetObj_u32(0x6426u + CO_401_PROFILE_OFFSET,
								port, &delta); 

						/* compares the current value with the last
						 * one transmitted via PDO.
						 * An interrupt is if the value exceeds the delta
						 * value compared with the last sent value.
						 */
						/*
						 * FIXME:
						 * Should be the evaluation disabled when delta == 0?
						 * Does a delta = 0 make sense?
						 */

						{
							static INTEGER16 aninlast[CO_401_NANIN16] = {{0}};
							if ( abs((int)aninlast[port - 1u] - (int)newValue) > delta) {
								CO_DEBUG1("--> detected delta trigger %d\n", port);
								intFlag = 1;
								CO_401_SET_ANIN_INT_SRC(port);
								/* store the last value used in the TPDO */
								aninlast[port - 1u] = newValue;
							}
						}
					}

					if (intFlag == 1) {
						// CO_DEBUG(" ==== send pdo ===\n");
						/* send TPDO object related */
						/* the third parameter 'flag' can be:
						 * MSG_OVERWRITE - if the last PDO is not transmitted yet,
						 * overwrite the last data with the new data
						 */
						retval = coPdoReqObj(0x6401u + CO_401_PROFILE_OFFSET, port, 0);
					}


				}
#endif	/* defined CO_PDO_TRANSMIT_CNT */

			} /* analog channel available */
		} /* for() all analog in channels */
	}
}


/***************************************************************************/
/**
* \brief - co401Task implements the CiA profile 401 functionality
*
* This function handles all process input ports (anin, digin)
* and should be called cyclically to poll the inputs.
* Typically, in non OS applications,
* it is called in the main application loop, as often as possible.
*
* This function calls the HW access functions for dig in and analog in.
* Take care how efficient, time consuming these functions are.
*
* A typical sequence
* \code
* 	while (1)  {
*			coCommTask();
*			co401Task();
*	}
* \endcode
*
*/

void co401Task(
		void	/* no parameter */
	)
{
#ifdef __linux__
static int counter = 0u;
 
	if (counter++ >= 100u)    /* thats only for test Linux, to reduce load */
#endif /* linux */
	{

		if (pDigIn != NULL) {
			/* if HW access is registered */
			profile401DigInTask();
		} /* we have a HW API function provided by the user */

		if (pAnIn != NULL) {
			profile401AnInTask();
		}
#ifdef __linux__
		counter = 0u;
#endif /* linux */
	} /* end if counter overflows */
}

/*
#define CO_401_NDIGIN8	8
#define CO_401_NANIN16	8
*/

/* vim: set ts=4 sw=4 spelllang=en : */
#endif /* CO_PROFILE_401 */
