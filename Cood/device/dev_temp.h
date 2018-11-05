/*
 * dev_temp.h
 *
 *  Created on: 2016��8��2��
 *      Author: pli
 */

#ifndef INC_DEV_TEMP_H_
#define INC_DEV_TEMP_H_


#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	TARGET_CHN1 = 0,
	PICK_CHN,
	ENV_CHN,
	MAX_TEMP_CHN = ENV_CHN,
	ON_BOARD = 3,
	TEMP_MAX_CHN,
}TempChn;
#define TARGET_CHN PICK_CHN

#define LEAK_CHN		0x03u
#define VOLT_CHN		0x07u
#define MAX_ADC_CHN		0x08u
typedef enum
{
	ATR_IDX_PREHEAT = 0,
	ATR_IDX_REQ0,
	ATR_IDX_REQ0TIME,
	ATR_IDX_REQ1,
	ATR_IDX_REQ1TIME,
	ATR_IDX_DURINGTIME,
}ATR_TEMPCTRL;
extern uint16_t ADCValue[MAX_ADC_CHN];
int16_t GetTempChn(uint16_t chn);
uint16_t GetResistor(uint16_t chn);
void CalcGainOffset(RTS100* temp);
uint16_t GetADCValueIdx(uint16_t chn);
uint16_t GetVoltageIdx(uint16_t chn);
void GetAllTemperature(void);

#ifdef __cplusplus
}
#endif


#endif /* INC_DEV_TEMP_H_ */
