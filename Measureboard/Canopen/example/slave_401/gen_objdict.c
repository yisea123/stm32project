/*
 * object dictionary for 401 - generated by CANopen DeviceDesigner 2.7.0
 * Mo Mai 8 11:52:44 2017
 */
#include <stdlib.h>

#include <gen_define.h>
#include <co_canopen.h>
#include <gen_indication.h>

#if CO_STACK_VERSION < 0x10600ul
typedef PDO_MAP_TABLE_T PDO_TR_MAP_TABLE_T;
typedef PDO_MAP_TABLE_T PDO_REC_MAP_TABLE_T;
#endif
#ifndef CO_ATTR_MAP_REC
#define CO_ATTR_MAP_REC CO_ATTR_MAP
#endif
#ifndef CO_ATTR_MAP_TR
#define CO_ATTR_MAP_TR CO_ATTR_MAP
#endif

/* ensure compability with older stacks */
#ifndef CO_ATTR_STORE
#define CO_ATTR_STORE 0
#endif

/* definition of memory specifier */
#ifndef CO_CONST
#define CO_CONST const
#endif

/* definition of compiler-specific storage classes */
#ifndef CO_STORAGE_CLASS
#define CO_STORAGE_CLASS
#endif
#ifndef CO_CONST_STORAGE_CLASS
#define CO_CONST_STORAGE_CLASS
#endif


/* definition of static indication function pointers */

/* number of objects */
#define CO_OD_ASSIGN_CNT 45u
#define CO_OBJ_DESC_CNT 156u

/* definition of managed variables */
static UNSIGNED8 CO_STORAGE_CLASS	od_u8[46]= {
	(UNSIGNED8)0u /* 0x1001:0 */,
	(UNSIGNED8)0u /* 0x6200:1 */,
	(UNSIGNED8)0u /* 0x6202:1 */,
	(UNSIGNED8)0u /* 0x6200:2 */,
	(UNSIGNED8)0u /* 0x6207:1 */,
	(UNSIGNED8)255u /* 0x6206:1 */,
	(UNSIGNED8)255u /* 0x6206:2 */,
	(UNSIGNED8)0u /* 0x6207:2 */,
	(UNSIGNED8)1u /* 0x6443:1 */,
	(UNSIGNED8)255u /* 0x6006:1 */,
	(UNSIGNED8)0u /* 0x6007:1 */,
	(UNSIGNED8)0u /* 0x6008:1 */,
	(UNSIGNED8)0u /* 0x6002:1 */,
	(UNSIGNED8)0u /* 0x6000:1 */,
	(UNSIGNED8)0u /* 0x6003:1 */,
	(UNSIGNED8)0u /* 0x6000:2 */,
	(UNSIGNED8)0u /* 0x6000:3 */,
	(UNSIGNED8)0u /* 0x6002:2 */,
	(UNSIGNED8)0u /* 0x6002:3 */,
	(UNSIGNED8)0u /* 0x6003:2 */,
	(UNSIGNED8)255u /* 0x6006:2 */,
	(UNSIGNED8)255u /* 0x6006:3 */,
	(UNSIGNED8)0u /* 0x6007:2 */,
	(UNSIGNED8)0u /* 0x6007:3 */,
	(UNSIGNED8)0u /* 0x6008:2 */,
	(UNSIGNED8)0u /* 0x6008:3 */,
	(UNSIGNED8)1u /* 0x6005:0 */,
	(UNSIGNED8)0u /* 0x6423:0 */,
	(UNSIGNED8)7u /* 0x6421:1 */,
	(UNSIGNED8)7u /* 0x6421:2 */,
	(UNSIGNED8)7u /* 0x6421:3 */,
	(UNSIGNED8)7u /* 0x6421:4 */,
	(UNSIGNED8)7u /* 0x6421:5 */,
	(UNSIGNED8)7u /* 0x6421:6 */,
	(UNSIGNED8)2u /* 0x1600:0 */,
	(UNSIGNED8)0u /* 0x1601:0 */,
	(UNSIGNED8)2u /* 0x1602:0 */,
	(UNSIGNED8)3u /* 0x1603:0 */,
	(UNSIGNED8)0u /* 0x1604:0 */,
	(UNSIGNED8)3u /* 0x1605:0 */,
	(UNSIGNED8)0u /* 0x6200:3 */,
	(UNSIGNED8)0u /* 0x6200:4 */,
	(UNSIGNED8)0u /* 0x6200:5 */,
	(UNSIGNED8)0u /* 0x6200:6 */,
	(UNSIGNED8)5u /* 0x1a00:0 */,
	(UNSIGNED8)4u /* 0x1a03:0 */};
static UNSIGNED16 CO_STORAGE_CLASS	od_u16[1]= {
	(UNSIGNED16)0u /* 0x1015:0 */};
static UNSIGNED32 CO_STORAGE_CLASS	od_u32[6]= {
	(UNSIGNED32)1UL /* 0x1018:4 */,
	(UNSIGNED32)0UL /* 0x1014:0 */,
	(UNSIGNED32)0UL /* 0x6426:1 */,
	(UNSIGNED32)0UL /* 0x6426:2 */,
	(UNSIGNED32)0UL /* 0x6426:3 */,
	(UNSIGNED32)0UL /* 0x6426:4 */};
static INTEGER16 CO_STORAGE_CLASS	od_i16[14]= {
	(INTEGER16)0 /* 0x6411:1 */,
	(INTEGER16)0 /* 0x6401:1 */,
	(INTEGER16)0 /* 0x6401:2 */,
	(INTEGER16)0 /* 0x6401:3 */,
	(INTEGER16)0 /* 0x6401:4 */,
	(INTEGER16)0 /* 0x6401:5 */,
	(INTEGER16)0 /* 0x6401:6 */,
	(INTEGER16)0 /* 0x6411:2 */,
	(INTEGER16)0 /* 0x6411:3 */,
	(INTEGER16)0 /* 0x6411:4 */,
	(INTEGER16)0 /* 0x6411:5 */,
	(INTEGER16)0 /* 0x6411:6 */,
	(INTEGER16)0 /* 0x6411:7 */,
	(INTEGER16)0 /* 0x6411:8 */};
static INTEGER32 CO_STORAGE_CLASS	od_i32[9]= {
	(INTEGER32)0L /* 0x6444:1 */,
 	(INTEGER32)0L /* 0x6424:1 */,
 	(INTEGER32)0L /* 0x6425:1 */,
 	(INTEGER32)0L /* 0x6424:2 */,
 	(INTEGER32)0L /* 0x6424:3 */,
 	(INTEGER32)0L /* 0x6424:4 */,
 	(INTEGER32)0L /* 0x6425:2 */,
 	(INTEGER32)0L /* 0x6425:3 */,
 	(INTEGER32)0L /* 0x6425:4 */};

/* definition of constants */
static CO_CONST UNSIGNED8 CO_CONST_STORAGE_CLASS	od_const_u8[10] = {
	(UNSIGNED8)0u,
	(UNSIGNED8)4u,
	(UNSIGNED8)6u,
	(UNSIGNED8)1u,
	(UNSIGNED8)2u,
	(UNSIGNED8)255u,
	(UNSIGNED8)8u,
	(UNSIGNED8)3u,
	(UNSIGNED8)7u,
	(UNSIGNED8)5u};
static CO_CONST UNSIGNED16 CO_CONST_STORAGE_CLASS	od_const_u16[2] = {
	(UNSIGNED16)1000u,
	(UNSIGNED16)0u};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_u32[32] = {
	(UNSIGNED32)983441UL,
	(UNSIGNED32)793UL,
	(UNSIGNED32)2069UL,
	(UNSIGNED32)65537UL,
	(UNSIGNED32)1UL,
	(UNSIGNED32)128UL,
	(UNSIGNED32)1536UL,
	(UNSIGNED32)1408UL,
	(UNSIGNED32)0UL,
	(UNSIGNED32)512UL,
	(UNSIGNED32)2147484416UL,
	(UNSIGNED32)1024UL,
	(UNSIGNED32)1280UL,
	(UNSIGNED32)2147483648UL,
	(UNSIGNED32)1644167432UL,
	(UNSIGNED32)1644167688UL,
	(UNSIGNED32)1644167944UL,
	(UNSIGNED32)1644168200UL,
	(UNSIGNED32)1644168456UL,
	(UNSIGNED32)1678835984UL,
	(UNSIGNED32)1678836240UL,
	(UNSIGNED32)1678836496UL,
	(UNSIGNED32)1678836752UL,
	(UNSIGNED32)1678837008UL,
	(UNSIGNED32)384UL,
	(UNSIGNED32)1152UL,
	(UNSIGNED32)1610613000UL,
	(UNSIGNED32)1610613256UL,
	(UNSIGNED32)1677787408UL,
	(UNSIGNED32)1677787664UL,
	(UNSIGNED32)1677787920UL,
	(UNSIGNED32)1677788176UL};
static CO_CONST INTEGER16 CO_CONST_STORAGE_CLASS	od_const_i16[1] = {
	(INTEGER16)0};
static CO_CONST INTEGER32 CO_CONST_STORAGE_CLASS	od_const_i32[1] = {
	(INTEGER32)0L};
static CO_CONST VIS_STRING CO_CONST_STORAGE_CLASS	od_const_vis_string[1] = {
	"Generic IO module according CiA 401"};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_vis_string_len[1] = {
	35};

/* definition of application variables */

/* description of the objects */
CO_CONST CO_OBJECT_DESC_T od_description[CO_OBJ_DESC_CNT] CO_CONST_STORAGE_CLASS = {
	{ (UNSIGNED8)0u, CO_DTYPE_U32_CONST, (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1000:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_VAR   , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1001:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_SYNC , (UNSIGNED16)4101u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1005:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_VS_CONST , (UNSIGNED16)0u, CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1008:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4116u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1014:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_EMCY , (UNSIGNED16)4117u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1015:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_ERRCTRL, (UNSIGNED16)4119u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1017:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1018:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_CONST, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1018:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_CONST, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1018:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_CONST, (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1018:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ,  (UNSIGNED16)4u},/* 0x1018:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x1200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1200:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1400:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x1400:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1400:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1401:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)10u},/* 0x1401:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1401:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1402:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)11u},/* 0x1402:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1402:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1403:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)12u},/* 0x1403:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1403:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1404:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)13u},/* 0x1404:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1404:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1405:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)13u},/* 0x1405:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1405:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1600:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x1600:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)15u},/* 0x1600:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1601:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1601:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1601:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1601:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1602:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)19u},/* 0x1602:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)20u},/* 0x1602:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1603:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)21u},/* 0x1603:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)22u},/* 0x1603:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)23u},/* 0x1603:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1604:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1604:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1604:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1604:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1604:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1605:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)16u},/* 0x1605:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)17u},/* 0x1605:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)18u},/* 0x1605:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1800:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)24u},/* 0x1800:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)5u},/* 0x1800:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1803:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)25u},/* 0x1803:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)5u},/* 0x1803:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x1a00:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)26u},/* 0x1a00:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)27u},/* 0x1a00:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)28u},/* 0x1a00:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)29u},/* 0x1a00:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)30u},/* 0x1a00:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1a03:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)28u},/* 0x1a03:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)29u},/* 0x1a03:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)30u},/* 0x1a03:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)31u},/* 0x1a03:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)13u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)15u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6000:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)16u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6000:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6002:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)12u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6002:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)17u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6002:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)18u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6002:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x6003:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)14u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6003:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)19u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6003:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_VAR   , (UNSIGNED16)26u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6005:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6006:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6006:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)20u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6006:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)21u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6006:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6007:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6007:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)22u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6007:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)23u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6007:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6008:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)11u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6008:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)24u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6008:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)25u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6008:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)40u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_VAR   , (UNSIGNED16)41u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U8_VAR   , (UNSIGNED16)42u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U8_VAR   , (UNSIGNED16)43u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:6*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6202:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6202:1*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x6206:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6206:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6206:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x6207:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6207:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6207:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6401:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I16_VAR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I16_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_I16_VAR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_I16_VAR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_I16_VAR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_I16_VAR  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6401:6*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x6411:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I16_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I16_VAR  , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_I16_VAR  , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_I16_VAR  , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_I16_VAR  , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_I16_VAR  , (UNSIGNED16)11u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_I16_VAR  , (UNSIGNED16)12u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_I16_VAR  , (UNSIGNED16)13u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6421:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)28u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)29u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)30u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_VAR   , (UNSIGNED16)31u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U8_VAR   , (UNSIGNED16)32u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U8_VAR   , (UNSIGNED16)33u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6421:6*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_VAR   , (UNSIGNED16)27u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6423:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6424:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6424:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I32_VAR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6424:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_I32_VAR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6424:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_I32_VAR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6424:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6425:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6425:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I32_VAR  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6425:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_I32_VAR  , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6425:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_I32_VAR  , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6425:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6426:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6426:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_VAR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6426:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_VAR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6426:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_VAR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x6426:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6443:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6443:1*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6444:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6444:1*/ 
};

/* object dictionary assignment */
CO_CONST CO_OD_ASSIGN_T od_assign[CO_OD_ASSIGN_CNT] CO_CONST_STORAGE_CLASS = {
	{ 0x1000u, 1u, 0u, CO_ODTYPE_VAR, 0u },
	{ 0x1001u, 1u, 0u, CO_ODTYPE_VAR, 1u },
	{ 0x1005u, 1u, 0u, CO_ODTYPE_VAR, 2u },
	{ 0x1008u, 1u, 0u, CO_ODTYPE_VAR, 3u },
	{ 0x1014u, 1u, 0u, CO_ODTYPE_VAR, 4u },
	{ 0x1015u, 1u, 0u, CO_ODTYPE_VAR, 5u },
	{ 0x1017u, 1u, 0u, CO_ODTYPE_VAR, 6u },
	{ 0x1018u, 5u, 4u, CO_ODTYPE_STRUCT, 7u },
	{ 0x1200u, 3u, 2u, CO_ODTYPE_STRUCT, 12u },
	{ 0x1400u, 3u, 2u, CO_ODTYPE_STRUCT, 15u },
	{ 0x1401u, 3u, 2u, CO_ODTYPE_STRUCT, 18u },
	{ 0x1402u, 3u, 2u, CO_ODTYPE_STRUCT, 21u },
	{ 0x1403u, 3u, 2u, CO_ODTYPE_STRUCT, 24u },
	{ 0x1404u, 3u, 2u, CO_ODTYPE_STRUCT, 27u },
	{ 0x1405u, 3u, 2u, CO_ODTYPE_STRUCT, 30u },
	{ 0x1600u, 3u, 2u, CO_ODTYPE_STRUCT, 33u },
	{ 0x1601u, 5u, 4u, CO_ODTYPE_STRUCT, 36u },
	{ 0x1602u, 3u, 2u, CO_ODTYPE_STRUCT, 41u },
	{ 0x1603u, 4u, 3u, CO_ODTYPE_STRUCT, 44u },
	{ 0x1604u, 5u, 4u, CO_ODTYPE_STRUCT, 48u },
	{ 0x1605u, 4u, 3u, CO_ODTYPE_STRUCT, 53u },
	{ 0x1800u, 3u, 2u, CO_ODTYPE_STRUCT, 57u },
	{ 0x1803u, 3u, 2u, CO_ODTYPE_STRUCT, 60u },
	{ 0x1a00u, 6u, 5u, CO_ODTYPE_STRUCT, 63u },
	{ 0x1a03u, 5u, 4u, CO_ODTYPE_STRUCT, 69u },
	{ 0x6000u, 4u, 3u, CO_ODTYPE_ARRAY, 74u },
	{ 0x6002u, 4u, 3u, CO_ODTYPE_ARRAY, 78u },
	{ 0x6003u, 3u, 2u, CO_ODTYPE_ARRAY, 82u },
	{ 0x6005u, 1u, 0u, CO_ODTYPE_VAR, 85u },
	{ 0x6006u, 4u, 3u, CO_ODTYPE_ARRAY, 86u },
	{ 0x6007u, 4u, 3u, CO_ODTYPE_ARRAY, 90u },
	{ 0x6008u, 4u, 3u, CO_ODTYPE_ARRAY, 94u },
	{ 0x6200u, 7u, 6u, CO_ODTYPE_ARRAY, 98u },
	{ 0x6202u, 2u, 1u, CO_ODTYPE_ARRAY, 105u },
	{ 0x6206u, 3u, 2u, CO_ODTYPE_ARRAY, 107u },
	{ 0x6207u, 3u, 2u, CO_ODTYPE_ARRAY, 110u },
	{ 0x6401u, 7u, 6u, CO_ODTYPE_ARRAY, 113u },
	{ 0x6411u, 9u, 8u, CO_ODTYPE_ARRAY, 120u },
	{ 0x6421u, 7u, 6u, CO_ODTYPE_ARRAY, 129u },
	{ 0x6423u, 1u, 0u, CO_ODTYPE_VAR, 136u },
	{ 0x6424u, 5u, 4u, CO_ODTYPE_ARRAY, 137u },
	{ 0x6425u, 5u, 4u, CO_ODTYPE_ARRAY, 142u },
	{ 0x6426u, 5u, 4u, CO_ODTYPE_ARRAY, 147u },
	{ 0x6443u, 2u, 1u, CO_ODTYPE_ARRAY, 152u },
	{ 0x6444u, 2u, 1u, CO_ODTYPE_ARRAY, 154u },
};

/* static PDO mapping tables */


/**********************************************************************/
/* Structure for all OD variables */
CO_CONST CO_OD_DATA_VARIABLES_T od_data_variables = {
   &od_const_u8[0]
    , &od_const_u16[0]
    , &od_const_u32[0]
    , NULL
#ifdef CO_EXTENDED_DATA_TYPES
    , NULL
    , NULL
    , NULL
    , NULL
#endif /* CO_EXTENDED_DATA_TYPES */
    , &od_u8[0]
    , &od_u16[0]
    , &od_u32[0]
    , NULL
#ifdef CO_EXTENDED_DATA_TYPES
    , NULL
    , NULL
    , NULL
    , NULL
#endif /* CO_EXTENDED_DATA_TYPES */
    , NULL
    , &od_const_i16[0]
    , &od_const_i32[0]
    , NULL
    , &od_i16[0]
    , &od_i32[0]
    , NULL
    , NULL
    , NULL
#ifdef CO_EXTENDED_DATA_TYPES
    , NULL
    , NULL
    , NULL
    , NULL
#endif /* CO_EXTENDED_DATA_TYPES */
    , NULL
    , NULL
    , NULL
    , NULL
    , &od_const_vis_string[0]
    , &od_const_vis_string_len[0]
    , NULL
    , NULL
#ifdef CO_UTF8_DATA_TYPE
    , NULL
    , NULL
    , NULL
    , NULL
#endif
    , NULL
    , NULL
    , NULL
    , NULL
};


RET_T coCanOpenStackInit(
         CO_EVENT_STORE_T pLoadFunction
    )
{
    RET_T retVal;

#if CO_STACK_VERSION < 0x20000ul
#else
    CO_SERVICE_INIT_VAL_T	serviceInitVals = {
       { CO_OBJECT_COUNTS },
       { CO_COB_COUNTS },
       { CO_TXPDO_COUNTS },
       { CO_RXPDO_COUNTS },
       { CO_SSDO_COUNTS },
       { CO_CSDO_COUNTS },
       { CO_ERR_CTRL_COUNTS },
       { CO_ERR_HIST_COUNTS },
       { CO_EMCY_CONS_COUNTS },
#ifdef CO_NETWORK_ROUTING_CNT
       { CO_NETWORK_ROUTING_COUNTS }, 
#endif /* CO_NETWORK_ROUTING_CNT */
       { CO_ASSIGN_COUNTS },
       { CO_GUARDING_COUNTS },
       { CO_NODE_IDS }, 
       { CO_NODE_ID_FUNCTIONS } 
    };

    /* variable initialization */
    coCanOpenStackVarInit(&serviceInitVals);
#endif /* CO_STACK_VERSION */

    /* initialize object dictionary variables and pointers */
    coOdInitOdPtr(&od_assign[0], CO_OD_ASSIGN_CNT, &od_description[0], CO_OBJ_DESC_CNT, NULL, &od_data_variables);

    /* library timer initialization */
    coTimerInit(CO_TIMER_INTERVAL);

    /* initialization of LED module */
    coLedInit();

    /* initialize sdo server(s) */
    retVal = coSdoServerInit(1u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize error control */
    retVal = coErrorCtrlInit(od_const_u16[0], 0u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize emergency */
    retVal = coEmcyProducerInit();
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize sync */
    retVal = coSyncInit(od_const_u32[5]);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 1 */
    retVal = coPdoTransmitInit(1u, od_const_u8[5], 0u, 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 4 */
    retVal = coPdoTransmitInit(4u, od_const_u8[5], 0u, 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 1 */
    retVal = coPdoReceiveInit(1u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 2 */
    retVal = coPdoReceiveInit(2u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 3 */
    retVal = coPdoReceiveInit(3u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 4 */
    retVal = coPdoReceiveInit(4u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 5 */
    retVal = coPdoReceiveInit(5u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 6 */
    retVal = coPdoReceiveInit(6u, od_const_u8[5], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }

    /* initialize load parameter function */
#if defined(CO_EVENT_DYNAMIC_STORE) || defined(CO_EVENT_PROFILE_STORE)
    retVal = coEventRegister_LOAD_PARA(pLoadFunction);
    if (retVal != RET_OK) {
        return (retVal);
    }
#else
    if (pLoadFunction != NULL) {
        return RET_INVALID_PARAMETER;
    }
#endif /* CO_EVENT_DYNAMIC_STORE || CO_EVENT_PROFILE_STORE */
    /* initialize NMT slave */ 
    retVal = coNmtInit(0u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    return(RET_OK);
}
