/*
 * tsk_temp_ctrl.h
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */

#ifndef INC_TSK_TEMP_CTRL_H_
#define INC_TSK_TEMP_CTRL_H_


extern osThreadId tempTaskHandle;
void StartTempCtrlTask(void const * argument);
void PIDInit(int32_t tempReq, uint8_t i);


#define CREATE_TEMP_TASK(priority)		\
osThreadDef(tempTask, StartTempTask, priority, 0, 512); \
tempTaskHandle = osThreadCreate(osThread(tempTask), NULL)




#endif /* INC_TSK_TEMP_CTRL_H_ */
