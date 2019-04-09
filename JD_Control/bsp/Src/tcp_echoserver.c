/**
  ******************************************************************************
  * �ļ�����: main.c 
  * ��    ��: ӲʯǶ��ʽ�����Ŷ�
  * ��    ��: V1.0
  * ��д����: 2017-03-30
  * ��    ��: TCP_Server����ʵ��
  ******************************************************************************
  * ˵����
  * ����������Ӳʯstm32������YS-F4Proʹ�á�
  * 
  * �Ա���
  * ��̳��http://www.ing10bbs.com
  * ��Ȩ��ӲʯǶ��ʽ�����Ŷ����У��������á�
  ******************************************************************************
  */
/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "tcp_echoserver.h"

#if LWIP_TCP
/* ˽�����Ͷ��� --------------------------------------------------------------*/
struct tcp_pcb *tcp_echoserver_pcb;
struct tcp_echoserver_struct *tcp_echoserver_es;

/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
uint8_t ServerIp[4];
uint8_t tcp_server_recvbuf[TCP_SERVER_RX_BUFSIZE];	

/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_echoserver_error(void *arg, err_t err);
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb);
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es);

/* ������ --------------------------------------------------------------------*/
/**
  * ��������: TCP����������
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void tcp_echoserver_connect(void)
{
  /* ����һ���µ�pcb */
  tcp_echoserver_pcb = tcp_new();
  
  if (tcp_echoserver_pcb != NULL)
  {
    printf("����һ���µ�pcb\n");
    err_t err;
    
    /* �����ص�IP��ָ���Ķ˿ڰ���һ��TCP_SERVER_PORT��Ϊָ���Ķ˿� */
    err = tcp_bind(tcp_echoserver_pcb, IP_ADDR_ANY, TCP_SERVER_PORT);
    if (err == ERR_OK)
    {
      printf("��pcb�ɹ�\n");
      /* tcp pcb�������״̬ */
      tcp_echoserver_pcb = tcp_listen(tcp_echoserver_pcb);
      
      /* ��ʼ��LwIP��tcp_accept�Ļص����� */
      tcp_accept(tcp_echoserver_pcb, tcp_echoserver_accept);
    }
    else 
    {      
      /* ����Ϊpcb�ͷ��ڴ� */
      memp_free(MEMP_TCP_PCB, tcp_echoserver_pcb);
      printf("��pcbʧ��\n");
    }
  }
  else
  {
    printf("�����µ�pcbʧ��\n");
  }
}

/**
  * ��������: �ر�TCP����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
void tcp_echoserver_close(void)
{
	tcp_echoserver_connection_close(tcp_echoserver_pcb,tcp_echoserver_es);
	printf("�ر�tcp server\n");
}


/**
  * ��������: LwIP��accept�ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(err);

  /* �����´���pcb�����ȼ� */
  tcp_setprio(newpcb, TCP_PRIO_MIN);

  /* Ϊά��pcb������Ϣ�����ڴ沢���ط����� */
  es = (struct tcp_echoserver_struct *)mem_malloc(sizeof(struct tcp_echoserver_struct));
  tcp_echoserver_es=es;
  if (es != NULL)
  {
    es->state = ES_ACCEPTED;    //��������
    es->pcb = newpcb;
    es->retries = 0;
    es->p = NULL;
    
    /* ͨ���·����es�ṹ����Ϊ��pcb���� */
    tcp_arg(newpcb, es);
    
    /* ��ʼ��tcp_recv()�Ļص�����  */ 
    tcp_recv(newpcb, tcp_echoserver_recv);
    
    /* ��ʼ��tcp_err()�ص�����  */
    tcp_err(newpcb, tcp_echoserver_error);
    
    /* ��ʼ��tcp_poll�ص����� */
    tcp_poll(newpcb, tcp_echoserver_poll, 1);

		ServerIp[0]=newpcb->remote_ip.addr&0xff; 		    //IADDR4
		ServerIp[1]=(newpcb->remote_ip.addr>>8)&0xff;  	//IADDR3
		ServerIp[2]=(newpcb->remote_ip.addr>>16)&0xff; 	//IADDR2
		ServerIp[3]=(newpcb->remote_ip.addr>>24)&0xff; 	//IADDR1 
    printf("���ӵĵ��Զ�IPΪ��%d %d %d %d\n",ServerIp[0],ServerIp[1],ServerIp[2],ServerIp[3]);
    ret_err = ERR_OK;
  }
  else
  {
    /*  �ر�TCP���� */
    tcp_echoserver_connection_close(newpcb, es);
    /* �����ڴ���� */
    ret_err = ERR_MEM;
  }
  return ret_err;  
}

/**
  * ��������: TCP�������ݻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
  char *recdata=0;
  struct tcp_echoserver_struct *es;
  err_t ret_err;
  
  LWIP_ASSERT("arg != NULL",arg != NULL);
  
  es = (struct tcp_echoserver_struct *)arg;
  
  /* if we receive an empty tcp frame from client => close connection */
  if (p == NULL)
  {
    /* remote host closed connection */
    es->state = ES_CLOSING;
    if(es->p == NULL)
    {
       /* we're done sending, close connection */
       tcp_echoserver_connection_close(tpcb, es);
    }
    else
    {
      /* we're not done yet */
      /* acknowledge received packet */
      tcp_sent(tpcb, tcp_echoserver_sent);
      
      /* send remaining data*/
      tcp_echoserver_send(tpcb, es);
    }
    ret_err = ERR_OK;
  }   
  /* else : a non empty frame was received from client but for some reason err != ERR_OK */
  else if(err != ERR_OK)
  {
    /* free received pbuf*/
    if (p != NULL)
    {
      es->p = NULL;
      pbuf_free(p);
    }
    ret_err = err;
  }
  else if(es->state == ES_ACCEPTED)
  {
    /* first data chunk in p->payload */
    es->state = ES_RECEIVED;
    
    /* store reference to incoming pbuf (chain) */
    es->p = p;
    
    /* initialize LwIP tcp_sent callback function */
    tcp_sent(tpcb, tcp_echoserver_sent);
    
    recdata=(char *)malloc(p->len*sizeof(char));
    if(recdata!=NULL)
    {
      memcpy(recdata,p->payload,p->len);
      printf("TCP_Server_Rec:%s\n",recdata);
    }
    free(recdata);    
    
    /* send back the received data (echo) */
    tcp_echoserver_send(tpcb, es);
    
    ret_err = ERR_OK;
  }
  else if (es->state == ES_RECEIVED)
  {
    /* more data received from client and previous data has been already sent*/
    if(es->p == NULL)
    {
      es->p = p;
			recdata=(char *)malloc(p->len*sizeof(char));
			if(recdata!=NULL)
			{
				memcpy(recdata,p->payload,p->len);
				printf("TCP_Server_Rec:%s\n",recdata);
			}
			free(recdata);  
      /* send back received data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      struct pbuf *ptr;

      /* chain pbufs to the end of what we recv'ed previously  */
      ptr = es->p;
      pbuf_chain(ptr,p);
    }
    ret_err = ERR_OK;
  }
  else if(es->state == ES_CLOSING)
  {
    /* odd case, remote side closing twice, trash data */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  else
  {
    /* unkown es->state, trash data  */
    tcp_recved(tpcb, p->tot_len);
    es->p = NULL;
    pbuf_free(p);
    ret_err = ERR_OK;
  }
  return ret_err;
}

/**
  * ��������: TCP����ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_error(void *arg, err_t err)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(err);

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    /*  free es structure */
    mem_free(es);
  }
}


/**
  * ��������: TCP_poll�ص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_poll(void *arg, struct tcp_pcb *tpcb)
{
  err_t ret_err;
  struct tcp_echoserver_struct *es;

  es = (struct tcp_echoserver_struct *)arg;
  if (es != NULL)
  {
    if (es->p != NULL)
    {
      tcp_sent(tpcb, tcp_echoserver_sent);
      /* there is a remaining pbuf (chain) , try to send data */
      tcp_echoserver_send(tpcb, es);
    }
    else
    {
      /* no remaining pbuf (chain)  */
      if(es->state == ES_CLOSING)
      {
        /*  close tcp connection */
        tcp_echoserver_connection_close(tpcb, es);
      }
    }
    ret_err = ERR_OK;
  }
  else
  {
    /* nothing to be done */
    tcp_abort(tpcb);
    ret_err = ERR_ABRT;
  }
  return ret_err;
}

/**
  * ��������: TCP���ͻص�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static err_t tcp_echoserver_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
  struct tcp_echoserver_struct *es;

  LWIP_UNUSED_ARG(len);

  es = (struct tcp_echoserver_struct *)arg;
  es->retries = 0;
  
  if(es->p != NULL)
  {
    /* still got pbufs to send */
    tcp_sent(tpcb, tcp_echoserver_sent);
    tcp_echoserver_send(tpcb, es);
  }
  else
  {
    /* if no more data to send and client closed connection*/
    if(es->state == ES_CLOSING)
      tcp_echoserver_connection_close(tpcb, es);
  }
  return ERR_OK;
}

/**
  * ��������: TCP�������ݺ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_send(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  struct pbuf *ptr;
  err_t wr_err = ERR_OK;
 
  while ((wr_err == ERR_OK) &&
         (es->p != NULL) && 
         (es->p->len <= tcp_sndbuf(tpcb)))
  {
    
    /* get pointer on pbuf from es structure */
    ptr = es->p;

    /* enqueue data for transmission */
    wr_err = tcp_write(tpcb, ptr->payload, ptr->len, 1);

    if (wr_err == ERR_OK)
    {
      u16_t plen;
      u8_t freed;
      
      plen = ptr->len;
     
      /* continue with next pbuf in chain (if any) */
      es->p = ptr->next;
      
      if(es->p != NULL)
      {
        /* increment reference count for es->p */
        pbuf_ref(es->p);
      }
      
     /* chop first pbuf from chain */
      do
      {
        /* try hard to free pbuf */
        freed = pbuf_free(ptr);
      }
      while(freed == 0);
     /* we can read more data now */
     tcp_recved(tpcb, plen);
   }
   else if(wr_err == ERR_MEM)
   {
      /* we are low on memory, try later / harder, defer to poll */
     es->p = ptr;
     tcp_output(tpcb);   
   }
   else
   {
     /* other problem ?? */
   }
  }
}

/**
  * ��������: �ر�TCP���Ӻ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ��: ��
  */
static void tcp_echoserver_connection_close(struct tcp_pcb *tpcb, struct tcp_echoserver_struct *es)
{
  
  /* remove all callbacks */
  tcp_arg(tpcb, NULL);
  tcp_sent(tpcb, NULL);
  tcp_recv(tpcb, NULL);
  tcp_err(tpcb, NULL);
  tcp_poll(tpcb, NULL, 0);
  
  /* delete es structure */
  if (es != NULL)
  {
    mem_free(es);
  }  
  
  /* close tcp connection */
  tcp_close(tpcb);

}

#endif /* LWIP_TCP */

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

