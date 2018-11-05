/*
 * unit_hmi_setting.h
 *
 *  Created on: 2017Äê7ÔÂ12ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_HMI_SETTING_H_
#define SUBSYSTEM_UNIT_HMI_SETTING_H_

enum
{
	OBJ_IDX_U8 = 0,
	OBJ_IDX_FACTORYSET = 4,
	OBJ_IDX_U8_RAM = 5,
};
enum
{
	ATR_ID_RTC = 0,
};
extern const T_UNIT HmiCfg;
extern uint8_t 	hmi_U8_Ram[16];
uint16_t AlignCfgtoHMI(void);

uint16_t Initialize_HmiCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_HmiCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

extern float 	measLimitRange[MEAS_RANGE_MAX][2];
#define HMI_Get(objId,attributeIndex,ptrValue)		Get_T_UNIT(&HmiCfg,objId,attributeIndex,ptrValue)
#define HMI_Put(objId,attributeIndex,ptrValue)		Put_HmiCfg(&HmiCfg,objId,attributeIndex,ptrValue)


#endif /* SUBSYSTEM_UNIT_HMI_SETTING_H_ */
