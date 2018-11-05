/*
 * t_data_obj_rst100.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef T_UNIT_T_DATA_OBJ_RST100_H_
#define T_UNIT_T_DATA_OBJ_RST100_H_
#pragma pack(push)
#pragma pack(1)
typedef struct
{
	uint16_t 	resist[2];
	uint16_t 	voltage[2];
	float    	offset;
	float    	gain;
	uint16_t	caliState;
}RTS100;



#pragma pack(pop)

static const uint8_t cmAryAttributeDataTypes[] = { ST_U16, ST_U16, \
													ST_U16, ST_U16, \
													ST_FLOAT, ST_FLOAT,\
													ST_U16};

static const T_DATA_CLASS cgClassInitializer_RST100 =
{
	cmAryAttributeDataTypes,
	Check_DATAOBJ,  // no business rule
	Get_DATAOBJ,              // inherit
	Put_DATAOBJ,              // inherit
	GetAttributeDescription_DATAOBJ,   // overload
	GetObjectDescription_DATAOBJ // inherit
};


//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::RST100
extern const T_DATA_CLASS cgClassInitializer_RST100;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::RST100
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_RST100(w,y) {#w, (void *)(w),7,sizeof(RTS100),y,STRUCT, NULL, &cgClassInitializer_RST100 }





#endif /* T_UNIT_T_DATA_OBJ_RST100_H_ */
