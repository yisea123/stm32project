
#ifndef SHELL_IO_H
#define SHELL_IO_H


#define 	MAX_PRINT_BUFF_SIZE		0x3000
#define     RESERV_LEN				0x04

#define 	MAX_PRINT_SIZE 			(size_t)(0xFF)

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	uint16_t	len;
	uint8_t		buff[2];
}PrintSt;
#define 	LEN_T		2u //sizeof(uint16_t);

#pragma pack(pop)


PrintSt* GetAvailablePrintBuff(uint16_t len);

void SendBugMsg(void);
void Print_SendRaw(const uint8_t* buff, uint16_t len);
void shell_Add(const char* str, ...);
void shell_Add_Rawdata(const char* str, uint16_t cnt);
void shell_AddRTC(const char* str, ...);
void xprintf(const char* str, ...);
extern void xprintfRTC(const char* str, ...);
extern void xprintf_rtc(void);
extern void xprintf_tick(void);

//void TaracePrint(uint16_t id, const char* str, ...);
//void TraceDBG(uint16_t chn, const char* str, ...);
//void TraceMsg(uint16_t chn, const char* str, ...);

void PrintChn(uint8_t* msgTable,uint8_t enable, uint16_t chn, const char* str, ...);
void Init_PrintSem(void);
#define  TraceMsg(...)		PrintChn(printMsgMap,1,__VA_ARGS__)
#define  TraceDBG(...) 		PrintChn(dbgMsgMap,2,__VA_ARGS__)
#define  TracePrint(...) 	PrintChn(printChnMap,0,__VA_ARGS__)
#define  TraceUser(...) 	PrintChn(printChnMap,3,0xFFFF,__VA_ARGS__)
void StartShellTXTask(void const * argument);

void StartShellRXTask(void const * argument);

#define eprintf   shell_AddRTC
#endif

