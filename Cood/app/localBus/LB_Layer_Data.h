/*
 * LB_Layer_Can.h
 *
 *  Created on: 2017骞�5鏈�19鏃�
 *      Author: pli
 */

#ifndef LOCALBUS_LB_LAYER_DATA_H_
#define LOCALBUS_LB_LAYER_DATA_H_



#define SLAVE_ADR		0x03//or function instead


#define MAX_MEM_BUFF_LEN		512
#define MAX_BUFF_DATA_LEN		512
#define EX_DATA_LEN				(6u)
#define CRC_DATA_LEN			(2u)
#define FUNCTION_MSK			0x3F
#define ERROR_BIT				0x80
#define RESPONSE_BIT			0X40


#define MEM_CMD_HEAD_LEN_IDX			(0x4u)
#define MEM_CMD_HEAD_DATA_IDX			(0x6u)
#define MAX_CMD_HEAD_LEN				(0x6u)
#define LENGTH_OF_LEN					(0x2u)


#define OBJ_CMD_HEAD_DATA_IDX			(0x3u)
#define POLL_CMD_HEAD_DATA_IDX			(0x2u)
#define UNSUPPORT_HEAD_DATA_IDX			(0x0u)

#define LB_FRAME_BUF_LEN 		(MAX_BUFF_DATA_LEN+EX_DATA_LEN+MAX_CMD_HEAD_LEN+CRC_DATA_LEN)

#pragma pack(push)
#pragma pack(1)
typedef union
{
	uint8_t data[MAX_BUFF_DATA_LEN+EX_DATA_LEN+MAX_CMD_HEAD_LEN+CRC_DATA_LEN];
	struct _St1
	{
		uint8_t dst;
		uint8_t src;
		uint8_t id;
		uint8_t function;
		uint16_t length;
		uint8_t dataBuff[MAX_BUFF_DATA_LEN+MAX_CMD_HEAD_LEN+CRC_DATA_LEN];

	}St;
}Layer2Frame;


enum
{
	FROM_UART,
	FROM_CAN,
};
typedef struct
{
	uint16_t usedSrc;
	uint16_t typeMsg;
	Layer2Frame frame;
}MsgFrame;


typedef struct
{
	uint8_t dst;
	uint8_t src;
	uint8_t exId;
	uint8_t frameType:2;
	uint8_t REV:3;
}CanHead;

#pragma pack(pop)
typedef uint16_t (*FrameHandler)(MsgFrame* data);
void LB_Layer_Data_Init(void);
MsgFrame* LB_Layer2_GetNewBuff(void);
uint16_t GetFrameLen(Layer2Frame* ptrFrame);
MsgFrame* GetRightFrameBuff(CanHead* ptrHead);

void FreeFrameBuff(MsgFrame* frame);
#endif /* LOCALBUS_LB_LAYER_DATA_H_ */
