/*
 * Stack definitions for master1_b - generated by CANopen DeviceDesigner 2.7.0
 * ���� ʮ�� 13 09:28:40 2017
 */

/* protect against multiple inclusion of the file */
#ifndef GEN_DEFINE_H
#define GEN_DEFINE_H 1

/* some information about the tool that has generated this file */
#define GEN_TOOL_NAME	CANOPEN_DEVICEDESIGNER
#define GEN_TOOL_VERSION	0x020700ul

/* Line Settings */
#define CO_MULTI_LINE_CNT	1u
#define CANOPEN_SUPPORTED	1u
/* Number of objects per line */
#define CO_OBJECTS_LINE_0_CNT	83u
#define CO_OBJECT_COUNTS	83u
#define CO_COB_COUNTS	60u
#define CO_TXPDO_COUNTS	16u
#define CO_RXPDO_COUNTS	16u
#define CO_SSDO_COUNTS	1u
#define CO_CSDO_COUNTS	2u
#define CO_ASSIGN_COUNTS	0u
#define CO_GUARDING_COUNTS	CO_ASSIGN_COUNTS
#define CO_ERR_CTRL_COUNTS	16u
#define CO_ERR_HIST_COUNTS	0u
#define CO_EMCY_CONS_COUNTS	2u
#define CO_NODE_IDS	2
#define CO_NODE_ID_FUNCTIONS NULL

/* Definition of numbers of CANopen services */
#define CO_NMT_MASTER	1u
#define CO_SDO_SERVER_CNT	1u
#define CO_SDO_CLIENT_CNT	2u
#define CO_PDO_TRANSMIT_CNT	16u
#define CO_PDO_RECEIVE_CNT	16u
#define CO_MAX_MAP_ENTRIES	2u
#define CO_TR_PDO_DYN_MAP_ENTRIES	16u
#define CO_REC_PDO_DYN_MAP_ENTRIES	16u
#define CO_HB_CONSUMER_CNT 16u
#define CO_SYNC_SUPPORTED	1u
#define CO_EMCY_CONSUMER_CNT 2u
#define CO_TIME_SUPPORTED	1u
/* number of used COB objects */
#define CO_COB_CNT	60u


/* Definition of number of call-back functions for each service*/
#define CO_EVENT_DYNAMIC_SDO_SERVER_READ	1u
#define CO_EVENT_DYNAMIC_SDO_SERVER_WRITE	1u
#define CO_EVENT_DYNAMIC_PDO	1u
#define CO_EVENT_DYNAMIC_NMT	1u
#define CO_EVENT_DYNAMIC_ERRCTRL	1u
#define CO_EVENT_DYNAMIC_SYNC	1u
#define CO_EVENT_DYNAMIC_LED	1u
#define CO_EVENT_DYNAMIC_SDO_CLIENT_READ	1u
#define CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE	1u
#define CO_EVENT_DYNAMIC_EMCY	1u
#define CO_EVENT_DYNAMIC_EMCY_CONSUMER	1u
#define CO_EVENT_DYNAMIC_STORE	1u
#define CO_EVENT_DYNAMIC_OBJECT_CHANGED	1u

/* Definition of CAN queue sizes */
#define CO_CONFIG_REC_BUFFER_CNT	30u
#define CO_CONFIG_TRANS_BUFFER_CNT	100u

/* Hardware settings */
/*  */
#define CO_TIMER_INTERVAL	1000u

/* do not modify comments starting with 'user-specific section:' */
/* user-specific section: start */


#include "main.h"

#define CODRV_USE_OS 1
#ifdef CODRV_USE_OS
#include <codrv_FreeRTOS.h>
#endif /* CODRV_USE_OS */


#define CODRV_BIT_TABLE_EXTERN 	1u
#define CODRV_CANCLOCK_45MHZ 	1u


#define DI_ADR		0x50u
#define DO_ADR		0x60u
#define AO_ADR		0x70u



extern uint8_t devAdr;
extern uint32_t devType;

extern uint32_t syncTime_us;
extern uint16_t heartBeat;
extern uint16_t	IOConsumer[2];
extern uint8_t doValue[32];
extern uint8_t diValue[32];
extern uint16_t aoValue[32];

#ifdef CO_NODE_IDS
#undef    CO_NODE_IDS
#define CO_NODE_IDS devAdr
#endif
#define CO_PROFILE_401	1u



extern char deviceName[32];
#define SIGNAL_CANOPEN_CAN 0x01u
#define SIGNAL_CANOPEN_TIMER 0x02u
#define SIGNAL_CANOPEN_CAN_TX 0x04u



/* user-specific section: end */

#endif /* GEN_DEFINE_H */
