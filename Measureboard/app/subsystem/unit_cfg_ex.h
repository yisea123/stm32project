/*
 * unit_cfg_ex.h
 *
 *  Created on: 2018Äê4ÔÂ20ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_CFG_EX_H_
#define SUBSYSTEM_UNIT_CFG_EX_H_


extern const T_UNIT ExCfg;

extern float  failedMeasureVal;
extern uint16_t  retryIimesMax;
extern uint16_t  failedMeasureFlag;
extern uint16_t  failedPostEnable;
uint16_t Initialize_ExCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);


extern float  measLimitRange[MEAS_RANGE_MAX][2];

#endif /* SUBSYSTEM_UNIT_CFG_EX_H_ */
