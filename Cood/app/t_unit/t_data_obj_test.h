/*
 * t_data_obj_test.h
 *
 *  Created on: 2017年2月20日
 *      Author: pli
 */

#ifndef APP_T_UNIT_T_DATA_OBJ_TEST_H_
#define APP_T_UNIT_T_DATA_OBJ_TEST_H_


typedef struct _TEST_VAL
{
	uint8_t 	testU8;
	int8_t 		testE8;
	uint8_t 	testBIT8;
	int8_t 		testCHAR;//byte 4
	uint16_t 	testU16;
	int16_t 	testI16;//byte 8
	uint16_t 	testWC;
	uint16_t 	testBIT16;//byte 12
	int16_t 	testE16; //byte 14
	uint16_t 	testU16_1;//byte 16 for align
	uint32_t 	testU32;
	int32_t 	testI32;
	int32_t 	testT32;
	float    	testFloat;
	double   	testDouble;
	int64_t	 	testI64;
	uint64_t 	testU64;
} TEST_VAL __attribute__ ((aligned (1)));


//----------------------------------------------------------------------------------------------------------
//! initializer for ojects of class T_DATA_OBJ::TEST
extern const T_DATA_CLASS cgClassInitializer_TEST;


//--------------------------------------------------------------------------------------------------
/*!
 \brief  constructor; Initializer for objects of type simple T_DATA_OBJ::TEST
 \param  w ptrValue; the address will be casted to (void FAST*)!
 \param  y storageQualifier
*/
//--------------------------------------------------------------------------------------------------
#define CONSTRUCT_STRUCT_TEST(w,y) {#w, (void *)(w),17,sizeof(TEST_VAL),y,STRUCT, NULL, &cgClassInitializer_TEST }




#endif /* APP_T_UNIT_T_DATA_OBJ_TEST_H_ */
