/*
* codrv_canbittiming.c - CAN Bittiming tables 
*
* Copyright (c) 2012-2015 emtas GmbH
*-------------------------------------------------------------------
* $Id: codrv_canbittiming.c 19988 2017-10-04 07:45:38Z ro $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \brief 
*
* \file 
* \author emtas GmbH
*
* This module contains different bittiming tables.
*
* http://www.can-wiki.info/bittiming/tq.html
*
*/
/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_datatype.h>
#include <co_drv.h>

#ifdef CODRV_BIT_TABLE_EXTERN 


/*
* 80 MHz table
*
* Samplepoint is not on 87.5%.
*/

#ifdef CODRV_CANCLOCK_80MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		{	10u,	500u,	0u,	13u,	2u },	/* 87.5% */
		{	20u,	250u,	0u, 13u,	2u },	/* 87.5% */
		{	50u,	100u,	0u, 13u,	2u },	/* 87.5% */
		{	100u,	50u,	0u, 13u,	2u },	/* 87.5% */
		{	125u,	40u,	0u, 13u,	2u },	/* 87.5% */
		{	250u,	20u,	0u, 13u,	2u },	/* 87.5% */
		{	500u,	10u,	0u, 13u,	2u },	/* 87.5% */
		{	800u,	10u,	0u, 8u,		1u },	/* 90.0% */
		{	1000u,	5u,		0u, 13u,	2u },	/* 87.5% */
		{	0u,		0u,		0u, 0u, 	0u }	/* last */
	};
#endif /*CODRV_CANCLOCK_80MHZ */

/*
* 72 MHz table
*
* Samplepoint is not on 87.5%.
*/

#ifdef CODRV_CANCLOCK_72MHZ
static CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 72MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u, 450u, 0u, 13u, 2u },
		{   20u, 225u, 0u, 13u, 2u },
		{   50u,  90u, 0u, 13u, 2u },
# endif
		{  100u,  45u, 0u, 13u, 2u },
		{  125u,  36u, 0u, 13u, 2u },
		{  250u,  18u, 0u, 13u, 2u },
		{  500u,   9u, 0u, 13u, 2u },
		{  800u,   6u, 0u, 12u, 2u }, /* 86.7% */
		{ 1000u,   9u, 0u,  6u, 1u }, /* 8tq 87.5% */
		{0u, 0u, 0u, 0u, 0u} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_64MHZ
static CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 64MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u, 400u, 0u, 13u, 2u },
		{   20u, 200u, 0u, 13u, 2u },
		{   50u,  80u, 0u, 13u, 2u },
# endif
		{  100u,  40u, 0u, 13u, 2u },
		{  125u,  32u, 0u, 13u, 2u },
		{  250u,  16u, 0u, 13u, 2u },
		{  500u,   8u, 0u, 13u, 2u },
		{  800u,   5u, 0u, 13u, 2u }, 
		{ 1000u,   4u, 0u, 13u, 2u }, 
		{0u, 0u, 0u, 0u, 0u} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_60MHZ
static CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 60MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u, 375u, 0u, 13u, 2u },
		{   20u, 200u, 0u, 12u, 2u }, /* 86.7% */
		{   50u,  75u, 0u, 13u, 2u },
# endif
		{  100u,  40u, 0u, 12u, 2u }, /* 86.7% */
		{  125u,  30u, 0u, 13u, 2u },
		{  250u,  15u, 0u, 13u, 2u },
		{  500u,   8u, 0u, 12u, 2u }, /* 86.7% */
		{  800u,   5u, 0u, 12u, 2u }, /* 86.7% */
		{ 1000u,   4u, 0u, 12u, 2u }, /* 86.7% */
		{0u, 0u, 0u, 0u, 0u} /* last */
	};
#endif

/*
* 50 MHz table
*
* Samplepoint is not on 87.5%.
*/

#ifdef CODRV_CANCLOCK_50MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 50MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{    10u,250u, 0, 16u, 3u }, /* !! 85% */
		{    20u,125u, 0, 16u, 3u }, /* !! 85% */
# endif
		{    50u, 50u, 0, 16u, 3u }, /* !! 85% */
		{   100u, 25u, 0, 16u, 3u }, /* !! 85% */
		{   125u, 25u, 0, 13u, 2u }, 
		{   250u, 10u, 0, 16u, 3u }, /* !! 85% */
		{   500u,  5u, 0, 16u, 3u }, /* !! */
		{  1000u,  5u, 0, 7u, 2u }, /* !! 80% */
		{0,0,0,0,0} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_48MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 48MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{    10u,300u, 0, 13u, 2u }, 
		{    20u,150u, 0, 13u, 2u },
# endif
		{    50u, 60u, 0, 13u, 2u },
		{   100u, 30u, 0, 13u, 2u },
		{   125u, 24u, 0, 13u, 2u },
		{   250u, 12u, 0, 13u, 2u },
		{   500u,  6u, 0, 13u, 2u },
		{  1000u,  3u, 0, 13u, 2u },
		{0,0,0,0,0} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_45MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 45MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{    10u,300u, 0, 12u, 2u },  /* 86.7% */ 
		{    20u,150u, 0, 12u, 2u },  /* 86.7% */
# endif
		{    50u, 60u, 0, 12u, 2u },  /* 86.7% */
		{   100u, 30u, 0, 12u, 2u },  /* 86.7% */
		{   125u, 24u, 0, 12u, 2u },  /* 86.7% */
		{   250u, 12u, 0, 12u, 2u },  /* 86.7% */
		{   500u,  6u, 0, 12u, 2u },  /* 86.7% */
		{  1000u,  3u, 0, 12u, 2u },  /* 86.7% */
		{0,0,0,0,0} /* last */
	};
#endif
	
	
#ifdef CODRV_CANCLOCK_42MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 42MHz table, prescaler 10bit (max 1024) */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u,280u, 0, 12u, 2u },
		{   20u,140u, 0, 12u, 2u }, /* 86,7% */
# endif
		{   50u, 56u, 0, 15u, 2u },
		{  100u, 28u, 0, 16u, 3u },
		{  125u, 21u, 0, 13u, 2u },
		{  250u, 12u, 0, 11u, 2u },
		{  500u,  6u, 0, 11u, 2u },
		{ 1000u,  3u, 0, 11u, 2u},
		{0,0,0,0,0} /* last */
	};
#endif




#ifdef CODRV_CANCLOCK_36MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 36MHz table, prescaler 10bit (max 1024) */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u,225u, 0, 13u, 2u }, 
		{   20u,120u, 0, 12u, 2u }, /* 86,7% */
# endif
		{   50u, 45u, 0, 13u, 2u }, /* 85% */
		{  100u, 18u, 0, 16u, 3u }, /* 85% */
		{  125u, 18u, 0, 13u, 2u },
		{  250u,  9u, 0, 13u, 2u },
		{  500u,  9u, 0,  6u, 1u }, /* only 8tq */
		{  800u,  3u, 0, 12u, 2u },
		{ 1000u,  2u, 0, 14u, 3u}, /* 83.3% */
		{0,0,0,0,0} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_32MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 32MHz table, prescaler 10bit (max 1024) */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u,200u, 0, 13u, 2u }, 
		{   20u,100u, 0, 13u, 2u }, 
# endif
		{   50u, 40u, 0, 13u, 2u }, 
		{  100u, 20u, 0, 13u, 2u },
		{  125u, 16u, 0, 13u, 2u },
		{  250u,  8u, 0, 13u, 2u },
		{  500u,  4u, 0, 13u, 2u }, 
		{  800u,  5u, 0,  6u, 1u },
		{ 1000u,  2u, 0, 13u, 2u},
		{0,0,0,0,0} /* last */
	};
#endif

#ifdef CODRV_CANCLOCK_25MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 25MHz table, prescaler 10bit (max 1024) */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u,125u, 0, 16u, 3u },
# endif
		{   20u, 50u, 0, 16u, 8u },
		{   50u, 25u, 0, 16u, 3u },
		{  100u, 10u, 0, 16u, 8u },
		{  125u, 10u, 0, 16u, 3u },
		{  250u,  5u, 0, 16u, 3u },
		{  500u,  2u, 0, 16u, 8u },
		{ 1000u,  1u, 0, 16u, 8u},
		{0,0,0,0} /* last */
	};
#endif


#ifdef CODRV_CANCLOCK_20MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 20MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{    10u,125u, 0, 13u, 2u }, 
# endif
		{    20u, 50u, 0, 16u, 3u }, /* !! 85% */
		{    50u, 25u, 0, 13u, 2u }, 
		{   100u, 10u, 0, 16u, 3u }, /* !! 85% */
		{   125u, 10u, 0, 13u, 2u }, 
		{   250u,  5u, 0, 13u, 2u },
		{   500u,  2u, 0, 16u, 3u }, /* !! 85% */
		{  1000u,  1u, 0, 16u, 3u }, /* !! 85% */
		{0,0,0,0,0} /* last */
	};
#endif


#ifdef CODRV_CANCLOCK_18MHZ
static CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 18MHz table, prescaler 10bit (max 1024) */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{   10u, 100u, 0u, 15u, 2u }, /* 88.9% */
# endif
		{   20u,  50u, 0u, 15u, 2u }, /* 88.9% */
		{   50u,  18u, 0u, 16u, 3u }, /* 85.0% */
		{  100u,  12u, 0u, 12u, 2u }, /* 86.7% */
		{  125u,   9u, 0u, 13u, 2u },
		{  250u,   4u, 0u, 15u, 2u }, /* 88.9% */
		{  500u,   2u, 0u, 15u, 2u }, /* 88.9% */
		{ 1000u,   1u, 0u, 15u, 2u }, /* 88.9% */
		{0u, 0u, 0u, 0u, 0u} /* last */
	};
#endif



#ifdef CODRV_CANCLOCK_16MHZ
CO_CONST CODRV_BTR_T codrvCanBittimingTable[] = {
		/* 16MHz table, prescaler 6bit (max 64) + BRPE 4bit == 1024 */
# ifdef CODRV_CANCLOCK_PRE_10BIT
		{    10u,100u, 0u, 13u, 2u }, 
# endif
		{    20u, 50u, 0u, 13u, 2u }, 
		{    50u, 20u, 0u, 13u, 2u }, 
		{   100u, 10u, 0u, 13u, 2u }, 
		{   125u,  8u, 0u, 13u, 2u }, 
		{   250u,  4u, 0u, 13u, 2u },
		{   500u,  2u, 0u, 13u, 2u }, 
		{   800u,  1u, 0u, 16u, 3u },
		{  1000u,  1u, 0u, 13u, 2u }, 
		{0u,0u,0u,0u,0u} /* last */
	};
#endif



#endif /* CODRV_BIT_TABLE_EXTERN */
