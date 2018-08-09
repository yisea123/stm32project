/*
 * unit_hmi_setting.h
 *
 *  Created on: 2017Äê7ÔÂ12ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_HMI_SETTING_H_
#define SUBSYSTEM_UNIT_HMI_SETTING_H_



extern const T_UNIT HmiCfg;

uint16_t Initialize_HmiCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);

extern float 	measLimitRange[MEAS_RANGE_MAX][2];
#define FLOWACT_Put(objId,attributeIndex,ptrValue)		Put_FlowAct(&flowAct,objId,attributeIndex,ptrValue)




#endif /* SUBSYSTEM_UNIT_HMI_SETTING_H_ */
