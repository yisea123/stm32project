/*
 * tsk_datalog.h
 *
 *  Created on: 2016��11��17��
 *      Author: pli
 */

#ifndef TSK_DATALOG_H_
#define TSK_DATALOG_H_


typedef enum
{
	DATALOG_IDLE,
	DATALOG_SAVE_TRIGGER,
	DATALOG_SAVE_FINISH,
	DATALOG_READ_TRIGGER,
	DATALOG_READ_FINISH,
	DATALOG_FINISH,
}DataLogState;



void StartDataLogTask(void const * argument);

typedef uint16_t (*DataLogProcess)(void);


#endif /* TSK_DATALOG_H_ */
