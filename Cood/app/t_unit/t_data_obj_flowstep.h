/*
 * t_data_obj_flowstep.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_FLOWSTEP_H_
#define T_UNIT_T_DATA_OBJ_FLOWSTEP_H_



typedef struct
{
	uint8_t dir;
	uint8_t speed;
	uint16_t steps;
    uint16_t preDefineTime;
    uint16_t preDefineSteps;
    uint16_t errorAction;
}PumpCfg;


typedef struct
{
	PumpCfg cfg;
    uint16_t delay;
}PumpCfgEx;
//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::FLOWSTEP
extern const T_DATA_CLASS cgClassInitializer_PumpCfg;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::FLOWSTEP
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_PUMPCFG(w,y) {#w, (void *)(w),6,sizeof(PumpCfg),y,STRUCT, NULL, &cgClassInitializer_PumpCfg }




#endif /* T_UNIT_T_DATA_OBJ_FLOWSTEP_H_ */
