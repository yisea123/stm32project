/*
 * t_data_obj_measdata.h
 *
 *  Created on: 2017骞�3鏈�13鏃�
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_MEASDATA_H_
#define T_UNIT_T_DATA_OBJ_MEASDATA_H_


#define FLAG_MEA_AUTORANGE_REQ 		0x0080
#define FLAG_MEA_MEAS				0x0100
#define FLAG_MEA_TRIG		   		0x0400
#define FLAG_MEA_ONLINE				0x0200
#define FLAG_MEA_STD0				0x0800
#define FLAG_MEA_STD1				0x1000
#define FLAG_CALI_STD0				0x2000
#define FLAG_CALI_STD1				0x4000
#define FLAG_CALI_STATUS_ER			0x8000
#define FLAG_CALI_STATUS_OK			0x0000
#define CALI_STATUS_MSK				0x8000
#define FLAG_RANGE_MSK				0x000F
#define FLAG_SHORT_SAT				0x0080
#define FLAG_LONG_SAT				0x0040
#define FLAG_BLANK_ENABLE 			0x0020
#define FLAG_RETRY_MAX    			0x0010
#define FLAG_POST_STD1    			0x0008
#define FLAG_MEAS_MSK				(uint16_t)(~(FLAG_CALI_STD0 | FLAG_CALI_STD1))
#define FLAG_CALI_MSK				(FLAG_CALI_STD0 | FLAG_CALI_STD1)


#pragma pack(push)
#pragma pack(1)

//128 bytes;
typedef struct
{
	uint32_t 	startTimeST;
	uint32_t	eventType;
	uint8_t		rev[6];
}EventData;

#pragma pack(pop)



#pragma pack(push)
#pragma pack(1)

//128 bytes;
typedef struct
{
	uint32_t 	startTimeST;
	float 		rawAbs[4];
	int16_t 	temperature[4];
	uint16_t 	rawFlag;//
}MeasDataRaw;

#pragma pack(pop)


typedef struct
{
	uint32_t 	startTimeST;
	float 		rawAbs[4];
	int16_t 	temperature[4];
}CaliDataRaw;
//total 7*4

typedef struct
{
	uint32_t 	startTimeST;			//the stored time;
	float		caliOffset[2];
	float		caliSlope[2];
	uint16_t	caliTimes[2];
	CaliDataRaw caliStd0;
	CaliDataRaw caliStd1;
	uint16_t 	caliRangeIdx;
	uint16_t	caliStatus;
}CaliDataSt;
//total 21 * 4

typedef struct
{
	uint32_t startTimeST;
	float rawAbs[4];
	float measValue;
	float caliSlope;
	float caliOffset;
	float adaptionFactor;
	float adaptionOffset;
	float dilutionFactor;
	int16_t temperature[4];
	uint16_t measRangeIdx;
	uint16_t measFlag;
	uint32_t caliTimeST;
}MeasDataSt;
//total 14*4
//15×4
//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::MeasData
extern const T_DATA_CLASS cgClassInitializer_MeasData;
extern const T_DATA_CLASS cgClassInitializer_CaliData;
extern const T_DATA_CLASS cgClassInitializer_MeasDataRaw;
extern const T_DATA_CLASS cgClassInitializer_EventData;
//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::MeasData
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_MEASDATA(w,y) 		{#w, (void *)(w),18,sizeof(MeasDataSt),y,STRUCT, NULL, &cgClassInitializer_MeasData }
#define CONSTRUCT_STRUCT_CALIDATA(w,y) 		{#w, (void *)(w),27,sizeof(CaliDataSt),y,STRUCT, NULL, &cgClassInitializer_CaliData }
#define CONSTRUCT_STRUCT_MEASDATARAW(w,y) 	{#w, (void *)(w),10,sizeof(MeasDataRaw),y,STRUCT, NULL, &cgClassInitializer_MeasDataRaw }
#define CONSTRUCT_STRUCT_EVENTDATA(w,y) 	{#w, (void *)(w),8,sizeof(EventData),y,STRUCT, NULL, &cgClassInitializer_EventData }


#endif /* T_UNIT_T_DATA_OBJ_MEASDATA_H_ */
