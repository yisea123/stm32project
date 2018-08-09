
#ifndef SHELL_IO_H
#define SHELL_IO_H


uint16_t PutObj(uint16_t obj_id, int16_t atr_id, double val,char* str);
uint16_t GetObj(uint16_t obj_id, int16_t atr_id, uint16_t from,char* str);
uint16_t UpdateObjType(uint16_t obj_id, int16_t atr_id);

uint16_t NewUartData(uint16_t type, uint8_t* ptrData);

int shell_init(void);
int shell_read(char* buf, int cnt, void *extobj);
void shell_printf(uint16_t chn, const char* str, ...);
#define shell_Add(...)		shell_printf(1,__VA_ARGS__)
void xprintf(const char* str, ...);
extern void xprintfRTC1(const char* str, ...);
extern void xprintfRTC(const char* str, ...);
extern void xprintf_rtc(void);
extern void xprintf_tick(void);
#endif

