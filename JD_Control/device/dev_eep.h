/**
  ******************************************************************************
  * @file    dev eep
  * @author  Paul Li
   * @version V1.0.0
  * @date    2013-05-07
  * @brief   This file contains all the functions prototypes for the EEPROM 
  *          emulation firmware library.
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEV_EEP_H
#define __DEV_EEP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f407xx.h"

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
/* Exported constants --------------------------------------------------------*/
/* Define the size of the sectors to be used */
#define PAGE_SIZE               (uint32_t)0x4000  /* Page size = 16KByte */

/* Device voltage range supposed to be [2.7V to 3.6V], the operation will 
   be done by word  */
#define VOLTAGE_RANGE           (uint8_t)VoltageRange_3

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS  	ADDR_FLASH_SECTOR_2 /* EEPROM emulation start address:
                                                  from sector2 : after 1KByte of used
                                                  Flash memory */
#define USER_FLASH_LAST_PAGE_ADDRESS  (uint32_t) (0x08010000 - 4)

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS    ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS     ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                 ((uint16_t)0x0000)
#define PAGE1                 ((uint16_t)0x0001)



#define EEP_SIGNAL_SAVE		0x01

extern __IO uint16_t eepStatus;
/* Exported types ------------------------------------------------------------*/

typedef enum
{
	EEP_OK,
	EEP_WRITE_ERROR,
	EEP_SEG_ERROR,
	EEP_DATA_CONSISTANT_ERR,	
	EEP_ERASE_ERR,
	EEP_NO_VALID_DATA,
	EEP_ERROR_PARA,
	EEP_ERASED,
}EEP_STATUS;


#ifdef __cplusplus
extern "C" {
#endif
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
EEP_STATUS EEP_Init(void);
//EEP_STATUS EEP_Read(uint8_t* data, uint16_t len);
//EEP_STATUS EEP_Write(uint8_t* data, uint16_t len);
EEP_STATUS EEP_ReadAdr(uint16_t adrRelative, uint8_t* data, uint16_t len);
EEP_STATUS EEP_WriteAdr(uint16_t adrRelative, uint8_t* data, uint16_t len);
uint32_t adler32(const uint8_t *data, uint32_t len);
uint32_t adler32_N(const uint8_t *data, uint32_t len, uint32_t initVal);
//EEP_STATUS EEP_WriteAdr(uint16_t adrRelative, uint8_t* data, uint16_t len);
void StartEEPTask(void const * argument);
uint16_t Init_EEPData(void);

#ifdef __cplusplus
}
#endif

 //#define TEST_EEP_ROM 1

#endif /* __EEPROM_H */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
