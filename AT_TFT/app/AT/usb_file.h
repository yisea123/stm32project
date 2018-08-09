/*
 * usb_file.h
 *
 *  Created on: 2016Äê9ÔÂ29ÈÕ
 *      Author: pli
 */

#ifndef AT_USB_FILE_H_
#define AT_USB_FILE_H_


uint16_t RWTest_USB(uint8_t* filename);

uint16_t Mount_USB(void);

uint16_t CreateNewFile_USB(const uint8_t *path,const uint8_t *filename, uint32_t psize);

uint16_t WriteFileAdr_USB(const uint8_t *filename, uint8_t* buff, uint32_t size,uint32_t adr);

uint16_t ReadFileAdr_USB(const uint8_t *filename, uint8_t* buff, uint32_t size,uint32_t adr);

uint16_t CreateDir_USB(const uint8_t *mainpath,const uint8_t *subpath);

uint16_t DeleteFile_USB(const uint8_t *filename);


#endif /* AT_USB_FILE_H_ */
