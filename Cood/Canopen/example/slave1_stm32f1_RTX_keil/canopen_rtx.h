/*
* CANopen Task relevant settings for RTX usage
* 
* This header is designed to include within gen_define.h .
*
*/
#ifndef CANOPEN_RTX_H
#define CANOPEN_RTX_H 1

/* required header */
#include <cmsis_os.h>

/* signals for CAN and timer events */
#define SIGNAL_CANOPEN_CAN 0x01u
#define SIGNAL_CANOPEN_TIMER 0x02u

/* all signals are used to wakeup the canopen task */
extern osThreadId canopenTaskId;

#define CO_OS_SIGNAL_CAN_TRANSMIT() \
		do { (void)osSignalSet(canopenTaskId, SIGNAL_CANOPEN_CAN ); } while(0)
#define CO_OS_SIGNAL_CAN_RECEIVE()  \
		do { (void)osSignalSet(canopenTaskId, SIGNAL_CANOPEN_CAN ); } while(0)
#define CO_OS_SIGNAL_TIMER() \
		do { (void)osSignalSet(canopenTaskId, SIGNAL_CANOPEN_TIMER ); } while(0)

/* Mutex is used to Lock the object dictionary access */
extern osMutexId canopenOdLockId;

#define CO_OS_LOCK_OD() \
	do { (void)osMutexWait(canopenOdLockId, osWaitForever); } while(0)

#define CO_OS_UNLOCK_OD() \
	do { (void)osMutexRelease(canopenOdLockId); } while(0)



#endif /* CANOPEN_RTX_H */

