/*
 * unit_cfg_ex.h
 *
 *  Created on: 2018Äê4ÔÂ20ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_CFG_EX_H_
#define SUBSYSTEM_UNIT_CFG_EX_H_

enum
{
	OBJ_IDX_LOAD_EPA = 10,
};

enum
{
	STORE_TO_EEP_IM,
	STORE_TMP_FACTOR,
	RECOVER_FROM_TMP,
};

extern const T_UNIT ExCfg;
extern uint16_t	schDelayTime;
extern uint16_t	caliTimesMax;
extern float  failedMeasureVal;
extern uint16_t  retryIimesMax;
extern uint16_t  failedMeasureFlag;
extern uint16_t  failedPostEnable;
extern uint16_t  loadEPACfg;
extern float     std1VeriDeviation;
extern uint16_t  std1VeriEnable;
extern uint16_t  measPostEnable;
extern uint16_t  caliPostMeas;
extern uint16_t  measPostStep[	MEAS_RANGE_MAX];
extern float 	filterKfactor;

uint16_t Initialize_ExCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_ExCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

extern float  measLimitRange[MEAS_RANGE_MAX][2];


#define EX_Get(objId,attributeIndex,ptrValue)		Get_T_UNIT(&ExCfg,objId,attributeIndex,ptrValue)
#define EX_Put(objId,attributeIndex,ptrValue)		Put_ExCfg(&ExCfg,objId,attributeIndex,ptrValue)


typedef struct
{
	float		calibrationOffset_Long[2];
	float		calibrationSlope_Long[2];
	float		calibrationOffset_Short[MEAS_RANGE_MAX];
	float		calibrationSlope_Short[MEAS_RANGE_MAX];
}CaliFactor;

#endif /* SUBSYSTEM_UNIT_CFG_EX_H_ */
