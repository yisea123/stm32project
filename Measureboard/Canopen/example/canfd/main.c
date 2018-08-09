/*
* main.c - contains program main
*
* Copyright (c) 2012-2014 emtas GmbH
*-------------------------------------------------------------------
* $Id: main.c 2027 2012-12-17 15:21:57Z  $
*
*
*-------------------------------------------------------------------
*
*
*/

/********************************************************************/
/**
* \file
* \brief main routine
*
* start sdo server by 
*	./canfd*  -n127
* start sdo client by 
*	./canfd*  -n1
*
* a special sdo transfermode can be enabled with -t1
* 	./canfd*  -n127
*	./canfd*  -t1 -n1
*/

/* header of standard C - libraries
---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

/* header of project specific types
---------------------------------------------------------------------------*/
#include <gen_define.h>

#include <co_canopen.h>

/* constant definitions
---------------------------------------------------------------------------*/
#define DOM_SIZE 1000000		/* used domain size if not specified */

/* ------------------------------ */
#define START 	1	/* control time measurement */
#define STOP	2

/* local defined data types
---------------------------------------------------------------------------*/
typedef struct {
	BOOL_T		write;
	UNSIGNED16	index;
	UNSIGNED8	*pBuf;
	UNSIGNED32	size;
	char		*comment;
} SDO_TRANSMIT_TABLE;


/* list of external used functions, if not in headers
--------------------------------------------------------------------------*/
void codrvHardwareInit(void);

/* list of global defined functions
---------------------------------------------------------------------------*/

/* list of local defined functions
---------------------------------------------------------------------------*/
static RET_T nmtInd(BOOL_T	execute, CO_NMT_STATE_T newState);
static void hbState(UNSIGNED8	nodeId, CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState);
static RET_T sdoServerReadInd(BOOL_T execute, UNSIGNED8	sdoNr, UNSIGNED16 index,
		UNSIGNED8	subIndex);
static RET_T sdoServerCheckWriteInd(BOOL_T execute, UNSIGNED8 node,
		UNSIGNED16 index, UNSIGNED8	subIndex, const UNSIGNED8 *pData);
static RET_T sdoServerWriteInd(BOOL_T execute, UNSIGNED8 sdoNr,
		UNSIGNED16 index, UNSIGNED8	subIndex);
static void pdoInd(UNSIGNED16);
static void pdoRecEvent(UNSIGNED16);
static void canInd(CO_CAN_STATE_T);
static void commInd(CO_COMM_STATE_EVENT_T);
static void startSdoTransfer(void *ptr);
static void sdoDomainInd(UNSIGNED16 index, UNSIGNED8 subIndex,
		UNSIGNED32	domainBufSize, UNSIGNED32	transferedSize);
static void usage(char *s);
static void timeMeasurement(int mode);

void dumpmsg(void);

/* external variables
---------------------------------------------------------------------------*/

/* global variables
---------------------------------------------------------------------------*/
UNSIGNED16	test;
UNSIGNED8	node = 127;
unsigned testmode = 0;			/* Standard test mode type
									0 = original
									1 = domain transfer
									x = ?? */

char c4lDevice[200] = "/dev/can0";

unsigned int dsize = DOM_SIZE;	/* default domain size in testmode == 1 */
unsigned int baud  = 250;
unsigned int speedfactor = 1;
unsigned int fillbyte = 0xCC;

static unsigned char	buf[200];
char	strg50 [50];
char	strg100[100];
char	strg200[200];
CO_TIMER_T sdoTimer;

SDO_TRANSMIT_TABLE sdoTransmitTable[] = {
	{ CO_FALSE,	0x1017,	buf, 2,	"read from 0x1017:0" },
	{ CO_TRUE,	0x1017,	buf, 2,	"write to 0x1017:0"},
	{ CO_FALSE,	0x2010,	buf, sizeof(buf),	"read from 0x2010:0 (ro)" },
	{ CO_TRUE,	0x2010,	buf, sizeof(buf),	"try write to 0x2010:0 (ro)" },
	{ CO_FALSE,	0x2011,	(UNSIGNED8 *)strg50, 50, "read from 2011:0  50 bytes" },
	{ CO_TRUE,	0x2011,	(UNSIGNED8 *)strg50, 50, "write to 2011:0  50 bytes"},
	{ CO_FALSE,	0x2011,	(UNSIGNED8 *)strg50, 50, "read from 2011:0  50 bytes" },
	{ CO_FALSE,	0x2012,	(UNSIGNED8 *)buf, 100, "read from 0x2012:0 100 bytes" },
	{ CO_TRUE,	0x2012,	(UNSIGNED8 *)strg100, 100, "write to 0x2012:0 100 bytes" },
	{ CO_FALSE,	0x2012,	(UNSIGNED8 *)buf, 100, "read from 0x2012:0 100 bytes" },
	{ CO_FALSE,	0x2013,	(UNSIGNED8 *)buf, 200, "read from 0x2013:0 200 bytes" },
	{ CO_TRUE,	0x2013,	(UNSIGNED8 *)strg200, 200, "read from 0x2013:0 200 bytes" },
	{ CO_FALSE,	0x2013,	(UNSIGNED8 *)buf, 200, "read from 0x2013:0 200 bytes" },
};

UNSIGNED8		*pDomainBuf;
CO_DOMAIN_PTR		domainBufPtr;


/* local defined variables
---------------------------------------------------------------------------*/

/***************************************************************************/
/**
* \brief usage information
*/ 
static void usage(
		char *s
	)
{
int i;
const  char *usage_text  = "\
CAN FD Test application (using CAN device \"%s\")\n\
-n nodeid	define the node id to be used\n\
	if nodeid == 1, it acts as SDO client, else SDO server\n\
-t mode - testmode, 0 - standard test (default)\n\
                    1 - SDO domain test\n\
-s size	- domain size for test mode 1 (default %d)\n\
-b bitrate, default is %d Kbit/s\n\
-f speedfactor - for CAN FD (default %d)\n\
\n\
When using the application with SocketCAN, Bit Timing parameters are set\n\
with the ip tool only.\n\
\n\
For the domain test, start the SDO client with -n127\n\
and the SDO server in mode = 1 with: -n1 -t1\n\
";

    fprintf(stderr, "usage: %s [options]\n", s);
    fprintf(stderr, (const char *)usage_text,
		c4lDevice, dsize, baud, speedfactor);

	dumpmsg();
    fprintf(stderr, "\nIn mode = 0 the following SDO requests are done by node 1:\n");
    for (i = 0; i <  (sizeof(sdoTransmitTable) / sizeof(SDO_TRANSMIT_TABLE));
	    i++) {
	fprintf(stderr, "\t%s\n", sdoTransmitTable[i].comment);

    }
}


void timeMeasurement(int mode)
{
static struct timeval start;
struct timeval stop, diff;

	if (mode == START) {
	    gettimeofday(&start, NULL);
	}
	else {
	    gettimeofday(&stop, NULL);
	    timersub(&stop, &start, &diff);
	    printf("Time used: %d.%d s\n",
		    (unsigned int)diff.tv_sec,
		    (unsigned int)diff.tv_usec);
	}
}

/***************************************************************************/
/**
* \brief main entry
*
* \param
*	nothing
* \results
*	nothing
*/
int main(
	int argc, char *argv[]
  )
{
int	i;
int c;
char *pname;
extern char *optarg;
extern int optind;
RET_T		retVal;

    pname = *argv;

	/* parse command line */
    while ((c = getopt(argc, argv, "db:D:f:F:t:n:s:")) != EOF) {
		switch (c) {
			case 'n':
				node = atoi(optarg);
				break;
			case 't':
				testmode = atoi(optarg);
				break;
			case 'b':
				baud = atoi(optarg);
				break;
			case 's':
				dsize = strtol(optarg, NULL, 0);
				break;
			case 'f':
				speedfactor = atoi(optarg);
				break;
			case 'F':
				fillbyte = (UNSIGNED8)strtol(optarg, NULL, 0);
				break;
			case 'D':
				strncpy(c4lDevice, optarg, 199); 
				break;
			case 'h':
			default: usage(pname); exit(0);
		}
	}
	if ((node < 1) || (node > 127))  {
		node = 127;
	}

	/* HW initialization */
	codrvHardwareInit();

	if (codrvCanInit(baud) != RET_OK)  {
		exit(1);
	}
	if (coCanOpenStackInit(NULL) != RET_OK)  {
		printf("error init library\n");
		exit(1);
	}

	if (codrvTimerSetup(CO_TIMER_INTERVAL) != RET_OK)  {
		exit(2);
	}

	/* register event functions */
	if (coEventRegister_NMT(nmtInd) != RET_OK)  {
		exit(3);
	}
	if (coEventRegister_ERRCTRL(hbState) != RET_OK)  {
		exit(4);
	}
	if (coEventRegister_USDO_SERVER_READ(sdoServerReadInd) != RET_OK)  {
		exit(5);
	}
	if (coEventRegister_USDO_SERVER_CHECK_WRITE(sdoServerCheckWriteInd) != RET_OK)  {
		exit(6);
	}
	if (coEventRegister_USDO_SERVER_WRITE(sdoServerWriteInd) != RET_OK)  {
		exit(7);
	}
	if (coEventRegister_PDO(pdoInd) != RET_OK)  {
		exit(8);
	}
	if (coEventRegister_PDO_REC_EVENT(pdoRecEvent) != RET_OK)  {
		exit(9);
	}
	if (coEventRegister_CAN_STATE(canInd) != RET_OK)  {
		exit(12);
	}
	if (coEventRegister_COMM_EVENT(commInd) != RET_OK)  {
		exit(13);
	}



	if (codrvCanEnable() != RET_OK)  {
		exit(17);
	}

	pDomainBuf = (UNSIGNED8 *)malloc(dsize);
	/* to avoid a maximum an stuff bits, set the buffer content */
	memset((void *)pDomainBuf, fillbyte, dsize);
	if (pDomainBuf == NULL) {
	    fprintf(stderr, "Unable to get the Domain Buffer [%d], exit\n",
		    dsize);
	    exit(100);
	}

	for (i = 0; i < sizeof(buf); i++)  {	
		buf[i] = 0x20 + i;
	}
	for (i = 0; i < sizeof(strg100); i++)  {	
		strg100[i] = 0x20 + i;
	}
	for (i = 0; i < sizeof(strg200); i++)  {	
		strg200[i] = 0x20 + i;
	}

	coOdDomainAddrSet(0x2020, 0, pDomainBuf, dsize);
	while (1)  {
            usleep(10);
    	    coCommTask();
	}
}


/*********************************************************************/
/* start SDO transfer
*/
static void startSdoTransfer(
		void *ptr
	)
{
#if 0
static UNSIGNED8	idx = 0;
RET_T		retVal;

	if (idx < (sizeof(sdoTransmitTable) / sizeof(SDO_TRANSMIT_TABLE)))  {

		printf("*** %s ***\n", sdoTransmitTable[idx].comment);
		if (sdoTransmitTable[idx].write != CO_TRUE)  {
			retVal = coUsdoRead(1, sdoTransmitTable[idx].index, 0,
				sdoTransmitTable[idx].pBuf, sdoTransmitTable[idx].size, 0, 1000);
		} else {
			retVal = coUsdoWrite(1, sdoTransmitTable[idx].index, 0,
				sdoTransmitTable[idx].pBuf, sdoTransmitTable[idx].size, 0, 1000);
		}

		printf("start sdo returns %d\n", retVal);

		idx++;
	} else {
		printf(" ===========================\n");
		printf(" === End of SDO sequence ===\n");
		printf(" ===========================\n");
	}
#endif
}



/*********************************************************************/
RET_T nmtInd(
		BOOL_T	execute,
		CO_NMT_STATE_T	newState
	)
{
	printf("nmtInd: New Nmt state %d - execute %d\n", newState, execute);

	return(RET_OK);
}


/*********************************************************************/
void pdoInd(
		UNSIGNED16	pdoNr
	)
{
	printf("pdoInd: pdo %d received\n", pdoNr);
}


/*********************************************************************/
void pdoRecEvent(
		UNSIGNED16	pdoNr
	)
{
	printf("pdoRecEvent: pdo %d time out\n", pdoNr);
}


/*********************************************************************/
void hbState(
		UNSIGNED8	nodeId,
		CO_ERRCTRL_T state,
		CO_NMT_STATE_T	nmtState
	)
{
	printf("hbInd: HB Event %d node %d nmtState: %d\n", state, nodeId, nmtState);

    return;
}


/*********************************************************************/
static RET_T sdoServerReadInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	printf("sdo server read ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}


/*********************************************************************/
static RET_T sdoServerCheckWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex,
		const UNSIGNED8	*pData
	)
{
	printf("sdo server check write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}


/*********************************************************************/
static RET_T sdoServerWriteInd(
		BOOL_T		execute,
		UNSIGNED8	sdoNr,
		UNSIGNED16	index,
		UNSIGNED8	subIndex
	)
{
	printf("sdo server write ind: exec: %d, sdoNr %d, index %x:%d\n",
		execute, sdoNr, index, subIndex);

   // return(RET_INVALID_PARAMETER);
	return(RET_OK);
}


/*********************************************************************/
static void canInd(
	CO_CAN_STATE_T	canState
	)
{
	switch (canState)  {
		case CO_CAN_STATE_BUS_OFF:
			printf("CAN: Bus Off\n");
			break;
		case CO_CAN_STATE_BUS_ON:
			printf("CAN: Bus On\n");
			break;
		case CO_CAN_STATE_PASSIVE:
			printf("CAN: Passiv\n");
			break;
		default:
			break;
	}
}


/*********************************************************************/
static void commInd(
		CO_COMM_STATE_EVENT_T	commEvent
	)
{
	switch (commEvent)  {
		case CO_COMM_STATE_EVENT_BUS_OFF:
			printf("COMM-Event Bus Off\n");
			break;
		case CO_COMM_STATE_EVENT_BUS_OFF_RECOVERY:
			printf("COMM-Event Bus Off\n");
			break;
		case CO_COMM_STATE_EVENT_BUS_ON:
			printf("COMM-Event Bus On\n");
			break;
		case CO_COMM_STATE_EVENT_PASSIVE:
			printf("COMM-Event Bus Passive\n");
			break;
		case CO_COMM_STATE_EVENT_ACTIVE:
			printf("COMM-Event Bus Active\n");
			break;
		case CO_COMM_STATE_EVENT_CAN_OVERRUN:
			printf("COMM-Event CAN Overrun\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_FULL:
			printf("COMM-Event Rec Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_REC_QUEUE_OVERFLOW:
			printf("COMM-Event Rec Queue Overflow\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_FULL:
			printf("COMM-Event Tr Queue Full\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_OVERFLOW:
			printf("COMM-Event Tr Queue Empty\n");
			break;
		case CO_COMM_STATE_EVENT_TR_QUEUE_EMPTY:
			printf("COMM-Event Tr Queue Empty\n");
			break;
		default:
			break;
	}
}

/*********************************************************************/
static void sdoDomainInd(
		UNSIGNED16      index,
		UNSIGNED8	subIndex,
		UNSIGNED32	domainBufSize,
		UNSIGNED32	transferedSize
	)
{
	printf("sdoDomainInd: index %x:%d, actual domainBufSize %ld, transfered size is %ld\n",
		index, subIndex, domainBufSize, transferedSize);
}


#include <can4linux.h>
void dumpmsg()
{
canmsg_t m;

	printf("size of can message structure is %d bytes\n", (int)sizeof(m));

	printf("Start:     %p\n", &m);
	printf("=========================\n");
	printf("flags:     %p\n", &m.flags);
	printf("cob:       %p\n", &m.cob);
	printf("id:        %p\n", &m.id);
	printf("timestamp: %p\n", &m.timestamp);
	printf("length:    %p\n", &m.length);
	printf("data:      %p\n", &m.data);
	printf("data:      %p\n", &m.data[0]);
	printf("data[]:    %d\n", (int)sizeof(m.data));
}
