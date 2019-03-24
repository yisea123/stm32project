/*
 * tft_uart.h
 *
 *  Created on: 2017Äê9ÔÂ18ÈÕ
 *      Author: Paul
 */

#ifndef SRC_TFT_UART_H_
#define SRC_TFT_UART_H_


#define TXT_BUF_LEN			32
//page1
#define TXT_SCROLL_BAR		0x2000
#define TXT_EEP_RESULT		0x5000
#define TXT_SD_RESULT		0x5100
#define TXT_USB_RESULT		0x5200
#define TXT_CAN_RESULT		0x5300
#define TXT_COPY_RESULT		0x5400

//page2 -copy
#define U16_TOTAL_COPY		0x6000
#define U16_COPIED_NUM		0x6010
#define TXT_COPIED_STATUS	0x6020
#define TXT_COPIED_NAME		0x6030
#define TXT_NOTIFY_USB		0x6100
#define TXT_NOTIFY_SD		0x6110
//page3 - display
#define U16_CURRENT_VALUE	0x6040
#define U16_SPEED_VALUE		0x6050
#define U16_VOLTAGE_VALUE	0x6060
#define U16_ENERGY_VALUE	0x6070

//curve chn
#define CURRENT_CHN			0
#define SPEED_CHN			1
#define VOLTAGE_CHN			2
#define ENGERY_CHN			3

enum
{
	ID_HEAD = 0,
	ID_LEN	= 2,
	ID_CMD = 3,
	ID_ADR = 4,
	ID_ADR_DATA = 6,
	ID_REG = 4,
	ID_REG_DATA = 5,
	ID_CURVE_DATA = 5,
};
#define CMD_READ_VALUE		0x83
#define CMD_WRITE_VALUE 	0x82
#define CMD_READ_REG		0x81
#define CMD_WRITE_REG		0x80
#define CMD_WRITE_WAVE		0x84

#define PAGE_REG			0x03


#define PAGE_START_TEST		0
#define PAGE_WELD_WAVE		1
#define PAGE_WELD_NOTIFY	3
#define PAGE_WELD_COPY		2



//
/*
 * send lines
5a a5 12 84 0f 20 10 00 90 10 a0 20 30 00 10 20 50 10 a0 00 f0
*/
#endif /* SRC_TFT_UART_H_ */
