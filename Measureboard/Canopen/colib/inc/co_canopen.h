/*
* co_canopen.h - contains defines for all services
*
* Copyright (c) 2012-2017 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: co_canopen.h 20165 2017-10-30 13:55:53Z boe $
*-------------------------------------------------------------------
*
*
*/

/**
* \brief defines for all services
*
* \file co_canopen.h - contains defines for all services
*
* This header inludes defines for all services of the CANopen library.
* It can be included instead of header files of each service.
*
*/

#ifndef CO_CANOPEN_H
#define CO_CANOPEN_H 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <co_datatype.h>
#include <co_cob.h>
#include <co_commtask.h>
#include <co_drv.h>
#include <co_emcy.h>
#include <co_cfgman.h>
#include <co_led.h>
#include <co_lss.h>
#include <co_nmt.h>
#include <co_odaccess.h>
#include <co_odindex.h>
#include <co_pdo.h>
#include <co_sdo.h>
#include <co_store.h>
#include <co_sleep.h>
#include <co_sync.h>
#include <co_time.h>
#include <co_timer.h>
#include <co_edsparse.h>
#ifdef CO_NETWORK_ROUTING_CNT
# include <co_network.h>
#endif /* CO_NETWORK_ROUTING_CNT */
#ifdef CO_SRD_SUPPORTED
# include <co_srd.h>
#endif /* CO_SRD_SUPPORTED */
#ifdef CO_FLYING_MASTER_SUPPORTED
# include <co_flyingmaster.h>
#endif /* CO_FLYING_MASTER_SUPPORTED */
#ifdef CO_BOOTUP_MANAGER
# include <co_manager.h>
#endif /* CO_BOOTUP_MANAGER */
#ifdef CO_USER_EXTENSION_SUPPORTED
# include <co_user.h>
#endif /* CO_USER_EXTENSION_SUPPORTED */
#ifdef CO_GFC_SUPPORTED
# include <co_gfc.h>
#endif /* CO_GFC_SUPPORTED */
#ifdef CO_SRDO_SUPPORTED
# include <co_srdo.h>
#endif /* CO_SRDO_SUPPORTED */
#ifdef CO_USDO_SUPPORTED
# include <co_usdo.h>
#endif /* CO_USDO_SUPPORTED */



/* actual CANopen stack software version */
#define CO_STACK_VERSION	0x20703ul


typedef struct {
	UNSIGNED8	master;		/**< device works as master */
} CO_INIT_OPTION_T;



/**
* line parameter definition 
*
* defines number of line parameter for services 
*/
typedef struct  {
        UNSIGNED16		objectCnt[1];
        UNSIGNED16		cobCnt[1];
        UNSIGNED16		pdoTrCnt[1];
        UNSIGNED16		pdoRecCnt[1];
        UNSIGNED8		sdoServerCnt[1];
        UNSIGNED8		sdoClientCnt[1];
        UNSIGNED8		errorCtrlCnt[1];
        UNSIGNED8		emcyErrHistCnt[1];
        UNSIGNED8		emcyConsCnt[1];
#ifdef CO_NETWORK_ROUTING_CNT
        UNSIGNED8		networkCnt[1];
#endif /* CO_NETWORK_ROUTING_CNT */
		UNSIGNED8		slaveAssign[1];
		UNSIGNED8		guardingCnt[1];
        UNSIGNED8		nodeId[1];
        CO_NODE_ID_T	nodeIdFct[1];
} CO_SERVICE_INIT_VAL_T;


/* function prototypes */

/**
* \brief coCanOpenStackInit - init of CANopen stack 
*
* This function is normally generated by the CANopen Device Designer
* and responsible for the whole intialization of the CANopen stack.
*
* \param  pLoadFunction
*			pointer to loadFunction
*
* \return RET_T
*/
EXTERN_DECL RET_T coCanOpenStackInit(CO_EVENT_STORE_T pLoadFunction);

/**
* \brief coCanOpenStackInit - init of CANopen stack 
* This function is normally generated by the CANopen Device Designer
* and responsible for the intialization of the CANopen stack.
* In addition to coCanOpenStackInit some options for services can be added.
*
* \param  pLoadFunction
*			pointer to loadFunction
* \param  pCoOptions
*			pointer to coOptions
*
*
* \return RET_T
*/
EXTERN_DECL RET_T coCanOpenStackInitPara(CO_EVENT_STORE_T pLoadFunction,
		CO_INIT_OPTION_T *pCoOptions);
 
/**
* \brief coCanOpenStackInit_common - init of common part of CANopen stack 
* This function is generated by the CANopen Device Designer
* and responsible for the common intialization of the CANopen stack.
* Normally called from coCanOpenStackInit();
*
* \param  pLoadFunction
*			pointer to loadFunction
*
* \return RET_T
*/
RET_T coCanOpenStackInit_common(CO_EVENT_STORE_T pLoadFunction);

/**
* \brief coCanOpenStackInit_line - init one CAN line of CANopen stack 
* This function is generated by the CANopen Device Designer
* and responsible for the line depending intialization of the CANopen stack.
* Normally called from coCanOpenStackInit();
*
* \param  pCoOptions
*			pointer to coOptions
*
* \return RET_T
*/
RET_T coCanOpenStackInit_line(CO_INIT_OPTION_T *pCoOptions);


/**
* \brief coCanOpenStackDeInit - deinit of CANopen stack 
*
* This function is normally generated by the CANopen Device Designer
* and responsible for the de-intialization of the CANopen stack.
*
* \return void
*/
EXTERN_DECL void coCanOpenStackDeInit(void);


EXTERN_DECL void coCanOpenStackVarInit(CO_SERVICE_INIT_VAL_T *pServiceInitVals);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CO_CANOPEN_H */
