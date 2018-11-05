/*
* gateway.c - Gateway part
*
* Copyright (c) 2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: gateway.c 9090 2015-04-10 16:08:24Z ro $
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
UNSIGNED8 str[80];
UNSIGNED8 size;
UNSIGNED8 i;
UNSIGNED8 msbidx;

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

#define	GW_EMTAS_MTYPE_CLASSIC 0x01
#define GW_EMTAS_CAN_RTR 0x10

#ifdef EMTAS_PROTOKOLL
		// Build message

		str[0] = 0x00; // free position for Frame start
		str[1] = GW_EMTAS_MTYPE_CLASSIC;


		if (pMsg->canCob.rtr == CO_TRUE) {
			str[2] = (UNSIGNED8)(pMsg->canCob.canId);
			str[3] = (UNSIGNED8)(pMsg->canCob.canId >> 8);
			str[4] = GW_EMTAS_CAN_RTR | pMsg->len;
			size = 4;
		} else {
			str[2] = (UNSIGNED8)(pMsg->canCob.canId);
			str[3] = (UNSIGNED8)(pMsg->canCob.canId >> 8);
			str[4] = pMsg->len;

			str[5] = pMsg->data[0]; // ignore later the unused datas
			str[6] = pMsg->data[1];
			str[7] = pMsg->data[2];
			str[8] = pMsg->data[3];
			
			str[9]  = pMsg->data[4];
			str[10] = pMsg->data[5];
			str[11] = pMsg->data[6];
			str[12] = pMsg->data[7];

			size = 4 + pMsg->len;
		}
		
		// create frame
		str[size + 1] = 0x00;
		str[size + 2] = 0x00;

		for (i = 0; i < size; i++) { //idx 1..size
			msbidx = size + 1 + (i / 7);
			if ((str[i + 1] & 0x80) != 0) {
				str[msbidx] |= 1 << (i % 7);
				str[i + 1] &= 0x7F;
			}
		}
		if ((size % 7) == 0) {
			size = size + 0 + (size / 7); 
		} else {
			size = size + 1 + (size / 7);
		}

		size++; //XOR

		str[0] = size;

		str[size] = 0; // XOR
		for ( i = 0; i < size; i++) {
			str[size] ^= str[i]; // XOR
		}

		// start bit after XOR -> no masking required
		str[0] |= 0x80;

		if (CDC_Transmit_FS((uint8_t*) str, size + 1) == USBD_BUSY) {
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
static UNSIGNED8 msg[80];
static UNSIGNED8 msgLen = 0;
static UNSIGNED8 msgsize = 0xFF; //parameter within the protocol
UNSIGNED8 i;
UNSIGNED8 chksum;
UNSIGNED8 msbcnt;
UNSIGNED8 msbidx;
	
	while (usbLen > 0) {

		__disable_irq(); // should be not required, because we do not enable the next transfer
		msg[msgLen++] = *usbPtr++;
		usbLen--;		
		__enable_irq();
		
#ifdef EMTAS_PROTOKOLL
		if ((msg[0] & 0x80) != 0x00) {
			//Start
			//msgLen = 1;
			msgsize = msg[0] & 0x7F;
			if (msgsize > 0x0f)  {
				msgsize = 0xFF;
			}

		}
		
		if (msgsize == 0xFF) {
			//msgsize not set
			msgLen = 0; // ignore character without start
		}
		
		if (msgLen == (msgsize + 1)) {			
		CO_CAN_MSG_T canMsg;

			// check checksum
			chksum = msg[0] & 0x7Fu;
			for (i = 1; i <= msgsize; i++) {
				chksum ^= msg[i];
			}
			
			if (chksum != 0) {
				//wrong checksum 
				msgLen = 0u;
				msgsize = 0xFFu;
				continue;
			}
			
			// MSBs
			msbcnt = (msgsize + 7) / 8; // +7 round up
			for (i = 0; i < (msgsize - msbcnt - 1); i++) {
				msbidx = (msgsize - msbcnt) + (i / 7);
				if ((msg[msbidx] & (1 << (i % 7))) != 0) {
					msg[i + 1] |= 0x80u;
				}				
			}
			
			//Protokoll
			if (msg[1] == GW_EMTAS_MTYPE_CLASSIC) {
				canMsg.canCob.extended = CO_FALSE;
				canMsg.canCob.canId = ((UNSIGNED32)msg[3] << 8) | msg[2];
				canMsg.len = msg[4] & 0x0Fu;
				if ((msg[4] & GW_EMTAS_CAN_RTR) == 0) {
					canMsg.canCob.rtr = CO_FALSE;
					for (i = 0; i < canMsg.len; i++) {
						canMsg.data[i] = msg[5 + i];
					}
				} else {
					canMsg.canCob.rtr = CO_TRUE;
				}
				
			}
			
			//reset message
			msgLen = 0u;
			msgsize = 0xFFu;
			
			coQueueRecMsgFromGw(&canMsg);
			
		}
#endif
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
