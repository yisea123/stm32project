/*
* fwupdate.h - firmware update
*
* Copyright (c) 2012 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: fwupdate.h 16403 2017-01-02 09:36:19Z hil $

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

