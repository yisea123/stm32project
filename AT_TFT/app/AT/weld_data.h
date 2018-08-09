/*
 * weld_data.h
 *
 *  Created on: 2016Äê9ÔÂ29ÈÕ
 *      Author: pli
 */

#ifndef AT_WELD_DATA_H_
#define AT_WELD_DATA_H_

#define MAX_BUF_LEN 				1024 //Max length
#define SAVE_COUNT_MIN				(MAX_BUF_LEN/ONE_PAGE_SIZE*3)
#define ONE_WELD_DATA_SIZE			10
#define ONE_PAGE_SIZE				32
#define ONE_PAGE_DATA_SIZE			30
#define ONE_PAGE_CRC_SIZE			2

#pragma pack(push)
#pragma pack(1)

typedef struct _weldData
{
	uint16_t volt;
	uint16_t curr;
	uint32_t time;
	uint16_t speed;
}OneWeldData;

#pragma pack(pop)



typedef struct _weldSeg
{
	OneWeldData weldData[3];
	uint16_t checkSum;
}OneWeldSeg;

uint16_t AddOneRawWeldData(void);
uint16_t AddOneData(uint16_t welding);


#endif /* AT_WELD_DATA_H_ */
