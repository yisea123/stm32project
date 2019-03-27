/*
 * unit_weld_cfg.h
 *
 *  Created on: 2019Äê3ÔÂ27ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_WELD_CFG_H_
#define SUBSYSTEM_UNIT_WELD_CFG_H_

#define	MAX_SEG_SIZE 20
#define	MAX_CALI_CURR 20

enum
{
	WELD_IDLE,
	WELD_ING,
};
extern SegWeld segWeld[MAX_SEG_SIZE];

typedef struct
{
	float homeSpeed;
	float jogSpeed;
	float accSpeedPerSeond;
}MotorSpeed;


typedef struct
{
	uint16_t preGasTime;
	uint16_t postGasTime;
	uint16_t preVolt;
	uint16_t preDelay;
	uint16_t upSlopeAcc;
	uint16_t upSlopeAccTime;
	uint16_t downSploeAcc;
	uint16_t downSlopeAcc;
}WeldProcessCfg;


extern const T_UNIT weldCfg;

uint16_t Initialize_WeldCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_WeldCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
                     void * ptrValue);

#endif /* SUBSYSTEM_UNIT_WELD_CFG_H_ */
