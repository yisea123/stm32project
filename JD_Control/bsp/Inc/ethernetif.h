#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"

/* ���Ͷ��� ------------------------------------------------------------------*/
struct link_str {
  struct netif *netif;
  osSemaphoreId semaphore;
};
   
/* �궨�� --------------------------------------------------------------------*/
#define ETH_RCC_CLK_ENABLE()                __HAL_RCC_ETH_CLK_ENABLE()

#define ETH_GPIO_ClK_ENABLE()              {__HAL_RCC_GPIOA_CLK_ENABLE();__HAL_RCC_GPIOC_CLK_ENABLE();\
                                            __HAL_RCC_GPIOB_CLK_ENABLE();__HAL_RCC_GPIOG_CLK_ENABLE();}

#define GPIO_AFx_ETH                        GPIO_AF11_ETH

/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
void ethernetif_set_link(void const *argument);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);

#endif


/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

