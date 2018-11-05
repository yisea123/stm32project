#include "gen_define.h"

#ifdef CO_USE_RTOS 
#include <co_canopen.h>
#include <colib_mutex.h>
#include <canopen_rtx.h>

void coLibCommTask(void)
{
	CO_OS_LOCK_OD();
	coCommTask();
	CO_OS_UNLOCK_OD();
}

RET_T coLibPdoReqNr(UNSIGNED16		pdoNr,	UNSIGNED8		flags) 
{
	RET_T ret;
	CO_OS_LOCK_OD();
	ret = coPdoReqNr(pdoNr, flags);
	CO_OS_UNLOCK_OD();
	
	return ret;
}

RET_T coLibEmcyWriteReq(UNSIGNED16	emcyErrCode, CO_CONST UNSIGNED8	pData[])
{
	RET_T ret;
	CO_OS_LOCK_OD();
	ret = coEmcyWriteReq(emcyErrCode,  pData);
	CO_OS_UNLOCK_OD();
	
	return ret;
}

RET_T coLibTimeWriteReq(const CO_TIME_T	*pTimeData)
{
	RET_T ret;
	CO_OS_LOCK_OD();
	ret = coTimeWriteReq(pTimeData);
	CO_OS_UNLOCK_OD();
	
	return ret;
}

#endif 

