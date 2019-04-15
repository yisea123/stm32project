#ifndef DEV_RTC__H
#define DEV_RTC__H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stm32f4xx.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */
#define ENCODER_CNT_REG		RTC_BKP_DR8
/* USER CODE END Includes */

extern RTC_HandleTypeDef sRtcHandle;



/* USER CODE BEGIN Prototypes */
uint32_t RTC_FatTime32Get(void);
uint32_t prvReadBackupRegister(uint32_t RTC_BKP_DR);
void prvWriteBackupRegister(uint32_t RTC_BKP_DR, const uint32_t Data);
uint16_t Init_RTC(void);
extern void xprintf_rtc(void);
extern void xprintf_tick(void);
uint8_t sprintf_rtc(uint8_t* buff, uint8_t len);
uint16_t RTC_SetCalibration(uint16_t val);
uint16_t RTC_GetCalibration(uint16_t* val);
void RTC_WriteMotorPos(uint32_t cnt);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif

