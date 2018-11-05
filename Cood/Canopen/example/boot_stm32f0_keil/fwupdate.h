/*
* fwupdate.h - firmware update
*
* Copyright (c) 2012 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: fwupdate.h 8728 2015-03-06 12:29:15Z ro $

*-------------------------------------------------------------------
*
* Changelog:
*
*
*/

/**
* \file
* \brief req. Information for the initiation of the firmware update
*/

#ifndef FWUPDATE_H
#define FWUPDATE_H 1


/* datatypes */

/* definitions */

#define COMMAND_SIZE 4
#define COMMAND_BOOT "BOOT"
#define COMMAND_NONE "NNNN"

/* external data */
extern UNSIGNED8 cobl_command[16];


/* function prototypes */
extern void jump2Bootloader(void);

#endif /* ANZEIGE_H */

