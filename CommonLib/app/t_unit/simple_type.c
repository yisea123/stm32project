
#include "simple_type.h"

//! byte-length for each simple-type
const uint8_t cgSimpleTypeLength[ST_NUMBER_OF_SIMPLE_TYPES]=
{
	SIZE_ST_NIL,
	SIZE_ST_U8,
	SIZE_ST_E8,
	SIZE_ST_BIT8,
	SIZE_ST_CHAR,
	SIZE_ST_U16,
	SIZE_ST_I16,
	SIZE_ST_WIDECHAR,
	SIZE_ST_BIT16,
	SIZE_ST_E16,
	SIZE_ST_U32,
	SIZE_ST_I32,
	SIZE_ST_T32,
	SIZE_ST_FLOAT,
	SIZE_ST_DOUBLE,
	SIZE_ST_I64,
	SIZE_ST_U64,
};

const uint8_t cgSimpleTypeString[ST_NUMBER_OF_SIMPLE_TYPES][10]=
{
	STR_ST_NIL,
	STR_ST_U8,
	STR_ST_E8,
	STR_ST_BIT8,
	STR_ST_CHAR,
	STR_ST_U16,
	STR_ST_I16,
	STR_ST_WIDECHAR,
	STR_ST_BIT16,
	STR_ST_E16,
	STR_ST_U32,
	STR_ST_I32,
	STR_ST_T32,
	STR_ST_FLOAT,
	STR_ST_DOUBLE,
	STR_ST_I64,
	STR_ST_U64,
};


