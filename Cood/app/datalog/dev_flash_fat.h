/*
 * dev_flash_fat.h
 *
 *  Created on: 2016��11��14��
 *      Author: pli
 */

#ifndef DEV_FLASH_FAT_H_
#define DEV_FLASH_FAT_H_




struct LogData;
#pragma pack(push)
#pragma pack(1)

//total 32 bytes;
typedef struct
{
	uint16_t fatType;		//different file, shall use different type;
	uint16_t fatVersion; 	//fatVersion decide the fat table is right or not;

	uint32_t fatStartAdr; 	//fat start address is the allocated start address;
	uint32_t fatEndAdr; 	//fat end address is the end address of location;
	uint32_t fatCurrentAdr; //fat end address is the current address of log data;
	uint16_t fatSegSize;	//the fat segsize which used to calculate the new address;
	uint16_t revData;
	uint32_t writeCount;
	uint32_t dataCount;		//the write counter which is to locate the new address for new data also;
	uint32_t time;			//the updated time;
	uint32_t reserved[2];

}FatList;

typedef struct
{
	uint16_t openType;
	FatList* ptrFile;
	const LogData* logInst;
	uint32_t readAdr;
	uint32_t readIdx;
	uint32_t validCount;

}FileInfo;
#pragma pack(pop)

enum
{
	OPEN_READ = 0x01,
	OPEN_WRITE = 0x02,
};

enum
{
	READ_NEW = 0x00,
	READ_OLD,
};

#define FF_SECTOR_SZ		SECTOR_SZ
#define FF_CAPACITY			FLASH_CAPACITY



#define FF_HEAD_SECTORS			0x04U
#define FF_HEAD_START_ADR		(uint32_t)(FF_CAPACITY - (FF_SECTOR_SZ*FF_HEAD_SECTORS))
#define FF_HEAD_END_ADR			FF_CAPACITY
#define FF_REV_SECTORS			0x04U
#define FF_FATLIST_SIZE			((uint16_t)64) // (1024/(1024/(sizeof(FatList) + CRC_LEN)))
#define FF_FATLIST_CRC_LEN		sizeof(FatList)



typedef struct
{
	uint32_t	fileEndAdr;
	uint16_t	isFlashRaw;
	uint16_t	isMount;
	uint8_t 	tempBuffer[FF_FATLIST_SIZE];

	SemaphoreHandle_t lock;
}FatInfo;

extern FileInfo	fatFiles[FF_FILE_COUNT];

//interfaces inside:
uint16_t FF_Init(void);//user may extend this function;
uint16_t FF_Mount(const LogData** inst, uint16_t num);//get all file include the address, size;..etc

//open, close
FileInfo* FF_Open(uint16_t fileType, uint16_t openType);//open file
uint16_t FF_Close(FileInfo* file);//close file


//write
uint16_t FF_InitFile(FileInfo* file);//init file storage,delete the old data;
uint16_t FF_Append_Write(FileInfo* file, uint8_t* data, uint16_t len);//get all file include the address, size;..etc

//read
uint16_t FF_ReadSeek(FileInfo* file, uint32_t* ptrIdx);//get all file include the address, size;..etc
uint16_t FF_Append_Read(FileInfo* file, uint8_t* data, uint16_t len, uint32_t* ptrIdx, uint16_t dir);//get all file include the address, size;..etc



#endif /* DEV_FLASH_FAT_H_ */
