/*
 * t_data_obj_flowstep.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_FLOWSTEP_H_
#define T_UNIT_T_DATA_OBJ_FLOWSTEP_H_


#pragma pack(push)
#pragma pack(1)

typedef struct
{
	uint8_t dir;
	uint8_t mode;
	uint16_t pps;
	uint32_t steps;
}PumpConfig;

typedef struct{
	uint16_t valvStatus;
	PumpConfig pump1;//8
	PumpConfig pump2;//8
	PumpConfig pump3;//8
}StepConfig;
#pragma pack(pop)
//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::FLOWSTEP
extern const T_DATA_CLASS cgClassInitializer_FLOWSTEP;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::FLOWSTEP
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_FLOWSTEP(w,y) {#w, (void *)(w),13,sizeof(StepConfig),y,STRUCT, NULL, &cgClassInitializer_FLOWSTEP }




#endif /* T_UNIT_T_DATA_OBJ_FLOWSTEP_H_ */
