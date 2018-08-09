/*
 * At_Gui.h
 *
 *  Created on: 2016Äê10ÔÂ2ÈÕ
 *      Author: pli
 */

#ifndef AT_AT_GUI_H_
#define AT_AT_GUI_H_




/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define MaxCurr 1300
#define MaxVolt 500
#define MaxSpeed 400
#define MaxLineEnergy 35


#define STR_INDEX_LEN	8

#define YSIZE1		1000

#define USB_TEST_TIME		20000
#define USB_IDLE_TIME		1000

void StartGuiTask(void const * argument);

void SetCopyFileName(uint8_t* fileName);

#endif /* AT_AT_GUI_H_ */
