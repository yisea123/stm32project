/*
 * t_data_obj_pid.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_PID_H_
#define T_UNIT_T_DATA_OBJ_PID_H_
#pragma GCC diagnostic ignored "-Wpadded"

typedef struct _PID_VAL
{
	float Proportion; //�������� Proportional Const
	float Integral; //���ֳ��� Integral Const
	float Derivative; //΢�ֳ��� Derivative Const
	uint16_t timeDelay;
} PID_VAL __attribute__ ((aligned (2)));


#pragma GCC diagnostic pop

//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::PID
extern const T_DATA_CLASS cgClassInitializer_PID;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::PID
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_PID(w,y) {#w, (void *)(w),4,sizeof(PID_VAL),y,STRUCT, NULL, &cgClassInitializer_PID }






#endif /* T_UNIT_T_DATA_OBJ_PID_H_ */
