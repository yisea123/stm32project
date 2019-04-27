/*
 * tsk_ethernet.c
 *
 *  Created on: 2019Äê4ÔÂ7ÈÕ
 *      Author: pli
 */
#include "main.h"
#include "unit_head.h"
#include "tsk_head.h"
#include "shell_io.h"
#include "netif.h"
#include "ip_addr.h"
#include "tsk_ethernet.h"
#include "ethernetif.h"
#include "tcpip.h"
#include "app_ethernet.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct netif gnetif; /* network interface structure */
/* Semaphore to signal Ethernet Link state update */
osSemaphoreId Netif_LinkSemaphore = NULL;
/* Ethernet link thread Argument */
struct link_str link_arg;

/* Private function prototypes -----------------------------------------------*/
static void Netif_Config(void);


#include <lwip/sockets.h>
#include <lwip/err.h>
#include <lwip/sys.h>

extern void tcp_server_init(void);


void StartEthPhy()
{
	  /* 初始化LWIP内核 */
	  tcpip_init( NULL, NULL );
	  /* 配置网络接口 */
	  Netif_Config();
	  /* 查看相关网络接口配置 */
	  User_notification(&gnetif);

}

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartEthernet(void const * argument)
{
//	osDelay(3000);
  /* 初始化LWIP内核 */
  tcpip_init( NULL, NULL );
  /* 配置网络接口 */
  Netif_Config();
  /* 查看相关网络接口配置 */
  User_notification(&gnetif);
  
#ifdef USE_DHCP
  /* Start DHCPClient */
  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(DHCP), &gnetif);
#endif
//  for( ;; )
  osDelay(1000);
  tcp_server_init();
//	tcp_echoserver_connect();
//  for( ;; )
  {
    /* Delete the Init Thread */ 
	
//    osThreadTerminate(NULL);
  }  
}

/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

#ifdef USE_DHCP
  ip_addr_set_zero_ip4(&ipaddr);
  ip_addr_set_zero_ip4(&netmask);
  ip_addr_set_zero_ip4(&gw);
#else
  IP_ADDR4(&ipaddr,IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
  IP_ADDR4(&netmask,NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
  IP_ADDR4(&gw,GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
  
	TraceUser("static IP  ........................%d.%d.%d.%d\r\n",IP_ADDR0,IP_ADDR1,IP_ADDR2,IP_ADDR3);
	TraceUser("ip mask  ..........................%d.%d.%d.%d\r\n",NETMASK_ADDR0,NETMASK_ADDR1,NETMASK_ADDR2,NETMASK_ADDR3);
	TraceUser("gate way ..........................%d.%d.%d.%d\r\n",GW_ADDR0,GW_ADDR1,GW_ADDR2,GW_ADDR3);
#endif /* USE_DHCP */

  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    TraceUser("sucessful to connect net\n");
    /* When the netif is fully configured this function must be called */
    netif_set_up(&gnetif);   
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }
  
}


