#ifndef __TCP_ECHOSERVER_H__
#define __TCP_ECHOSERVER_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "lwipopts.h"
/* ���Ͷ��� ------------------------------------------------------------------*/
/* TCP����������״̬ */
enum tcp_echoserver_states
{
  ES_NONE = 0,
  ES_ACCEPTED,
  ES_RECEIVED,
  ES_CLOSING
};

/* LwIP�ص�����ʹ�ýṹ�� */
struct tcp_echoserver_struct
{
  u8_t state;             /* ��ǰ����״̬ */
  u8_t retries;
  struct tcp_pcb *pcb;    /* ָ��ǰ��pcb */
  struct pbuf *p;         /* ָ��ǰ���ջ����pbuf */
};

/* �궨�� --------------------------------------------------------------------*/
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void tcp_echoserver_connect(void);
void tcp_echoserver_close(void);



#endif /* __TCP_ECHOSERVER */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

