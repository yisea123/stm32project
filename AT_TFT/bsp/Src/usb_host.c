/**
 ******************************************************************************
 * @file            : USB_HOST
 * @version         : v1.0_Cube
 * @brief           :  This file implements the USB Host
 ******************************************************************************
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"


#include "main.h"
#include "parameter_idx.h"
#include "unit_parameter.h"
/* USB Host Core handle declaration */
USBH_HandleTypeDef hUsbHostFS;

/**
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callbak declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/**
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* init function */
void MX_USB_HOST_Init(void)
{
	/* Init Host Library,Add Supported Class and Start the library*/
	USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS);

	USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);

	USBH_Start(&hUsbHostFS);
}


void RenitUsb_User(void)
{
	MX_DriverVbusFS(FALSE);
	__USB_OTG_FS_FORCE_RESET();
	__USB_OTG_FS_RELEASE_RESET();

	osDelay(500);
	MX_USB_HOST_Init();
	osDelay(2000);

}
/*
 * user callbak definition
 */

static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
	uint8_t usbconnect = USH_USR_DISCONNECT;
	/* USER CODE BEGIN 2 */
	switch (id)
	{
		case HOST_USER_SELECT_CONFIGURATION:
			break;

		case HOST_USER_DISCONNECTION:
			usbconnect = USH_USR_DISCONNECT;
			if (__get_IPSR() == 0)
				parameter_Put(PARA_IDX_usbConnectedStatus, WHOLE_OBJECT, &usbconnect);
			else
				parameterDynamic.usbConnectedStatus = usbconnect;
			break;

		case HOST_USER_CLASS_ACTIVE:
			usbconnect = USH_USR_FS_READY;
			if (__get_IPSR() == 0)
				parameter_Put(PARA_IDX_usbConnectedStatus, WHOLE_OBJECT, &usbconnect);
			else
				parameterDynamic.usbConnectedStatus = usbconnect;

			break;

		case HOST_USER_CONNECTION:
			/*
			usbconnect = USH_USR_APPLICATION;
			if (__get_IPSR() == 0)
				parameter_Put(PARA_IDX_usbConnected, WHOLE_OBJECT, &usbconnect);
			else
				parameterDynamic.usbConnected = usbconnect;
			*/
			break;
		case HOST_USER_UNRECOVERED_ERROR:
			usbconnect = USH_USR_HW_ERROR;
			if (__get_IPSR() == 0)
				parameter_Put(PARA_IDX_usbConnectedStatus, WHOLE_OBJECT, &usbconnect);
			else
				parameterDynamic.usbConnectedStatus = usbconnect;
			break;

		default:
			break;
	}
	/* USER CODE END 2 */
}

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
