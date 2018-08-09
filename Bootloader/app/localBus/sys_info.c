/*
 * unit_sys_info.c
 *
 *      Author: pli
 */


#include <string.h>
#include "main.h"


#define MAJOR_VERSION			0
#define SUB_VERSION				0
#define MINOR_VERSION			1
#define CSV_VERSION				0x05


#define SW_GIT_VERSION			1710251301


__attribute__ ((section(".bootInfo"),used))
const SysInfoST _sysInfo =
{
		0xFFFF,
		__DATE__,
		__TIME__,
		"Rainbow",
		"Bootloader",
		{MAJOR_VERSION, SUB_VERSION, MINOR_VERSION},
		{1, 0, 0},
		SW_GIT_VERSION,
};





const uint16_t   	burstLength = 0x0;
uint16_t 			deviceStatus = 0x0;


