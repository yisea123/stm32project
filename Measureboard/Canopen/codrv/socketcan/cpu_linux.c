/*
* cpu_linux.c - contains driver for linux
*
* Copyright (c) 2012-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_linux.c 8611 2015-02-27 15:23:16Z boe $
*
*
*-------------------------------------------------------------------
* Changelog:
*
*
*/

/********************************************************************/
/**
* \file
* \brief 
*
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>
#include <co_datatype.h>
#include <co_timer.h>
#include <co_drv.h>

/* constant definitions
---------------------------------------------------------------------------*/
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

/* local defined data types
---------------------------------------------------------------------------*/

/* list of external used functions, if not in headers
---------------------------------------------------------------------------*/

/* list of global defined functions
---------------------------------------------------------------------------*/
void codrvHardwareInit(void);

/* list of local defined functions
---------------------------------------------------------------------------*/
static void timerInt (int sig, siginfo_t *si, void *uc);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/

/* local defined variables
---------------------------------------------------------------------------*/

/***************************************************************************/
/**
* \brief codrvHardwareInit - hardware initialization
*
* This function initialize the hardware, incl. Clock and CAN hardware.
*/
void codrvHardwareInit(void)
{
	/* normally nothing to do */
}

/***************************************************************************/
/**
* \brief codrvCanEnableInterrupt - enable the CAN interrupt
*
*/
void codrvCanEnableInterrupt(
		void	/* no parameter */
	)
{
	/* enable CAN interrupts */
}

/***************************************************************************/
/**
* \brief codrvCanDisableInterrupt - disable the CAN interrupt
*
*/
void codrvCanDisableInterrupt(
		void	/* no parameter */
	)
{
	/* disable CAN interrupts */
}



/***************************************************************************/
/**
* \brief codrvTimerSetup - init Timer
*
* \param
* \results
*	nothing
*/
RET_T codrvTimerSetup(
		UNSIGNED32	timerInterval
	)
{
timer_t timerid;
struct sigevent sev;
struct sigaction sa;
struct itimerspec its;
#define SIG SIGRTMIN

	/* Establish handler for timer signal */
	printf("Establishing handler for signal %d\n", SIG);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = timerInt;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIG, &sa, NULL) == -1)  {
		return(RET_INTERNAL_ERROR);
	}

	/* Create the timer */
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIG;
	sev.sigev_value.sival_ptr = &timerid;
	if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)  {
		return(RET_INTERNAL_ERROR);
	}
	printf("timer ID is 0x%lx\n", (long) timerid);

   	/* Start the timer (value is in µsec */
	its.it_value.tv_sec = timerInterval / 1000000;
	its.it_value.tv_nsec = (timerInterval % 1000000) * 1000;
	its.it_interval.tv_sec = its.it_value.tv_sec;
	its.it_interval.tv_nsec = its.it_value.tv_nsec;

	if (timer_settime(timerid, 0, &its, NULL) == -1)  {
		return(RET_INTERNAL_ERROR);
	}

	return(RET_OK);
}


void timerInt (
		int sig,
		siginfo_t *si,
		void *uc
    )
{
//	printf("Caught signal %d\n", sig);
	coTimerTick();
}

