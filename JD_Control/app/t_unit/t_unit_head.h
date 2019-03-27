/*
 * t_unit_head.h
 *
 *  Created on: 2018Äê2ÔÂ11ÈÕ
 *      Author: vip
 */

#ifndef T_UNIT_T_UNIT_HEAD_H_
#define T_UNIT_T_UNIT_HEAD_H_


#include "t_unit_cfg.h"
#include "t_data_obj.h"
#include "t_data_obj_simple.h"
#include "t_data_obj_time.h"
#include "t_dataclass.h"
#include "t_data_obj_segweld.h"
typedef enum
{
	READ_ACCESS = 0x01u,
	WRITE_ACCESS=0x02u,
}ADR_RW_STATUS;


uint16_t CheckAdrRWStatus(uint32_t adr, uint32_t len, ADR_RW_STATUS rwStatus);

uint16_t GetObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		OBJ_DATA* inst);
uint16_t GetName(uint16_t subID,  uint16_t objectIndex,OBJ_DATA* inst);
uint16_t GetAdrInfo(uint16_t subId, uint16_t objId,int16_t atrId, uint32_t* adr, uint16_t* len);
uint16_t GetObjectType(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		uint16_t* type);

uint16_t PutObject(uint16_t subID,  uint16_t objectIndex, int16_t attributeIndex,
		void* data);

uint16_t LoadDefaultCfg(uint16_t id);

const T_UNIT* GetSubsystem(uint16_t id);

#endif /* T_UNIT_T_UNIT_HEAD_H_ */
