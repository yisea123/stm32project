/*
* cpu_linux.c - contains driver for linux
*
* Copyright (c) 2012-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: cpu_linux.c 11230 2015-10-27 15:02:45Z ro $
*
*
*-------------------------------------------------------------------
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
#ifndef LINUX_THREAD_TIMER
# define LINUX_SIGNAL_TIMER	1
#endif

#define SIG SIGRTMIN

#define errExit(msg)	do { perror(msg); exit(EXIT_FAILURE); \
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


#ifdef LINUX_SIGNAL_TIMER
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


/***************************************************************************/
/**
* \internal
*
* \brief timerInt - timer interrupt
*
* \param
* \results
*	nothing
*/
void timerInt (
		int sig,
		siginfo_t *si,
		void *uc
    )
{
//	printf("Caught signal %d\n", sig);
	coTimerTick();
}
#endif /* LINUX_SIGNAL_TIMER */


#ifdef LINUX_THREAD_TIMER
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
static __useconds_t interval = 0;
interval = timerInterval;
pthread_t inc_x_thread = 0;

	if (inc_x_thread != 0) {
		printf("ERROR: resetting the CANopen timre not (yet) supported\n");
		return RET_INTERNAL_ERROR;
	}

	pthread_create(&inc_x_thread, NULL, coTimerThreadFn, &interval);

	return RET_OK;
}


void *coTimerThreadFn(void *x_void_ptr)
{
struct timeval  tv;
static unsigned long l_time_usec_last=0;
static long n=0;
__useconds_t interval = *(__useconds_t*)x_void_ptr;

	while(1) {
		gettimeofday(&tv, NULL);
		unsigned long l_time_usec = (tv.tv_sec) * 1000000 + (tv.tv_usec);
		if (l_time_usec_last == 0)  {
			l_time_usec_last = l_time_usec;
		}
		++n;

		coTimerTick();
		usleep(interval-58);
	}

	return x_void_ptr;
}
#endif /* LINUX_THREAD_TIMER */


#ifdef THREAD_SPAWN
RET_T codrvTimerSetup(
		UNSIGNED32	timerInterval
	)
{
	timer_t timerid;
	struct sigevent sev;
	struct itimerspec its;

	/* Create the timer */

	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_signo = 0;
	sev.sigev_value.sival_int = 0;
	sev.sigev_notify_function = timerIntSpawnedThread;
	sev.sigev_notify_attributes = 0;

	if (timer_create(CLOCK_MONOTONIC, &sev, &timerid) == -1)  {
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


void timerIntThread(
		sigval_t sival
	)
{
	coTimerTick();
}
#endif
