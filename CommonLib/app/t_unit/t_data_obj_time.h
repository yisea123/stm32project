/*
 * t_data_obj_time.h
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_TIME_H_
#define T_UNIT_T_DATA_OBJ_TIME_H_

enum
{
	UNIT_SECOND,
	UNIT_MINUTES,
	UNIT_HOUR,
	UNIT_DAY,
	UINT_WEEK,
	UNIT_MONTH,
	UNIT_YEAR,
};


enum
{
	ATR_ID_YEAR,
	ATR_ID_MONTH,
	ATR_ID_DAY,
	ATR_ID_HOUR,
	ATR_ID_MINUTES,
	ATR_ID_SECOND,
	ATR_ID_ALL,
};
#pragma pack(push)
#pragma pack(1)


typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t date;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}TimeCfg;
#pragma pack(pop)
//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::TIME
extern const T_DATA_CLASS cgClassInitializer_TIME;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::TIME
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_TIME(w,y) {#w, (void *)(w),6,sizeof(TimeCfg),y,STRUCT, NULL, &cgClassInitializer_TIME }

uint16_t   Check_TIME(const struct _T_DATA_OBJ *me,
                         void * ptrValue,
                         int16_t attributeIndex );



#endif /* T_UNIT_T_DATA_OBJ_TIME_H_ */
