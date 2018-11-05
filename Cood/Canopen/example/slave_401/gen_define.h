/*
 * Stack definitions for 401 - generated by CANopen DeviceDesigner 2.7.0
 * Mo Mai 8 11:52:44 2017
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
#define CO_OBJECTS_LINE_0_CNT	45u
#define CO_OBJECT_COUNTS	45u
#define CO_COB_COUNTS	14u
#define CO_TXPDO_COUNTS	2u
#define CO_RXPDO_COUNTS	6u
#define CO_SSDO_COUNTS	1u
#define CO_CSDO_COUNTS	0u
#define CO_ASSIGN_COUNTS	0u
#define CO_GUARDING_COUNTS	CO_ASSIGN_COUNTS
#define CO_ERR_CTRL_COUNTS	0u
#define CO_ERR_HIST_COUNTS	0u
#define CO_EMCY_CONS_COUNTS	0u
#define CO_NODE_IDS	32
#define CO_NODE_ID_FUNCTIONS NULL

/* Definition of numbers of CANopen services */
#define CO_PROFILE_401	1u
#define CO_SDO_SERVER_CNT	1u
#define CO_PDO_TRANSMIT_CNT	2u
#define CO_PDO_RECEIVE_CNT	6u
#define CO_MAX_MAP_ENTRIES	5u
#define CO_TR_PDO_DYN_MAP_ENTRIES	2u
#define CO_REC_PDO_DYN_MAP_ENTRIES	6u
#define CO_SYNC_SUPPORTED	1u
#define CO_EMCY_PRODUCER	1u
/* number of used COB objects */
#define CO_COB_CNT	14u


/* Definition of number of call-back functions for each service*/
#define CO_EVENT_DYNAMIC_SDO_SERVER_READ	1u
#define CO_EVENT_DYNAMIC_SDO_SERVER_WRITE	1u
#define CO_EVENT_DYNAMIC_PDO	1u
#define CO_EVENT_DYNAMIC_NMT	1u
#define CO_EVENT_DYNAMIC_ERRCTRL	1u
#define CO_EVENT_DYNAMIC_LED	1u
#define CO_EVENT_DYNAMIC_CAN	1u

/* Definition of CAN queue sizes */
#define CO_CONFIG_REC_BUFFER_CNT	5u
#define CO_CONFIG_TRANS_BUFFER_CNT	5u

/* Hardware settings */
/*  */
#define CO_TIMER_INTERVAL	10000u

/* do not modify comments starting with 'user-specific section:' */
/* user-specific section: start */

#define C4L_DEVICE	"/dev/can0" /* can4linux */
#define CAN_INTERFACE "vcan0" /* socketcan */



/* user-specific section: end */

#endif /* GEN_DEFINE_H */
