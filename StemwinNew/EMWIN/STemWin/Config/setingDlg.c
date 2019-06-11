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
#include "rtc.h"
#include "bsp.h"
#include "main.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x3C)
#define ID_SPINBOX_0 (GUI_ID_USER + 0x3D)
#define ID_DROPDOWN_0 (GUI_ID_USER + 0x40)
#define ID_TEXT_0 (GUI_ID_USER + 0x41)
#define ID_TEXT_1 (GUI_ID_USER + 0x45)
#define ID_SPINBOX_1 (GUI_ID_USER + 0x47)
#define ID_TEXT_2 (GUI_ID_USER + 0x48)
#define ID_SPINBOX_2 (GUI_ID_USER + 0x49)
#define ID_TEXT_3 (GUI_ID_USER + 0x4A)
#define ID_BUTTON_0 (GUI_ID_USER + 0x4B)
#define ID_BUTTON_1 (GUI_ID_USER + 0x4C)
#define ID_GRAPH_0 (GUI_ID_USER + 0x4D)
#define ID_TEXT_4 (GUI_ID_USER + 0x4E)
#define ID_BUTTON_2 (GUI_ID_USER + 0x4F)
#define ID_CHECKBOX_0 (GUI_ID_USER + 0x50)
#define ID_BUTTON_3 (GUI_ID_USER + 0x51)
extern const GUI_FONT GUI_Fontused_U48;
extern uint16_t newWindow;
// USER START (Optionally insert additional defines)
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
  { FRAMEWIN_CreateIndirect, "Setting", ID_FRAMEWIN_0, 0, 0, 1024, 600, 0, 0x0, 0 },
  { SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_0, 778, 170, 231, 129, 0, 0x0, 0 },
  { DROPDOWN_CreateIndirect, "Dropdown", ID_DROPDOWN_0, 550, 169, 210, 300, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_0, 543, 90, 467, 70, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_1, 540, 5, 470, 70, 0, 0x64, 0 },
  { SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_1, 260, 193, 240, 120, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_2, 30, 220, 170, 70, 0, 0x64, 0 },
  { SPINBOX_CreateIndirect, "Spinbox", ID_SPINBOX_2, 260, 330, 240, 120, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_3, 30, 360, 170, 70, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 160, 480, 200, 80, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 670, 470, 200, 90, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 533, 1, 4, 583, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Text", ID_TEXT_4, 5, 5, 526, 70, 0, 0x64, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_2, 300, 100, 200, 80, 0, 0x0, 0 },
  { CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_0, 60, 99, 230, 80, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Button", ID_BUTTON_3, 670, 345, 200, 80, 0, 0x0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

static const uint16_t timeRange[6][2] =
{
		{2019, 3100},
		{1, 12},
		{1, 31},
		{0, 23},
		{0, 59},
		{0, 59},
};
void SaveLoadRTC(WM_MESSAGE * pMsg, U16 type)
{
	WM_HWIN hItem;
	hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
	U16 eTp = DROPDOWN_GetSel(hItem);
	hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_0);

	if(type == 0)
	{
		//update
		I32 val = SPINBOX_GetValue(hItem);
		UpdateRTC(eTp, val);
	}
	else
	{
		I32 val= GetRTC(eTp);
		SPINBOX_SetRange(hItem, timeRange[eTp][0], timeRange[eTp][1]);
		SPINBOX_SetValue(hItem, val);
		//load
	}
}
void ChkBox(WM_MESSAGE * pMsg, U16 type)
{
	WM_HWIN hItem;

	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
	if(type)
	{

		if( prvReadBackupRegister(MANUAL_STATE) != 0)
		{
			CHECKBOX_Check(hItem);
		}
		else
		{
			CHECKBOX_Uncheck(hItem);
		}
	}
	if(CHECKBOX_IsChecked(hItem))
	{
		  //
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);

		WM_EnableWindow(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
		WM_EnableWindow(hItem);

		WM_ValidateWindow(hItem);
	}
	else
	{


		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);
		WM_DisableWindow(hItem);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
		WM_DisableWindow(hItem);
	}
}


void SaveLoadValue(WM_MESSAGE * pMsg, U16 type)
{
	WM_HWIN hItem;
	I32 val;

	if(type != 0)
	{
// save
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);
		val = SPINBOX_GetValue(hItem);
		prvWriteBackupRegister(INTERVAL_SET, val);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_2);
		val = SPINBOX_GetValue(hItem);
		prvWriteBackupRegister(TIME_EXEC, val);

		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
		if(CHECKBOX_IsChecked(hItem))
			val = 1;
		else
			val = 0;
		prvWriteBackupRegister(MANUAL_STATE, val);

	}
	else
	{
//load
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);
		val = prvReadBackupRegister(INTERVAL_SET);
		SPINBOX_SetValue(hItem, val);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_2);
		val = prvReadBackupRegister(TIME_EXEC);
		SPINBOX_SetValue(hItem, val);


	}
}

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
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_TIMER:
	  hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
	  TEXT_SetText(hItem, GetRTCStr());
	  WM_RestartTimer(pMsg->Data.v, 1000);
	  break;
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Setting'
    //
    hItem = pMsg->hWin;
    FRAMEWIN_SetFont(hItem, &GUI_Fontused_U48);
    FRAMEWIN_SetTextColor(hItem, GUI_BLUE);
    FRAMEWIN_SetTitleVis(hItem, 0);
    //
    // Initialization of 'Spinbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_0);
    SPINBOX_SetFont(hItem, &GUI_Fontused_U48);
    //
    // Initialization of 'Dropdown'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_DROPDOWN_0);
    DROPDOWN_SetFont(hItem, &GUI_Fontused_U48);
    DROPDOWN_AddString(hItem, "Year");
    DROPDOWN_AddString(hItem, "Month");
    DROPDOWN_AddString(hItem, "Date");
    DROPDOWN_AddString(hItem, "Hour");
    DROPDOWN_AddString(hItem, "Min");
    DROPDOWN_AddString(hItem, "Second");
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetText(hItem, GetRTCStr());
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetText(hItem, "时钟设置");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

    TEXT_SetTextColor(hItem, GUI_BLUE);
    //
    // Initialization of 'Spinbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_1);
    SPINBOX_SetFont(hItem, &GUI_Fontused_U48);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetText(hItem, "吹扫间隔");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Spinbox'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_SPINBOX_2);
    SPINBOX_SetFont(hItem, &GUI_Fontused_U48);
    //
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetText(hItem, "吹扫时间");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "确  定");
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
     BUTTON_SetText(hItem, "返  回");
   //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    // Initialization of 'Text'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetText(hItem, "控 制 模 式");
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, &GUI_Fontused_U48);
    TEXT_SetTextColor(hItem, GUI_BLUE);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetText(hItem, "同步零点");
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
    CHECKBOX_SetText(hItem, "手  动");
    CHECKBOX_SetFont(hItem, &GUI_Fontused_U48);
    CHECKBOX_SetState(hItem, 1);
    //
    // Initialization of 'Button'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, &GUI_Fontused_U48);
    BUTTON_SetText(hItem, "确  定");
    ChkBox(pMsg, 1);
    SaveLoadValue(pMsg, 0);
    SaveLoadRTC(pMsg, 1);
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_SPINBOX_0: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_DROPDOWN_0: // Notifications sent by 'Dropdown'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
    	 // SaveLoadRTC(pMsg, 1);
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
    	  SaveLoadRTC(pMsg, 1);
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_SPINBOX_1: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:

        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_SPINBOX_2: // Notifications sent by 'Spinbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_MOVED_OUT:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_0: // Notifications sent by 'Button'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
    	  SaveLoadValue(pMsg, 1);
        // USER START (Optionally insert code for reacting on notification message)
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
		newWindow = 1;
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
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
        // USER END
    	  osMessagePut(MB_MAINSTEP, (uint32_t) IO_STATE_ON, 0);
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_CHECKBOX_0: // Notifications sent by 'Checkbox'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_VALUE_CHANGED:
    	  ChkBox(pMsg, 0);
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
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
    	  SaveLoadRTC(pMsg, 0);
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
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
*       CreateSetting
*/
WM_HWIN settingUI;
extern WM_HWIN allUI[3];
WM_HWIN CreateSetting(void);
WM_HWIN CreateSetting(void) {
  WM_HWIN hWin;
  WM_SetCreateFlags(WM_CF_HIDE|WM_CF_MEMDEV);
  allUI[1] = settingUI = hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  WM_HideWindow(hWin);
  return hWin;
}

// USER START (Optionally insert additional public code)
void StartUISetting(uint16_t type)
{
	WM_HideWindow(allUI[2]);
	WM_HideWindow(allUI[0]);
	WM_ShowWindow(allUI[1]);

	WM_CreateTimer(WM_HBKWIN,0,1000,0);
	WM_SetCallback(WM_HBKWIN, &_cbDialog);
}
// USER END

/*************************** End of file ****************************/
