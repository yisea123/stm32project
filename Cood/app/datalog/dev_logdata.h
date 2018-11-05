/*
 * dev_logdata.h
 *
 *  Created on: 2016��11��14��
 *      Author: pli
 */

#ifndef DEV_LOGDATA_H_
#define DEV_LOGDATA_H_


#define MEAS_LOG_TIMES	20000	//64*20000   = 1280000
#define	CALI_LOG_TIMES	5000	//128*5000   = 640000
#define EVENT_LOG_TIMES	5000	//16*5000	 = 80000
#define MEAS_RAW_LOG_TIMES	80000 //80000*32 = 2560000





typedef struct
{
	Filter 		filter;
	uint32_t 	startAdr;
	uint32_t	currentAdr;
	uint32_t 	endAdr;

}FilterData;



uint16_t Save2FF(uint16_t);

//functions work together;
uint16_t SetFilter(Filter* filter);
uint16_t GetOneData_Filter(Filter* filter,uint8_t* rawData, uint16_t len);
//end



#endif /* DEV_LOGDATA_H_ */
