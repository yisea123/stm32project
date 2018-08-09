/*
 * object dictionary for slave1 - generated by CANopen DeviceDesigner 2.7.1.9
 * Di. Sep. 12 08:49:41 2017
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
#define CO_OD_ASSIGN_CNT 20u
#define CO_OBJ_DESC_CNT 97u

/* definition of managed variables */
static UNSIGNED8 CO_STORAGE_CLASS	od_u8[5]= {
	(UNSIGNED8)0u /* 0x1001:0 */,
	(UNSIGNED8)0u /* 0x1003:0 */,
	(UNSIGNED8)0u /* 0x1029:2 */,
	(UNSIGNED8)0u /* 0x1601:0 */,
	(UNSIGNED8)2u /* 0x1a00:0 */};
static UNSIGNED16 CO_STORAGE_CLASS	od_u16[1]= {
	(UNSIGNED16)0u /* 0x1015:0 */};
static UNSIGNED32 CO_STORAGE_CLASS	od_u32[3]= {
	(UNSIGNED32)0UL /* 0x1003:1 */,
	(UNSIGNED32)0UL /* 0x1014:0 */,
	(UNSIGNED32)0UL /* 0x1018:4 */};
static INTEGER16 CO_STORAGE_CLASS	od_i16[44]= {
	(INTEGER16)0 /* 0x3000:1 */,
	(INTEGER16)2 /* 0x3000:2 */,
	(INTEGER16)0 /* 0x4000:1 */,
	(INTEGER16)0 /* 0x4000:2 */,
	(INTEGER16)0 /* 0x4000:3 */,
	(INTEGER16)0 /* 0x4000:4 */,
	(INTEGER16)0 /* 0x4000:5 */,
	(INTEGER16)0 /* 0x4000:6 */,
	(INTEGER16)0 /* 0x4000:7 */,
	(INTEGER16)0 /* 0x4000:8 */,
	(INTEGER16)0 /* 0x4000:9 */,
	(INTEGER16)0 /* 0x4000:10 */,
	(INTEGER16)0 /* 0x4000:11 */,
	(INTEGER16)0 /* 0x4000:12 */,
	(INTEGER16)0 /* 0x4000:13 */,
	(INTEGER16)0 /* 0x4000:14 */,
	(INTEGER16)0 /* 0x4000:15 */,
	(INTEGER16)0 /* 0x4000:16 */,
	(INTEGER16)0 /* 0x4000:17 */,
	(INTEGER16)0 /* 0x4000:18 */,
	(INTEGER16)0 /* 0x4000:19 */,
	(INTEGER16)0 /* 0x4000:128 */,
	(INTEGER16)0 /* 0x4000:129 */,
	(INTEGER16)0 /* 0x4000:130 */,
	(INTEGER16)0 /* 0x4000:131 */,
	(INTEGER16)0 /* 0x4000:132 */,
	(INTEGER16)0 /* 0x4000:133 */,
	(INTEGER16)0 /* 0x4000:134 */,
	(INTEGER16)0 /* 0x4000:135 */,
	(INTEGER16)0 /* 0x4000:136 */,
	(INTEGER16)0 /* 0x4000:137 */,
	(INTEGER16)0 /* 0x4000:138 */,
	(INTEGER16)0 /* 0x4000:139 */,
	(INTEGER16)0 /* 0x4000:140 */,
	(INTEGER16)0 /* 0x4000:141 */,
	(INTEGER16)0 /* 0x4000:142 */,
	(INTEGER16)0 /* 0x4000:143 */,
	(INTEGER16)0 /* 0x4000:144 */,
	(INTEGER16)0 /* 0x4000:145 */,
	(INTEGER16)0 /* 0x4000:146 */,
	(INTEGER16)0 /* 0x4000:147 */,
	(INTEGER16)0 /* 0x4000:148 */,
	(INTEGER16)0 /* 0x4000:149 */,
	(INTEGER16)0 /* 0x4000:150 */};
static INTEGER32 CO_STORAGE_CLASS	od_i32[1]= {
	(INTEGER32)4L /* 0x2001:0 */};

/* definition of constants */
static CO_CONST UNSIGNED8 CO_CONST_STORAGE_CLASS	od_const_u8[7] = {
	(UNSIGNED8)0u,
	(UNSIGNED8)1u,
	(UNSIGNED8)4u,
	(UNSIGNED8)2u,
	(UNSIGNED8)5u,
	(UNSIGNED8)254u,
	(UNSIGNED8)150u};
static CO_CONST UNSIGNED16 CO_CONST_STORAGE_CLASS	od_const_u16[3] = {
	(UNSIGNED16)0u,
	(UNSIGNED16)1000u,
	(UNSIGNED16)3000u};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_u32[14] = {
	(UNSIGNED32)0UL,
	(UNSIGNED32)128UL,
	(UNSIGNED32)67584UL,
	(UNSIGNED32)793UL,
	(UNSIGNED32)1234UL,
	(UNSIGNED32)1UL,
	(UNSIGNED32)1536UL,
	(UNSIGNED32)1408UL,
	(UNSIGNED32)512UL,
	(UNSIGNED32)768UL,
	(UNSIGNED32)805306896UL,
	(UNSIGNED32)536936480UL,
	(UNSIGNED32)384UL,
	(UNSIGNED32)805306640UL};
static CO_CONST INTEGER16 CO_CONST_STORAGE_CLASS	od_const_i16[2] = {
	(INTEGER16)0,
	(INTEGER16)2};
static CO_CONST INTEGER32 CO_CONST_STORAGE_CLASS	od_const_i32[1] = {
	(INTEGER32)4L};
static CO_CONST VIS_STRING CO_CONST_STORAGE_CLASS	od_const_vis_string[1] = {
	"emtas Slave 1"};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_vis_string_len[1] = {
	13};

/* definition of application variables */

/* description of the objects */
CO_CONST CO_OBJECT_DESC_T od_description[CO_OBJ_DESC_CNT] CO_CONST_STORAGE_CLASS = {
	{ (UNSIGNED8)0u, CO_DTYPE_U32_CONST, (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1000:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_VAR   , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ,  (UNSIGNED16)0u},/* 0x1001:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_EMCY  , (UNSIGNED16)4099u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1003:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4099u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1003:1*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_VS_CONST , (UNSIGNED16)0u, CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1008:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4116u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1014:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_EMCY , (UNSIGNED16)4117u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1015:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1016:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_ERRCTRL, (UNSIGNED16)4118u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1016:1*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_ERRCTRL, (UNSIGNED16)4119u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)1u},/* 0x1017:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1018:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_CONST, (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1018:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_CONST, (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1018:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_CONST, (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1018:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1018:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1029:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_ERRCTRL, (UNSIGNED16)4137u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1029:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)0u},/* 0x1029:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x1200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1200:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1400:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1400:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)5u},/* 0x1400:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1400:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)0u},/* 0x1400:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1400:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1401:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x1401:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1401:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1600:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_CONST, (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)10u},/* 0x1600:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_CONST, (UNSIGNED16)11u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)11u},/* 0x1600:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1601:1*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1800:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)12u},/* 0x1800:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x1800:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1800:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1800:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1800:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1a00:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)11u},/* 0x1a00:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)13u},/* 0x1a00:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1a00:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_I32_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)0u},/* 0x2001:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x3000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I16_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR,  (UNSIGNED16)0u},/* 0x3000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I16_VAR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC,  (UNSIGNED16)1u},/* 0x3000:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x4000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I16_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I16_VAR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_I16_VAR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_I16_VAR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_I16_VAR  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_I16_VAR  , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_I16_VAR  , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_I16_VAR  , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:8*/ 
	{ (UNSIGNED8)9u, CO_DTYPE_I16_VAR  , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:9*/ 
	{ (UNSIGNED8)10u, CO_DTYPE_I16_VAR  , (UNSIGNED16)11u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:10*/ 
	{ (UNSIGNED8)11u, CO_DTYPE_I16_VAR  , (UNSIGNED16)12u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:11*/ 
	{ (UNSIGNED8)12u, CO_DTYPE_I16_VAR  , (UNSIGNED16)13u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:12*/ 
	{ (UNSIGNED8)13u, CO_DTYPE_I16_VAR  , (UNSIGNED16)14u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:13*/ 
	{ (UNSIGNED8)14u, CO_DTYPE_I16_VAR  , (UNSIGNED16)15u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:14*/ 
	{ (UNSIGNED8)15u, CO_DTYPE_I16_VAR  , (UNSIGNED16)16u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:15*/ 
	{ (UNSIGNED8)16u, CO_DTYPE_I16_VAR  , (UNSIGNED16)17u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:16*/ 
	{ (UNSIGNED8)17u, CO_DTYPE_I16_VAR  , (UNSIGNED16)18u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:17*/ 
	{ (UNSIGNED8)18u, CO_DTYPE_I16_VAR  , (UNSIGNED16)19u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:18*/ 
	{ (UNSIGNED8)19u, CO_DTYPE_I16_VAR  , (UNSIGNED16)20u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:19*/ 
	{ (UNSIGNED8)128u, CO_DTYPE_I16_VAR  , (UNSIGNED16)21u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:128*/ 
	{ (UNSIGNED8)129u, CO_DTYPE_I16_VAR  , (UNSIGNED16)22u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:129*/ 
	{ (UNSIGNED8)130u, CO_DTYPE_I16_VAR  , (UNSIGNED16)23u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:130*/ 
	{ (UNSIGNED8)131u, CO_DTYPE_I16_VAR  , (UNSIGNED16)24u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:131*/ 
	{ (UNSIGNED8)132u, CO_DTYPE_I16_VAR  , (UNSIGNED16)25u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:132*/ 
	{ (UNSIGNED8)133u, CO_DTYPE_I16_VAR  , (UNSIGNED16)26u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:133*/ 
	{ (UNSIGNED8)134u, CO_DTYPE_I16_VAR  , (UNSIGNED16)27u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:134*/ 
	{ (UNSIGNED8)135u, CO_DTYPE_I16_VAR  , (UNSIGNED16)28u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:135*/ 
	{ (UNSIGNED8)136u, CO_DTYPE_I16_VAR  , (UNSIGNED16)29u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:136*/ 
	{ (UNSIGNED8)137u, CO_DTYPE_I16_VAR  , (UNSIGNED16)30u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:137*/ 
	{ (UNSIGNED8)138u, CO_DTYPE_I16_VAR  , (UNSIGNED16)31u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:138*/ 
	{ (UNSIGNED8)139u, CO_DTYPE_I16_VAR  , (UNSIGNED16)32u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:139*/ 
	{ (UNSIGNED8)140u, CO_DTYPE_I16_VAR  , (UNSIGNED16)33u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:140*/ 
	{ (UNSIGNED8)141u, CO_DTYPE_I16_VAR  , (UNSIGNED16)34u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:141*/ 
	{ (UNSIGNED8)142u, CO_DTYPE_I16_VAR  , (UNSIGNED16)35u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:142*/ 
	{ (UNSIGNED8)143u, CO_DTYPE_I16_VAR  , (UNSIGNED16)36u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:143*/ 
	{ (UNSIGNED8)144u, CO_DTYPE_I16_VAR  , (UNSIGNED16)37u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:144*/ 
	{ (UNSIGNED8)145u, CO_DTYPE_I16_VAR  , (UNSIGNED16)38u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:145*/ 
	{ (UNSIGNED8)146u, CO_DTYPE_I16_VAR  , (UNSIGNED16)39u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:146*/ 
	{ (UNSIGNED8)147u, CO_DTYPE_I16_VAR  , (UNSIGNED16)40u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:147*/ 
	{ (UNSIGNED8)148u, CO_DTYPE_I16_VAR  , (UNSIGNED16)41u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:148*/ 
	{ (UNSIGNED8)149u, CO_DTYPE_I16_VAR  , (UNSIGNED16)42u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:149*/ 
	{ (UNSIGNED8)150u, CO_DTYPE_I16_VAR  , (UNSIGNED16)43u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x4000:150*/ 
};

/* object dictionary assignment */
CO_CONST CO_OD_ASSIGN_T od_assign[CO_OD_ASSIGN_CNT] CO_CONST_STORAGE_CLASS = {
	{ 0x1000u, 1u, 0u, CO_ODTYPE_VAR, 0u },
	{ 0x1001u, 1u, 0u, CO_ODTYPE_VAR, 1u },
	{ 0x1003u, 2u, 1u, CO_ODTYPE_ARRAY, 2u },
	{ 0x1008u, 1u, 0u, CO_ODTYPE_VAR, 4u },
	{ 0x1014u, 1u, 0u, CO_ODTYPE_VAR, 5u },
	{ 0x1015u, 1u, 0u, CO_ODTYPE_VAR, 6u },
	{ 0x1016u, 2u, 1u, CO_ODTYPE_ARRAY, 7u },
	{ 0x1017u, 1u, 0u, CO_ODTYPE_VAR, 9u },
	{ 0x1018u, 5u, 4u, CO_ODTYPE_STRUCT, 10u },
	{ 0x1029u, 3u, 2u, CO_ODTYPE_ARRAY, 15u },
	{ 0x1200u, 3u, 2u, CO_ODTYPE_STRUCT, 18u },
	{ 0x1400u, 6u, 5u, CO_ODTYPE_STRUCT, 21u },
	{ 0x1401u, 3u, 2u, CO_ODTYPE_STRUCT, 27u },
	{ 0x1600u, 3u, 2u, CO_ODTYPE_STRUCT, 30u },
	{ 0x1601u, 2u, 1u, CO_ODTYPE_STRUCT, 33u },
	{ 0x1800u, 6u, 5u, CO_ODTYPE_STRUCT, 35u },
	{ 0x1a00u, 9u, 8u, CO_ODTYPE_STRUCT, 41u },
	{ 0x2001u, 1u, 0u, CO_ODTYPE_VAR, 50u },
	{ 0x3000u, 3u, 2u, CO_ODTYPE_ARRAY, 51u },
	{ 0x4000u, 43u, 150u, CO_ODTYPE_ARRAY, 54u },
};

/* static PDO mapping tables */
/* unique rpdo defintion */
static CO_CONST PDO_REC_MAP_TABLE_T CO_CONST_STORAGE_CLASS	mapTableRPDO0 = {
	2,
	{
		{&od_i16[1], 2u, CO_TRUE, 0x30000210UL },
		{&od_i32[0], 4u, CO_TRUE, 0x20010020UL },
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul},
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul},
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul},
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul},
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul},
		{NULL, (UNSIGNED8)0u, CO_FALSE, 0ul}
	}
};


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
    retVal = coErrorCtrlInit(od_const_u16[1], 1u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize emergency */
    retVal = coEmcyProducerInit();
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 1 */
    retVal = coPdoTransmitInit(1u, od_const_u8[5], od_const_u16[0], od_const_u16[1], 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 1 */
    retVal = coPdoReceiveInit(1u, od_const_u8[5], od_const_u16[0], od_const_u16[2], &mapTableRPDO0);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 2 */
    retVal = coPdoReceiveInit(2u, od_const_u8[5], 0u, 0u, NULL);
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
