/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.32                          *
*        Compiled Oct  8 2015, 11:59:02                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END

#include "DIALOG.h"
#include "stdint.h"
#include "main.h"
#include "bsp.h"


extern WM_HWIN allUI[3];
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x7C)
#define ID_GRAPH_0 (GUI_ID_USER + 0x7D)
#define ID_GRAPH_1 (GUI_ID_USER + 0x7E)
#define ID_BUTTON_0 (GUI_ID_USER + 0x80)
#define ID_BUTTON_1 (GUI_ID_USER + 0x8C)
#define ID_BUTTON_2 (GUI_ID_USER + 0x8D)
#define ID_BUTTON_3 (GUI_ID_USER + 0x8E)
#define ID_BUTTON_4 (GUI_ID_USER + 0x8F)
#define ID_TEXT_0 (GUI_ID_USER + 0x86)
#define ID_TEXT_1 (GUI_ID_USER + 0x87)
#define ID_TEXT_2 (GUI_ID_USER + 0x88)
#define ID_TEXT_3 (GUI_ID_USER + 0x89)
#define ID_TEXT_4 (GUI_ID_USER + 0x8A)

extern const GUI_FONT GUI_Fontused_U48;

static float tempRatio = 0.4*50;
static float humRatio = 1.0;
static float tempOffset = 50;
extern uint16_t newWindow;

static GRAPH_DATA_Handle pdataTemp, pdataHumidity;
// USER START (Optionally insert additional defines)
uint16_t UpdateDisplay(uint16_t ret)
{
	if(ret == FATAL_ERROR)
	{
		GRAPH_DATA_YT_Clear(pdataTemp);
		GRAPH_DATA_YT_Clear(pdataHumidity);
		WM_HWIN hItem = WM_GetDialogItem(allUI[2], ID_TEXT_1);
		TEXT_SetText(hItem, "No Data");
		hItem = WM_GetDialogItem(allUI[2], ID_TEXT_2);
		TEXT_SetText(hItem, "No Data");
	}
}
uint16_t UpdateHistNewData(uint16_t* ptrVal, uint16_t size, uint32_t* ptrStartTime )
{
	uint16_t i = 0;
	GRAPH_DATA_YT_Clear(pdataTemp);
	GRAPH_DATA_YT_Clear(pdataHumidity);

	for( i =0; i<size/2; i++)
	{
		uint16_t temp = ptrVal[i*2];
		uint16_t hum = ptrVal[i*2+1];
		if((temp != 0xFFFF) && (hum != 0xFFFF))
		{
			float t = temp/10.0f;
			float h = hum/10.0f;
			GRAPH_DATA_YT_AddValue(pdataTemp, tempOffset + (int)(t*tempRatio+0.5));
			GRAPH_DATA_YT_AddValue(pdataHumidity, (int)(h*humRatio+0.5));
		}
		else
		{
			break;
		}
	}
	uint32_t laterTime = GetST_SecLater(*ptrStartTime, i);

	WM_HWIN hItem = WM_GetDialogItem(allUI[2], ID_TEXT_1);
	TEXT_SetText(hItem, GetSTStr(*ptrStartTime));
	hItem = WM_GetDialogItem(allUI[2], ID_TEXT_2);
	TEXT_SetText(hItem, GetSTStr(laterTime));
	*ptrStartTime = laterTime;
	return i;

}


// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "history", ID_FRAMEWIN_0, 0, 0, 1024, 600, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 5, 14, 695, 260, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_1, 5, 279, 695, 310, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 755, 500, 200, 90, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "From", ID_TEXT_0, 715, 22, 290, 50, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 715, 79, 290, 50, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, 715, 192, 290, 50, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "To", ID_TEXT_3, 715, 134, 290, 50, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 740, 240, 199, 55, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 705, 300, 150, 85, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 862, 300, 150, 85, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 705, 400, 150, 85, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_4, 862, 400, 150, 85, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static const char* displayText[4] = {
		"历史  一",
		"历史  二",
		"历史  三",
		"历史  四",
};
// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  static  int selId = -1;
  int 	  flashId = 0;
  GRAPH_SCALE_Handle hScaleV, hScaleH;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'history'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetFont(hItem, &GUI_Fontused_U48);
    FRAMEWIN_SetTextColor(hItem, GUI_BLUE);
    FRAMEWIN_SetTitleVis(hItem, 0);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem,40, 5, 5, 30);
    GRAPH_SetGridVis(hItem,1);
    GRAPH_SetGridFixedX(hItem,1);
    GRAPH_SetGridDistY(hItem,25);
    GRAPH_SetGridDistX(hItem,50);
    GRAPH_SetVSizeY(hItem,100);
    GRAPH_SetVSizeX(hItem, 50);
    GRAPH_SetColor (hItem, GUI_LIGHTGREEN,   GRAPH_CI_GRID);
    hScaleV =GRAPH_SCALE_Create(40, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 50);
    GRAPH_SCALE_SetTextColor(hScaleV,GUI_RED);
    GRAPH_SCALE_SetFont(hScaleV,GUI_FONT_24_ASCII);
    GRAPH_SCALE_SetOff(hScaleV, 50);
    GRAPH_SCALE_SetFactor(hScaleV,0.4f);
    tempRatio = 1.0f/0.4f;
    tempOffset = 50;

    GRAPH_AttachScale(hItem,hScaleV);

    hScaleH =GRAPH_SCALE_Create(235, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL,50);
    GRAPH_SCALE_SetFont(hScaleH,GUI_FONT_24_ASCII);
    GRAPH_SCALE_SetTextColor(hScaleH,GUI_RED);
    GRAPH_SCALE_SetFactor(hScaleH,0.1f);
    GRAPH_AttachScale(hItem,hScaleH);

    pdataTemp =GRAPH_DATA_YT_Create(GUI_MAGENTA, 650, 0, 0);
    GRAPH_AttachData(hItem,pdataTemp);

    //GRAPH_DATA_XY_SetPenSize(hItem, 3);
    ///////////////////////////////**************************/////////////////
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_1);
    GRAPH_SetBorder(hItem,40, 5, 5, 30);
    GRAPH_SetGridVis(hItem,1);
    GRAPH_SetGridFixedX(hItem,1);
    GRAPH_SetGridDistY(hItem,25);
    GRAPH_SetGridDistX(hItem,50);
    GRAPH_SetVSizeY(hItem,100);
    GRAPH_SetVSizeX(hItem, 50);
    GRAPH_SetColor (hItem, GUI_LIGHTGREEN,   GRAPH_CI_GRID);
    hScaleV =GRAPH_SCALE_Create(40, GUI_TA_RIGHT, GRAPH_SCALE_CF_VERTICAL, 50);
    GRAPH_SCALE_SetTextColor(hScaleV,GUI_RED);
    GRAPH_SCALE_SetFont(hScaleV,GUI_FONT_24_ASCII);
    //GRAPH_SCALE_SetOff(hScaleV, 60);
    GRAPH_SCALE_SetFactor(hScaleV,0.4f);
    humRatio = 1.0f/0.4f;

    GRAPH_AttachScale(hItem,hScaleV);

    hScaleH =GRAPH_SCALE_Create(282, GUI_TA_HCENTER, GRAPH_SCALE_CF_HORIZONTAL, 50);
    GRAPH_SCALE_SetTextColor(hScaleH,GUI_RED);

    GRAPH_SCALE_SetFont(hScaleH,GUI_FONT_24_ASCII);
    GRAPH_SCALE_SetFactor(hScaleH,0.1f);
    GRAPH_AttachScale(hItem,hScaleH);
    pdataHumidity =GRAPH_DATA_YT_Create(GUI_BLUE, 650/*鏈�澶ф暟鎹釜鏁�*/, 0, 0);
    GRAPH_AttachData(hItem,pdataHumidity);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "返  回");
    //
    // Initialization of 'From'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'To'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetText(hItem, "历      史");
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "<<历史");
    WM_DisableWindow(hItem);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "历史>>");
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "<<时间");
    WM_DisableWindow(hItem);
    //WM_DisableWindow(hItem);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "时间>>");
    WM_DisableWindow(hItem);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
		newWindow = 1;
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)


		if(selId < 3)
		{
			selId+=1;
			uint16_t ret= loadFromFlash(selId, DIR_NONE, &flashId);
			if(ret != OK)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_DisableWindow(hItem);
			}
			else
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_EnableWindow(hItem);
			}
			//disbale window
		}
		if(selId >= 3)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			WM_DisableWindow(hItem);
		}
		if(selId >= 1)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_EnableWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
		if((selId >=0) && (selId<=3))
			TEXT_SetText(hItem, displayText[selId]);
		else
			TEXT_SetText(hItem, "历        史");
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
		if (selId >= 1) {
			selId -= 1;
			uint16_t ret= loadFromFlash(selId, DIR_NONE, &flashId);
			if(ret != OK)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_DisableWindow(hItem);
			}
			else
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
				WM_DisableWindow(hItem);
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_EnableWindow(hItem);
			}
		}
		if (selId <= 0)
		{
			selId = 0;
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			WM_DisableWindow(hItem);
			//disbale window
		}
		if(selId < 3)
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
			WM_EnableWindow(hItem);
		}

		hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
		if((selId >=0) && (selId<=3))
			TEXT_SetText(hItem, displayText[selId]);
		else
			TEXT_SetText(hItem, "历        史");

        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
		if ((selId >= 0) && (selId <= 3)) {

			uint16_t ret= loadFromFlash(selId, DIR_INC, &flashId);
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
			WM_EnableWindow(hItem);
			if(ret != OK)
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_DisableWindow(hItem);
			}
			else
			{
				hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
				WM_EnableWindow(hItem);
			}

		}
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
    	  if ((selId >= 0) && (selId <= 3)) {
		uint16_t ret= loadFromFlash(selId, DIR_DEC, &flashId);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
		WM_EnableWindow(hItem);
		if((ret != OK) || (flashId == 0))
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
			WM_DisableWindow(hItem);
		}
		else
		{
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
			WM_EnableWindow(hItem);
		}
	}
        break;

      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       Createhistory
*/
WM_HWIN Createhistory(void);
WM_HWIN Createhistory(void) {
  WM_HWIN hWin;
  WM_SetCreateFlags(WM_CF_HIDE|WM_CF_MEMDEV);
  allUI[2] = hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_HideWindow(hWin);
  return hWin;
}
void StartHistory(void)
{
	//WM_DeleteTimer(0);
	WM_HideWindow(allUI[1]);
	WM_HideWindow(allUI[0]);
	WM_ShowWindow(allUI[2]);
	WM_SetCallback(WM_HBKWIN, &_cbDialog);
}
// USER START (Optionally insert additional public code)

// USER END

/*************************** End of file ****************************/
