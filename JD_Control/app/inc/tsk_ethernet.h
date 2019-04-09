/*
 * tsk_ethernet.h
 *
 *  Created on: 2019年4月7日
 *      Author: pli
 */

#ifndef INC_TSK_ETHERNET_H_
#define INC_TSK_ETHERNET_H_

/* 包含头文件 ----------------------------------------------------------------*/
/* 类型定义 --------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/

void User_notification(struct netif *netif);
#ifdef USE_DHCP
void DHCP_Process(struct netif *netif);
void DHCP_Periodic_Handle(struct netif *netif);
#endif



/* USER CODE BEGIN Includes */
#define IP_ADDR0   (uint8_t) 192
#define IP_ADDR1   (uint8_t) 168
#define IP_ADDR2   (uint8_t) 31
#define IP_ADDR3   (uint8_t) 14

/*NETMASK*/
#define NETMASK_ADDR0   (uint8_t) 255
#define NETMASK_ADDR1   (uint8_t) 255
#define NETMASK_ADDR2   (uint8_t) 255
#define NETMASK_ADDR3   (uint8_t) 0

/*Gateway Address*/
#define GW_ADDR0   (uint8_t) 192
#define GW_ADDR1   (uint8_t) 168
#define GW_ADDR2   (uint8_t) 31
#define GW_ADDR3   (uint8_t) 1

#define TCP_SERVER_RX_BUFSIZE	200		//¶¨Òåtcp server×î´ó½ÓÊÕÊý¾Ý³¤¶È
#define TCP_SERVER_PORT			1234	//¶¨Òåtcp serverµÄ¶Ë¿Ú

#endif /* INC_TSK_ETHERNET_H_ */
