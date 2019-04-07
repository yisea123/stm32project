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
#include "lwip.h"
#include "app_ethernet.h"
#include "httpserver-netconn.h"

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

/**
  * @brief  Start Thread
  * @param  argument not used
  * @retval None
  */
void StartEthernet(void const * argument)
{
  /* Initialize LCD and LEDs */
  BSP_Config();

  /* Create tcp_ip stack thread */
  tcpip_init(NULL, NULL);

  /* Initialize the LwIP stack */
  Netif_Config();

  /* Initialize webserver demo */
  http_server_netconn_init();

  /* Notify user about the network interface config */
  User_notification(&gnetif);

#ifdef USE_DHCP
  /* Start DHCPClient */
  osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
  osThreadCreate (osThread(DHCP), &gnetif);
#endif



  for( ;; )
  {
    /* Delete the Init Thread */
    osThreadTerminate(NULL);
  }
}
#define PHY_MISR                        ((uint16_t)0x12)    /*!< MII Interrupt Status and Misc. Control Register */
#define PHY_LINK_INTERRUPT              ((uint16_t)0x2000)  /*!< PHY link status interrupt mask                  */
#define PHY_LINK_STATUS                 ((uint16_t)0x0001)  /*!< PHY Link mask                                   */


/**
  * @brief  This function sets the netif link status.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_set_link(void const *argument)
{
  uint32_t regvalue = 0;
  struct link_str *link_arg = (struct link_str *)argument;

  for(;;)
  {
    if (osSemaphoreWait( link_arg->semaphore, osWaitForever)== osOK)
    {
      /* Read PHY_MISR*/
      HAL_ETH_ReadPHYRegister(&heth, PHY_MISR, &regvalue);

      /* Check whether the link interrupt has occurred or not */
      if((regvalue & PHY_LINK_INTERRUPT) != (uint16_t)RESET)
      {
        /* Read PHY_SR*/
        HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);

        /* Check whether the link is up or down*/
        if((regvalue & PHY_LINK_STATUS)!= (uint16_t)RESET)
        {
          netif_set_link_up(link_arg->netif);
        }
        else
        {
          netif_set_link_down(link_arg->netif);
        }
      }
    }
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
#endif /* USE_DHCP */

  netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

  /*  Registers the default network interface. */
  netif_set_default(&gnetif);

  if (netif_is_link_up(&gnetif))
  {
    /* When the netif is fully configured this function must be called.*/
    netif_set_up(&gnetif);
  }
  else
  {
    /* When the netif link is down this function must be called */
    netif_set_down(&gnetif);
  }

  /* Set the link callback function, this function is called on change of link status*/
  netif_set_link_callback(&gnetif, ethernetif_update_config);

  /* create a binary semaphore used for informing ethernetif of frame reception */
  osSemaphoreDef(Netif_SEM);
  Netif_LinkSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM) , 1 );

  link_arg.netif = &gnetif;
  link_arg.semaphore = Netif_LinkSemaphore;
  /* Create the Ethernet link handler thread */
#if defined(__GNUC__)
  osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 5);
#else
  osThreadDef(LinkThr, ethernetif_set_link, osPriorityNormal, 0, configMINIMAL_STACK_SIZE * 2);
#endif

  osThreadCreate (osThread(LinkThr), &link_arg);
}


