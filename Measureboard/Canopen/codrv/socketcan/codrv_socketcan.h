/*
* codrv_socketcan.h - contains driver for SocketCAN
*
* Copyright (c) 2012-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: codrv_socketcan.h 15810 2016-10-28 09:38:07Z boe $
*
*
*-------------------------------------------------------------------
*
*
*/

#ifndef CODRV_SOCKETCAN_H
#define CODRV_SOCKETCAN_H 1

RET_T codrvCanInitWithDev(const char * pDevStr, UNSIGNED16 bitRate); 


#endif /* CODRV_SOCKETCAN_H */
