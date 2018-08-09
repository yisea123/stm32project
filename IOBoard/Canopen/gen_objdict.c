/*
 * object dictionary for slave_t - generated by CANopen DeviceDesigner 2.7.0
 * 周四 十一月 2 16:31:42 2017
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
#define CO_OD_ASSIGN_CNT 39u
#define CO_OBJ_DESC_CNT 120u

/* definition of managed variables */
static UNSIGNED8 CO_STORAGE_CLASS	od_u8[34]= {
	(UNSIGNED8)0u /* 0x1001:0 */,
	(UNSIGNED8)7u /* 0x6421:1 */,
	(UNSIGNED8)7u /* 0x6421:2 */,
	(UNSIGNED8)7u /* 0x6421:3 */,
	(UNSIGNED8)7u /* 0x6421:4 */,
	(UNSIGNED8)7u /* 0x6421:5 */,
	(UNSIGNED8)7u /* 0x6421:6 */,
	(UNSIGNED8)1u /* 0x6443:1 */,
	(UNSIGNED8)255u /* 0x6206:1 */,
	(UNSIGNED8)0u /* 0x6207:1 */,
	(UNSIGNED8)255u /* 0x6208:1 */,
	(UNSIGNED8)0u /* 0x6200:1 */,
	(UNSIGNED8)0u /* 0x6202:1 */,
	(UNSIGNED8)0u /* 0x6008:1 */,
	(UNSIGNED8)0u /* 0x6007:1 */,
	(UNSIGNED8)255u /* 0x6006:1 */,
	(UNSIGNED8)1u /* 0x6005:0 */,
	(UNSIGNED8)0u /* 0x6003:1 */,
	(UNSIGNED8)0u /* 0x6002:1 */,
	(UNSIGNED8)0u /* 0x6000:1 */,
	(UNSIGNED8)0u /* 0x6200:2 */,
	(UNSIGNED8)0u /* 0x6202:2 */,
	(UNSIGNED8)255u /* 0x6206:2 */,
	(UNSIGNED8)0u /* 0x6207:2 */,
	(UNSIGNED8)255u /* 0x6208:2 */,
	(UNSIGNED8)1u /* 0x6443:2 */,
	(UNSIGNED8)0u /* 0x6000:2 */,
	(UNSIGNED8)0u /* 0x6002:2 */,
	(UNSIGNED8)0u /* 0x6003:2 */,
	(UNSIGNED8)255u /* 0x6006:2 */,
	(UNSIGNED8)0u /* 0x6007:2 */,
	(UNSIGNED8)0u /* 0x6008:2 */,
	(UNSIGNED8)2u /* 0x1600:0 */,
	(UNSIGNED8)2u /* 0x1a00:0 */};
static UNSIGNED16 CO_STORAGE_CLASS	od_u16[1]= {
	(UNSIGNED16)0u /* 0x1015:0 */};
static UNSIGNED32 CO_STORAGE_CLASS	od_u32[1]= {
	(UNSIGNED32)0UL /* 0x1014:0 */};
static INTEGER16 CO_STORAGE_CLASS	od_i16[2]= {
	(INTEGER16)0 /* 0x6411:1 */,
	(INTEGER16)0 /* 0x6411:2 */};
static INTEGER32 CO_STORAGE_CLASS	od_i32[6]= {
	(INTEGER32)1L /* 0x6447:1 */,
 	(INTEGER32)0L /* 0x6446:1 */,
 	(INTEGER32)0L /* 0x6444:1 */,
 	(INTEGER32)0L /* 0x6444:2 */,
 	(INTEGER32)0L /* 0x6446:2 */,
 	(INTEGER32)1L /* 0x6447:2 */};

/* definition of constants */
static CO_CONST UNSIGNED8 CO_CONST_STORAGE_CLASS	od_const_u8[10] = {
	(UNSIGNED8)0u,
	(UNSIGNED8)4u,
	(UNSIGNED8)2u,
	(UNSIGNED8)1u,
	(UNSIGNED8)6u,
	(UNSIGNED8)7u,
	(UNSIGNED8)8u,
	(UNSIGNED8)255u,
	(UNSIGNED8)3u,
	(UNSIGNED8)5u};
static CO_CONST UNSIGNED16 CO_CONST_STORAGE_CLASS	od_const_u16[2] = {
	(UNSIGNED16)0u,
	(UNSIGNED16)1000u};
static CO_CONST UNSIGNED32 CO_CONST_STORAGE_CLASS	od_const_u32[15] = {
	(UNSIGNED32)983441UL,
	(UNSIGNED32)128UL,
	(UNSIGNED32)793UL,
	(UNSIGNED32)2069UL,
	(UNSIGNED32)65537UL,
	(UNSIGNED32)1UL,
	(UNSIGNED32)1536UL,
	(UNSIGNED32)1408UL,
	(UNSIGNED32)512UL,
	(UNSIGNED32)384UL,
	(UNSIGNED32)1644167432UL,
	(UNSIGNED32)1644167688UL,
	(UNSIGNED32)1610613000UL,
	(UNSIGNED32)1610613256UL,
	(UNSIGNED32)0UL};
static CO_CONST INTEGER16 CO_CONST_STORAGE_CLASS	od_const_i16[1] = {
	(INTEGER16)0};
static CO_CONST INTEGER32 CO_CONST_STORAGE_CLASS	od_const_i32[2] = {
	(INTEGER32)1L,
	(INTEGER32)0L};
static CO_CONST REAL32 CO_CONST_STORAGE_CLASS	od_const_r32[1] = {
	(REAL32)0.0000000f};

/* definition of application variables */
static UNSIGNED8 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u8[6] = {
	&(simuDO1) /* 0x2000:1 */,
	&(simuDO2) /* 0x2000:2 */,
	&(diState1) /* 0x2002:1 */,
	&(diState2) /* 0x2002:2 */,
	&(doState1) /* 0x2000:3 */,
	&(doState2) /* 0x2000:4 */};
static UNSIGNED16 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u16[11] = {
	&(simuAO1) /* 0x2001:1 */,
	&(simuAO2) /* 0x2001:2 */,
	&(readBackAO1) /* 0x2001:3 */,
	&(readBackAO2) /* 0x2001:4 */,
	&(caliStatus1) /* 0x2001:5 */,
	&(caliStatus2) /* 0x2001:6 */,
	&(aoValue1) /* 0x2001:7 */,
	&(aoValue2) /* 0x2001:8 */,
	&(dicObjId) /* 0x2006:1 */,
	&(dicAtrId) /* 0x2006:2 */,
	&(dicObjType) /* 0x2006:3 */};
static UNSIGNED32 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_u32[11] = {
	&(devType) /* 0x1000:0 */,
	&(serialNumber) /* 0x1018:4 */,
	&(version) /* 0x2003:1 */,
	&(fctTest) /* 0x2003:2 */,
	&(masterAdr) /* 0x2004:1 */,
	&(heartBeatTime) /* 0x2004:2 */,
	&(errCnt_Clr) /* 0x2005:1 */,
	&(enableConsumer) /* 0x2003:3 */,
	&(errCnt_Can) /* 0x2005:2 */,
	&(errCnt_Nmt) /* 0x2005:3 */,
	&(objValueU32) /* 0x2006:4 */};
static REAL32 CO_CONST_STORAGE_CLASS * CO_CONST 	od_ptr_r32[1] = {
	&(objValueR32) /* 0x2006:5 */};
static VIS_STRING	od_vis_string[1] = {
	&(deviceName[0])};
static UNSIGNED32	od_vis_string_len[1] = {
	32ul};

/* description of the objects */
CO_CONST CO_OBJECT_DESC_T od_description[CO_OBJ_DESC_CNT] CO_CONST_STORAGE_CLASS = {
	{ (UNSIGNED8)0u, CO_DTYPE_U32_PTR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1000:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_VAR   , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1001:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_SYNC , (UNSIGNED16)4101u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1005:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_VS_PTR   , (UNSIGNED16)0u, CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1008:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U32_EMCY , (UNSIGNED16)4116u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1014:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_EMCY , (UNSIGNED16)4117u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x1015:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1016:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_ERRCTRL, (UNSIGNED16)4118u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x1016:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_ERRCTRL, (UNSIGNED16)4118u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x1016:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U16_ERRCTRL, (UNSIGNED16)4119u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1017:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x1018:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_CONST, (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1018:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_CONST, (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1018:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_CONST, (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x1018:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_PTR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_STORE,  (UNSIGNED16)5u},/* 0x1018:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x1200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_SDO_SERVER, (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x1200:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1400:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x1400:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_RPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x1400:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_RMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1600:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)10u},/* 0x1600:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_RMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)11u},/* 0x1600:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1800:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TPDO , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x1800:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_TPDO  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE,  (UNSIGNED16)3u},/* 0x1800:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_TMAP  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x1a00:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)12u},/* 0x1a00:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_TMAP , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)13u},/* 0x1a00:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x2000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_PTR   , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_PTR   , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2000:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_PTR   , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2000:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_PTR   , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2000:4*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)6u},/* 0x2001:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U16_PTR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U16_PTR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_PTR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U16_PTR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U16_PTR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U16_PTR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:6*/ 
	{ (UNSIGNED8)7u, CO_DTYPE_U16_PTR  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:7*/ 
	{ (UNSIGNED8)8u, CO_DTYPE_U16_PTR  , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2001:8*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x2002:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_PTR   , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2002:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_PTR   , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2002:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x2003:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_PTR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2003:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_PTR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2003:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_PTR  , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2003:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x2004:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_PTR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2004:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_PTR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2004:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)8u},/* 0x2005:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U32_PTR  , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2005:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U32_PTR  , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2005:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U32_PTR  , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2005:3*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)9u},/* 0x2006:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U16_PTR  , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2006:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U16_PTR  , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2006:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U16_PTR  , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2006:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U32_PTR  , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)14u},/* 0x2006:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_R32_PTR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x2006:5*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6000:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)19u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6000:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)26u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_MAP_TR | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6000:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6002:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)18u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6002:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)27u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6002:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6003:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)17u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6003:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)28u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6003:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_BOOL_VAR , (UNSIGNED16)16u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6005:0*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6006:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)15u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6006:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)29u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6006:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6007:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)14u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6007:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)30u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6007:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6008:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)13u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6008:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)31u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6008:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6200:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)11u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)20u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6200:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6202:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)12u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6202:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)21u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6202:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6206:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)8u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6206:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)22u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6206:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6207:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)9u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6207:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)23u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6207:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6208:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)10u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6208:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)24u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)7u},/* 0x6208:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6411:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I16_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I16_VAR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6411:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)4u},/* 0x6421:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:2*/ 
	{ (UNSIGNED8)3u, CO_DTYPE_U8_VAR   , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:3*/ 
	{ (UNSIGNED8)4u, CO_DTYPE_U8_VAR   , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:4*/ 
	{ (UNSIGNED8)5u, CO_DTYPE_U8_VAR   , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:5*/ 
	{ (UNSIGNED8)6u, CO_DTYPE_U8_VAR   , (UNSIGNED16)6u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_DEFVAL,  (UNSIGNED16)5u},/* 0x6421:6*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6443:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_U8_VAR   , (UNSIGNED16)7u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6443:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_U8_VAR   , (UNSIGNED16)25u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)3u},/* 0x6443:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6444:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6444:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I32_VAR  , (UNSIGNED16)3u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6444:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6446:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)1u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6446:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I32_VAR  , (UNSIGNED16)4u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)1u},/* 0x6446:2*/ 
	{ (UNSIGNED8)0u, CO_DTYPE_U8_CONST , (UNSIGNED16)2u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_DEFVAL,  (UNSIGNED16)2u},/* 0x6447:0*/ 
	{ (UNSIGNED8)1u, CO_DTYPE_I32_VAR  , (UNSIGNED16)0u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6447:1*/ 
	{ (UNSIGNED8)2u, CO_DTYPE_I32_VAR  , (UNSIGNED16)5u, CO_ATTR_NUM | CO_ATTR_READ | CO_ATTR_WRITE | CO_ATTR_MAP_TR | CO_ATTR_MAP_REC | CO_ATTR_DEFVAL,  (UNSIGNED16)0u},/* 0x6447:2*/ 
};

/* object dictionary assignment */
CO_CONST CO_OD_ASSIGN_T od_assign[CO_OD_ASSIGN_CNT] CO_CONST_STORAGE_CLASS = {
	{ 0x1000u, 1u, 0u, CO_ODTYPE_VAR, 0u },
	{ 0x1001u, 1u, 0u, CO_ODTYPE_VAR, 1u },
	{ 0x1005u, 1u, 0u, CO_ODTYPE_VAR, 2u },
	{ 0x1008u, 1u, 0u, CO_ODTYPE_VAR, 3u },
	{ 0x1014u, 1u, 0u, CO_ODTYPE_VAR, 4u },
	{ 0x1015u, 1u, 0u, CO_ODTYPE_VAR, 5u },
	{ 0x1016u, 3u, 2u, CO_ODTYPE_ARRAY, 6u },
	{ 0x1017u, 1u, 0u, CO_ODTYPE_VAR, 9u },
	{ 0x1018u, 5u, 4u, CO_ODTYPE_STRUCT, 10u },
	{ 0x1200u, 3u, 2u, CO_ODTYPE_STRUCT, 15u },
	{ 0x1400u, 3u, 2u, CO_ODTYPE_STRUCT, 18u },
	{ 0x1600u, 3u, 2u, CO_ODTYPE_STRUCT, 21u },
	{ 0x1800u, 3u, 2u, CO_ODTYPE_STRUCT, 24u },
	{ 0x1a00u, 3u, 2u, CO_ODTYPE_STRUCT, 27u },
	{ 0x2000u, 5u, 4u, CO_ODTYPE_STRUCT, 30u },
	{ 0x2001u, 9u, 8u, CO_ODTYPE_STRUCT, 35u },
	{ 0x2002u, 3u, 2u, CO_ODTYPE_STRUCT, 44u },
	{ 0x2003u, 4u, 3u, CO_ODTYPE_STRUCT, 47u },
	{ 0x2004u, 3u, 2u, CO_ODTYPE_STRUCT, 51u },
	{ 0x2005u, 4u, 3u, CO_ODTYPE_STRUCT, 54u },
	{ 0x2006u, 6u, 5u, CO_ODTYPE_STRUCT, 58u },
	{ 0x6000u, 3u, 2u, CO_ODTYPE_ARRAY, 64u },
	{ 0x6002u, 3u, 2u, CO_ODTYPE_ARRAY, 67u },
	{ 0x6003u, 3u, 2u, CO_ODTYPE_ARRAY, 70u },
	{ 0x6005u, 1u, 0u, CO_ODTYPE_VAR, 73u },
	{ 0x6006u, 3u, 2u, CO_ODTYPE_ARRAY, 74u },
	{ 0x6007u, 3u, 2u, CO_ODTYPE_ARRAY, 77u },
	{ 0x6008u, 3u, 2u, CO_ODTYPE_ARRAY, 80u },
	{ 0x6200u, 3u, 2u, CO_ODTYPE_ARRAY, 83u },
	{ 0x6202u, 3u, 2u, CO_ODTYPE_ARRAY, 86u },
	{ 0x6206u, 3u, 2u, CO_ODTYPE_ARRAY, 89u },
	{ 0x6207u, 3u, 2u, CO_ODTYPE_ARRAY, 92u },
	{ 0x6208u, 3u, 2u, CO_ODTYPE_ARRAY, 95u },
	{ 0x6411u, 3u, 2u, CO_ODTYPE_ARRAY, 98u },
	{ 0x6421u, 7u, 6u, CO_ODTYPE_ARRAY, 101u },
	{ 0x6443u, 3u, 2u, CO_ODTYPE_ARRAY, 108u },
	{ 0x6444u, 3u, 2u, CO_ODTYPE_ARRAY, 111u },
	{ 0x6446u, 3u, 2u, CO_ODTYPE_ARRAY, 114u },
	{ 0x6447u, 3u, 2u, CO_ODTYPE_ARRAY, 117u },
};

/* static PDO mapping tables */


/**********************************************************************/
/* Structure for all OD variables */
CO_CONST CO_OD_DATA_VARIABLES_T od_data_variables = {
   &od_const_u8[0]
    , &od_const_u16[0]
    , &od_const_u32[0]
    , &od_const_r32[0]
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
    , &od_ptr_r32[0]
    , NULL
    , NULL
    , &od_vis_string[0]
    , &od_vis_string_len[0]
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
    retVal = coErrorCtrlInit(od_const_u16[1], 2u);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize emergency */
    retVal = coEmcyProducerInit();
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize sync */
    retVal = coSyncInit(od_const_u32[1]);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize TPDO 1 */
    retVal = coPdoTransmitInit(1u, od_const_u8[3], 0u, 0u, 0u, NULL);
    if (retVal != RET_OK) {
        return(retVal);
    }
    /* initialize RPDO 1 */
    retVal = coPdoReceiveInit(1u, od_const_u8[3], 0u, 0u, NULL);
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
