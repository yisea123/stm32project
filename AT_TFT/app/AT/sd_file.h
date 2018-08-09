/*
 * sd_file.h
 *
 *  Created on: 2016Äê9ÔÂ29ÈÕ
 *      Author: pli
 */

#ifndef AT_SD_FILE_H_
#define AT_SD_FILE_H_

uint16_t RWTest_SD(uint8_t* filename);

uint16_t Mount_SD(void);

uint16_t CreateNewFile_SD(const uint8_t *path,const uint8_t *filename, uint32_t size);

uint16_t WriteFile_SD(const uint8_t *filename, const uint8_t* buff, uint32_t size);

uint16_t AppendFile_SD(const uint8_t *filename, uint8_t* buff, uint32_t size);

uint16_t ReadFileAdr_SD(const uint8_t *filename, uint8_t* buff, uint32_t* pSize,uint32_t adr);

uint16_t IsFileReady_SD(const uint8_t *filename);

#endif /* AT_SD_FILE_H_ */
