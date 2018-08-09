
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"
#include "diag/Trace.h"

#include "cmsis_os.h"
#include "bsp.h"

#include "shell_io.h"
#include "string.h"
#include "ntlibc.h"
#include "shell_io.h"
#include "main.h"


#include "co_odaccess.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Waggregate-return"

#define OBJ_DATA_LEN		50
typedef struct
{
	uint16_t 	type;
	uint16_t 	len;
	uint8_t		data[OBJ_DATA_LEN];
}OBJ_DATA;

static OBJ_DATA objData;

enum
{
	CMD_PUT_OBJ, CMD_GET_OBJ, CMD_PUT_MEM, CMD_GET_MEM,
};

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
#pragma GCC diagnostic pop



void cmd_i(void)
{
	const command_table_t *p = &cmdlist[0];

	shell_Add("Commands supported:\n");
	while (p->command != NULL)
	{
		shell_Add("%s\t- %s\n", p->command, p->description);
		p++;
	}
}

#if 1

static uint32_t GetIValFromStr(const char *nptr)
{
    uint32_t val_set = 0;
    val_set = (uint32_t) ntlibc_atoi(nptr);
    
    if (val_set == 0)
    {
        val_set = strtoul(nptr, NULL, 16);
	}
    return val_set;
}

static float GetFValFromStr(const char *nptr)
{
    double val_set = 0;
    if (nptr == NULL)
    {
        return (float)val_set;
    }
    if ((nptr[0] == 0) && (nptr[1] == 'x' || nptr[1] == 'X'))
    {
        val_set = strtoul(nptr, NULL, 16);
    }
	else
    {
        val_set = atof(nptr);
	}
    
    return (float) val_set;
}

static char* CastDataToString(OBJ_DATA* data1)
{
#define MAX_LEN	64
	static char dataStr[MAX_LEN + 2];
    uint8_t* data = data1->data;
    
	switch (data1->type)
    {
    case ST_NIL:
        memcpy(dataStr, "NULL", sizeof("NULL"));
        break;
        
    case ST_U8:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%02x", *(uint8_t*) data,
                 *(uint8_t*) data);
        break;
    case ST_E8:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%02x", *(uint8_t*) data,
                 *(uint8_t*) data);
        break;
    case ST_BIT8:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0X%02x", *(uint8_t*) data,
                 *(uint8_t*) data);
        break;
    case ST_CHAR:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%c,0x%02x", *(uint8_t*) data,
                 *(uint8_t*) data);
        break;
    case ST_U16:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%04x", *(uint16_t*) data,
                 *(uint16_t*) data);
        break;
    case ST_I16:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%04x", *(int16_t*) data,
                 *(int16_t*) data);
        break;
    case ST_WIDECHAR:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%04x", *(int16_t*) data,
                 *(int16_t*) data);
        break;
    case ST_BIT16:
        //lint -e586
        snprintf(dataStr, MAX_LEN, "%d,0x%04x", *(uint16_t*) data,
                 *(uint16_t*) data);
        break;
    case ST_E16:
        {
            uint16_t val = 0;
            memcpy((void*) &val, (void*) data, 2);
            
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%d,0x%04x", val,
                     val);
        }
        break;
    case ST_U32:
        {
            uint32_t val = 0;
            memcpy((void*) &val, (void*) data, 4);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%d,0x%x", val, val);
        }
        break;
    case ST_I32:
        {
            int32_t val = 0;
            memcpy((void*) &val, (void*) data, 4);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%d,0x%x", val, val);
        }
        break;

    case ST_FLOAT:
        {
            float val = 0;
            uint32_t val1 = 0;
            memcpy((void*) &val, (void*) data, 4);
            memcpy((void*) &val1, (void*) data, 4);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%.6f,0x%x", val, val1);
        }
        break;
    case ST_DOUBLE:
        //lint -e586
        {
            double val = 0;
            uint64_t val1 = 0;
            memcpy((void*) &val, (void*) data, 8);
            memcpy((void*) &val1, (void*) data, 8);
            memcpy((void*) &val, data, sizeof(double));
            snprintf(dataStr, MAX_LEN, "%.6f,0x%llx", val, val1);
        }
        break;
    case ST_I64:
        {
            int64_t val = 0;
            
            memcpy((void*) &val, (void*) data, 8);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%ld,0x%016llx", val, val);
        }
        break;
    case ST_U64:
		{
            int64_t val = 0;
            
            memcpy((void*) &val, (void*) data, 8);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%ld,0x%016llx", val, val);
            break;
        }
    }
    return dataStr;
    
}


static double CastStringToData(const uint8_t* val)
{
	 return GetFValFromStr((const char*) val);
}

static void CastToData(double value, void* data, uint8_t type)
{
	switch (type)
	{
		case ST_NIL:
			break;

		case ST_U8:
			*(uint8_t*) data = (uint8_t) value;
			break;

		case ST_U16:
			*(uint16_t*) data = (uint16_t) value;
			break;
		case ST_U32:
			*(uint32_t*) data = (uint32_t) value;
			break;
		case ST_FLOAT:
		{
			*(float*) data = value;
		}
			break;
	}

}

extern uint16_t dicObjType;
extern uint16_t dicAtrId;
extern uint16_t dicObjId;
extern uint32_t objValueU32;
extern uint32_t objValueR32;


uint16_t UpdateObjType(uint16_t obj_id, int16_t atr_id)
{
	const CO_OBJECT_DESC_T *pDesc;
	RET_T	retVal = FATAL_ERROR;
	retVal = coOdGetObjDescPtr(obj_id, atr_id,	&pDesc);
	if (retVal != RET_OK)
	{
		return FATAL_ERROR;
	}

	switch (pDesc->dType)
	{
		case CO_DTYPE_U32_CONST:
		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
		case CO_DTYPE_U32_SDO_SERVER:
		case CO_DTYPE_U32_SDO_CLIENT:
		case CO_DTYPE_U32_TPDO:
		case CO_DTYPE_U32_RPDO:
		case CO_DTYPE_U32_TMAP:
		case CO_DTYPE_U32_RMAP:
		case CO_DTYPE_U32_ERRCTRL:
		case CO_DTYPE_U32_EMCY:
		case CO_DTYPE_U32_SYNC:
		case CO_DTYPE_U32_STORE:
		case CO_DTYPE_U32_SRD:
		case CO_DTYPE_U32_NMT:
		case CO_DTYPE_U32_NETWORK:
		case CO_DTYPE_U32_SRDO:
			dicObjType = ST_U32;

			break;
		case CO_DTYPE_U16_CONST:
		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
		case CO_DTYPE_U16_TPDO:
		case CO_DTYPE_U16_RPDO:
		case CO_DTYPE_U16_ERRCTRL:
		case CO_DTYPE_U16_EMCY:
		case CO_DTYPE_U16_NMT:
		case CO_DTYPE_U16_NETWORK:
		case CO_DTYPE_U16_SRDO:
			dicObjType = ST_U16;
			break;
		case CO_DTYPE_BOOL_CONST:
		case CO_DTYPE_BOOL_VAR:
		case CO_DTYPE_U8_CONST:
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
		case CO_DTYPE_U8_SDO_SERVER:
		case CO_DTYPE_U8_SDO_CLIENT:
		case CO_DTYPE_U8_TPDO:
		case CO_DTYPE_U8_RPDO:
		case CO_DTYPE_U8_TMAP:
		case CO_DTYPE_U8_RMAP:
		case CO_DTYPE_U8_SYNC:
		case CO_DTYPE_U8_EMCY:
		case CO_DTYPE_U8_ERRCTRL:
		case CO_DTYPE_U8_NETWORK:
		case CO_DTYPE_U8_GFC:
		case CO_DTYPE_U8_SRDO:
			dicObjType = ST_U8;
			break;
		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
		case CO_DTYPE_I32_CONST:
			dicObjType = ST_I32;
			break;
		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
		case CO_DTYPE_I16_CONST:
			dicObjType = ST_I16;
			break;
		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_CONST:
		case CO_DTYPE_I8_PTR:
			dicObjType = ST_U8;
			break;
		case CO_DTYPE_R32_VAR:
		case CO_DTYPE_R32_PTR:
		case CO_DTYPE_R32_CONST:
			dicObjType = ST_FLOAT;
			break;
		default:
			dicObjType = ST_NIL;
			shell_printf(0, "Error type: ret:%d\n", retVal);
			break;
	}
	return retVal;
}

uint16_t GetObj(uint16_t obj_id, int16_t atr_id, uint16_t from, char* lines)
{
	const CO_OBJECT_DESC_T *pDesc;
	RET_T	retVal = FATAL_ERROR;
	uint32_t val = 0;
	uint16_t val1 = 0;
	uint8_t	val2 = 0;
	float valF = 0;
	retVal = coOdGetObjDescPtr(obj_id, atr_id,	&pDesc);
	if (retVal != RET_OK)  {
		shell_printf(0, 0, "\033[31m error! %s\n \033[0m",retVal);
		return FATAL_ERROR;
	}

	switch (pDesc->dType)  {
		case CO_DTYPE_U32_CONST:
		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
		case CO_DTYPE_U32_SDO_SERVER:
		case CO_DTYPE_U32_SDO_CLIENT:
		case CO_DTYPE_U32_TPDO:
		case CO_DTYPE_U32_RPDO:
		case CO_DTYPE_U32_TMAP:
		case CO_DTYPE_U32_RMAP:
		case CO_DTYPE_U32_ERRCTRL:
		case CO_DTYPE_U32_EMCY:
		case CO_DTYPE_U32_SYNC:
		case CO_DTYPE_U32_STORE:
		case CO_DTYPE_U32_SRD:
		case CO_DTYPE_U32_NMT:
		case CO_DTYPE_U32_NETWORK:
		case CO_DTYPE_U32_SRDO:
			retVal = coOdGetObj_u32(obj_id, atr_id, &val);
			if(from)
			{
				dicObjType = ST_U32;
				objValueU32 = val;
			}
			objData.type = ST_U32;
			memcpy(objData.data,&val,sizeof(val));
			break;
		case CO_DTYPE_U16_CONST:
		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
		case CO_DTYPE_U16_TPDO:
		case CO_DTYPE_U16_RPDO:
		case CO_DTYPE_U16_ERRCTRL:
		case CO_DTYPE_U16_EMCY:
		case CO_DTYPE_U16_NMT:
		case CO_DTYPE_U16_NETWORK:
		case CO_DTYPE_U16_SRDO:
			retVal = coOdGetObj_u16(obj_id, atr_id, &val1);
			if(from)
			{
				dicObjType = ST_U16;
				objValueU32 = val1;
			}

			objData.type = ST_U16;
			memcpy(objData.data,&val1,sizeof(val1));
			break;
		case CO_DTYPE_BOOL_CONST:
		case CO_DTYPE_BOOL_VAR:
		case CO_DTYPE_U8_CONST:
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
		case CO_DTYPE_U8_SDO_SERVER:
		case CO_DTYPE_U8_SDO_CLIENT:
		case CO_DTYPE_U8_TPDO:
		case CO_DTYPE_U8_RPDO:
		case CO_DTYPE_U8_TMAP:
		case CO_DTYPE_U8_RMAP:
		case CO_DTYPE_U8_SYNC:
		case CO_DTYPE_U8_EMCY:
		case CO_DTYPE_U8_ERRCTRL:
		case CO_DTYPE_U8_NETWORK:
		case CO_DTYPE_U8_GFC:
		case CO_DTYPE_U8_SRDO:
			retVal = coOdGetObj_u8(obj_id, atr_id, &val2);
			if(from)
			{
				dicObjType = ST_U8;
				objValueU32 = val2;
			}
			objData.type = ST_U8;
			memcpy(objData.data,&val2,sizeof(val2));
			break;
		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
		case CO_DTYPE_I32_CONST:
			retVal = coOdGetObj_i32(obj_id, atr_id, &val);
			if(from)
			{
				dicObjType = ST_I32;
				objValueU32 = val;
			}
			objData.type = ST_I32;
			memcpy(objData.data,&val,sizeof(val));
			break;
		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
		case CO_DTYPE_I16_CONST:
			retVal = coOdGetObj_i16(obj_id, atr_id, &val1);
			if(from)
			{
				dicObjType = ST_I16;
				objValueU32 = val1;
			}
			objData.type = ST_I16;
			memcpy(objData.data,&val1,sizeof(val1));
			break;
		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_CONST:
		case CO_DTYPE_I8_PTR:
			retVal = coOdGetObj_i8(obj_id, atr_id, &val2);
			if(from)
			{
				dicObjType = ST_U8;
				objValueU32 = val2;
			}
			objData.type = ST_I16;
			memcpy(objData.data,&val2,sizeof(val2));
			break;
		case CO_DTYPE_R32_VAR:
		case CO_DTYPE_R32_PTR:
		case CO_DTYPE_R32_CONST:
			retVal = coOdGetObj_r32(obj_id, atr_id, &valF);
			if(from)
			{
				dicObjType = ST_FLOAT;
				objValueR32 = valF;
			}
			objData.type = ST_FLOAT;
			memcpy(objData.data,&valF,sizeof(valF));
			break;
		default:
			shell_printf(0, "Error type: ret:%d\n", retVal);
	}
	if(from == 0)
	{
		if(retVal == OK)
			shell_printf(0, "OK--get %s,#%s&, arguments: %d, %d,@val=%s\n",
	                         "IO_canopen",lines, obj_id, atr_id,
	                         CastDataToString(&objData));
		else
			 shell_printf(0, "\033[31m ERROR: %d! get %s,#%s&, arguments: %d, %d,@val=0 \n\033[0m", retVal,
			                         (char*)"IO_canopen",lines, obj_id, atr_id);
	}
	return retVal;
}
static void cmd_get(int argc, char **argv, char* lines)
{
	uint16_t obj_id;
	int8_t atr_id;

	if (argc < 3) // Print out t parameters
	{
		shell_printf(0, "\033[31m error, arguments! %s\n \033[0m", cmdlist[CMD_GET_OBJ].description);
		return;
	}

	UpdateOutput();
	obj_id = (uint16_t) GetIValFromStr(argv[1]);
	atr_id = (int8_t) GetIValFromStr(argv[2]);
	GetObj(obj_id, atr_id, 0, lines);


	return;
}

uint16_t PutObj(uint16_t obj_id, int16_t atr_id, double val, char* lines)
{
	const CO_OBJECT_DESC_T *pDesc;
	RET_T	retVal;
	uint32_t val32 = 0;
	uint16_t val16 = 0;
	uint8_t	val8 = 0;
	float valF = 0;

	retVal = coOdGetObjDescPtr(obj_id, atr_id,	&pDesc);
	if (retVal != RET_OK)  {
		shell_printf(0, "\033[31m error! %s\n \033[0m",retVal);
		return FATAL_ERROR;
	}

	switch (pDesc->dType)  {
		case CO_DTYPE_U32_CONST:
		case CO_DTYPE_U32_VAR:
		case CO_DTYPE_U32_PTR:
		case CO_DTYPE_U32_SDO_SERVER:
		case CO_DTYPE_U32_SDO_CLIENT:
		case CO_DTYPE_U32_TPDO:
		case CO_DTYPE_U32_RPDO:
		case CO_DTYPE_U32_TMAP:
		case CO_DTYPE_U32_RMAP:
		case CO_DTYPE_U32_ERRCTRL:
		case CO_DTYPE_U32_EMCY:
		case CO_DTYPE_U32_SYNC:
		case CO_DTYPE_U32_STORE:
		case CO_DTYPE_U32_SRD:
		case CO_DTYPE_U32_NMT:
		case CO_DTYPE_U32_NETWORK:
		case CO_DTYPE_U32_SRDO:
			CastToData(val, (void*)&val32, ST_U32);
			retVal = coOdPutObj_u32(obj_id, atr_id, val32);
			objData.type = ST_U32;
			memcpy(objData.data,&val32,sizeof(val32));
			break;
		case CO_DTYPE_U16_CONST:
		case CO_DTYPE_U16_VAR:
		case CO_DTYPE_U16_PTR:
		case CO_DTYPE_U16_TPDO:
		case CO_DTYPE_U16_RPDO:
		case CO_DTYPE_U16_ERRCTRL:
		case CO_DTYPE_U16_EMCY:
		case CO_DTYPE_U16_NMT:
		case CO_DTYPE_U16_NETWORK:
		case CO_DTYPE_U16_SRDO:
			CastToData(val, (void*)&val16, ST_U16);
			retVal = coOdPutObj_u16(obj_id, atr_id, val16);
			objData.type = ST_U16;
			memcpy(objData.data,&val16,sizeof(val16));
			break;
		case CO_DTYPE_BOOL_CONST:
		case CO_DTYPE_BOOL_VAR:
		case CO_DTYPE_U8_CONST:
		case CO_DTYPE_U8_VAR:
		case CO_DTYPE_U8_PTR:
		case CO_DTYPE_U8_SDO_SERVER:
		case CO_DTYPE_U8_SDO_CLIENT:
		case CO_DTYPE_U8_TPDO:
		case CO_DTYPE_U8_RPDO:
		case CO_DTYPE_U8_TMAP:
		case CO_DTYPE_U8_RMAP:
		case CO_DTYPE_U8_SYNC:
		case CO_DTYPE_U8_EMCY:
		case CO_DTYPE_U8_ERRCTRL:
		case CO_DTYPE_U8_NETWORK:
		case CO_DTYPE_U8_GFC:
		case CO_DTYPE_U8_SRDO:
			CastToData(val,(void*)&val8, ST_U8);
			retVal = coOdPutObj_u8(obj_id, atr_id, val8);
			objData.type = ST_U8;
			memcpy(objData.data,&val8,sizeof(val8));
			break;
		case CO_DTYPE_I32_VAR:
		case CO_DTYPE_I32_PTR:
		case CO_DTYPE_I32_CONST:
			CastToData(val,(void*)&val32, ST_U32);
			retVal = coOdPutObj_i32(obj_id, atr_id, val32);
			objData.type = ST_U32;
			memcpy(objData.data,&val32,sizeof(val32));
			break;
		case CO_DTYPE_I16_VAR:
		case CO_DTYPE_I16_PTR:
		case CO_DTYPE_I16_CONST:
			CastToData(val, (void*)&val16, ST_U16);
			retVal = coOdPutObj_i16(obj_id, atr_id, val16);
			objData.type = ST_U16;
			memcpy(objData.data,&val16,sizeof(val16));
			break;
		case CO_DTYPE_I8_VAR:
		case CO_DTYPE_I8_CONST:
		case CO_DTYPE_I8_PTR:
			CastToData(val, (void*)&val8, ST_U8);
			retVal = coOdPutObj_i8(obj_id, atr_id, val8);
			objData.type = ST_U8;
			memcpy(objData.data,&val8,sizeof(val8));
			break;
		case CO_DTYPE_R32_VAR:
		case CO_DTYPE_R32_PTR:
		case CO_DTYPE_R32_CONST:
			CastToData(val, (void*)&valF, ST_FLOAT);
			retVal = coOdPutObj_r32(obj_id, atr_id, valF);
			objData.type = ST_FLOAT;
			memcpy(objData.data,&valF,sizeof(valF));
			break;
		default:
			shell_printf(0, "Error type: ret:%d\n", retVal);
	}
	extern void sdoWrite(uint16_t,uint16_t);
	sdoWrite(obj_id, atr_id);
	if (retVal == OK)
    {
        shell_printf(0, "OK--set %s,#%s&,arguments: %d, %d, @val=%s\n",
        			"IO_canopen",lines, obj_id, atr_id,
                     CastDataToString(&objData));
    }
    else if (retVal == WARNING)
    {
        shell_printf(0, "Warning: %d! set %s,#%s&, arguments: %d, %d, %s,@val=0\n\033[0m", retVal,
        			"IO_canopen",lines, obj_id, atr_id,
                     CastDataToString(&objData));
    }
    else if (retVal == DECLINE)
    {
        shell_printf(0, "Warning: need password first!\n");
    }
	else
    {
        shell_printf(0, "\033[31m ERROR: %d! set %s,#%s&, arguments: %d, %d, %s,@val=0\n\033[0m", retVal,
        			"IO_canopen",lines, obj_id, atr_id,
                     CastDataToString(&objData));
    }
	return retVal;
}

static void cmd_put(int argc, char **argv, char* lines)
{
	uint16_t obj_id;
	int16_t atr_id;
	uint16_t ret;
	if (argc < 4) // Print out t parameters
	{
		shell_printf(0, "\033[31m error, arguments! %s\n \033[0m", cmdlist[CMD_PUT_OBJ].description);
		return;
	}
	double val = CastStringToData((uint8_t*) argv[3]);
	obj_id = (uint16_t) GetIValFromStr(argv[1]);
	atr_id = (int16_t) GetIValFromStr(argv[2]);
	ret = PutObj(obj_id,atr_id,val,lines);

	return;
}

#endif


static void cmd_getmem(int argc, char **argv, char* lines)
{
	static uint8_t strData[100];
	uint32_t adr, len;
	if (argc < 3) // Print out t parameters
	{
		shell_printf(0, "\033[31m error, arguments! %s\n \033[0m", cmdlist[CMD_GET_MEM].description);
		return;
	}

	adr = (uint32_t) GetIValFromStr(argv[1]);
	len = (uint32_t) GetIValFromStr(argv[2]);
	if (CheckAdrRWStatus(adr, len, READ_ACCESS) == OK)
	{
		if (len > 32)
		{
			len = 32;
		}

		memset(strData, 0, sizeof(strData));

		for (uint16_t i = 0; i < len; i++)
		{
			//lint -e586
			snprintf((void*) &strData[i * 3], 4, "%02x ",
					*(uint8_t*) (adr + i));
		}
		shell_printf(0, "OK--get mem,#%s&, arguments: %d, %d,@val=%s", lines, adr, len, strData);
	}
	else
	{
		shell_printf(0, "\033[31m ERROR! get mem,#%s&, arguments: %d, %d,@val=0 \n\033[0m",lines, adr, len);
	}
	return;
}



static void cmd_putmem(int argc, char **argv,char* lines)
{
	uint32_t adr, len, byte;

	if (argc < 4) //
	{
		shell_printf(0, "\033[31m error, arguments! %s\n \033[0m", cmdlist[CMD_PUT_MEM].description);
		return;
	}

	adr = (uint32_t) GetIValFromStr(argv[1]);
	len = (uint32_t) GetIValFromStr(argv[2]);
	if (CheckAdrRWStatus(adr, len, WRITE_ACCESS) == OK)
	{
		if (len > 32)
		{
			len = 32;
		}

		for (uint16_t i = 0; i < len; i++)
		{
			byte = (uint32_t) GetIValFromStr(argv[3 + i]);
			*(uint8_t*) (adr + i) = (uint8_t) byte;
		}
		shell_printf(0, "OK--put mem,#%s&, arguments: %d, %d;", lines, adr, len);
	}
	else
	{
		shell_printf(0, "\033[31m ERROR! put mem,#%s&, arguments: %d, %d, \n\033[0m", lines, adr, len);
	}
	return;
}



const command_table_t cmdlist[] =
{
	{ "put", "put obj_id atr_id data ", cmd_put },
	{ "get", "get obj_id atr_id ", cmd_get },
	{ "putmem", "putmem adr len byte0 byte1 .... ", cmd_putmem },
	{ "getmem", "getmem adr len ", cmd_getmem },
	{ NULL, NULL, NULL }
};

#pragma GCC diagnostic pop
