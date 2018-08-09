/*
* codrv_socketcan.c - contains driver for SocketCAN
*
* Copyright (c) 2012-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: codrv_socketcan.c 10839 2015-09-30 12:54:15Z ro $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief  CANopen stack driver for SocketCAN
*
* adapted for kernel 3.2
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* OS headers
---------------------------------------------------------------------------*/
#include <net/if.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>
#include <co_datatype.h>
#include <co_drv.h>
#include <co_commtask.h>
#include "codrv_socketcan.h"

/* constant definitions
---------------------------------------------------------------------------*/
#ifndef CAN_INTERFACE
#  define CAN_INTERFACE "can0"
#endif /* CAN_INTERFACE */

const char * interface_cstr = CAN_INTERFACE;


/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T codrvCanTransmit(CO_CONST CO_CAN_MSG_T *pBuf);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/
static int    mBSDSocket;	/*!< handle of BSD socket */
static BOOL_T canEnabled = { CO_FALSE }; /*! flag if can has been enabled */

static CO_CAN_MSG_T *pTxBuf = { NULL };


#if defined CANFD
/* get data length from can_dlc with sanitized can_dlc */
unsigned char can_dlc2len(unsigned char can_dlc);
/* map the sanitized data length to an appropriate data length code */
unsigned char can_len2dlc(unsigned char len);

/* get data length from can_dlc with sanitized can_dlc */
unsigned char can_dlc2len(unsigned char dlc)
{
static const unsigned char dlc2len[] = {0, 1, 2, 3, 4, 5, 6, 7,
					8, 12, 16, 20, 24, 32, 48, 64};
	return dlc2len[dlc & 0x0F];
}

/* map the sanitized data length to an appropriate data length code */
unsigned char can_len2dlc(unsigned char len)
{
static const unsigned char len2dlc[] = {
				0, 1, 2, 3, 4, 5, 6, 7, 8,		/* 0 - 8 */
				9, 9, 9, 9,				/* 9 - 12 */
				10, 10, 10, 10,				/* 13 - 16 */
				11, 11, 11, 11,				/* 17 - 20 */
				12, 12, 12, 12,				/* 21 - 24 */
				13, 13, 13, 13, 13, 13, 13, 13,		/* 25 - 32 */
				14, 14, 14, 14, 14, 14, 14, 14,		/* 33 - 40 */
				14, 14, 14, 14, 14, 14, 14, 14,		/* 41 - 48 */
				15, 15, 15, 15, 15, 15, 15, 15,		/* 49 - 56 */
				15, 15, 15, 15, 15, 15, 15, 15};	/* 57 - 64 */

	if (len > 64) {
		return 15;
	}
	return len2dlc[len];
}
#endif

/***************************************************************************/
/**
* \brief codrvCanInitWithDev - init can with device name
*
* codrvCanInit() is called from this function.
* 
*/
RET_T codrvCanInitWithDev(
		const char * pDevStr,
		UNSIGNED16 bitRate
	) 
{
RET_T ret;

	interface_cstr = pDevStr;
	ret = codrvCanInit(bitRate);
	interface_cstr = NULL; //unsure, if pDevStr is avalable later

	return ret;
}

/***************************************************************************/
/**
* \brief codrvCanInit - initialize CAN controller
*
* Initialize the CAN controller and setup bitrate
* The CAN controller is disabled.
*
* \param
*	bitrate	- CANopen bitrate
* \results
*	nothing
*/
RET_T codrvCanInit(
		UNSIGNED16	bitRate		/**< Bitrate */
	)
{
RET_T	retVal = RET_OK;
struct ifreq ifr;
struct sockaddr_can addr;
#if defined CANFD
const int canfd_on = 1;
int mtu;
#endif

    /* create RAW socket for CAN */
    mBSDSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (mBSDSocket < 0) {
        perror("socket");
        return RET_DRV_ERROR;
    }

    /* link socket to specified CAN interface */
    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, interface_cstr);
#ifdef DEBUG
    printf("using SocketCAN device %s\n", ifr.ifr_name);
#endif
    if (ioctl(mBSDSocket, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        return  RET_DRV_ERROR;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

#if defined CANFD
    /* check if the frame fits into the CAN netdevice */
    if (ioctl(mBSDSocket, SIOCGIFMTU, &ifr) < 0) {
	    perror("SIOCGIFMTU");
	    return RET_DRV_ERROR;
    }
    mtu = ifr.ifr_mtu;

    if (mtu != CANFD_MTU) {
	    fprintf(stderr, "CAN interface ist not CAN FD capable - sorry.\n");
	    return RET_DRV_ERROR;
	}
#endif



    /* bind to socket, so that we can receive messages from it */
    if (bind(mBSDSocket, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return RET_DRV_ERROR;
    }

#if defined CANFD
    /* try to switch the socket into CAN FD mode */
    if (setsockopt(mBSDSocket, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
			&canfd_on, sizeof(canfd_on))) {
		fprintf(stderr, "error when enabling CAN FD support\n");
    }
#endif
/*------------------------------- can_dump.c ---------------------------*/
	do {
	int rcvbuf_size = 500000; 
		if (rcvbuf_size) {

			int curr_rcvbuf_size;
			socklen_t curr_rcvbuf_size_len = sizeof(curr_rcvbuf_size);

			/* try SO_RCVBUFFORCE first, if we run with CAP_NET_ADMIN */
			if (setsockopt(mBSDSocket, SOL_SOCKET, SO_RCVBUFFORCE,
				       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
#ifdef DEBUG
				printf("SO_RCVBUFFORCE failed so try SO_RCVBUF ...\n");
#endif
				if (setsockopt(mBSDSocket, SOL_SOCKET, SO_RCVBUF,
					       &rcvbuf_size, sizeof(rcvbuf_size)) < 0) {
					perror("setsockopt SO_RCVBUF");
					break;
					//return 1;
				}

				if (getsockopt(mBSDSocket, SOL_SOCKET, SO_RCVBUF,
					       &curr_rcvbuf_size, &curr_rcvbuf_size_len) < 0) {
					perror("getsockopt SO_RCVBUF");
					break;
					//return 1;
				}

				/* Only print a warning the first time we detect the adjustment */
				/* n.b.: The wanted size is doubled in Linux in net/sore/sock.c */
				if (/*!i &&*/ curr_rcvbuf_size < rcvbuf_size*2)
					fprintf(stderr, "The socket receive buffer size was "
						"adjusted due to /proc/sys/net/core/rmem_max.\n");
			}
		}
	} while(0);

	/* read back */
	{
	int rcvbuf_size = 0;
	unsigned int cnt = sizeof(rcvbuf_size);
		if (getsockopt(mBSDSocket, SOL_SOCKET, SO_RCVBUF,
				   &rcvbuf_size, &cnt) >= 0) 
		{
			//printf("rxbuf %d (%d==%d)\n", rcvbuf_size, cnt, sizeof(rcvbuf_size));
			printf("rxbuf %d\n", rcvbuf_size);
		}
	}
/*---------------------------------------------------------------------*/

    /* set bitrate */
    retVal = codrvCanSetBitRate(bitRate);

/*----- enable error frames -------------------------------------------*/
	{
	can_err_mask_t err_mask = CAN_ERR_FLAG | CAN_ERR_CRTL | CAN_ERR_PROT | CAN_ERR_BUSOFF;//0xFFFFFFFFu; //all error

		if (setsockopt(mBSDSocket, SOL_CAN_RAW, CAN_RAW_ERR_FILTER,
                                           &err_mask, sizeof(err_mask)) < 0)
		{
			fprintf(stderr,"no error messages possible\n");
		}
	}
/*---------------------------------------------------------------------*/
    return(retVal);
}


/***********************************************************************
* codrvCanSetBitRate - sets the CAN bit rate
*
* \internal
*
* SocketCAN does not support setting the bitrate with user rights.
* So the bitrate has to be configured before as root.
*
* \param
*	bitrate
* \results
*	RET_T
*/
RET_T codrvCanSetBitRate(
		UNSIGNED16	bitRate			/**< bit rate */
	)
{
    // avoid warning
    (void)bitRate;

    fprintf(stderr," setting bitrate not supported by SocketCAN\n");

    // we return RET_OK anyway to continue
    return(RET_OK);
}


/***********************************************************************
* codrvCanEnable - enable CAN controller
*
* \internal
*
* \param
*	file descriptor
*	bitrate
* \results
*	RET_T
*/
RET_T codrvCanEnable(
		void	/* no parameter */
	)
{
RET_T	retVal = RET_OK;

    canEnabled = CO_TRUE;

    coCommStateEvent(CO_COMM_STATE_EVENT_BUS_ON);

    // poll Transmit
    codrvCanStartTransmission();

    return(retVal);
}

/***********************************************************************/
/**
* \brief codrvCanDisable - disable CAN controller
*
* This function disables the CAN controller. Often this function wait 
* that the CAN controller is disabled, because ofter after them
* are functionality implemented, that require this.
* But note, the required time could be the time of one CAN frame.
*
* \return RET_T
* \retval RET_OK
*	CAN controller is set to disable
*
*/
RET_T codrvCanDisable(
		void	/* no parameter */
	)
{
RET_T	retVal = RET_OK;

	/* if error is occured */
	//retVal = RET_DRV_ERROR;
	
	/* disable CAN controller */


	canEnabled = CO_FALSE;

	return(retVal);
}



/***********************************************************************
* codrvCanTransmit - transmit can message
*
* \internal
*
* \param
*	pBuf	pointer to message to be transmitted
* \results
*	RET_T
*/
static RET_T codrvCanTransmit(
		CO_CONST CO_CAN_MSG_T *pBuf
	)
{
RET_T retVal = RET_OK;
#if defined CANFD
struct canfd_frame frame;
#else
struct can_frame frame;
#endif
int i; /*! loop counter */
int nbytes;

    // copy CAN data into Socket CAN format
    frame.can_id  = pBuf->canCob.canId;

    // set rtr and extended flag
    if (pBuf->canCob.rtr == CO_TRUE) {
        frame.can_id |= CAN_RTR_FLAG;
    }

    if (pBuf->canCob.extended == CO_TRUE) {
        frame.can_id |= CAN_EFF_FLAG;
    }

    /* set length and copy data
     * ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
#if defined CANFD
    frame.len = can_dlc2len(can_len2dlc(pBuf->len));
    frame.flags = 0;
#else
    frame.can_dlc = (pBuf->len) & 0x0f;
#endif
    if (pBuf->canCob.rtr == CO_FALSE) {
        for (i = 0; i < pBuf->len; i++) {
            frame.data[i] = pBuf->data[i];
        }
    }


    /* send the can frame */
    if ((nbytes = write(mBSDSocket, &frame, sizeof(frame)))
			!= sizeof(frame)) {
		if (errno == EINTR) {
			/* ignore, but write later again. At the moment write() is not possible */
			return RET_DRV_ERROR; /* req. a better error code */
		}

		/* socketcan TX Buffer full */
        if (errno == ENOBUFS) {
			coCommStateEvent(CO_COMM_STATE_EVENT_TR_QUEUE_FULL);
		} else {
			perror("write");
		}

        return RET_DRV_ERROR;
    }

    /* send acq immediately - message was transmitted */
    /* we do not have a TX interrupt */
    coQueueMsgTransmitted(pBuf);

    return(retVal);
}


/***********************************************************************
* coCanStartTransmission - start can transmission if not active
*
* \internal
*
*
* \param
* \results
*/
RET_T codrvCanStartTransmission(
		void	/* no parameter */
	)
{
RET_T retval;

    if (canEnabled != CO_TRUE)  {
        return(RET_OK);
    }

	if (pTxBuf == NULL) {
		// query next message to be transmitted
		pTxBuf = coQueueGetNextTransmitMessage();
	}

    if (pTxBuf != NULL)  {
        // send message
        retval = codrvCanTransmit(pTxBuf);
		if (retval == RET_OK) {
			pTxBuf = NULL;
		}
		return RET_OK;
    }

    return(RET_OK);
}


/***********************************************************************
* coCanDriverHandler - can driver handler
*
* \internal
*
* should be called cyclically for drivers at a operating system
* Handles received data and CAN state
*
* \param
* \results
*/
void codrvCanDriverHandler(
		void	/* no parameter */
	)
{
CO_CAN_MSG_T *pRecBuf;  /*! pointer for received CAN messages */
int	got;             /*! return value for recvmsg and while-loop-variable */
UNSIGNED8	j;       /*! internal loop counter */
fd_set rdfs;             /*! set of file descriptors for select call */
struct timeval timeout;  /*! timeout for select call */
int ret;                 /*! return value for select call */
#if defined CANFD
struct canfd_frame frame;  /*! received socket can frame */
#else
struct can_frame frame;  /*! received socket can frame */
#endif

    /* handle received messages */
    got = 1;

    FD_ZERO(&rdfs);
    FD_SET(mBSDSocket, &rdfs);

    while (got > 0)  {
        /* get receiveBuffer */
        pRecBuf = coQueueGetReceiveBuffer();
        if (pRecBuf == NULL)  {
            break;
        }

        /* set no timeout for select */
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

		/* was it our socket ? */
		if ((ret = select(mBSDSocket + 1 , &rdfs, NULL, NULL, &timeout)) <= 0) {
            break;
        }

        /* was it our socket ? */
        if (FD_ISSET(mBSDSocket, &rdfs)) {

			/* read for CAN message */
			got = read(mBSDSocket, &frame, sizeof(frame));

			/* check for CAN error information */
			if (frame.can_id & CAN_ERR_FLAG) {
				//printf("CAN_ERROR_FLAG set 0x%08x\n", frame.can_id);

				if (frame.can_id & CAN_ERR_CRTL) {

					if (frame.data[1] & CAN_ERR_CRTL_RX_OVERFLOW) {
						coCommStateEvent(CO_COMM_STATE_EVENT_CAN_OVERRUN);
					}
					if (frame.data[1] & CAN_ERR_CRTL_TX_OVERFLOW) {
						coCommStateEvent(CO_COMM_STATE_EVENT_CAN_OVERRUN);
					}
					if (frame.data[1] & CAN_ERR_CRTL_RX_PASSIVE) {
						coCommStateEvent(CO_COMM_STATE_EVENT_PASSIVE);
					}
					if (frame.data[1] & CAN_ERR_CRTL_TX_PASSIVE) {
						coCommStateEvent(CO_COMM_STATE_EVENT_PASSIVE);
					}
				}

				if (frame.can_id & CAN_ERR_PROT) {
					// CAN_ERR_PROT_ACTIVE is often used for Error Active
					// but it was not created for this state change
					// Discussion: 
					// id - CAN_ERR_STATE_CHANGE
					// data[1] - CAN_ERR_CRTL_ACTIVE
					//
					if (frame.data[2] & CAN_ERR_PROT_ACTIVE) {
						coCommStateEvent(CO_COMM_STATE_EVENT_ACTIVE);
					}
				}

				if (frame.can_id & CAN_ERR_BUSOFF) {
					coCommStateEvent(CO_COMM_STATE_EVENT_BUS_OFF);
				}
			} else {

				/* copy CAN frame */
				pRecBuf->canCob.canId = frame.can_id & 0x1FFFFFFFul;
				if (frame.can_id & CAN_RTR_FLAG) {
					pRecBuf->canCob.rtr = CO_TRUE; 
				} else {
					pRecBuf->canCob.rtr = CO_FALSE; 
				}

				if (frame.can_id & CAN_EFF_FLAG) {
					pRecBuf->canCob.extended = CO_TRUE; 
				} else  {
					pRecBuf->canCob.extended = CO_FALSE; 
				}

#if defined DEBUG
#if defined CANFD
    printf("Rec frame.len = %d\n", frame.len);
#else
    printf("Rec frame.len = %d\n", frame.can_dlc);
#endif
#endif

#if defined CANFD
				pRecBuf->len = frame.len;
#else
				if(frame.can_dlc > 8) {
					pRecBuf->len = 8;
				} else {
					pRecBuf->len = frame.can_dlc;
				}
#endif
				for (j = 0; j < pRecBuf->len; j++) {
					pRecBuf->data[j] = frame.data[j];
				}

				
#ifdef DEBUG_HB
				// Debug
				if ((pRecBuf->canCob.canId >= 0x701)
					&& (pRecBuf->canCob.canId <= 0x77F))
				{
				struct timeval t;
					
					gettimeofday(&t, NULL);
					printf("%ld.%06ld: 0x%03x\n", t.tv_sec % 10, t.tv_usec, (unsigned int)(pRecBuf->canCob.canId));
					
				}
				// Debug end
#endif /*  DEBUG_HB */

				/* set buffer filled */
				coQueueReceiveBufferIsFilled();
				coCommTaskSet(CO_COMMTASK_EVENT_MSG_AVAIL);
			}
		}
    }

	// poll Transmit
	codrvCanStartTransmission();

    return;
}

/***********************************************************************/
/**
*
* \brief codrvGetCanFd - get can file descriptor
*
* \return can file descriptor
*
*/
int codrvGetCanFd(
		void	/* no parameter */
	)
{
	return(mBSDSocket);
}




/***********************************************************************/
/**
* \brief codrvWaitForEvent - wait until event is occurred
*
* This function sleeps until an event has occurred.
* This can be a CAN event or a timer elapsed event.
* 
* \return none
*
*/
void codrvWaitForEvent(
		UNSIGNED32	msecTimeout		/**< max sleep time in msec */
	)
{
struct timeval  timeOut;
fd_set	fdsRd, fdsWr;

	FD_ZERO(&fdsRd);
	FD_ZERO(&fdsWr);

//	FD_SET(0, &fdsRd);				/* watch on fd for stdin */

	{
		FD_SET(mBSDSocket, &fdsRd);		/* watch on CAN read */
		FD_SET(mBSDSocket, &fdsWr);		/* watch on CAN write */
	}

	timeOut.tv_sec = msecTimeout / 1000;
	timeOut.tv_usec =  (msecTimeout % 1000) * 1000;

//	result = select(FD_SETSIZE, &fdsRd, &fdsWr, NULL, &timeOut);
	select(FD_SETSIZE, &fdsRd, NULL, NULL, &timeOut);

	/* was CAN select, get new messages from CAN */
	{
		if (FD_ISSET(mBSDSocket, &fdsRd) != 0)  {
			codrvCanDriverHandler();
		}
	}

}


