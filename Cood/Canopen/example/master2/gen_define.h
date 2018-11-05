/*
 * Stack definitions for master2 - generated by CANopen DeviceDesigner 2.7.0
 * Mo Mai 8 11:44:14 2017
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
#define CO_OBJECTS_LINE_0_CNT	10u
#define CO_OBJECT_COUNTS	10u
#define CO_COB_COUNTS	10u
#define CO_TXPDO_COUNTS	0u
#define CO_RXPDO_COUNTS	0u
#define CO_SSDO_COUNTS	1u
#define CO_CSDO_COUNTS	2u
#define CO_ASSIGN_COUNTS	0u
#define CO_GUARDING_COUNTS	CO_ASSIGN_COUNTS
#define CO_ERR_CTRL_COUNTS	2u
#define CO_ERR_HIST_COUNTS	0u
#define CO_EMCY_CONS_COUNTS	0u
#define CO_NODE_IDS	1
#define CO_NODE_ID_FUNCTIONS NULL

/* Definition of numbers of CANopen services */
#define CO_NMT_MASTER	1u
#define CO_SDO_SERVER_CNT	1u
#define CO_SDO_CLIENT_CNT	2u
#define CO_HB_CONSUMER_CNT 2u
/* number of used COB objects */
#define CO_COB_CNT	10u


/* Definition of number of call-back functions for each service*/
#define CO_EVENT_DYNAMIC_SDO_CLIENT_READ	1u
#define CO_EVENT_DYNAMIC_SDO_CLIENT_WRITE	1u
#define CO_EVENT_DYNAMIC_EMCY_CONSUMER	1u
#define CO_EVENT_DYNAMIC_TIME	1u

/* Definition of CAN queue sizes */
#define CO_CONFIG_REC_BUFFER_CNT	5u
#define CO_CONFIG_TRANS_BUFFER_CNT	5u

/* Hardware settings */
/*  */
#define CO_TIMER_INTERVAL	10000u

/* do not modify comments starting with 'user-specific section:' */
/* user-specific section: start */

#define C4L_DEVICE      "/dev/can0"

/* user-specific section: end */

#endif /* GEN_DEFINE_H */
