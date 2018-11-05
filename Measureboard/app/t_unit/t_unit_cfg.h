/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __T_UNIT_CFG_H
#define __T_UNIT_CFG_H
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "assert.h"
typedef void *		OS_RSEMA;

//os related configuration
#define OS_Use_Unit(x)		OS_Use(*x)
#define OS_Unuse_Unit(x)	OS_Unuse(*x)
#define OS_CREATERSEMA		OS_CreateSemaphore



void OS_Use(OS_RSEMA);
void OS_Unuse(OS_RSEMA);
OS_RSEMA OS_CreateSemaphore(void);


//eep related configuration
typedef enum
{
	SYNC_IM,//sync immediately
	SYNC_CYCLE,//sync in a buffer time, defined by eep interfaces
}EEP_SYNC;

uint16_t Trigger_EEPSaveInst(uint8_t* adr, uint16_t len, uint8_t sync,uint32_t line);
#define Trigger_EEPSave(x,y,z)		Trigger_EEPSaveInst(x,y,z,__LINE__)


#ifdef __cplusplus
}
#endif
#endif /*__ __T_UNIT_CFG_H */

/**
 * @}
 */

/**
 * @}
 */

