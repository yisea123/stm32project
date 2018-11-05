/*
 * dev_spiflash.h
 *
 *  Created on: 2016Äê9ÔÂ26ÈÕ
 *      Author: pli
 */

#ifndef DEV_SPIFLASH_H_
#define DEV_SPIFLASH_H_


#ifdef __cplusplus
extern "C"
{
#endif

/** M25P SPI Flash supported commands */
#define sFLASH_CMD_WRITE          0x02  /* Write to Memory instruction */
#define sFLASH_CMD_WRSR           0x01  /* Write Status Register instruction */
#define sFLASH_CMD_WREN           0x06  /* Write enable instruction */
#define sFLASH_CMD_UNSE			  0x39  /* Un-protect sector */
#define sFLASH_CMD_RDSEPR  		  0x3C  /* Read sector protect register */
#define sFLASH_CMD_READ           0x03  /* Read from Memory instruction */
#define sFLASH_CMD_RDSR           0x05  /* Read Status Register instruction  */
#define sFLASH_CMD_RDID           0x9F  /* Read identification */
#define sFLASH_CMD_SE             0xD8  /* Sector (64k bytes) Erase instruction */
#define sFLASH_CMD_BE             0xC7  /* Bulk Erase instruction */

/** Command List from HeavyMetal's project */
#define FLASH_CMD_READ0   				sFLASH_CMD_READ
#define FLASH_CMD_READ1   				(0x0B)
#define FLASH_CMD_READ2   				(0x1B)
#define FLASH_CMD_READ    				FLASH_CMD_READ2
#define FLASH_CMD_WRITE_ENABLE			sFLASH_CMD_WREN
#define FLASH_CMD_WRITE_DISABLE 		0x04
#define FLASH_CMD_WRITE           		sFLASH_CMD_WRITE
#define FLASH_CMD_BLOCK_ERASE_4K  		0x20
#define FLASH_CMD_BLOCK_ERASE_32K  		0x52
#define FLASH_CMD_BLOCK_ERASE_64K  		sFLASH_CMD_SE
#define FLASH_CMD_CHIP_ERASE0   		0x60
#define FLASH_CMD_CHIP_ERASE1     		sFLASH_CMD_BE
#define FLASH_CMD_READ_STAT_REG    		0x05
#define FLASH_CMD_WRITE_STATUS_BYTE1	sFLASH_CMD_WRSR
#define FLASH_CMD_WRITE_STATUS_BYTE2 	0x31
#define FLASH_CMD_RESET           		0xF0
#define FLASH_CMD_READ_MANU_n_DevID  	sFLASH_CMD_RDID
#define FLASH_CMD_FREE_LOCK				0x34
#define sFLASH_WEL_FLAG			  		0x02  /* Write enable flag */
#define sFLASH_WIP_FLAG           		0x01  /* Write In Progress (WIP) flag */

#define sFLASH_DUMMY_BYTE         		0xA5
#define sFLASH_SPI_PAGESIZE      	 	0x100		// 256 bytes
#define sFLASH_SPI_SECTORSIZE	  		0x10000	// 64k bytes
//#define sFLASH_M25P128_ID         	0x202018
//#define sFLASH_M25P64_ID          	0x202017
#define sFLASH_AT25DF641_ID		  		0x1F4800


/*SPI Flash devices list*/
#define AT25DF641A 		0
#define AT25DF321A		1

/*define the flash device */

#define DISK_TYPE AT25DF641A


#if (DISK_TYPE == AT25DF641A)

#define SECTOR_SZ  				4096U
#define FLASH_PAGE_SIZE        	(uint16_t)(256)			/* The size of page , 256Bytes */
#define FLASH_N_SECTOR_SIZE		4U 			/* 16 pages-per-sector */
#define FLASH_CAPACITY          (0x800000)	/*  8Mbytes */
#define FLASH_BLOCK_SIZE       ((FLASH_PAGE_SIZE)<<(FLASH_N_SECTOR_SIZE)) 	/* Size of sector, 4kB */
#define FLASH_BLOCK_COUNT       (FLASH_CAPACITY/FLASH_BLOCK_SIZE)

#endif

uint16_t Dev_SpiFlash_EraseBlk(uint32_t lb);
uint16_t Dev_SpiFlash_EraseAll(void);
uint16_t Dev_SpiFlash_Init(void);

uint16_t Dev_SpiFlash_ReadWrite(const uint8_t *txBuf, uint8_t *rxBuf,
		uint32_t addr, uint16_t rwBytes);


#ifdef __cplusplus
}
#endif

#endif /* DEV_SPIFLASH_H_ */
