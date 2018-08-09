/*
 * t_data_obj_stepinfo.h
 *
 *  Created on: 2017Äê9ÔÂ13ÈÕ
 *      Author: pli
 */

#ifndef APP_T_UNIT_T_DATA_OBJ_STEPINFO_H_
#define APP_T_UNIT_T_DATA_OBJ_STEPINFO_H_

#pragma pack(push)
#pragma pack(1)
 typedef struct
 {
 	uint32_t 	step;
 	uint32_t 	startTime; //T32
 	uint32_t 	duringTime;
 	uint32_t 	remainTime;
 }FlowStepRun;
#pragma pack(pop)

//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::FlowStepRun
extern const T_DATA_CLASS cgClassInitializer_STEPINFO;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::FlowStepRun
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_STEPINFO(w,y) {#w, (void *)(w),4,sizeof(FlowStepRun),y,STRUCT, NULL, &cgClassInitializer_STEPINFO }



#endif /* APP_T_UNIT_T_DATA_OBJ_STEPINFO_H_ */
