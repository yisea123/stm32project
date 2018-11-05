/*
* co_401.h - contains defines for ciA 401 profile implementation
*
* Copyright (c) 2013-2017 emtas GmbH
*
*-------------------------------------------------------------------
* $Id: co_p401.h 20151 2017-10-30 13:06:25Z boe $

*-------------------------------------------------------------------
*
*
*
*/

/**
* \brief defines for ciA 401 profile implementation
*
* \file co_p401.h
* co_401 profile specific CANopen header
*
*/

#ifndef CO_401_H
#define CO_401_H 1


/* Profile offset, multiple of 0x800
 * Set by the CANopen Device Designer */
#if !defined(CO_401_PROFILE_OFFSET)
# define  CO_401_PROFILE_OFFSET 0u
#endif


/* Makes it sense to let the design tool specify the number of used
 * process IO, to reserve only needed resources?
 * */
#define CO_401_NDIGIN8	16		/* 8x16 = 128 bits */
#define CO_401_NDIGOUT8	16		/* 8x16 = 128 bits */
#define CO_401_NANIN16	16
#define CO_401_NANOUT16	16

/* Number of possible mappings in RPDOs
 * ! Objects can be mapped more than one time !*/
#define MAPPINGTABLE_MAX (		\
		(CO_401_NDIGOUT8 * 2)	\
		+ (CO_401_NANOUT16 * 1))

/* HW API functions */

/* digout */
typedef void (* CO_EVENT_401_DO_T)(UNSIGNED8, UNSIGNED8);
/* anout */
typedef void (* CO_EVENT_401_AO_T)(UNSIGNED8, INTEGER16);
/* digin */
typedef UNSIGNED8 (* CO_EVENT_401_DI_T)(UNSIGNED8, UNSIGNED8);
/* anin */
typedef INTEGER16 (* CO_EVENT_401_AI_T)(UNSIGNED8);


/* HW API registration functions */
/* all HW API functions at all */
RET_T coEventRegister_401(CO_EVENT_401_DI_T, CO_EVENT_401_DO_T,
							CO_EVENT_401_AI_T, CO_EVENT_401_AO_T);


RET_T co401Init(void);
void co401Task(void);
void co401DigOutDef(void);
void co401DigOutErr(void);
void co401AnOutDef(void);
void co401AnOutErr(void);


/* vim: set ts=4 sw=4 spelllang=en : */
#endif /*  CO_401_H */
