/*
* CANopen Task relevant settings for FreeRTOS usage
* 
* This header is designed to include within gen_define.h .
*
*/
#ifndef CODRV_FREERTOS_H
#define CODRV_FREERTOS_H 1

#define CODRV_USE_FREERTOS 1

/* required header */
#include <cmsis_os.h>
#include <co_datatype.h>

/* required external variables */
/* semphCANopen is used to wake up the CANopen task */
extern SemaphoreHandle_t semphCANopen;
/* semphObjDir is used to Lock the object dictionary access */
extern SemaphoreHandle_t semphObjDir;

#define CO_OS_SIGNAL_CAN_TRANSMIT() \
		do { (void)xSemaphoreGiveFromISR(semphCANopen, NULL); } while(0)

#define CO_OS_SIGNAL_CAN_RECEIVE()  \
		do { (void)xSemaphoreGiveFromISR(semphCANopen, NULL); } while(0)

#define CO_OS_SIGNAL_TIMER() \
		do { (void)xSemaphoreGive(semphCANopen); } while(0)

#define CO_OS_SIGNAL_CAN_STATE() \
		do { (void)xSemaphoreGiveFromISR(semphCANopen, NULL); } while(0)

#define CO_OS_LOCK_OD() \
		do { (void)xSemaphoreTake(semphObjDir, portMAX_DELAY); } while(0)

#define CO_OS_UNLOCK_OD() \
		do { (void)xSemaphoreGive(semphObjDir); } while(0)

/* function declarations */
RET_T codrvOSConfig(void);

#endif /* CODRV_FREERTOS_H */
