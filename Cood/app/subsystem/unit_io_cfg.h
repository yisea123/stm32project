/*
 * unit_io_cfg.h
 *
 *  Created on: 2017骞�5鏈�22鏃�
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_IO_CFG_H_
#define SUBSYSTEM_UNIT_IO_CFG_H_

extern const T_UNIT IOCfg;

#define AO_DEFAULT_OUT		0

typedef enum
{
	RO_CLR = 0,
	RO_SET = 1,
}RO_STATE;


typedef enum
{
	DI_CLR = 0,
	DI_SET = 1,
}DI_STATE;


typedef enum
{
	NONE_IO,
	DI_BOARD,
	DO_BOARD,
	AI_BOARD,
	AO_BOARD,

}BoardType;

typedef enum
{
	TYPE_DI_BOARD = 0x00,
	TYPE_DO_BOARD = 0x01,
	TYPE_AO_BOARD = 0x02,
	TYPE_NONE,
}IO_BoradType;

typedef enum
{
	DO_NONE,
	DO_ALARM_LOW,
	DO_ALARM_HIGH,
	DO_EVENT,
	DO_PRETREAT,
	DO_SWITCH,
	DO_FUNC_MAX,
}FuncDO;


typedef enum
{
	IN_MODE_NONE =0x0,
	IN_MODE_CLEAN = 0x01,
	IN_MODE_CALIBRATION=0x02,
	IN_MODE_MEASURE=0x03,
	IN_MODE_DRAIN=0x04,
	IN_MODE_FLUSH=0x05,
	IN_MODE_PRIME=0x06,
	IN_MODE_DISABLE_SCH=0x07,
	IN_MODE_MAX,
}IN_FUNCTION;


extern uint8_t doFuncValue[DO_FUNC_MAX];
typedef enum
{
	AO_NONE,
	AO_MEASURE0_ACTIVE,
	AO_MEASURE0_HOLD,
	AO_MEASURE0_TRANSFER,
	AO_MEASURE1_ACTIVE,
	AO_MEASURE1_HOLD,
	AO_MEASURE1_TRANSFER,
	AO_MAX,
}FuncAO;


typedef enum
{
	SIMU_NONE,
	SIMU_IO,
}SimuSrc;

typedef enum
{
	BOARD_OK,
	BOARD_NOTMAP,
	BOARD_OFFLINE,
	BOARD_PREOPERATIONAL,
}StatusBoard;


typedef struct
{


//set cfg
	uint8_t*		ptrFuncSelChn1;
	uint8_t*		ptrFuncSelChn2;

	float*			ptrAOSrcChn1_Low;
	float*			ptrAOSrcChn2_Low;
	float*			ptrAOSrcChn1_High;
	float*			ptrAOSrcChn2_High;

	//ro below

	uint16_t* 		ptrAdr;
	uint8_t*		ptrBoardType;
	uint8_t*		ptrDicAdrMap;
	uint8_t*		ptrDicAdrIdChn1;
	uint8_t*		ptrDicAdrIdChn2;
	uint16_t*		ptrStatus;




	//set simu
	uint8_t*		ptrSimuCfgChn1;
	uint8_t*		ptrSimuCfgChn2;
	float*			ptrAOSrcChn1_Simu;
	float*			ptrAOSrcChn2_Simu;
	uint16_t* 		ptrAO_Chn1_Simu;
	uint16_t*		ptrAO_Chn2_Simu;

	uint8_t* 		ptrDI_Chn1_Simu;
	uint8_t*		ptrDI_Chn2_Simu;
	uint8_t* 		ptrDO_Chn1_Simu;
	uint8_t*		ptrDO_Chn2_Simu;
}BoardCfg;

typedef struct
{
	uint16_t adr;
	uint16_t setValue;
	uint16_t readBack;
	uint16_t resetCali;
	uint16_t caliStatus;
}Calibration_AO;

enum
{
	OBJ_IDX_BoardCfg0 = 0,
	OBJ_IDX_BoardCfg15 = 15,
	OBJ_IDX_BoardMap0 = 16,
	OBJ_IDX_BoardMap15 = 31,


};

extern uint8_t doValue[32];
extern uint8_t diValue[32];
extern uint16_t aoValue[32];

extern uint16_t	heartBeat;
extern uint16_t     AO_Event_Trig;

extern uint8_t		IO_Connected[MAX_IO_BOARD_NUM];
extern uint8_t		IO_BoardType[MAX_IO_BOARD_NUM];
uint16_t	IO_Status[MAX_IO_BOARD_NUM];

extern uint16_t 	IO_Poll_Time;
extern uint8_t		IO_BoardEnable[MAX_IO_BOARD_NUM];
extern uint8_t		IO_BD_TypeSetting[MAX_IO_BOARD_NUM];
extern uint8_t		IO_FuncSelChn1[MAX_IO_BOARD_NUM];
extern uint8_t		IO_FuncSelChn2[MAX_IO_BOARD_NUM];
extern uint8_t		DI_Chn1_Value[MAX_IO_BOARD_NUM];
extern uint8_t		DI_Chn2_Value[MAX_IO_BOARD_NUM];
extern uint8_t		DO_Chn1_Value[MAX_IO_BOARD_NUM];
extern uint8_t		DO_Chn2_Value[MAX_IO_BOARD_NUM];
extern uint16_t		AO_Chn1_Value[MAX_IO_BOARD_NUM];
extern uint16_t		AO_Chn2_Value[MAX_IO_BOARD_NUM];
extern uint8_t		IO_DO_Chn1_Simu[MAX_IO_BOARD_NUM];
extern uint8_t		IO_DO_Chn2_Simu[MAX_IO_BOARD_NUM];
extern uint8_t		IO_DI_Chn1_Simu[MAX_IO_BOARD_NUM];
extern uint8_t		IO_DI_Chn2_Simu[MAX_IO_BOARD_NUM];

extern Calibration_AO AO_Calibration_Chn[2];
extern uint32_t 	TickCntCanOpen[2];
extern uint32_t 	TickDelayCanOpen[2];
extern uint16_t  	masterState;
extern uint8_t 		slaveState[MAX_IO_BOARD_NUM];
extern uint8_t		IO_SimuCfgChn1[MAX_IO_BOARD_NUM];
extern uint8_t		IO_SimuCfgChn2[MAX_IO_BOARD_NUM];
extern uint16_t		IO_AO_Chn1_Simu[MAX_IO_BOARD_NUM];
extern uint16_t		IO_AO_Chn2_Simu[MAX_IO_BOARD_NUM];
extern uint16_t		IO_AO_Chn1_Simu[MAX_IO_BOARD_NUM];
extern uint16_t		IO_AO_Chn2_Simu[MAX_IO_BOARD_NUM];
extern float		IO_AOSrcChn1_Low[MAX_IO_BOARD_NUM];
extern float		IO_AOSrcChn2_Low[MAX_IO_BOARD_NUM];
extern float		IO_AOSrcChn1_High[MAX_IO_BOARD_NUM];
extern float		IO_AOSrcChn2_High[MAX_IO_BOARD_NUM];

extern Calibration_AO AO_Calibration_Chn1;
extern Calibration_AO AO_Calibration_Chn2;


enum
{
	ID_FAKEINIT = 0,
	ID_HBSTATE = 1,
	ID_IO_MONITOR = 2,
	IO_ERR_SET = 3,
	IO_APP_INIT	= 4,
	IO_ADO_ERR = 5,
	IO_TICK_CONSUMER = 6,
	ID_IOCAN_MAX,
};

extern uint32_t	timeCanIO_ST[ID_IOCAN_MAX];
extern uint32_t	timesErrCnt[ID_IOCAN_MAX];
enum
{
	OBJ_IDX_CALI_AO_CHN1,
	OBJ_IDX_CALI_AO_CHN2,
	OBJ_IDX_CLR_ERROR,
	OBJ_IDX_SIMU_MEAS_FUNC,
	OBJ_IDX_MASTER_STATE = 6,
	OBJ_IDX_SLAVE_STATE = 7,
	OBJ_IDX_FAKE_INI = 8,
	OBJ_IDX_MASTER_SYNCTIME = 9,
	OBJ_IDX_SIMU_CFG_START = 22,
	OBJ_IDX_SIMU_CFG_END = 29,
	OBJ_IDX_SLAVE_ENABLE = 30,
	OBJ_IDX_SLAVE_TYPECFG = 31,
	OBJ_IDX_HEARTBEAT = 50,
	OBJ_IDX_IO_CONSUMER	= 54,
	OBJ_IDX_IO_CON = 55,
	OBJ_IDX_IO_RAW_VALUE_U32 = 56,
	OBJ_IDX_IO_RAW_VALUE_FLOAT = 57,
};

typedef struct
{
	float measure;
	uint16_t rangeIdx;
	uint16_t status;
}Measurements;


enum
{
	STATE_INIT,
	STATE_SCAN,
	STATE_SETCHECK,
	STATE_HEART,
	STATE_PREOP,
	STATE_MAP,
	STATE_OPERATION,
	STATE_OP_HARTBEAT,
	STATE_CAN_MAX,
};


enum
{
	E_CAN_OPEN_DIAG = 0xFF00,
	E_CAN_OPEN_SLAVE_EN = 0xFF0000,
	E_CAN_OPEN_SLAVE_REFRESH = 0xFF0001,

	E_CAN_OPEN_HEART_BEAT = 0xFF0002,
	E_CAN_OPEN_SIMU_UPDATE = 0xFF0003,
	E_CAN_OPEN_SLAVE_UPDATE = 0xFF0005
};

extern uint16_t canOpenState;
uint16_t SdoRead(uint8_t adr, uint16_t idx, uint8_t subIdx, uint8_t* data, uint16_t len);
uint16_t SdoWrite(uint8_t adr, uint16_t idx, uint8_t subIdx, uint8_t* data, uint16_t len);

void FakeInit_Setting(void);

uint16_t CaliAOCurrent(uint8_t adr, uint8_t chn, uint16_t val);
uint16_t CaliAOReadback(uint8_t adr, uint8_t chn,  uint16_t val);
uint16_t CaliAOReset(uint8_t adr, uint8_t chn);
uint16_t CaliAOStatus(uint8_t adr, uint8_t chn, uint16_t* ptrStatus);
uint16_t Initialize_IOCfg(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_IOCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Get_IOCfg(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

uint16_t DiagIOState(uint16_t nodeId, uint8_t id, uint8_t nmtState);
uint16_t CheckIOErrorSetting(void);
void AlignFunc_IO(void);
void CanOpenOnLine(void);
void CanOpenOffLine(void);

void UpdateAOSrc(float val, uint16_t rangeIdx, uint16_t status, uint16_t src);
void UpdateAOValue(void);
void UpdateDOValue(uint8_t* doValue);
void UpdateDIValue(void);


#define IO_Put(objId,attributeIndex,ptrValue)		Put_IOCfg(&IOCfg, objId, attributeIndex, ptrValue)
#define IO_Get(objId,attributeIndex,ptrValue)		Get_IOCfg(&IOCfg, objId, attributeIndex, ptrValue)
#endif /* SUBSYSTEM_UNIT_IO_CFG_H_ */
