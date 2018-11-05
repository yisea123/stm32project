/*
 * unit_meas_cfg.h
 *
 *  Created on: 2016��8��19��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_MEAS_CFG_H_
#define SUBSYSTEM_UNIT_MEAS_CFG_H_




#define IDX_REG_28		0x28
#define IDX_REG_29		0x29



#define ATR_REGCFG_MAX			0x3B
#define ATR_REGCFG_PUT_MAX		0x4

typedef enum _AD_TEST_STATE_
{
	e_ad_test_t1 = 0,
	e_ad_test_t2,
	e_ad_test_t3,
	e_ad_test_t4,
	e_ad_test_t5,
	e_ad_test_t6,
	e_ad_test_t7,
	e_ad_test_t8,
	e_ad_test_t9,
	e_ad_test_t10,
	e_ad_test_t11,
	e_ad_test_t12,
	e_ad_test_tmax,
} E_AD_TEST_STATE;



typedef enum _ENUM_MEA_STEP {

	MEA_STEP_START = 0,
	MEA_STEP_660nm_LONG=MEA_STEP_START,
//	MEA_STEP_880nm_LONG,
	MEA_STEP_660nm_SHORT,
//	MEA_STEP_880nm_SHORT,

	MEA_STEP_DONE,
	MEA_STEP_FINAL = 15,
	MEA_STEP_FINAL_SW,
	MEA_DATA_MAX = MEA_STEP_DONE,
} ENUM_MEA_STEP;


#ifdef __cplusplus
 extern "C" {
#endif

 enum
 {
 	MEASURE_DATA_NONE = 0,
 	MEASURE_DATA_INVALID_MSK = 0x0F,
 	MEASURE_DATA_VALID_MSK = 0xF0,
 };
enum
{
	OBJ_IDX_REG_MEA,
	OBJ_IDX_REG_REF,
	OBJ_IDX_GAIN_MEA,
	OBJ_IDX_GAIN_REF,
	OBJ_IDX_ADTIME,
	OBJ_IDX_DATA_GET,

	OBJ_IDX_TEST_GAIN = 17,
};
typedef struct{
	uint16_t reCalcGain;
	uint16_t refreshAbs;
}CalibrationAct;

typedef struct{
	float lastAbs;
	float firstAbs;
	float diffAbs;
	float warnAbs;
}AbsInfo;
extern const T_UNIT measCfg;
extern CalibrationAct calAct;
extern  uint16_t measTimeCfg[e_ad_test_tmax];
extern  uint32_t gainMask;
extern  AbsInfo  absInfo[MEA_STEP_DONE];
extern volatile uint32_t measResultPrint;

extern  volatile uint16_t dataRdyTime;

uint16_t Initialize_MeasCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);

uint16_t Put_MeasCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
		void * ptrValue);
uint16_t Get_MeasCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
		void * ptrValue);

void CheckBlank_Optics(float* ptrBlank);
void CheckLedError(void);
uint16_t UpdateGainSetting(uint16_t type);
uint16_t UpdateMeaGain(const int32_t rawAd, uint8_t idx);
uint16_t UpdateRefGain(const int32_t rawAd, uint8_t idx);
uint16_t UpdateAbsInfo(const float* absFloat);
#ifdef __cplusplus
 }
#endif


#endif /* SUBSYSTEM_UNIT_MEAS_CFG_H_ */