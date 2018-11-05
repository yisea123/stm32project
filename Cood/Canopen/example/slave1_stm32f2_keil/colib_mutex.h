#ifndef COLIB_MUTEX_H
#define COLIB_MUTEX_H 1

#include <co_canopen.h>


void coLibCommTask(void);
RET_T coLibPdoReqNr(UNSIGNED16		pdoNr,	UNSIGNED8		flags);
RET_T coLibEmcyWriteReq(UNSIGNED16	emcyErrCode, CO_CONST UNSIGNED8	pData[]);
RET_T coLibTimeWriteReq(const CO_TIME_T	*pTimeData);

#endif /* COLIB_MUTEX_H */

