#include <stdint.h>
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_rcc.h"
#include "stm32f2xx_hal_gpio.h"

#include "gen_define.h"

#ifdef CO_USE_RTOS
#include <cmsis_os.h>
#include "canopen_rtx.h"
#include "colib_mutex.h"
osThreadId canopenTaskId;
osMutexId canopenOdLockId;

void canopen_task(void const * arg);
void idle_task(void const * arg);
void appl_task1(void const * arg);
void appl_task2(void const * arg);

/* Thread definition 
---------------------------------------------------------------------------*/
osThreadDef(canopen_task, osPriorityNormal, 1, 1024);
osThreadDef(idle_task, osPriorityIdle, 1, 120);
osThreadDef(appl_task1, osPriorityBelowNormal, 1, 1024);
osThreadDef(appl_task2, osPriorityAboveNormal, 1, 1024);
osMutexDef(canopenOdLock);
#endif

#include "co_canopen.h"
#include "cpu_stm32.h"

#ifdef BOOT
# include "fwupdate.h"
# include <string.h>
#endif


/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void my_exit(		int error	);

int main( void ) {
#ifdef CO_USE_RTOS
	  osThreadId id;
	  osStatus stat;
		stat = osKernelInitialize();
#endif
	  

  /* HW initialization */
	codrvHardwareInit();
	
#ifdef CO_USE_RTOS
  canopenTaskId = osThreadCreate(osThread(canopen_task), NULL);
	
  (void)osThreadCreate(osThread(idle_task), NULL);
	(void)osThreadCreate(osThread(appl_task1), NULL);
	(void)osThreadCreate(osThread(appl_task2), NULL);
	
	canopenOdLockId = osMutexCreate(osMutex(canopenOdLock));
	
	stat = osKernelStart();
	
	__enable_irq();
	
	// stop main task
	id = osThreadGetId();
	osThreadTerminate(id);
	while(1);
#endif 

    /* CANopen without RTOS */
	  if (codrvCanInit(250) != RET_OK)  {
      while(1);
	  }
	  if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
      while(1);
	  }

    if (coCanOpenStackInit(NULL) != RET_OK)  {
	    while(1);
	  }
		
		if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		  while(7);
		}
	

	  __enable_irq();
	
		
		if (codrvCanEnable() != RET_OK)  {
			while(1);
		}

    /* do CANopen task */
    while ( 1 ) {
    		coCommTask();
    }
}



#ifdef CO_USE_RTOS
/***************************************************************************/
void idle_task(void const * arg) 
{
	// easier to use than the idle deamon
	while(1) {
		int a = 4;
		a++;
		
	}
}

void appl_task1(void const * arg) {
	(void)arg; /* unused */
	
	while (1) {
		osDelay(200);
		coLibPdoReqNr(1,  MSG_OVERWRITE);
	}
}

void appl_task2(void const * arg) {
	(void)arg; /* unused */	
	
	while (1) {
		UNSIGNED8 emcyData[5] = { 1, 2, 3, 4, 5};
		osDelay(300);
		coLibEmcyWriteReq(0xffee, emcyData);
	}
}
	
/***************************************************************************/
/**
* \brief canopen task
*
* \param
*	unused
* \results
*	nothing
*/

void canopen_task(void const * arg) 
{
	(void)arg; /* unused */

	if (codrvCanInit(250) != RET_OK)  {
		my_exit(1);
	}
	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		my_exit(2);
	}

  if (coCanOpenStackInit(NULL) != RET_OK)  {
 		my_exit(1);
	}

 
	if (coEventRegister_SDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		my_exit(7);
	}
	
	if (codrvCanEnable() != RET_OK)  {
		my_exit(14);
	}


	/* canopen Task */
	while(1) {
		if (coQueueReceiveMessageAvailable() == CO_FALSE) {
			// do not sleep in case there are messages in the queue			 
			(void)osSignalWait(0, osWaitForever);//every signal
		}		
		
		coLibCommTask();		
	}
}
#endif

/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
#ifdef BOOT
UNSIGNED8 u8;
RET_T retVal;


	if((execute) && (index == 0x1f51) && (subIndex == 1)) {
		retVal = coOdGetObj_u8(0x1f51, 1, &u8);
		if((retVal == RET_OK) && (u8 == 0)) {
			
			codrvCanDisable();
			
			jump2Bootloader();
			
	} else  {
		return retVal;
		}
	}
#endif
	
	if (index == 0x2000) {
		 	memcpy( &cobl_command[0], COMMAND_BOOT, COMMAND_SIZE);
	}
	
	if (index == 0x2001) {
		coLibPdoReqNr(1, MSG_OVERWRITE);
	}
	if ((index == 0x3000) && (execute == CO_TRUE)) {
		CO_TIME_T t;
		t.days = 44;
		t.msec = 42;
		coLibTimeWriteReq(&t);
	}

	return(RET_OK);
}


/*********************************************************************/
static void my_exit(
		int error
	)
{
	
	while(1) {}
}


