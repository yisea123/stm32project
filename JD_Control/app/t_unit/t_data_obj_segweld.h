/*
 * t_data_obj_segweld.h
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_SEGWELD_H_
#define T_UNIT_T_DATA_OBJ_SEGWELD_H_

typedef struct
{
	float weldSpeed;
	float currHigh;
	float currLow;
	uint16_t currHighMs;
	uint16_t currLowMs;
	uint16_t startAng;
	uint16_t endAng;
}SegWeld;

typedef struct
{
	float setValue;
	uint32_t deviceValue;
	uint32_t caliFlag;
}CaliPoints;




//----------------------------------------------------------------------------------------------------------
extern const T_DATA_CLASS cgClassInitializer_SegWeldData;

#define CONSTRUCT_STRUCT_SEGWELD(w,y) {#w, (void *)(w),7,sizeof(SegWeld),y,STRUCT, NULL, &cgClassInitializer_SegWeldData }

extern const T_DATA_CLASS cgClassInitializer_CaliPoint;
#define CONSTRUCT_STRUCT_CALIPOINT(w,y) {#w, (void *)(w),3,sizeof(CaliPoints),y,STRUCT, NULL, &cgClassInitializer_CaliPoint }

#endif /* T_UNIT_T_DATA_OBJ_SEGWELD_H_ */
