/**
  ******************************************************************************
  * @file           : usbh_platform.c
  * @brief          : source file for the usbh_platform file
  ******************************************************************************
  *
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

#include "usbh_platform.h"

/**
  * @brief  MX_DriverVBUS 
  *         Drive VBUS.
  * @param  state : VBUS state
  *          This parameter can be one of the these values:
  *           0 : VBUS Active 
  *           1 : VBUS Inactive
  */
void MX_DriverVbusFS( uint8_t state) 
{ 
  uint8_t data = state; 
  /* USER CODE BEGIN PREPARE_GPIO_DATA_VBUS_FS */
  if(state == 0)
  {
    /* Drive high Charge pump */ 	     
    data = GPIO_PIN_SET;
  }
  else
  {
    /* Drive low Charge pump */
    data = GPIO_PIN_RESET;
  }
  /* USER CODE END PREPARE_GPIO_DATA_VBUS_FS */
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,(GPIO_PinState)data);
}

