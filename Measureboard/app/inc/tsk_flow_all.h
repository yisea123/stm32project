/*
 * tsk_flow_all.h
 *
 *  Created on: 2016��12��1��
 *      Author: pli
 */

#ifndef INC_TSK_FLOW_ALL_H_
#define INC_TSK_FLOW_ALL_H_




#ifdef __cplusplus
 extern "C" {
#endif


 uint16_t MonitorMotorStatus(void);

void StartFlowTask(void const * argument) ;
TSK_STATUS GetFlowTskStatus(void);
void StartValveTask(void const * argument);
void StartMixTask(void const * argument);
void StartMotorTask(void const * argument);
void StartSubStepTask(void const * argument);
void StartPrintTask(void const * argument);
void StartPollTask(void const * argument);
void StartPollSchTask(void const * argument);
#ifdef __cplusplus
 }
#endif



#endif /* INC_TSK_FLOW_ALL_H_ */
