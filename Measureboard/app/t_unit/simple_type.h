/*
 * simple_type.h
 *
 *  Created on: 2016��8��18��
 *      Author: pli
 */

#ifndef SRC_SIMPLE_TYPE_H_
#define SRC_SIMPLE_TYPE_H_


#include <stdio.h>
#include "t_unit_cfg.h"
#include "assert.h"

#pragma GCC diagnostic ignored "-Wpadded"
//----------------------------------------------------------------------------------------------------------
//! code-tab for simple datatypes
enum E_SIMPLE_TYPES
{
    ST_NIL, // no data eg. actions
    #define SIZE_ST_NIL   (0)
	#define STR_ST_NIL   	"NULL"
    ST_U8, // uint8_t with defined range
    #define SIZE_ST_U8    sizeof(uint8_t)
	#define STR_ST_U8   	"U8"

    ST_E8, // enumeration uint8_t
    #define SIZE_ST_E8    sizeof(uint8_t)
	#define STR_ST_E8   	"E8"

    ST_BIT8, // uint8_t bit-field with a mask that defined which bits are allowed to be 1
    #define SIZE_ST_BIT8  sizeof(uint8_t)
	#define STR_ST_BIT8   	"BIT8"

    ST_CHAR, // char with defined range
    #define SIZE_ST_CHAR  sizeof(char)
	#define STR_ST_CHAR   	"CHAR"

	ST_NUMBER_OF_U8 = ST_CHAR,

	ST_U16, // uint16_t with defined range
    #define SIZE_ST_U16   sizeof(uint16_t)
	#define STR_ST_U16   	"U16"

    ST_I16, // int16_t with defined range
    #define SIZE_ST_I16   sizeof(int16_t)
	#define STR_ST_I16   	"I16"

    ST_WIDECHAR, // TWIDECHAR with defined range
    #define SIZE_ST_WIDECHAR  sizeof(uint16_t)
	#define STR_ST_WIDECHAR   	"WC16"

	ST_BIT16, // TWIDECHAR with defined range
	#define SIZE_ST_BIT16  		sizeof(uint16_t)
	#define STR_ST_BIT16   		"BIT16"
    // fill in new simple types
    // consider the RANGE-type and the corresponding length-tab

    ST_E16,
    #define SIZE_ST_E16 sizeof(uint16_t)
	#define STR_ST_E16   	"E16"
	ST_NUMBER_OF_U16 = ST_E16,

	ST_U32, // uint32_t with defined range
    #define SIZE_ST_U32   sizeof(uint32_t)
	#define STR_ST_U32   	"U32"

	ST_I32, // int32_t with defined range
    #define SIZE_ST_I32   sizeof(int32_t)
	#define STR_ST_I32   	"I32"

	ST_T32, // uint32_t with defined range
    #define SIZE_ST_T32   sizeof(uint32_t)
	#define STR_ST_T32   	"T32"

	ST_NUMBER_OF_INT = ST_T32,

    ST_FLOAT, // float with defined range
    #define SIZE_ST_FLOAT sizeof(float)
	#define STR_ST_FLOAT   	"FLOAT"

    ST_DOUBLE, // double with defined range
    #define SIZE_ST_DOUBLE  sizeof(double)
	#define STR_ST_DOUBLE   	"DOUBLE"

    ST_I64, // long int with defined range
    #define SIZE_ST_I64  sizeof(int64_t)
	#define STR_ST_I64   	"I64"

    ST_U64, // long int with defined range
    #define SIZE_ST_U64  sizeof(uint64_t)
	#define STR_ST_U64   	"U64"
	// must be the last enum
    ST_NUMBER_OF_SIMPLE_TYPES
};



//----------------------------------------------------------------------------------------------------------
//! Range-Info ST_U8
typedef struct _T_RANGE_U8
{
    uint8_t min;
    uint8_t max;
    uint8_t increment;
} T_RANGE_U8;

//! range-info ST_E8
typedef struct _T_RANGE_E8
{
    const uint8_t * ptrCodeTab;
    uint8_t numberOfCodes;
} T_RANGE_E8;

//! range-info ST_E16
typedef struct _T_RANGE_E16
{
    const uint16_t * ptrCodeTab;
    uint16_t numberOfCodes;
} T_RANGE_E16;

//! range-info ST_U16
typedef struct _T_RANGE_U16
{
    uint16_t min;
    uint16_t max;
    uint16_t increment;
} T_RANGE_U16;

//! range-info ST_I16
typedef struct _T_RANGE_I16
{
    int16_t min;
    int16_t max;
    int16_t increment;
} T_RANGE_I16;

//! range-info ST_U32
typedef struct _T_RANGE_U32
{
    uint32_t min;
    uint32_t max;
    uint32_t increment;
} T_RANGE_U32;

//! range-info ST_FLOAT
typedef struct _T_RANGE_FLOAT
{
    float min;
    float max;
    float increment;  // e.g. 0.01
} T_RANGE_FLOAT;

//! range-info ST_DOUBLE
typedef struct _T_RANGE_DOUBLE
{
    double min;
    double max;
    double increment; // e.g. 0.01
} T_RANGE_DOUBLE;

//! range-info ST_BIT8
typedef uint8_t T_RANGE_BIT8;



//----------------------------------------------------------------------------------------------------------
/*! container for range-data-types
    usually the range-information is used by algorithm
    that handle more than one data-type, so the union
    guarantee that enough memory will be allocated for all
    possible range-informations
*/
typedef union _U_RANGE_CONTAINER
{
    T_RANGE_U8     u8;
    T_RANGE_E8     e8;
    T_RANGE_U16    u16;
    T_RANGE_E16    e16;
    T_RANGE_I16    i16;
    T_RANGE_U32    u32;
    T_RANGE_FLOAT  flt;
    T_RANGE_DOUBLE dbl;
    T_RANGE_BIT8   bit8;
} T_RANGE_CONTAINER;


//! byte-length for each simple-type
extern const uint8_t cgSimpleTypeLength[ST_NUMBER_OF_SIMPLE_TYPES];

#define VIP_ASSERT	assert
#pragma GCC diagnostic pop

#endif /* SRC_SIMPLE_TYPE_H_ */
