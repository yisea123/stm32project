/*
* co_usdo.h - contains defines for usdo services
*
* Copyright (c) 2012-2016 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: co_usdo.h 17026 2017-02-28 15:36:33Z phi $
*
*-------------------------------------------------------------------
*
*
*
*/

/**
* \brief defines for sdo service
*
* \file co_sdo.h - contains defines for sdo service
*/

#ifndef CO_USDO_H
#define CO_USDO_H 1

#include <co_datatype.h>


/* datatypes */

/** \brief function pointer to USDO server event
 * \param execute - execute or test only
 * \param usdo client - nodeId of client
 * \param index - object index
 * \param subindex - object subindex
 * 
 * \return RET_T
 */
typedef	RET_T (* CO_EVENT_USDO_SERVER_T)(BOOL_T, UNSIGNED8, UNSIGNED16, UNSIGNED8); /*lint !e960 customer specific parameter names */


/* datatypes */

/** \brief function pointer to USDO server pre write event 
 * \param execute - execute or test only
 * \param usdo client - nodeId of client
 * \param index - object index
 * \param subindex - object subindex
 * 
 * \return RET_T
 */
typedef	RET_T (* CO_EVENT_USDO_SERVER_CHECK_WRITE_T)(UNSIGNED8, UNSIGNED16, UNSIGNED8, const UNSIGNED8 *); /*lint !e960 customer specific parameter names */


/** \brief function pointer to USDO server write domain event
 * \param index - object index
 * \param subindex - object subindex
 * \param domainBufSize - actual size at domain buffer
 * \param transferSize - actual transfered size
 * 
 * \return RET_T
 */
typedef RET_T (* CO_EVENT_USDO_SERVER_DOMAIN_WRITE_T)(UNSIGNED16, UNSIGNED8, UNSIGNED32, UNSIGNED32); /*lint !e960 customer specific parameter names */


/** \brief function pointer to USDO client write event
 * \param nodeId - nodeId of responding node
 * \param index - object index
 * \param subindex - object subindex
 * \param result - result of transfer
 * 
 * \return void
 */
typedef	void (* CO_EVENT_USDO_CLIENT_WRITE_T)(UNSIGNED8, UNSIGNED16, UNSIGNED8, UNSIGNED32); /*lint !e960 customer specific parameter names */

/* function prototypes */

EXTERN_DECL RET_T coUsdoServerInit(void);
EXTERN_DECL RET_T coUsdoClientInit(void);

EXTERN_DECL RET_T coEventRegister_USDO_SERVER_READ(CO_EVENT_USDO_SERVER_T pFunction);
EXTERN_DECL RET_T coEventRegister_USDO_SERVER_WRITE(CO_EVENT_USDO_SERVER_T pFunction);
EXTERN_DECL RET_T coEventRegister_USDO_SERVER_CHECK_WRITE(CO_EVENT_USDO_SERVER_CHECK_WRITE_T pFunction);
EXTERN_DECL RET_T coEventRegister_USDO_SERVER_DOMAIN_WRITE(CO_EVENT_USDO_SERVER_DOMAIN_WRITE_T pFunction);

EXTERN_DECL RET_T coEventRegister_USDO_CLIENT_WRITE(CO_EVENT_USDO_CLIENT_WRITE_T pFunction);


EXTERN_DECL RET_T coUsdoWrite(UNSIGNED8 nodeId, UNSIGNED16 index, UNSIGNED8 subIndex, CO_DATA_TYPE_T dataType, UNSIGNED32 dataLen, UNSIGNED16 numeric, UNSIGNED32 timeout);


#endif /* CO_USDO_H */

