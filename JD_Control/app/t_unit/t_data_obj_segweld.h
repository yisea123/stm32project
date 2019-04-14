/*
 * t_data_obj_segweld.h
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_SEGWELD_H_
#define T_UNIT_T_DATA_OBJ_SEGWELD_H_
#define MAX_CURRENT_OUTPUT		1000.0f


typedef struct
{
	float weldSpeed;
	float currHigh;
	float currLow;
	uint16_t currHighMs;
	uint16_t currLowMs;
	uint16_t startAng;
	uint16_t endAng;
	uint16_t state;
}SegWeld;

typedef struct
{
	float adValue;
	float actRead;
	uint32_t caliFlag;
}CaliVolt;


typedef struct
{
	float outValue;
	float actCurrent;
	uint32_t caliFlag;
}CaliCurrent;


typedef struct
{
	float outValue;
	float actSpeed;
	uint32_t caliFlag;
}CaliSpeed;


//----------------------------------------------------------------------------------------------------------
extern const T_DATA_CLASS cgClassInitializer_SegWeldData;
extern SegWeld* ptrCurrWeldSeg;
#define CONSTRUCT_STRUCT_SEGWELD(w,y) {#w, (void *)(w),7,sizeof(SegWeld),y,STRUCT, NULL, &cgClassInitializer_SegWeldData }

extern const T_DATA_CLASS cgClassInitializer_CaliPoint;
#define CONSTRUCT_STRUCT_CALIPOINT(w,y) {#w, (void *)(w),3,sizeof(CaliVolt),y,STRUCT, NULL, &cgClassInitializer_CaliPoint }

#endif /* T_UNIT_T_DATA_OBJ_SEGWELD_H_ */
