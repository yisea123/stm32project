/*
 * unit_parameter.h
 *
 *  Created on: 2016��9��22��
 *      Author: vip
 */

#ifndef UNIT_PARAMETER_H_
#define UNIT_PARAMETER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define PARA_TEST_IDLE 0 //
#define PARA_TEST_ING 1 //
#define PARA_TEST_OK 2 //
#define PARA_TEST_ERROR 3 //
#define PARA_TEST_FINISH 4 //

#define PARA_WELD_IDLE 0 //
#define PARA_WELD_ING 1 //

 typedef enum
 {
 	TEST_IDLE = 0,
 	TEST_ING = 1,
 	TEST_OK = 2,
	TEST_WARNING =3,
 	TEST_ERROR = 4,
	TEST_TIMEOUT,
	TEST_FINISHED,
 }testStatus;

enum _E_PARAMETER_DC_INDEXES
{
  PARAMETER_DC_IDX_weldSetParameter,
  PARAMETER_DC_IDX_CONSTANT,
  PARAMETER_DC_IDX_DYNAMIC,
  PARAMETER_DC_IDX_STATIC_RARE,
  PARAMETER_DC_IDX_NUMBER_OF_DATACLASSES
};

typedef enum
{
	USH_USR_DISCONNECT = 0,
	USH_USR_HW_ERROR,
	USH_USR_ENUMERATION,
	USH_USR_APPLICATION,
	USH_USR_FS_READY
}USB_STATUS;
typedef struct
{
	unsigned char cmd           : 3;
	unsigned char type          : 2;//0-> norma frame, 1-> start multi frame, 2-> during multi frame, 3-> end of multi frame
	unsigned char status        : 1;
	unsigned char length        : 2;
} HeadBit;

typedef struct _MessageData
{
	unsigned char head;
	unsigned char subID;
	unsigned char objID;
	unsigned char frameID;//normal frame -> attribute; multi frame-> *4 = the memcpy start index;
	unsigned char data[4];// return the actual changed value
}MessageData;

enum _cmd_idx{
	READ_OBJ,
	WRITE_OBJ,
	GET_DESCRIPTION,
	GET_UPRANGE,
	GET_LOWRANGE,
};

enum _frame_type{
	WHOLE_FRAME,
	FIRST_FRAME,
	MID_FRAME,
	LAST_FRAME,
};


typedef enum _copyStatus
{
	COPY_IDLE = 0,
	COPY_ING = 1,
	COPY_OK = 2,
	COPY_ERROR = 3,
	COPY_TIMEOUT = 4,
}COPYSTATUS;



 //-------------------------------------------------------------------------------------------------
 //! structure of CONSTANT block
 typedef struct _T_PARAMETER_CONSTANT
 {
    uint8_t getWireSize[12];
    uint8_t getWireType[12];
    uint8_t getProcessType[12];
    uint8_t getGasType[12];
    uint8_t getUnitCode[12];
 } T_PARAMETER_CONSTANT;



 //-------------------------------------------------------------------------------------------------
 //! structure of STATIC_RARE block
 typedef struct _T_PARAMETER_STATIC_RARE
 {
    uint8_t swVersion[4];
    uint8_t regData[4];
    uint8_t company[12];
    uint8_t owner[8];
    uint32_t deviceID;
    uint32_t factoryTime;
    uint8_t deviceType;
    uint32_t adjustRTCFactor[6];
    uint16_t speedMax;
    uint16_t currentMax;
    uint16_t voltMax;
    uint16_t energyMax;
 } T_PARAMETER_STATIC_RARE;


 //-------------------------------------------------------------------------------------------------
 //! structure of WeldSetParameter block
 typedef struct _T_PARAMETER_WELD_SET_PARAMETER
 {
    uint16_t setMode;
    uint16_t setCurrent;
    uint16_t setWorkPoint;
    uint16_t setWeldSpeed;
    uint16_t setWeldMask;
    uint16_t strikeSpeed;
    uint16_t startTime;
    uint16_t strikeTime;
    uint16_t upslopeTime;
    uint16_t downSlopeTime;
    uint16_t craterTime;
    uint16_t startMoveDelay;
    uint16_t stopMoveDelay;
    uint16_t strikeTrim;
    uint16_t strikeWorkpoint;
    uint16_t startTrim;
    uint16_t startWorkpoint;
    uint16_t craterTrim;
    uint16_t craterWorkpoint;
    uint16_t strikeInch;
    uint16_t codeInchSpeed;
 } T_PARAMETER_WELD_SET_PARAMETER;



 //-------------------------------------------------------------------------------------------------
 //! structure of DYNAMIC block
 typedef struct _T_PARAMETER_DYNAMIC
 {
    uint8_t usbConnectedStatus;
    uint8_t sd_status;
    uint16_t usbInit_retry;
    uint16_t usbWrite_retry;
    uint8_t usbWriteStatus;
    uint8_t copy_status;
    uint16_t copy_filenum;
    uint16_t totalNum_Copy;
    uint32_t resetFileID;
    uint8_t loadRomDefault;
    uint8_t saveNV;
    uint16_t weldCurrent;
    uint16_t weldWorkPoint;
    uint16_t weldSpeed;
    uint16_t weldEnergy;
    uint16_t inchSpeed;
    uint16_t inchCurrent;
    uint16_t shortCount;
    uint16_t loseArcNum;
    uint16_t weldMode;
    uint16_t can_innum;
    uint16_t can_validatenum;
    uint32_t setDifferenceTime;
    uint8_t weldStatus;
    uint8_t InitIIC_Data;
    uint16_t IIC_Status;
    uint16_t simulateSave[2];
 } T_PARAMETER_DYNAMIC;

extern uint16_t systemReset;

extern const T_UNIT parameter;

enum
{
	COPY_ID =0,
	NEW_ID = 1,
	ERR_CNT= 3,
	MAX_ID,
};

extern T_PARAMETER_WELD_SET_PARAMETER parameterWeldSetParameter;
extern T_PARAMETER_STATIC_RARE parameterStaticRare;
extern T_PARAMETER_DYNAMIC parameterDynamic;

void HandleCommunicationSRV(const uint8_t* ptrData,uint32_t stdId);
void RenewHanjieParaSRV(void);

uint16_t Initialize_PARAMETER(const T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Get_PARAMETER(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex, void * ptrValue);
uint16_t Put_PARAMETER(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex, void * ptrValue);

void SetErrorFileCount(uint32_t num);
uint32_t GetStartCopyFileID(void);
uint32_t GetEndCopyFileID(void);
void SetStartCopyFileID(uint32_t id);
uint32_t GetNewFileID(void);
uint32_t UpdateNewFileID(void);
void SetNewFileID(uint32_t id);

#define parameter_Put(x,y,z)		parameter.Put(&parameter, x,y,z)
#define parameter_Get(x,y,z)		parameter.Get(&parameter, x,y,z)

#ifdef __cplusplus
 }
#endif


#endif /* UNIT_PARAMETER_H_ */
