/*
* usbd_cdc_if - CDC interface
*
* Copyright (c) 2015 emtas GmbH
*-------------------------------------------------------------------
* $Id:  $
*
*
*-------------------------------------------------------------------
* This source base on an example/template source from STMicroelectronics!
*
*/

/********************************************************************/
/**
* \file
* \brief main routine
*
*/


/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_CDC 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CDC_Private_TypesDefinitions
  * @{
  */ 
  /* USER CODE BEGIN 0 */ 
  /* USER CODE END 0 */ 
/**
  * @}
  */ 

/** @defgroup USBD_CDC_Private_Defines
  * @{
  */ 
  /* USER CODE BEGIN 1 */
/* Define size for the receive and transmit buffer over CDC */
/* It's up to user to redefine and/or remove those define */
#define APP_RX_DATA_SIZE  2048
#define APP_TX_DATA_SIZE  2048
  /* USER CODE END 1 */  
/**
  * @}
  */ 

/** @defgroup USBD_CDC_Private_Macros
  * @{
  */ 
  /* USER CODE BEGIN 2 */ 
  /* USER CODE END 2 */
/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_Variables
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/* Received Data over USB are stored in this buffer       */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/* Send Data over USB CDC are stored in this buffer       */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

uint32_t BuffLength;
uint32_t UserTxBufPtrIn = 0;/* Increment this pointer or roll it back to
                               start address when data are received over USART */
uint32_t UserTxBufPtrOut = 0; /* Increment this pointer or roll it back to
                                 start address when data are sent over USB */




/* USB handler declaration */
/* Handle for USB Full Speed IP */
USBD_HandleTypeDef  *hUsbDevice_0;

extern USBD_HandleTypeDef hUsbDeviceFS;

// UART Parameter for Teminal programs
USBD_CDC_LineCodingTypeDef LineCoding =
  {
    115200, /* baud rate*/
    0x00,   /* stop bits-1*/
    0x00,   /* parity - none*/
    0x08    /* nb. of bits 8*/
  };


int openPort = 0;

/**
  * @}
  */ 
  
/** @defgroup USBD_CDC_Private_FunctionPrototypes
  * @{
  */
static int8_t CDC_Init_FS     (void);
static int8_t CDC_DeInit_FS   (void);
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS  (uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = 
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,  
  CDC_Receive_FS
};



void printU32(char* s, uint32_t val) 
{
#ifdef GW_DEBUG
	char buf[40];
	
	sprintf(buf, "%s %lx\n", s, val);
	CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
#endif
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  CDC_Init_FS
  *         Initializes the CDC media low layer over the FS USB IP
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  hUsbDevice_0 = &hUsbDeviceFS;
  /* USER CODE BEGIN 3 */ 
	UserTxBufPtrIn = 0;
	UserTxBufPtrOut = 0;
	
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(hUsbDevice_0, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(hUsbDevice_0, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */ 
}

/**
  * @brief  CDC_DeInit_FS
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */ 
  return (USBD_OK);
  /* USER CODE END 4 */ 
}

/**
  * @brief  CDC_Control_FS
  *         Manage the CDC class requests
  * @param  cmd: Command code            
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS  (uint8_t cmd, uint8_t* pbuf, uint16_t length)
{ 
  /* USER CODE BEGIN 5 */	
	
#ifdef GW_DEBUG	
	printU32("cmd", cmd);
#endif
	
  switch (cmd)
  {
  case CDC_SEND_ENCAPSULATED_COMMAND:
 
    break;

  case CDC_GET_ENCAPSULATED_RESPONSE:
 
    break;

  case CDC_SET_COMM_FEATURE:
 
    break;

  case CDC_GET_COMM_FEATURE:

    break;

  case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */ 
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
  case CDC_SET_LINE_CODING:   
	
    break;

  case CDC_GET_LINE_CODING:     
//after open
    pbuf[0] = (uint8_t)(LineCoding.bitrate);
    pbuf[1] = (uint8_t)(LineCoding.bitrate >> 8);
    pbuf[2] = (uint8_t)(LineCoding.bitrate >> 16);
    pbuf[3] = (uint8_t)(LineCoding.bitrate >> 24);
    pbuf[4] = LineCoding.format;
    pbuf[5] = LineCoding.paritytype;
    pbuf[6] = LineCoding.datatype;     
    

  
    break;

  case CDC_SET_CONTROL_LINE_STATE:
  {
#define CDC_RTS_MASK   0x02
#define CDC_DTR_MASK   0x01
	  
#ifdef GW_DEBUG	  
	printU32("line", hUsbDevice_0->request.wValue);
#endif
	  
	// Data Terminal Ready
	if ((hUsbDevice_0->request.wValue & CDC_DTR_MASK) != 0) 
	{
		openPort = 1;
	  } else {
		 openPort = 0;
	  }
	}
    break;

  case CDC_SEND_BREAK:
 
    break;    
    
  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  CDC_Receive_FS
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will block any OUT packet reception on USB endpoint 
  *         untill exiting this function. If you exit this function before transfer
  *         is complete on CDC interface (ie. using DMA controller) it will result 
  *         in receiving more data while previous ones are still not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
extern void gateway_receiveIrqHandler(uint8_t* Buf, uint32_t len);
static int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */

	//ISR code!
	gateway_receiveIrqHandler(Buf, *Len);
	
  	return (USBD_OK);
  /* USER CODE END 6 */ 
}

/*
* free/enable Receive Endpoint for the next transfer
*/
void CDC_Enable_Next_Receive(void)
{
	USBD_CDC_ReceivePacket(hUsbDevice_0);
}


/**
  * @brief  CDC_Transmit_FS
  *         Data send over USB IN endpoint are sent over CDC interface 
  *         through this function.           
  *         @note
  *         
  *                 
  * @param  Buf: Buffer of data to be send
  * @param  Len: Number of data to be send (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(
		uint8_t* Buf, 
		uint16_t Len
	)
{
uint8_t result = USBD_OK;
uint32_t buffptr;
uint32_t buffsize;
int i;

	/* check - can we send */
	if (openPort != 1) {
		return USBD_FAIL;
	}

	if (hUsbDevice_0 == NULL) {
		return USBD_FAIL;
	}
	
	if ( hUsbDevice_0->dev_state != USBD_STATE_CONFIGURED) {
		return USBD_FAIL;
	}


	/* fill buffer */
	buffptr = UserTxBufPtrIn;
	for( i = 0; i < Len; i++) {
		
		UserTxBufferFS[buffptr] = Buf[i];		
		
		/* Increment Index for buffer writing */
		buffptr++;
	  
		/* To avoid buffer overflow */
		if(buffptr == APP_RX_DATA_SIZE)
		{
			buffptr = 0;
		}
		
		if (buffptr == UserTxBufPtrOut) {
				// bufffer full
			buffptr = UserTxBufPtrIn; // write only complete packages
			break;
		}
		
	}
	UserTxBufPtrIn = buffptr;
	
	/* send buffer */
  	if(UserTxBufPtrOut != UserTxBufPtrIn)
  	{
    	if(UserTxBufPtrOut > UserTxBufPtrIn) /* rollback */
    	{
      		buffsize = APP_RX_DATA_SIZE - UserTxBufPtrOut;
    	}
    	else 
    	{
      		buffsize = UserTxBufPtrIn - UserTxBufPtrOut;
    	}
    
    	buffptr = UserTxBufPtrOut;
    
		/* set buffer for the enxt transfer */
    	USBD_CDC_SetTxBuffer(hUsbDevice_0, (uint8_t*)&UserTxBufferFS[buffptr], buffsize);
    
		/* transmit current buffer */
    	if(USBD_CDC_TransmitPacket(hUsbDevice_0) == USBD_OK)
    	{
		
			UserTxBufPtrOut += buffsize;
      		if (UserTxBufPtrOut == APP_RX_DATA_SIZE)
      		{
        		UserTxBufPtrOut = 0;
      		}
    	} else {
			result = USBD_BUSY;
		}
  	}
	
	if(UserTxBufPtrOut != UserTxBufPtrIn) {
		// swap buffer, only one part was transmitted
		result = USBD_BUSY; 
	}

  	return result;
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 


