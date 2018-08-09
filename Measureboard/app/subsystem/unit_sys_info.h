/*
 * unit_sys_info.h
 *
 *  Created on: 2016��11��1��
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_SYS_INFO_H_
#define SUBSYSTEM_UNIT_SYS_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif
#define CST_LEN					16


#define INFO_LEN				32
extern const T_UNIT sysInfo;
#define BURST_LEN_CFG			2



enum
{
	STANDARD_VERSION = 0,
	ADVANCED_VERSION = 1,
};


#define STANDARD_VERSION_STARTACTION	172
#define ADVANCED_VERSION_STARTACTION	173
#define STANDARD_VERSION_STOPACTION		172
#define ADVANCED_VERSION_STOPACTION		173


extern uint16_t instrumentType;
// Example of __DATE__ string: "Jul 27 2012"
// Example of __TIME__ string: "21:06:19"

#define COMPUTE_BUILD_YEAR \
    ( \
        (__DATE__[ 7] - '0') * 1000 + \
        (__DATE__[ 8] - '0') *  100 + \
        (__DATE__[ 9] - '0') *   10 + \
        (__DATE__[10] - '0') \
    )


#define COMPUTE_BUILD_DAY \
    ( \
        ((__DATE__[4] >= '0') ? (__DATE__[4] - '0') * 10 : 0) + \
        (__DATE__[5] - '0') \
    )


#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')


#define COMPUTE_BUILD_MONTH \
    ( \
        (BUILD_MONTH_IS_JAN) ?  1 : \
        (BUILD_MONTH_IS_FEB) ?  2 : \
        (BUILD_MONTH_IS_MAR) ?  3 : \
        (BUILD_MONTH_IS_APR) ?  4 : \
        (BUILD_MONTH_IS_MAY) ?  5 : \
        (BUILD_MONTH_IS_JUN) ?  6 : \
        (BUILD_MONTH_IS_JUL) ?  7 : \
        (BUILD_MONTH_IS_AUG) ?  8 : \
        (BUILD_MONTH_IS_SEP) ?  9 : \
        (BUILD_MONTH_IS_OCT) ? 10 : \
        (BUILD_MONTH_IS_NOV) ? 11 : \
        (BUILD_MONTH_IS_DEC) ? 12 : \
        /* error default */  99 \
    )

#define COMPUTE_BUILD_HOUR ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define COMPUTE_BUILD_MIN  ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define COMPUTE_BUILD_SEC  ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0')


#define BUILD_DATE_IS_BAD (__DATE__[0] == '?')

#define BUILD_YEAR  ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_YEAR)
#define BUILD_MONTH ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_MONTH)
#define BUILD_DAY   ((BUILD_DATE_IS_BAD) ? 99 : COMPUTE_BUILD_DAY)

#define BUILD_TIME_IS_BAD (__TIME__[0] == '?')

#define BUILD_HOUR  ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_HOUR)
#define BUILD_MIN   ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_MIN)
#define BUILD_SEC   ((BUILD_TIME_IS_BAD) ? 99 :  COMPUTE_BUILD_SEC)




typedef struct
{
	uint16_t DeviceID;
	uint8_t compileDate[CST_LEN];
	uint8_t compileTime[CST_LEN];
	uint8_t DeviceType[CST_LEN];
	uint8_t Type_Info[CST_LEN];
	uint8_t SW_Info[3];
	uint8_t HW_Info[3];
	uint32_t VersionCode;
}SysInfoST;


typedef struct
{
	uint8_t* adr;
	uint8_t  len;
}BurstCfg;


extern const SysInfoST _sysInfo;
extern const uint16_t   burstLength;
extern const BurstCfg 	burstCfg[BURST_LEN_CFG];
extern uint16_t deviceStatus;
extern uint16_t	autoReset;

enum
{
	FVT_NONE,
	FVT_SEND_UART1,
	FVT_SEND_UART2,
	FVT_SEND_UART3,
	FVT_SEND_CAN1,
	FVT_SEND_CAN2,
};

uint16_t Initialize_SysInfo(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Get_SysInfo(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
uint16_t Put_SysInfo(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);
#ifdef __cplusplus
 }
#endif



#endif /* SUBSYSTEM_UNIT_SYS_INFO_H_ */
