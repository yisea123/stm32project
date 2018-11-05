/*
* codrv_bxcan.h
*
* Copyright (c) 2012-2016 emtas GmbH
*-------------------------------------------------------------------
* SVN  $Id: codrv_bxcan.h 12590 2016-03-03 10:49:39Z ro $
*
*
*-------------------------------------------------------------------
*
*
*/

/* function like macros - solution would be inline functions */
/*lint -e961 */ 

/********************************************************************/
/**
* \file
* \brief bxcan from ST (e.g. STM32)
*
*/

#ifndef CODRV_BXCAN_H
#define CODRV_BXCAN_H 1

/* Register offsets (U32) */
#define BXCAN_CANMCR 	(0x0000u/4u)
#define BXCAN_CANMSR	(0x0004u/4u)
#define BXCAN_CANTSR	(0x0008u/4u)
#define BXCAN_CANRF0R	(0x000Cu/4u)
#define BXCAN_CANRF1R	(0x0010u/4u)
#define BXCAN_CANRFXR(nr)	((0x000Cu + (nr * 4u)) / 4u)

#define BXCAN_CANIER	(0x0014u/4u)
#define BXCAN_CANESR	(0x0018u/4u)
#define BXCAN_CANBTR	(0x001Cu/4u)


#define BXCAN_CANTIXR(nr)	((0x0180u + (nr * 0x10u)) / 4u)
#define BXCAN_CANTDTXR(nr)	((0x0184u + (nr * 0x10u)) / 4u)
#define BXCAN_CANTDLXR(nr)	((0x0188u + (nr * 0x10u)) / 4u)
#define BXCAN_CANTDHXR(nr)	((0x018Cu + (nr * 0x10u)) / 4u)

#define BXCAN_CANRIXR(nr)	((0x01B0u + (nr * 0x10u)) / 4u)
#define BXCAN_CANRDTXR(nr)	((0x01B4u + (nr * 0x10u)) / 4u)
#define BXCAN_CANRDLXR(nr)	((0x01B8u + (nr * 0x10u)) / 4u)
#define BXCAN_CANRDHXR(nr)	((0x01BCu + (nr * 0x10u)) / 4u)

#define BXCAN_CANFMR	(0x0200u/4u)
#define BXCAN_CANFM1R	(0x0204u/4u)
#define BXCAN_CANFS1R	(0x020Cu/4u)
#define BXCAN_CANFFA1R	(0x0214u/4u)

#define BXCAN_CANFA1R	(0x021Cu/4u)

/* filter bank nr = 0..27, i = 1..2 */
#define BXCAN_CANFIRX(nr,i)		((0x0240u + ((nr) * 8u) + (((i) - 1u) * 4u)) / 4u)
#define FIRX_MASK(mask) ((UNSIGNED32)(mask) << 16)
#define FIRX_ID(id) 	((UNSIGNED32)(id) << 5)
#define FIRX_RTR		(1u << 4)
#define FIRX_IDE		(1u << 3)
/* 0.. 13, connectivity line use for both CAN 0..27 */
#define BXCAN_MAX_FILTERBANK	13u
#define BXCAN_FILTERBANK_CNT	(BXCAN_MAX_FILTERBANK + 1u)

/** CANMCR */
#define	BXCAN_CANMCR_INRQ 	(1ul << 0)
#define	BXCAN_CANMCR_SLEEP 	(1ul << 1)
#define	BXCAN_CANMCR_TXFP 	(1ul << 2)
#define	BXCAN_CANMCR_RFLM 	(1ul << 3)
#define	BXCAN_CANMCR_NART 	(1ul << 4)
#define	BXCAN_CANMCR_AWUM 	(1ul << 5)
#define	BXCAN_CANMCR_ABOM 	(1ul << 6)
#define	BXCAN_CANMCR_TTCM 	(1ul << 7)
#define	BXCAN_CANMCR_RESET 	(1ul << 15)
#define	BXCAN_CANMCR_DBF	(1ul << 16)

/** CANMSR */
#define	BXCAN_CANMSR_INAK 	(1ul << 0)
#define	BXCAN_CANMSR_SLAK 	(1ul << 1)
#define	BXCAN_CANMSR_ERRI	(1ul << 2)
#define	BXCAN_CANMSR_WKUI 	(1ul << 3)
#define	BXCAN_CANMSR_SLAKI	(1ul << 4)
#define	BXCAN_CANMSR_TXM	(1ul << 8)
#define	BXCAN_CANMSR_RXM 	(1ul << 9)
#define	BXCAN_CANMSR_SAMP 	(1ul << 10)
#define	BXCAN_CANMSR_RX 	(1ul << 11)

/** CANTSR */
#define	BXCAN_CANTSR_RQCP0 	(1ul << 0)
#define	BXCAN_CANTSR_TXOK0 	(1ul << 1)
#define	BXCAN_CANTSR_ALST0 	(1ul << 2)
#define	BXCAN_CANTSR_TERR0 	(1ul << 3)
#define	BXCAN_CANTSR_ABRQ0	(1ul << 7)

#define	BXCAN_CANTSR_RQCP1 	(1ul << 8)
#define	BXCAN_CANTSR_TXOK1 	(1ul << 9)
#define	BXCAN_CANTSR_ALST1 	(1ul << 10)
#define	BXCAN_CANTSR_TERR1 	(1ul << 11)
#define	BXCAN_CANTSR_ABRQ1 	(1ul << 15)

#define	BXCAN_CANTSR_RQCP2	(1ul << 16)
#define	BXCAN_CANTSR_TXOK2 	(1ul << 17)
#define	BXCAN_CANTSR_ALST2 	(1ul << 18)
#define	BXCAN_CANTSR_TERR2 	(1ul << 19)
#define	BXCAN_CANTSR_ABRQ2 	(1ul << 23)
	
#define	BXCAN_CANTSR_CODE_MSK 	((1ul << 24) | (1ul << 25))
#define	BXCAN_CANTSR_TME0  	(1ul << 26)
#define	BXCAN_CANTSR_TME1  	(1ul << 27)
#define	BXCAN_CANTSR_TME2  	(1ul << 28)

#define	BXCAN_CANTSR_TME_MSK	((1ul << 26) | (1ul << 27) | (1ul << 28))
#define	BXCAN_CANTSR_TME_ALL	((1ul << 26) | (1ul << 27) | (1ul << 28))



#define	BXCAN_CANTSR_LOW0  	(1ul << 29)
#define	BXCAN_CANTSR_LOW1  	(1ul << 30)
#define	BXCAN_CANTSR_LOW2  	(1ul << 31)


/** CANRFXR - CANRF0R CANRF1R */
#define	BXCAN_CANRFXR_FMP_MSK 	((1ul << 0) | (1ul << 1))
#define	BXCAN_CANRFXR_FULL 	(1ul << 3)
#define	BXCAN_CANRFXR_FOVR 	(1ul << 4)
#define	BXCAN_CANRFXR_RFOM 	(1ul << 5)

/** CANIER */
#define	BXCAN_CANIER_TMEIE 	(1ul << 0)

#define	BXCAN_CANIER_FMPIE0 	(1ul << 1)
#define	BXCAN_CANIER_FFIE0 	(1ul << 2)
#define	BXCAN_CANIER_FOVIE0 	(1ul << 3)

#define	BXCAN_CANIER_FMPIE1 	(1ul << 4)
#define	BXCAN_CANIER_FFIE1 	(1ul << 5)
#define	BXCAN_CANIER_FOVIE1 	(1ul << 6)

#define	BXCAN_CANIER_EWGIE 	(1ul << 8)
#define	BXCAN_CANIER_EPVIE 	(1ul << 9)
#define	BXCAN_CANIER_BOFIE 	(1ul << 10)
#define	BXCAN_CANIER_LECIE 	(1ul << 11)

#define	BXCAN_CANIER_ERRIE 	(1ul << 15)
#define	BXCAN_CANIER_WKUIE 	(1ul << 16)
#define	BXCAN_CANIER_SLKIE 	(1ul << 17)

/** CANESR */
#define	BXCAN_CANESR_EWGF 	(1ul << 0)
#define	BXCAN_CANESR_EPVF 	(1ul << 1)
#define	BXCAN_CANESR_BOFF 	(1ul << 2)
#define	BXCAN_CANESR_LECC_MSK 	((1ul << 4) | (1ul << 5) | (1ul << 6))
#define	BXCAN_CANESR_TEC_MSK 	0x00FF0000ul
#define	BXCAN_CANESR_REC_MSK 	0xFF000000ul

/** CANBTR */
#define	BXCAN_CANBTR_BRP_MSK 	0x000003FFul
#define	BXCAN_CANBTR_BRP_SHIFT 	0

#define	BXCAN_CANBTR_TS1_MSK 	0x000F0000ul
#define	BXCAN_CANBTR_TS1_SHIFT 	16

#define	BXCAN_CANBTR_TS2_MSK 	0x00700000ul
#define	BXCAN_CANBTR_TS2_SHIFT 	20

#define	BXCAN_CANBTR_SJW_MSK 	0x03000000ul
#define	BXCAN_CANBTR_SJW_SHIFT 	24

#define	BXCAN_CANBTR_LBKM 	(1ul << 30)
#define	BXCAN_CANBTR_SILM 	(1ul << 31)

/** CANTIXR */
#define	BXCAN_CANTIXR_TXRQ 	(1ul << 0)
#define	BXCAN_CANTIXR_RTR  	(1ul << 1)
#define	BXCAN_CANTIXR_IDE  	(1ul << 2)
#define	BXCAN_CANTIXR_EXID_MSK	0xFFFFFFF8ul
#define	BXCAN_CANTIXR_EXID_SHIFT	3
#define	BXCAN_CANTIXR_STID_MSK	0xFF800000ul
#define	BXCAN_CANTIXR_STID_SHIFT	21

/** CANTDTXR */
#define	BXCAN_CANTDTXR_DLC_MSK 	0x0000000Ful
#define	BXCAN_CANTDTXR_TGT  	(1ul << 8)
#define	BXCAN_CANTDTXR_TIME_MSK	0xFFFF0000ul

/** CANRIXR */
#define	BXCAN_CANRIXR_RTR  	(1ul << 1)
#define	BXCAN_CANRIXR_IDE  	(1ul << 2)
#define	BXCAN_CANRIXR_EXID_MSK 	0xFFFFFFF8ul
#define	BXCAN_CANRIXR_EXID_SHIFT 	3
#define	BXCAN_CANRIXR_STID_MSK 	0xFF800000ul
#define	BXCAN_CANRIXR_STID_SHIFT 	21
	

/** CANRDTXR */
#define	BXCAN_CANRDTXR_DLC_MSK 	0x0000000Ful
#define	BXCAN_CANRDTXR_FMI_MSK 	0x0000FF00ul
#define	BXCAN_CANRDTXR_TIME_MSK 0xFFFF0000ul

/** CANFMR */
#define	BXCAN_CANFMR_FINIT 	(1ul << 0)
#define	BXCAN_CANFMR_CAN2SB_MSK 	0x00007F00ul

/* global prototypes, that not in co_drv.h */
void codrvCanReceiveInterrupt(void);
void codrvCanTransmitInterrupt(void);
void codrvCanErrorInterrupt(void);

/* extern required functions */
extern void codrvCanSetTxInterrupt(void);


#endif
