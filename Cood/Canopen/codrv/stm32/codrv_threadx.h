/*
* CANopen Task relevant settings for ThreadX usage
*
* This header is designed to include within gen_define.h .
*
*/
#ifndef CODRV_THREADX_H
#define CODRV_THREADX_H 1

#define CODRV_USE_THREADX 1

/* required header */
#include <co_datatype.h>
#include <tx_api.h>

/* required external variables */
/* mutexCANopen is used to wake up the CANopen task */
extern TX_MUTEX mutexCANopen;
/* mutexObjDir is used to Lock the object dictionary access */
extern TX_MUTEX mutexObjDir;

#define CO_OS_SIGNAL_CAN_TRANSMIT() \
		do { (void)tx_mutex_put(&mutexCANopen); } while(0)

#define CO_OS_SIGNAL_CAN_RECEIVE()  \
		do { (void)tx_mutex_put(&mutexCANopen); } while(0)

#define CO_OS_SIGNAL_TIMER() \
		do { (void)tx_mutex_put(&mutexCANopen); } while(0)

#define CO_OS_SIGNAL_CAN_STATE() \
		do { (void)tx_mutex_put(&mutexCANopen); } while(0)

#define CO_OS_LOCK_OD() \
		do { (void)tx_mutex_get(&mutexObjDir, TX_WAIT_FOREVER); } while(0)

#define CO_OS_UNLOCK_OD() \
		do { (void)tx_mutex_put(&mutexObjDir); } while(0)

/* function declarations */
RET_T codrvOSConfig(void);

#endif /* CODRV_THREADX_H */
