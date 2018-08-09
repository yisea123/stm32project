/*
* gateway.c - Gateway part
*
* Copyright (c) 2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: gateway.c 9134 2015-04-20 14:06:47Z ro $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief Gateway functionality - application part
* This example demonstrate the Gateway API.
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>
#include <co_canopen.h>

#include "stm32f0xx_hal.h"
#include "usb_device.h"

/* constant definitions
---------------------------------------------------------------------------*/

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
extern uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
extern void CDC_Enable_Next_Receive(void);

/* list of global defined functions
---------------------------------------------------------------------------*/
void SystemClock_Config(void);

/* list of local defined functions
---------------------------------------------------------------------------*/
static void MX_GPIO_Init(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
/* USB TX */
volatile UNSIGNED8 usbTxFlag = 0u;

/* USB RX */
volatile UNSIGNED8 * usbPtr = NULL;
volatile UNSIGNED32 usbLen = 0ul;
volatile UNSIGNED8 usbRxFlag = 0u;

/* local defined variables
---------------------------------------------------------------------------*/

/***************************************************************************/
/**
* \brief Gateway initialization - USB FS Device
*
* \param
*	nothing
* \results
*	nothing
*/
void gateway_initalization(void)
{
  	/* Initialize all configured peripherals */
	MX_GPIO_Init();
  
  	MX_USB_DEVICE_Init();
}

/***************************************************************************/
/**
* \brief System Clock Configuration
*
* \param
*	nothing
* \results
*	nothing
*/
void SystemClock_Config(void)
{

  	RCC_OscInitTypeDef RCC_OscInitStruct;
  	RCC_ClkInitTypeDef RCC_ClkInitStruct;
  	RCC_PeriphCLKInitTypeDef PeriphClkInit;
  	RCC_CRSInitTypeDef RCC_CRSInitStruct;

  	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  	RCC_OscInitStruct.HSICalibrationValue = 16;
  	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  	RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  	HAL_RCC_OscConfig(&RCC_OscInitStruct);

  	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK;
  	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
  	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

  	__CRS_CLK_ENABLE();

  	RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  	RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  	RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  	RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_CALCULATE_RELOADVALUE(48000000,1000);
  	RCC_CRSInitStruct.ErrorLimitValue = 34;
  	RCC_CRSInitStruct.HSI48CalibrationValue = 32;
  	HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

  	__SYSCFG_CLK_ENABLE();

}

/***************************************************************************/
/**
* \brief Pinout Configuration
*
* \param
*	nothing
* \results
*	nothing
*/
static void MX_GPIO_Init(void)
{

  	/* GPIO Ports Clock Enable */
  	__GPIOA_CLK_ENABLE();
  	__GPIOB_CLK_ENABLE();

}

/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

/*************************************************************************/
/**
* \brief transmit the received CAN messages with USB
*
* CAN -> USB
*/
void coGatewayTransmitMessage(
		const CO_CAN_MSG_T *pMsg /**< CAN Message */
	)
{
#ifdef GW_DEBUG	
	char str[80];
#else
	char str[13];
#endif

#ifdef GW_DEBUG	
	sprintf(str, "0x%3x (%d) %02x %02x %02x %02x %02x %02x %02x %02x\n",
		pMsg->canCob.canId,
		pMsg->len,
		pMsg->data[0],
		pMsg->data[1],
		pMsg->data[2],
		pMsg->data[3],
		pMsg->data[4],
		pMsg->data[5],
		pMsg->data[6],
		pMsg->data[7]);
	
	CDC_Transmit_FS((uint8_t*) str, strlen(str));
#endif
	
#ifdef MQ_PROTOKOLL
		str[0] = 0xB2; // Start
		str[1] = 0x01; // CAN 1
		if (pMsg->canCob.rtr == CO_TRUE) {
			str[2] = 0x40 | pMsg->len;
			str[3] = (UNSIGNED8)(pMsg->canCob.canId >> 8);
			str[4] = (UNSIGNED8)(pMsg->canCob.canId);
			memset(&str[5], 0, 8);
		} else {
			str[2] = 0x10 | pMsg->len;
			str[3] = (UNSIGNED8)(pMsg->canCob.canId >> 8);
			str[4] = (UNSIGNED8)(pMsg->canCob.canId);
			
			str[5] = pMsg->data[0];
			str[6] = pMsg->data[1];
			str[7] = pMsg->data[2];
			str[8] = pMsg->data[3];
			
			str[9]  = pMsg->data[4];
			str[10] = pMsg->data[5];
			str[11] = pMsg->data[6];
			str[12] = pMsg->data[7];

		}
		
		if (CDC_Transmit_FS((uint8_t*) str, 13) == USBD_BUSY) {
			usbTxFlag = 1u;
		} else {
			usbTxFlag = 0u;
		}
#endif		
}



/*************************************************************************/
/**
* \brief USB has received new data
*
* subcalled IRQ handler
*
* USB -> CAN
*/

static UNSIGNED32 maxLen = 0ul; // for debugging

void gateway_receiveIrqHandler(
		uint8_t* Buf, /**< receive messages */
		uint32_t len  /**< count of received characters */
	)
{
	if (maxLen < len) {
		maxLen = len;
	}

	//simple - do nothing
	usbPtr = Buf;
	usbLen = len;
	
	usbRxFlag = 1u;
	
	// EndPoint not freeing - no new data
	
}

/*************************************************************************/
/**
* \brief check cyclic the received data 
*
* check the received data
*   -> the CANopen GW API cannot be called from interrupt
* check for transmit data in case of the USB TX EP was busy
*
*/

void gateway_receiveCyclic(void)
{
static UNSIGNED8 msg[13];
static UNSIGNED8 msgLen = 0;
	
	while (usbLen > 0) {

		__disable_irq(); // should be not required, because we do not enable the next transfer
		msg[msgLen++] = *usbPtr++;
		usbLen--;		
		__enable_irq();
		
#ifdef MQ_PROTOKOLL
		if ((msgLen == 1) && (msg[0] != 0xB2)) {
			//incorrect Header
			msgLen = 0;
		}
		
		if ((msgLen == 2) && 
			((msg[1] != 0x01) && (msg[1] != 0x07)) )
		{
			//incorrect target / CAN1, Internal
			msgLen = 0;
		}
		
		if ((msgLen == 3) && 
			(((msg[2] & 0xF0) != 0x10) && (msg[2] & 0xF0) != 0x40) )
		{
			//incorrect Msg type / data, rtr
			msgLen = 0;
		}
		
		if (msgLen == 13) {
		CO_CAN_MSG_T canMsg;

			canMsg.len = msg[2] & 0x0F;
			if (canMsg.len > 8) {
				//wrong len
				msgLen = 0;
				continue;
			}
			
			if ((msg[2] & 0xF0) == 0x40) {
				canMsg.canCob.rtr = CO_TRUE;
			} else {
				canMsg.canCob.rtr = CO_FALSE;
				memcpy(&canMsg.data[0], &msg[5], 8);
			}
			
			canMsg.canCob.canId = ((UNSIGNED32)msg[3] << 8) | msg[4];
			canMsg.canCob.extended = CO_FALSE;
			
			//reset message
			msgLen = 0;
			
			coQueueRecMsgFromGw(&canMsg);

		}
#endif /* MQ_PROTOKOLL */
	}

	//usbLen == 0
	if (usbRxFlag == 1) {
		usbRxFlag = 0u;
		CDC_Enable_Next_Receive(); //free Endpoint for next receive
	}
	
	// TX polling
	if (usbTxFlag == 1) {
		if (CDC_Transmit_FS(NULL, 0) == USBD_OK) {
			usbTxFlag = 0;
		}
	}
}
