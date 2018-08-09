/*
 * unit_data_log.h
 *
 *  Created on: 2016Äê11ÔÂ18ÈÕ
 *      Author: pli
 */

#ifndef SUBSYSTEM_UNIT_DATA_LOG_H_
#define SUBSYSTEM_UNIT_DATA_LOG_H_


extern const T_UNIT dataLog;

enum
{
	IDX_FLASH_OPERATION,
	IDX_INIT_DATALOG,
	IDX_SET_FILTER_TIME_START,
	IDX_SET_FILTER_TIME_END,
	IDX_SET_FILTER_TYPE,
	IDX_FILTER_ACTION,
	IDX_SET_FILTER_INFO,
	IDX_NULL_SET,
	IDX_READ_BUF0,
	IDX_READ_BUF1,
	IDX_READ_BUF2,
	IDX_READ_BUF3,

	IDX_TRIG_FAKESAVE = 20,

};

uint16_t Initialize_DataLog(const struct _T_UNIT *me, uint8_t typeOfStartUp);
uint16_t Put_DataLog(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);

uint16_t Get_DataLog(const T_UNIT *me, uint16_t objectIndex, int16_t attributeIndex,
					void * ptrValue);


#endif /* SUBSYSTEM_UNIT_DATA_LOG_H_ */
