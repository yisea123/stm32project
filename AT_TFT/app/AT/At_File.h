/*
 * At_File.h
 *
 *  Created on: 2016Äê9ÔÂ29ÈÕ
 *      Author: pli
 */

#ifndef AT_AT_FILE_H_
#define AT_AT_FILE_H_

#include <stdint.h>

enum
{
	USB_FILE_TEST = 0,
	SD_FILE_TEST,
	IIC_DATA_TEST,
	CAN_TEST,
	WELD_DATA_SAVE_APPEND,
	WELD_DATA_SAVE_NEW,
	COPY_FILES_REQ,
	COPY_FILES_ALL,
	SIMULATE_SAVE,
	IDLE_ACTION,
};




typedef struct
{
	uint32_t action;
	uint8_t* buff;
	uint32_t len;
}FileAction;

void SendFileMsg(const FileAction* ptrMsg);
void SimulateWeldData(uint16_t num,uint8_t type);
void Actual_SaveData(void);
#endif /* AT_AT_FILE_H_ */
