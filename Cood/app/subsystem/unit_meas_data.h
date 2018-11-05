/*
 * unit_meas_data.h
 *
 *  Created on: 2018��2��9��
 *      Author: vip
 */

#ifndef APP_UNIT_MEAS_DATA_H_
#define APP_UNIT_MEAS_DATA_H_



enum
{
	ID_VAL_REF,//0
	ID_VAL_MEAS,//1
	ID_VAL_LED_LV0,//2
	ID_VAL_LED_LV1,//3
	ID_PHOTO_LED_LIGHT_MAX,
	ID_DARK_REF = ID_PHOTO_LED_LIGHT_MAX,//4
	ID_DARK_MEAS,//5
	ID_DARK_LED_LV0,//6
	ID_DARK_LED_LV1,//7
	ID_PHOTO_LED_MAX,
	ID_TEMPERATURE0 = ID_PHOTO_LED_MAX,//8
	ID_TEMPERATURE_REF,//9
	ID_TEMPERATURE1,//10
	ID_PCB_TEMP,//11
	ID_ADC_MAX,
};
#define ID_HOUSE_TEMPERATURE   ID_TEMPERATURE0
#define ID_DIG_TEMPERATURE      ID_TEMPERATURE1


enum
{
    AD_NONE_ACT = 0x0,
    AD_MEASURE_ACT = 0x05,
    AD_BLANK_ACT = 0x0A,
    AD_MEASURE_ACT_FINISH = 0x15,    
    AD_BLANK_ACT_FINISH = 0x1A,
};
enum
{
	ID_PUMP_POS,
	ID_PUMP_STATUS,
	ID_PUMP_MAX
};

typedef struct
{
	uint16_t idleTemp;
	uint16_t preHeatTemp;
    uint16_t tempStep2;
	uint16_t measureTemp;
	uint16_t digTemp;
	uint16_t drainTemp;
    uint16_t cleanTemp;
    uint16_t tempDecrease;
}HeatCfg;


typedef struct{
	uint16_t cool_delay;
	uint16_t cool_buffer;
	uint16_t heat_delay;
	uint16_t heat_buffer;
}HeatCtrlCfg;



typedef struct
{
    float std0;
    float std1;
    float offset;
    float kFactor;
}CalibrationFactor;

enum
{
    ID_LED0_POS,
    ID_LED1_POS,
    ID_MEAS2_POS,
    ID_LED_POS_MAX,
};

#define LONG_SATURATION 		0x0F
#define SHORT_SATURATION 		0xF0
#define NONE_SATURATION			0x00

enum
{
    OBJ_IDX_PIDCFG = 10,
    OBJ_IDX_REF = 20,
    OBJ_IDX_MEAS = 21,

};
#define HEAT_NO_ADJUST			    0x00
#define HEAT_NEEDS_SMALLER			0x01
#define HEAT_NEEDS_STRONGER			0x02


extern volatile uint16_t adcAction;
extern volatile uint8_t led0DetectLiquid;
extern volatile uint8_t led1DetectLiquid;
extern volatile uint8_t chkLeakageCfg;
extern volatile uint8_t chkSafetyLockCfg;
extern uint32_t  ledDetectLimit[ID_LED_POS_MAX];

extern HeatCfg	heatCfg;
extern uint16_t adValChn[ID_ADC_MAX];
extern uint16_t advalFinal[ID_ADC_MAX];
extern uint16_t tempIdle[3];
extern uint16_t tempMeas[3];
extern volatile uint32_t PumpPosStatus[ID_PUMP_MAX];
extern volatile uint16_t leakageReadback;
extern volatile uint16_t safetyLockReadback;
extern volatile uint16_t ceramicReadback;
extern volatile uint16_t heatStatusReadback;
extern volatile uint16_t 	measDataSaturation;

extern float  realTimeAbs;
extern CalibrationFactor caliFactor[4];
extern float  realTimeConcentration;
extern float  tmpConcentration;
extern uint32_t caliTime;
extern const T_UNIT measData;
void PIDInit(int32_t tempReq, uint8_t i);
void Position_PID_Init(uint16_t target);
uint16_t Initialize_MeasData(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_MeasData(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

float CalcConcentraction(float valN, const CalibrationFactor* ptrFactor);


#endif /* APP_UNIT_MEAS_DATA_H_ */
