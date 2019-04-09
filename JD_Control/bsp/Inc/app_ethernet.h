#ifndef __APP_ETHERNET_H
#define __APP_ETHERNET_H

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "lwip/netif.h"
#include "main.h"
   
/* ���Ͷ��� ------------------------------------------------------------------*/
/* �궨�� --------------------------------------------------------------------*/
#define DHCP_OFF                   (uint8_t) 0
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5
   
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void User_notification(struct netif *netif);
#ifdef USE_DHCP
void DHCP_thread(void const * argument);
#endif

#endif /* __APP_ETHERNET_H */


/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

