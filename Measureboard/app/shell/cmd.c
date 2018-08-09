#include <stdlib.h>

#include "cmd.h"
#include "main.h"

#include "string.h"

#include "ntlibc.h"
#include <string.h>
#include "simple_type.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "unit_rtc_cfg.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Waggregate-return"
#pragma GCC diagnostic ignored "-Wformat="

enum
{
    CMD_PUT_OBJ, CMD_GET_OBJ, CMD_PUT_MEM, CMD_GET_MEM,CMD_GET_NAME,CMD_GET_ADR,
};

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

static OBJ_DATA objData;







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
    case ST_T32:
        {
            uint32_t val = 0;
            TimeCfg time;
            memcpy((void*) &val, (void*) data, 4);
            ConvertBack_U32Time(val,&time);
            //lint -e586
            snprintf(dataStr, MAX_LEN, "%d-> Time %d-%d-%d %d:%d:%d,0x%x", val, \
                time.year,time.month,time.date,time.hour,time.minute,time.second, val);
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

static void CastStringToData(const uint8_t* val, OBJ_DATA* data)
{
	float value1 = 0.0f;
    uint32_t value = (uint32_t) GetIValFromStr((const char*) val);
	switch (data->type)
	{
    case ST_NIL:
        break;
        
    case ST_U8:
    case ST_E8:
    case ST_BIT8:
    case ST_CHAR:
        *(uint8_t*) data->data = (uint8_t) value;
        break;
        
    case ST_U16:
    case ST_I16:
    case ST_BIT16:
    case ST_WIDECHAR:
    case ST_E16:
        *(uint16_t*) data->data = (uint16_t) value;
        break;
    case ST_U32:
    case ST_I32:
    case ST_T32:
        *(uint32_t*) data->data = (uint32_t) value;
        break;
    case ST_FLOAT:
        {
			value1 = GetFValFromStr((const char*) val);
			*(float*) data->data = value1;
        }
        break;
    case ST_DOUBLE:
		{
			value1 = GetFValFromStr((const char*) val);
			*(double*) data->data = value1;
        }
        break;
    case ST_I64:
    case ST_U64:
        {
            *(uint64_t*) data->data = (uint64_t) value;
        }
        break;
	}
    
}
static void cmd_get(int argc, char **argv, char* lines)
{
	uint16_t obj_id, sub_id;
    int16_t atr_id;
    
    if (argc < 4) // Print out t parameters
    {
        shell_AddRTC("\033[31merror, arguments! %s\n\033[0m", cmdlist[CMD_GET_OBJ].description);
        return;
	}
    
    sub_id = (uint16_t) GetIValFromStr(argv[1]);
    obj_id = (uint16_t) GetIValFromStr(argv[2]);
    atr_id = (int16_t) GetIValFromStr(argv[3]);
	if (sub_id < IDX_SUB_MAX)
	{
        objData.len = OBJ_DATA_LEN;
        uint16_t ret = GetObject(sub_id, obj_id, atr_id, &objData);
        //subSystem[sub_id].GET_T((uint16_t)obj_id,(uint16_t)atr_id,&val_set);
        
        if (ret == OK)
        {
            
            shell_AddRTC("OK--get %s,#%s&, arguments: %d, %d,@val=%s\n",
                         (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id,
                         CastDataToString(&objData));
        }
        else
        {
            shell_AddRTC("\033[31m ERROR: %d! get %s,#%s&, arguments: %d, %d,@val=0 \n\033[0m", ret,
                         (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id);
        }
	}
    else
    {
         shell_AddRTC("\033[31m Error: Illegal subsystems id,#%s&, arguments: %d, %d, %d,@val=0\n\033[0m", lines,sub_id, obj_id, atr_id);
    }
	return;
}

static void cmd_getname(int argc, char **argv, char* lines)
{
    uint16_t obj_id, sub_id;
    
	if (argc < 3) // Print out t parameters
	{
        shell_AddRTC("\033[31m error, arguments! %s\n\033[0m", cmdlist[CMD_GET_OBJ].description);
        return;
	}
    
    sub_id = (uint16_t) GetIValFromStr(argv[1]);
    obj_id = (uint16_t) GetIValFromStr(argv[2]);
    if (sub_id < IDX_SUB_MAX)
	{
        objData.len = OBJ_DATA_LEN;
        memset(objData.data,0,sizeof(objData.data));
        uint16_t ret = GetName(sub_id, obj_id, &objData);
        //subSystem[sub_id].GET_T((uint16_t)obj_id,(uint16_t)atr_id,&val_set);
        
        if (ret == OK)
        {
            
            shell_AddRTC("OK--getname %s,#%s&, arguments: %d,@val=%s\n",
                         (char*) subSystem[sub_id]->t_unit_name,lines ,obj_id, objData.data);
        }
		else
        {
            shell_AddRTC("\033[31m ERROR: %d! getname %s,#%s&, arguments: %d,@val=0  \n\033[0m", ret,
                         (char*) subSystem[sub_id]->t_unit_name,lines, obj_id);
        }
    }
    else
    {
         shell_AddRTC("\033[31m Error: Illegal subsystems id,#%s&, arguments: %d, %d,@val=0\n\033[0m", lines,sub_id, obj_id);
    }
    return;
}

static void cmd_getadr(int argc, char **argv, char* lines)
{
	uint16_t obj_id, sub_id;
    int16_t atr_id;
    
    if (argc < 4) // Print out t parameters
    {
		shell_AddRTC("\033[31m error, arguments! %s\n\033[0m", cmdlist[CMD_GET_OBJ].description);
        return;
	}
    
    sub_id = (uint16_t) GetIValFromStr(argv[1]);
	obj_id = (uint16_t) GetIValFromStr(argv[2]);
	uint16_t atr_id1 = (uint16_t) GetIValFromStr(argv[3]);
	atr_id = (atr_id1==0xFF)?WHOLE_OBJECT:(int16_t)atr_id1;
    if (sub_id < IDX_SUB_MAX)
    {
		objData.len = OBJ_DATA_LEN;
        uint16_t ret = GetAdrInfo(sub_id, obj_id, atr_id, (uint32_t*)objData.data, &objData.len);
		//subSystem[sub_id].GET_T((uint16_t)obj_id,(uint16_t)atr_id,&val_set);
        
        if (ret == OK)
        {
            
			shell_AddRTC("OK--getadr %s,#%s&, arguments: %d, %d,@val=%d, 0x%x: len: %d, 0x%x\n",\
                (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id,\
                    *(uint32_t*)objData.data,*(uint32_t*)objData.data,\
                        objData.len,objData.len);
        }
		else
		{
            shell_AddRTC("\033[31m ERROR: %d! get %s,#%s&, arguments: %d, %d,@val=0 \n\033[0m", ret,
                         (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id);
        }
    }
    else
    {
         shell_AddRTC("\033[31m Error: Illegal subsystems id,#%s&, arguments: %d, %d, %d,@val=0\n\033[0m", lines,sub_id, obj_id, atr_id);
    }
    return;
}


static void cmd_getmem(int argc, char **argv, char* lines)
{
	static uint8_t strData[100];
    uint32_t adr, len;
    if (argc < 3) // Print out t parameters
    {
        shell_AddRTC("\033[31m error, arguments! %s\n\033[0m", cmdlist[CMD_GET_MEM].description);
        return;
    }
    
    adr = (uint32_t) GetIValFromStr(argv[1]);
    len = (uint32_t) GetIValFromStr(argv[2]);
	if (CheckAdrRWStatus(adr, len, READ_ACCESS) == OK)
	{
        if (len > OBJ_DATA_LEN)
        {
            len = OBJ_DATA_LEN;
		}
        
        memset(strData, 0, sizeof(strData));
        
		for (uint16_t i = 0; i < len; i++)
        {
            //lint -e586
            snprintf((void*) &strData[i * 3], 4, "%02x ",
                     *(uint8_t*) (adr + i));
        }
        shell_AddRTC("OK--get mem,#%s&, arguments: %d, %d,@val=%s", lines, adr, len, strData);
	}
    else
    {
        shell_AddRTC("\033[31m ERROR! get mem,#%s&, arguments: %d, %d,@val=0 \n\033[0m",lines, adr, len);
	}
    return;
}

static void cmd_put(int argc, char **argv, char* lines)
{
    uint16_t obj_id, sub_id;
    int16_t atr_id;
    if (argc < 5) // Print out t parameters
    {
		shell_AddRTC("\033[31m error, arguments! %s\n\033[0m", cmdlist[CMD_PUT_OBJ].description);
		return;
	}
    
    sub_id = (uint16_t) GetIValFromStr(argv[1]);
	obj_id = (uint16_t) GetIValFromStr(argv[2]);
	atr_id = (int16_t) GetIValFromStr(argv[3]);
    //	val_set = (uint32_t)GetValFromStr(argv[4]);
    
    if (sub_id < IDX_SUB_MAX)
	{
        
        objData.len = OBJ_DATA_LEN;
        uint16_t ret = GetObjectType(sub_id, obj_id, atr_id, &objData.type);
        if (ret == OK)
        {
            CastStringToData((uint8_t*) argv[4], &objData);
            ret = PutObject(sub_id, obj_id, atr_id, objData.data);
            if (ret == OK)
            {
                shell_AddRTC("OK--set %s,#%s&,arguments: %d, %d, @val=%s\n",
                             (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id,
                             CastDataToString(&objData));
            }
            else if (ret == WARNING)
            {
                shell_AddRTC("Warning: %d! set %s,#%s&, arguments: %d, %d, %s,@val=0\n\033[0m", ret,
                             (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id,
                             CastDataToString(&objData));
            }
            else if (ret == DECLINE)
            {
                shell_AddRTC("Warning: need password first!\n");
            }
			else
            {
                shell_AddRTC("\033[31m ERROR: %d! set %s,#%s&, arguments: %d, %d, %s,@val=0\n\033[0m", ret,
                             (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id,
                             CastDataToString(&objData));
            }
        }
		else
		{
            shell_AddRTC("\033[31m Error: %d! Illegal,#%s&, arguments: %d, %d,@val=0\n\033[0m", ret,
                         (char*) subSystem[sub_id]->t_unit_name,lines, obj_id, atr_id);
        }
    }
    else
    {
         shell_AddRTC("\033[31m Error: Illegal subsystems id,#%s&, arguments: %d, %d, %d,@val=0\n\033[0m", lines,sub_id, obj_id, atr_id);
    }
	return;
}
static void cmd_putmem(int argc, char **argv, char* lines)
{
	uint32_t adr, len, byte;
    
    if (argc < 4) //
	{
        shell_AddRTC("\033[31m error, arguments! %s\n\033[0m", cmdlist[CMD_PUT_MEM].description);
        return;
    }
    
    adr = (uint32_t) GetIValFromStr(argv[1]);
    len = (uint32_t) GetIValFromStr(argv[2]);
    if (CheckAdrRWStatus(adr, len, WRITE_ACCESS) == OK)
    {
        if (len > OBJ_DATA_LEN)
        {
            len = OBJ_DATA_LEN;
        }
        
        for (uint16_t i = 0; i < len; i++)
        {
            byte = (uint32_t) GetIValFromStr(argv[3 + i]);
            *(uint8_t*) (adr + i) = (uint8_t) byte;
        }
        shell_AddRTC("OK--put mem,#%s&, arguments: %d, %d;", lines, adr, len);
    }
    else
    {
        shell_AddRTC("\033[31m ERROR! put mem,#%s&, arguments: %d, %d, \n\033[0m", lines, adr, len);
	}
    return;
}

const command_table_t cmdlist[] =
{
    //{ "i", "Show command information.", cmd_i },
    {"put", "put sub_id obj_id atr_id data ", cmd_put },
    {"get", "get sub_id obj_id atr_id ", cmd_get },
    {"putmem", "putmem adr len byte0 byte1 .... ", cmd_putmem },
    {"getmem", "getmem adr len ", cmd_getmem },
    {"getname", "getname sub_id, obj_id",cmd_getname},
    {"getadr", "getadr sub_id, obj_id atr_id",cmd_getadr},
    { NULL, NULL, NULL }
};

#pragma GCC diagnostic pop
