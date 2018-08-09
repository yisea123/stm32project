/*
 * object dictionary for master4 - generated by CANopen DeviceDesigner 2.4.4.6
 * Fr. Juni 17 12:44:43 2016
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

/* application header file(s) */
#include <appl.h>

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
#define CO_OD_ASSIGN_CNT 33u
#define CO_OBJ_DESC_CNT 151u

/* definition of managed variables */
static UNSIGNED8 CO_STORAGE_CLASS	od_u8[10]= {
	(UNSIGNED8)0u /* 0x1001:0 */,
	(UNSIGNED8)0u /* 0x1029:2 */,
	(UNSIGNED8)0u /* 0x1601:0 */,
	(UNSIGNED8)4u /* 0x1600:0 */,
	(UNSIGNED8)4u /* 0x1a00:0 */,
	(UNSIGNED8)0u /* 0x1a01:0 */,
	(UNSIGNED8)0u /* 0x1a02:0 */,
	(UNSIGNED8)0u /* 0x1a03:0 */,
	(UNSIGNED8)0u /* 0x1602:0 */,
	(UNSIGNED8)0u /* 0x1603:0 */};

/* definition of constants */
static CO_CONST UNSIGNED8 CO_CONST_STORAGE_CLASS	od_const_u8[7] = {
	(UNSIGNED8)0u,
	(UNSIGNED8)4u,
	(UNSIGNED8)2u,
	(UNSIGNED8)3u,
	(UNSIGNED8)254u,
	(UNSIGNED8)5u,
	(UNSIGNED8)8u};
static CO_CONST UNSIGNED16 CO_CONST_STORAGE_CLASS	od_const_u16[2] = {
	(UNSIGNED16)2000u,
	(UNSIGNED16)0u};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_u32[30] = {
	(UNSIGNED32)0UL,
	(UNSIGNED32)793UL,
	(UNSIGNED32)2069UL,
	(UNSIGNED32)65537UL,
	(UNSIGNED32)1UL,
	(UNSIGNED32)1073741952UL,
	(UNSIGNED32)500000UL,
	(UNSIGNED32)160UL,
	(UNSIGNED32)2098152UL,
	(UNSIGNED32)1073742080UL,
	(UNSIGNED32)161UL,
	(UNSIGNED32)2163688UL,
	(UNSIGNED32)1536UL,
	(UNSIGNED32)1408UL,
	(UNSIGNED32)1280UL,
	(UNSIGNED32)1024UL,
	(UNSIGNED32)768UL,
	(UNSIGNED32)512UL,
	(UNSIGNED32)384UL,
	(UNSIGNED32)640UL,
	(UNSIGNED32)896UL,
	(UNSIGNED32)1152UL,
	(UNSIGNED32)536871176UL,
	(UNSIGNED32)536871432UL,
	(UNSIGNED32)536871688UL,
	(UNSIGNED32)536871944UL,
	(UNSIGNED32)536936720UL,
	(UNSIGNED32)536936976UL,
	(UNSIGNED32)536937232UL,
	(UNSIGNED32)536937488UL};

/* definition of application variables */
static UNSIGNED8 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u8[8] = {
	&(app_u8[0]) /* 0x2000:1 */,
	&(app_u8[1]) /* 0x2000:2 */,
	&(app_u8[2]) /* 0x2000:3 */,
	&(app_u8[3]) /* 0x2000:4 */,
	&(app_u8[3]) /* 0x2000:5 */,
	&(app_u8[3]) /* 0x2000:6 */,
	&(app_u8[3]) /* 0x2000:7 */,
	&(app_u8[3]) /* 0x2000:8 */};
static UNSIGNED16 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u16[4] = {
	&(app_u16[0]) /* 0x2001:1 */,
	&(app_u16[1]) /* 0x2001:2 */,
	&(app_u16[2]) /* 0x2001:3 */,
	&(app_u16[3]) /* 0x2001:4 */};
static UNSIGNED32 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u32[4] = {
	&(app_u32[0]) /* 0x2002:1 */,
	&(app_u32[1]) /* 0x2002:2 */,
	&(app_u32[2]) /* 0x2002:3 */,
	&(app_u32[3]) /* 0x2002:4 */};

/* description of the objects */
CO_CONST CO_OBJECT_DESC_T od_description[CO_OBJ_DESC_CNT] CO_CONST_STORAGE_CLASS = {
	{ (UNSIGNED8)0u, CO_DTYPE_U32_CONST, (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1000:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_EMCY  , (UNSIGNED16)4097u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1001:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_SYNC , (UNSIGNED16)4101u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1005:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_SYNC , (UNSIGNED16)4102u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x1006:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_TIME , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x1012:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1016:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_ERRCTRL, (UNSIGNED16)4118u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1016:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_ERRCTRL, (UNSIGNED16)4118u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)11u},/* 0x1016:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_ERRCTRL, (UNSIGNED16)4119u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1017:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1018:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_CONST, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1018:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_CONST, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1018:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_CONST, (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1018:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_CONST, (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1018:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1028:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4136u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1028:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4136u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)10u},/* 0x1028:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1029:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_ERRCTRL, (UNSIGNED16)4137u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1029:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1029:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_NMT  , (UNSIGNED16)4138u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x102a:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)12u},/* 0x1200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)13u},/* 0x1200:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1280:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_CLIENT, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1280:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_CLIENT, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1280:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_SDO_CLIENT, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1280:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1281:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_CLIENT, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1281:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_CLIENT, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1281:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_SDO_CLIENT, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1281:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1400:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)17u},/* 0x1400:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1400:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1401:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)16u},/* 0x1401:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1401:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1402:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)15u},/* 0x1402:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1402:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1403:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x1403:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1403:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1600:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)22u},/* 0x1600:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)23u},/* 0x1600:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)24u},/* 0x1600:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)25u},/* 0x1600:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1600:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1600:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1600:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1600:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_RMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1602:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_RMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1603:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1800:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)18u},/* 0x1800:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)4u},/* 0x1800:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1800:3*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1800:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1801:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)19u},/* 0x1801:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)4u},/* 0x1801:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_TPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1801:3*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_TPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1801:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1802:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)20u},/* 0x1802:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)4u},/* 0x1802:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_TPDO , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1802:3*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_TPDO , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1802:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1803:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)21u},/* 0x1803:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)4u},/* 0x1803:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_TPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1803:3*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_TPDO , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1803:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1a00:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)26u},/* 0x1a00:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)27u},/* 0x1a00:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)28u},/* 0x1a00:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)29u},/* 0x1a00:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_TMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a01:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_TMAP , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a02:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_TMAP , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a03:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x2000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_PTR   , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_PTR   , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_PTR   , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_PTR   , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U8_PTR   , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U8_PTR   , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U8_PTR   , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U8_PTR   , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2000:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x2001:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U16_PTR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)1u},/* 0x2001:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U16_PTR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)1u},/* 0x2001:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_PTR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)1u},/* 0x2001:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U16_PTR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)1u},/* 0x2001:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x2002:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_PTR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2002:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_PTR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2002:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_PTR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2002:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_PTR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2002:4*/ 
};

/* object dictionary assignment */
CO_CONST CO_OD_ASSIGN_T od_assign[CO_OD_ASSIGN_CNT] CO_CONST_STORAGE_CLASS = {
	{ 0x1000u, 1u, 0u, CO_ODTYPE_VAR, 0u },
	{ 0x1001u, 1u, 0u, CO_ODTYPE_VAR, 1u },
	{ 0x1005u, 1u, 0u, CO_ODTYPE_VAR, 2u },
	{ 0x1006u, 1u, 0u, CO_ODTYPE_VAR, 3u },
	{ 0x1012u, 1u, 0u, CO_ODTYPE_VAR, 4u },
	{ 0x1016u, 3u, 2u, CO_ODTYPE_ARRAY, 5u },
	{ 0x1017u, 1u, 0u, CO_ODTYPE_VAR, 8u },
	{ 0x1018u, 5u, 4u, CO_ODTYPE_STRUCT, 9u },
	{ 0x1028u, 3u, 2u, CO_ODTYPE_ARRAY, 14u },
	{ 0x1029u, 3u, 2u, CO_ODTYPE_ARRAY, 17u },
	{ 0x102au, 1u, 0u, CO_ODTYPE_VAR, 20u },
	{ 0x1200u, 3u, 2u, CO_ODTYPE_STRUCT, 21u },
	{ 0x1280u, 4u, 3u, CO_ODTYPE_STRUCT, 24u },
	{ 0x1281u, 4u, 3u, CO_ODTYPE_STRUCT, 28u },
	{ 0x1400u, 3u, 2u, CO_ODTYPE_STRUCT, 32u },
	{ 0x1401u, 3u, 2u, CO_ODTYPE_STRUCT, 35u },
	{ 0x1402u, 3u, 2u, CO_ODTYPE_STRUCT, 38u },
	{ 0x1403u, 3u, 2u, CO_ODTYPE_STRUCT, 41u },
	{ 0x1600u, 9u, 8u, CO_ODTYPE_STRUCT, 44u },
	{ 0x1601u, 9u, 8u, CO_ODTYPE_STRUCT, 53u },
	{ 0x1602u, 9u, 8u, CO_ODTYPE_STRUCT, 62u },
	{ 0x1603u, 9u, 8u, CO_ODTYPE_STRUCT, 71u },
	{ 0x1800u, 5u, 5u, CO_ODTYPE_STRUCT, 80u },
	{ 0x1801u, 5u, 5u, CO_ODTYPE_STRUCT, 85u },
	{ 0x1802u, 5u, 5u, CO_ODTYPE_STRUCT, 90u },
	{ 0x1803u, 5u, 5u, CO_ODTYPE_STRUCT, 95u },
	{ 0x1a00u, 5u, 4u, CO_ODTYPE_STRUCT, 100u },
	{ 0x1a01u, 9u, 8u, CO_ODTYPE_STRUCT, 105u },
	{ 0x1a02u, 9u, 8u, CO_ODTYPE_STRUCT, 114u },
	{ 0x1a03u, 9u, 8u, CO_ODTYPE_STRUCT, 123u },
	{ 0x2000u, 9u, 8u, CO_ODTYPE_ARRAY, 132u },
	{ 0x2001u, 5u, 4u, CO_ODTYPE_ARRAY, 141u },
	{ 0x2002u, 5u, 4u, CO_ODTYPE_ARRAY, 146u },
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
    , NULL
    , NULL
    , &od_ptr_u8[0]
    , &od_ptr_u16[0]
    , &od_ptr_u32[0]
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
    , NULL
    , NULL
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

/**
* \brief initialisation of object dictionary
*
* \param 
* nothing 
* \results 
* nothing 
*/
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

    /* initialize sdo server(s) */
    retVal = coSdoServerInit(1u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize sdo clients(s) */
    retVal = coSdoClientInit(1u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    retVal = coSdoClientInit(2u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize error control */
    retVal = coErrorCtrlInit(od_const_u16[0], 2u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize sync */
    retVal = coSyncInit(od_const_u32[5]);
    if (retVal != RET_OK) {
        return(retVal);
    }

    /* initialize EMCY consumer */
    retVal = coEmcyConsumerInit(2u);
    if (retVal != RET_OK) {
        return(retVal);
    }

    /* initialize Time service */
    retVal = coTimeInit(CO_TRUE, CO_FALSE);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 1 */
    retVal = coPdoTransmitInit(1u, od_const_u8[4], od_const_u16[1], od_const_u16[1], 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 2 */
    retVal = coPdoTransmitInit(2u, od_const_u8[4], od_const_u16[1], od_const_u16[1], 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 3 */
    retVal = coPdoTransmitInit(3u, od_const_u8[4], od_const_u16[1], od_const_u16[1], 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 4 */
    retVal = coPdoTransmitInit(4u, od_const_u8[4], od_const_u16[1], od_const_u16[1], 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 1 */
    retVal = coPdoReceiveInit(1u, od_const_u8[4], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 2 */
    retVal = coPdoReceiveInit(2u, od_const_u8[4], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 3 */
    retVal = coPdoReceiveInit(3u, od_const_u8[4], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 4 */
    retVal = coPdoReceiveInit(4u, od_const_u8[4], 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }

    /* initialize load parameter function */
#if defined(CO_EVENT_DYNAMIC_STORE) || defined(CO_EVENT_PROFILE_STORE)
    coEventRegister_LOAD_PARA(pLoadFunction);
#else
    if (pLoadFunction != NULL) {
        return RET_INVALID_PARAMETER;
    }
#endif /* CO_EVENT_DYNAMIC_STORE || CO_EVENT_PROFILE_STORE */
    /* initialize NMT master */ 
    retVal = coNmtInit(1u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    return(RET_OK);
}
